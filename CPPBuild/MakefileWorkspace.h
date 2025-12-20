#pragma once

#include <string>

class BuildSetup;

class MakefileWorkspace
{
public:
	void generate(const BuildSetup& setup, const std::string& workDir, const std::string &cppbuildDir);
};
