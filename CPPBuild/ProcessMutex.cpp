
#include "Precomp.h"
#include "ProcessMutex.h"

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

class ProcessMutexWin32 : public ProcessMutex
{
public:
	ProcessMutexWin32()
	{
		handle = CreateMutex(nullptr, FALSE, L"Local\\CPPBuildMutex");
		if (!handle)
			throw std::runtime_error("Could not create Local\\CPPBuildMutex");
		WaitForSingleObject(handle, INFINITE);
	}

	~ProcessMutexWin32()
	{
		ReleaseMutex(handle);
		CloseHandle(handle);
	}

	HANDLE handle = {};
};

std::unique_ptr<ProcessMutex> ProcessMutex::lock()
{
	return std::make_unique<ProcessMutexWin32>();
}

#else

class ProcessMutexUnix : public ProcessMutex
{
public:
};

std::unique_ptr<ProcessMutex> ProcessMutex::lock()
{
	return std::make_unique<ProcessMutexUnix>();
}

#endif
