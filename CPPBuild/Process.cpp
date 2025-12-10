
#include "Precomp.h"
#include "Process.h"
#include "Text/UTF16.h"
#include <stdexcept>
#include <mutex>
#include <array>

#ifdef WIN32
#define NOMINMAX
#define WIN32_MEAN_AND_LEAN
#include <Windows.h>

int Process::runCommand(const std::string& commandline, const std::function<void(const std::string& outputLine)> printLine)
{
	std::wstring cmd = to_utf16("cmd.exe /c " + commandline);

	static std::mutex mutex;
	std::unique_lock lock(mutex); // CreateProcess duplicates ALL inheriting handles in our process

	SECURITY_ATTRIBUTES inheritAttr = {};
	inheritAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	inheritAttr.bInheritHandle = TRUE;

	HANDLE stdinRead = INVALID_HANDLE_VALUE, stdinWrite = INVALID_HANDLE_VALUE;
	BOOL result = CreatePipe(&stdinRead, &stdinWrite, &inheritAttr, 0);
	if (result == FALSE)
		throw std::runtime_error("Could not create stdin pipe");

	HANDLE stdoutRead = INVALID_HANDLE_VALUE, stdoutWrite = INVALID_HANDLE_VALUE;
	result = CreatePipe(&stdoutRead, &stdoutWrite, &inheritAttr, 0);
	if (result == FALSE)
	{
		CloseHandle(stdinRead);
		CloseHandle(stdinWrite);
		throw std::runtime_error("Could not create stdout pipe");
	}

	HANDLE stderrRead = INVALID_HANDLE_VALUE, stderrWrite = INVALID_HANDLE_VALUE;
	result = CreatePipe(&stderrRead, &stderrWrite, &inheritAttr, 0);
	if (result == FALSE)
	{
		CloseHandle(stdinRead);
		CloseHandle(stdinWrite);
		CloseHandle(stdoutRead);
		CloseHandle(stdoutWrite);
		throw std::runtime_error("Could not create stderr pipe");
	}

	if (SetHandleInformation(stdinWrite, HANDLE_FLAG_INHERIT, 0) == FALSE ||
		SetHandleInformation(stdoutRead, HANDLE_FLAG_INHERIT, 0) == FALSE ||
		SetHandleInformation(stderrRead, HANDLE_FLAG_INHERIT, 0) == FALSE)
	{
		CloseHandle(stdinRead);
		CloseHandle(stdinWrite);
		CloseHandle(stdoutRead);
		CloseHandle(stdoutWrite);
		CloseHandle(stderrRead);
		CloseHandle(stderrWrite);
		throw std::runtime_error("SetHandleInformation failed");
	}

	STARTUPINFO startupInfo = {};
	startupInfo.cb = sizeof(STARTUPINFO);
	startupInfo.dwFlags = STARTF_FORCEOFFFEEDBACK | STARTF_USESTDHANDLES;
	startupInfo.hStdInput = stdinRead;
	startupInfo.hStdOutput = stdoutWrite;
	startupInfo.hStdError = stderrWrite;
	PROCESS_INFORMATION processInfo = {};
	result = CreateProcess(nullptr, cmd.data(), nullptr, nullptr, TRUE, CREATE_NO_WINDOW, nullptr, nullptr, &startupInfo, &processInfo);

	CloseHandle(stdinRead);
	CloseHandle(stdoutWrite);
	CloseHandle(stderrWrite);
	if (result == FALSE)
	{
		CloseHandle(stdinWrite);
		CloseHandle(stdoutRead);
		CloseHandle(stderrRead);
		throw std::runtime_error("Could not create process");
	}

	lock.unlock(); // No more inheriting handles open from this point

	// We have nothing to write to stdin
	CloseHandle(stdinWrite);

	std::array<char, 4096> buffer;
	std::string line;
	line.reserve(1024);

	// To do: how do we know which OEM codepage is in use by the process we called? We need the output as UTF-8

	// Read stdout
	while (true)
	{
		DWORD bytesRead = 0;
		result = ReadFile(stdoutRead, buffer.data(), (DWORD)buffer.size(), &bytesRead, nullptr);
		if (result == FALSE || bytesRead == 0)
			break;
		for (DWORD i = 0; i < bytesRead; i++)
		{
			if (buffer[i] == '\n')
			{
				printLine(line);
				line.clear();
			}
			else if (buffer[i] != '\r')
			{
				line.push_back(buffer[i]);
			}
		}
	}
	if (!line.empty())
		printLine(line);
	line.clear();

	// Read stderr
	while (true)
	{
		DWORD bytesRead = 0;
		result = ReadFile(stderrRead, buffer.data(), (DWORD)buffer.size(), &bytesRead, nullptr);
		if (result == FALSE || bytesRead == 0)
			break;
		for (DWORD i = 0; i < bytesRead; i++)
		{
			if (buffer[i] == '\n')
			{
				printLine(line);
				line.clear();
			}
			else if (buffer[i] != '\r')
			{
				line.push_back(buffer[i]);
			}
		}
	}
	if (!line.empty())
		printLine(line);
	line.clear();

	WaitForSingleObject(processInfo.hProcess, INFINITE);

	DWORD exitCode = 255;
	GetExitCodeProcess(processInfo.hProcess, &exitCode);

	CloseHandle(stdoutRead);
	CloseHandle(stderrRead);
	CloseHandle(processInfo.hThread);
	CloseHandle(processInfo.hProcess);

	return (int)exitCode;
}

#else

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int Process::runCommand(const std::string& commandline, const std::function<void(const std::string& outputLine)> printLine)
{
	std::string arg;
	arg.reserve(1024);
	std::vector<std::string> args;
	bool inQuotes = false;
	for (char c : commandline)
	{
		if (inQuotes)
		{
			if (c == '"')
			{
				inQuotes = false;
			}
			else
			{
				arg.push_back(c);
			}
		}
		else if (c == '"')
		{
			inQuotes = true;
		}
		else if (c == ' ')
		{
			if (!arg.empty())
				args.push_back(arg);
			arg.clear();
		}
		else
		{
			arg.push_back(c);
		}
	}
	if (!arg.empty())
		args.push_back(arg);

	std::vector<char*> argv;
	for (std::string& v : args)
		argv.push_back(v.c_str());
	argv.push_back(nullptr);

	int stdoutfd[2] = {};
	int result = pipe(stdoutfd);
	if (result < 0)
		throw std::runtime_error("pipe failed");

	int stderrfd[2] = {};
	int result = pipe(stderrfd);
	if (result < 0)
	{
		close(stdoutfd[0]);
		close(stdoutfd[1]);
		throw std::runtime_error("pipe failed");
	}

	int stdinfd[2] = {};
	int result = pipe(stdinfd);
	if (result < 0)
	{
		close(stdoutfd[0]);
		close(stdoutfd[1]);
		close(stderrfd[0]);
		close(stderrfd[1]);
		throw std::runtime_error("pipe failed");
	}

	pid_t cpid = fork();
	if (cpid == -1)
	{
		close(stdoutfd[0]);
		close(stdoutfd[1]);
		close(stderrfd[0]);
		close(stderrfd[1]);
		close(stdinfd[0]);
		close(stdinfd[1]);
		throw std::runtime_error("fork failed");
	}

	if (cpid == 0)
	{
		// Child process
		close(stdinfd[1]);
		close(stdoutfd[0]);
		close(stderrfd[0]);
		// stdin = stdinfd[0];
		// stdout = stdoutfd[1];
		// stderr = stderrfd[1];
		execvp(argv[0], argv.data());
		_exit(EXIT_FAILURE);
		return 0;
	}
	else
	{
		// Parent process
		close(stdinfd[0]);
		close(stdoutfd[1]);
		close(stderrfd[1]);

		// We have nothing to write to stdin
		close(stdinfd[1]);

		std::array<char, 4096> buffer;
		std::string line;
		line.reserve(1024);
		while (true)
		{
			int bytesRead = read(stdoutfd[0], buffer.data(), (int)buffer.size());
			if (bytesRead <= 0)
				break;
			for (int i = 0; i < bytesRead; i++)
			{
				if (buffer[i] == '\n')
				{
					printLine(line);
					line.clear();
				}
				else
				{
					line.push_back(buffer[i]);
				}
			}
		}
		if (!line.empty())
			printLine(line);
		line.clear();

		while (true)
		{
			int bytesRead = read(stderrfd[0], buffer.data(), (int)buffer.size());
			if (bytesRead <= 0)
				break;
			for (int i = 0; i < bytesRead; i++)
			{
				if (buffer[i] == '\n')
				{
					printLine(line);
					line.clear();
				}
				else
				{
					line.push_back(buffer[i]);
				}
			}
		}
		if (!line.empty())
			printLine(line);
		line.clear();

		close(stdoutfd[0]);
		close(stderrfd[0]);

		int wstatus = 0;
		result = waitpid(cpid, &wstatus, 0);
		if (result < 0)
			throw std::runtime_error("waitpid failed");
		return WEXITSTATUS(wstatus);
	}
}

#endif
