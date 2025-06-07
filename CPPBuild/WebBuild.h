#pragma once

#include "Json/JsonValue.h"

class ZipWriter;

enum class ProjectType
{
	library,
	component,
	website
};

class WebBuild
{
public:
	WebBuild(const std::string& workDir, const std::string& target, const std::string& configuration);

	std::string workDir;
	std::string target;
	std::string configuration;

	std::string binDir;
	std::string objDir;

	ProjectType projectType = ProjectType::website;
	std::vector<std::string> sourceFiles;
	std::vector<std::string> dependencies;

	std::string wwwrootDir;
	std::string cssFile;
	std::string shellFile;
	std::vector<std::string> includePaths;

	std::vector<std::string> outputFiles;

	std::string emcc;
	std::string emar;
	std::string compileFlags;
	std::string linkFlags;

	void build();
	void clean();
	void rebuild();

private:
	void loadTargets();
	bool isCppFile(const std::string& filename);
	void compile();
	void link();
	void linkCSS();
	void package();

	JsonValue getConfigDef(const JsonValue& config);
	JsonValue getTargetDef(const JsonValue& config);

	std::string getLibPrefix() const { return projectType == ProjectType::library ? "lib" : ""; }

	std::string runCommand(const std::string& commandline, const std::string& errorMessage);
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
};
