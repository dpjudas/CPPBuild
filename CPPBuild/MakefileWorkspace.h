#pragma once

#include <string>

class BuildSetup;

class MakefileWorkspace
{
public:
	bool generate(const BuildSetup& setup, const std::string& workDir, const std::string &cppbuildDir);
};
