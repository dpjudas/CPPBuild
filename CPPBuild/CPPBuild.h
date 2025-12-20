#pragma once

class JsonValue;
class BuildSetup;

class CPPBuild
{
public:
	CPPBuild(std::string workDir);

	void configure(std::string sourcePath);
	void updateMakefile();
	void checkMakefile();
	JsonValue runConfigureScript(const std::string& sourcePath);
	void validateConfig(const BuildSetup& setup);
	void generateWorkspace();
	void build(std::string target, std::string configuration);
	void rebuild(std::string target, std::string configuration);
	void clean(std::string target, std::string configuration);
	void createInstaller();
	BuildSetup loadBuildSetup();

	std::vector<std::string> getBuildOrder(BuildSetup& setup, std::string target, std::string configuration);

	std::string workDir;
	std::string cppbuildDir;
};
