#pragma once

class JsonValue;

class VSGuids
{
public:
	std::string solutionGuid;
	std::map<std::string, std::string> projectGuids;
};

class CPPBuild
{
public:
	CPPBuild(std::string workDir);

	void configure(std::string sourcePath);
	JsonValue runConfigureScript(const std::string& sourcePath);
	void validateConfig(const JsonValue& config);
	void generateWorkspace();
	void build(std::string target, std::string configuration);
	void rebuild(std::string target, std::string configuration);
	void clean(std::string target, std::string configuration);

	VSGuids loadSolutionGuids();
	void saveSolutionGuids(const VSGuids& guids);

	std::string workDir;
	std::string cppbuildDir;
};
