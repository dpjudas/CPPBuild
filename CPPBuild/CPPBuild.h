#pragma once

class CPPBuild
{
public:
	CPPBuild();

	void generate();
	void build(std::string target);
	void rebuild(std::string target);
	void clean(std::string target);

private:
	std::vector<std::string> sourceFiles;
	std::vector<std::string> headerFiles;
	std::vector<std::string> extraFiles;
	std::vector<std::string> defines;
	std::vector<std::string> includes;
};
