#pragma once

#include <unordered_map>
#include <unordered_set>

class JsonValue;
class BuildSetup;
class BuildTarget;

class CPPBuild
{
public:
	CPPBuild(std::string workDir);

	void configure(std::string sourcePath);
	void checkMakefile();
	int build(std::string target, std::string configuration);
	int rebuild(std::string target, std::string configuration);
	void clean(std::string target, std::string configuration);
	void createInstaller();
	void createPackage();

private:
	void updateMakefile();
	JsonValue runConfigureScript(const std::string& sourcePath);
	void validateConfig(const BuildSetup& setup);
	void generateWorkspace();

	BuildSetup loadBuildSetup();

	static std::vector<std::string> getBuildOrder(BuildSetup& setup, std::string target, std::string configuration);
	static void addTarget(std::vector<std::string>& buildOrder, const BuildTarget* target, std::unordered_map<std::string, const BuildTarget*>& allTargets, std::unordered_set<std::string>& added, int depth);

	std::string workDir;
	std::string cppbuildDir;
};
