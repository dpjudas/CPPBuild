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
	int postBuild(std::string target, std::string configuration);
	void clean(std::string target, std::string configuration);
	void createInstaller();
	void createPackage();
	void setProperty(std::string name, std::string value, bool global);
	void listProperties();

private:
	void updateMakefile();
	JsonValue runConfigureScript(const std::string& sourcePath, const JsonValue& properties);
	void validateConfig(const BuildSetup& setup);
	void generateWorkspace(const JsonValue& properties);

	std::string getGlobalConfigDir();
	std::string getLocalConfigDir();
	static JsonValue loadProperties(const std::string& filename);

	BuildSetup loadBuildSetup();

	static std::vector<std::string> getBuildOrder(BuildSetup& setup, std::string target, std::string configuration);
	static void addTarget(std::vector<std::string>& buildOrder, const BuildTarget* target, std::unordered_map<std::string, const BuildTarget*>& allTargets, std::unordered_set<std::string>& added, int depth);

	std::string workDir;
	std::string cppbuildDir;
};
