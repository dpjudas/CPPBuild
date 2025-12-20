#pragma once

#include <string>
#include <map>

class BuildSetup;
class VSCppProjectConfiguration;

class VSGuids
{
public:
	std::string solutionGuid;
	std::map<std::string, std::string> projectGuids;
};

class VSWorkspace
{
public:
	void generate(const BuildSetup& setup, const std::string& workDir, const std::string& cppbuildDir);

private:
	VSGuids loadSolutionGuids(const std::string& cppbuildDir);
	void saveSolutionGuids(const VSGuids& guids, const std::string& cppbuildDir);

	void applyCompileOptions(VSCppProjectConfiguration* projConfig, const std::vector<std::string>& options);
	void applyLinkOptions(VSCppProjectConfiguration* projConfig, const std::vector<std::string>& options);

	struct SimpleOption
	{
		const char* value;
		std::string* prop;
	};
};
