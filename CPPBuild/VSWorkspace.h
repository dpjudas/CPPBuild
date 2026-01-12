#pragma once

#include <string>
#include <map>

class BuildSetup;
class PackageManager;

class VSGuids
{
public:
	std::string solutionGuid;
	std::map<std::string, std::string> projectGuids;
};

class VSWorkspace
{
public:
	void generate(const BuildSetup& setup, PackageManager* packages, const std::string& workDir, const std::string& cppbuildDir);

private:
	VSGuids loadSolutionGuids(const std::string& cppbuildDir);
	void saveSolutionGuids(const VSGuids& guids, const std::string& cppbuildDir);
	std::string addPathToCommand(std::string cmdline, const std::string& configName, const BuildSetup& setup, const std::string& workDir);
};
