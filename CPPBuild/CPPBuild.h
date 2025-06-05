#pragma once

class CPPBuild
{
public:
	CPPBuild();

	void generate(std::string sourcePath);
	void build(std::string workdir, std::string target);
	void rebuild(std::string workdir, std::string target);
	void clean(std::string workdir, std::string target);

private:
	std::vector<std::string> sourceFiles;
	std::vector<std::string> headerFiles;
	std::vector<std::string> extraFiles;
	std::vector<std::string> defines;
	std::vector<std::string> includes;
};
