#pragma once

#include <string>
#include <map>
#include <set>

class BuildSetup;
class PackageManager;
class VSSolution;
class BuildTarget;

class VSGuids
{
public:
	std::string solutionGuid;
	std::map<std::string, std::string> projectGuids;
	std::map<std::string, std::string> solutionFolderGuids;
};

class VSWorkspace
{
public:
	static void generate(const BuildSetup& setup, PackageManager* packages, const std::string& workDir, const std::string& cppbuildDir, const std::string& vsversion);

private:
	VSWorkspace(const BuildSetup& setup, PackageManager* packages, const std::string& workDir, const std::string& cppbuildDir, const std::string& vsversion);
	void generate();

	VSGuids loadSolutionGuids(const std::string& cppbuildDir);
	void saveSolutionGuids(const VSGuids& guids, const std::string& cppbuildDir);
	std::string addPathToCommand(std::string cmdline, const std::string& configName, const BuildSetup& setup, const std::string& workDir);

	static std::string getActivePlatform();

	void createProjectGuids();
	void addSolutionFolders();
	void addCPPBuildCheck();
	void addTargetProject(const BuildTarget& targetDef);

	const BuildSetup& setup;
	PackageManager* packages;
	const std::string& workDir;
	const std::string& cppbuildDir;
	const std::string& vsversion;

	std::string cppbuildexe;
	VSGuids guids;
	std::unique_ptr<VSSolution> solution;
	std::set<std::string> solutionFolders;
	std::string platform;
	std::map<std::string, std::string> fileFolderGuids;
};
