#pragma once

class CPPBuild
{
public:
	CPPBuild();

	void generate(std::string sourcePath);
	void build(std::string workdir, std::string target);
	void rebuild(std::string workdir, std::string target);
	void clean(std::string workdir, std::string target);
};
