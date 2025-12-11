#pragma once

#include "Json/JsonValue.h"
#include <mutex>

class ZipWriter;

enum class TargetType
{
	application,
	console,
	lib,
	dll,
	webLibrary,
	webComponent,
	website
};

class Target
{
public:
	Target(const std::string& workDir, const std::string& target, const std::string& configuration);

	std::string workDir;
	std::string target;
	std::string configuration;

	std::string binDir;
	std::string objDir;

	TargetType targetType = TargetType::website;
	std::vector<std::string> sourceFiles;
	std::vector<std::string> linkLibraries;

	std::string wwwrootDir;
	std::string cssFile;
	std::string shellFile;
	std::vector<std::string> includePaths;
	std::vector<std::string> defines;
	std::vector<std::string> libraryPaths;

	std::vector<std::string> outputFiles;

	std::string cc, ccpp;
	std::string ar;
	std::string cflags, cxxflags;
	std::string linkFlags;

	void build();
	void clean();
	void rebuild();

private:
	void compileThreadMain(int threadIndex, int numThreads);
	void loadTargets();
	bool isCppFile(const std::string& filename);
	void compile();
	void link();
	void linkCSS();
	void package();

	std::string getLibPrefix() const;

	void printLine(const std::string& text);
	void runCommand(const std::string& commandline, const std::string& errorMessage);
	std::string processCSSFile(const std::string& filename, std::string text, std::vector<std::string>& includes, int level = 0);
	void addFolder(ZipWriter* zip, std::string srcdir, std::string destdir);

	enum class MakeTokenType
	{
		string,
		colon,
		newline
	};

	struct MakeToken
	{
		MakeTokenType type = MakeTokenType::string;
		std::string value;
	};

	static std::vector<MakeToken> parseMakefile(const std::string& text);
	static std::vector<std::string> readMakefileDependencyFile(const std::string& filename);
	static std::vector<std::string> readDependencyFile(const std::string& filename);
	static void writeDependencyFile(const std::string& filename, const std::string& inputFile, const std::vector<std::string>& includes);

	std::mutex mutex;
	bool compileFailed = false;
};
