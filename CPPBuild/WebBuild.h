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
	WebBuild(const std::string& projectdir, const std::string& platform, const std::string& configuration);

	std::string binDir;
	std::string objDir;
	std::string projectDir;
	std::string wwwrootDir;

	std::string projectName;
	ProjectType projectType = ProjectType::website;
	std::vector<std::string> sourceFiles;
	std::vector<std::string> headerFiles;
	std::vector<std::string> extraFiles;
	std::vector<std::string> ignoreList;
	std::vector<std::string> dependencies;
	std::string platform;
	std::string configuration;

	std::string cssFile;
	std::string shellFile;
	std::string includePath;

	std::vector<std::string> outputFiles;

	std::string emcc;
	std::string emar;
	std::string compileFlags;
	std::string linkFlags;

	void build();
	void clean();
	void rebuild();

private:
	void loadProjectFile();
	void findFiles(const std::string& folder);
	void findFiles(const std::string& folder, const std::string& relativeFolder);
	bool isCppFile(const std::string& filename);
	void compile();
	void link();
	void linkCSS();
	void package();

	std::string getLibPrefix() const { return projectType == ProjectType::library ? "lib" : ""; }

	std::string runCommand(const std::string& commandline, const std::string& errorMessage);
	std::string processCSSFile(const std::string& filename, std::string text, std::vector<std::string>& includes, const std::string& includePath, int level = 0);
	void addFolder(ZipWriter* zip, std::string srcdir, std::string destdir);

	std::string substituteVars(std::string str);
	std::string substituteVar(std::string str, const std::string& name, const std::string& value);

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
