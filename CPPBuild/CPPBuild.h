#pragma once

class JsonValue;

class CPPBuild
{
public:
	CPPBuild(std::string workDir);

	void configure(std::string sourcePath);
	JsonValue runConfigureScript(const std::string& sourcePath);
	void validateConfig(const JsonValue& config);
	void generateWorkspace(std::string workDir);
	void build(std::string target, std::string configuration);
	void rebuild(std::string target, std::string configuration);
	void clean(std::string target, std::string configuration);

	std::string workDir;
	std::string cppbuildDir;
};
