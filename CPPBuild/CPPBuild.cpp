
#include "Precomp.h"
#include "CPPBuild.h"
#include "VSGenerator.h"

CPPBuild::CPPBuild()
{
	sourceFiles = { "CPPBuild.cpp" };
	headerFiles = { "CPPBuild.h" };
	defines = { "WIN32", "_WIN32" };
	includes = { "$(SolutionDir)" };
}

void CPPBuild::generate()
{
#ifdef _DEBUG
	std::string cppbuildexe = "$(SolutionDir)Build\\Debug\\x64\\bin\\cppbuild.exe";
#else
	std::string cppbuildexe = "$(SolutionDir)Build\\Release\\x64\\bin\\cppbuild.exe";
#endif

	auto debugConfig = std::make_unique<VSCppProjectConfiguration>("Debug", "x64");
	debugConfig->general.configurationType = "Makefile";
	debugConfig->general.nmakePreprocessorDefinitions = defines;
	debugConfig->general.nmakeIncludeSearchPath = includes;
	debugConfig->general.nmakeOutput = "musicplayer.exe";
	debugConfig->general.nmakeBuildCommandLine = cppbuildexe + " build debug";
	debugConfig->general.nmakeCleanCommandLine = cppbuildexe + " clean debug";
	debugConfig->general.nmakeReBuildCommandLine = cppbuildexe + " rebuild debug";

	auto releaseConfig = std::make_unique<VSCppProjectConfiguration>("Release", "x64");
	releaseConfig->general.configurationType = "Makefile";
	releaseConfig->general.nmakePreprocessorDefinitions = defines;
	releaseConfig->general.nmakeIncludeSearchPath = includes;
	releaseConfig->general.nmakeBuildCommandLine = cppbuildexe + " build release";
	releaseConfig->general.nmakeCleanCommandLine = cppbuildexe + " clean release";
	releaseConfig->general.nmakeReBuildCommandLine = cppbuildexe + " rebuild release";

	auto project = std::make_unique<VSCppProject>("musicplayer");
	project->sourceFiles = sourceFiles;
	project->headerFiles = headerFiles;
	project->extraFiles = extraFiles;
	project->configurations.push_back(std::move(debugConfig));
	project->configurations.push_back(std::move(releaseConfig));

	auto solution = std::make_unique<VSSolution>("musicplayer");
	solution->projects.push_back(std::move(project));
	solution->configurations.push_back(std::make_unique<VSSolutionConfiguration>("Debug", "x64"));
	solution->configurations.push_back(std::make_unique<VSSolutionConfiguration>("Release", "x64"));

	solution->generate();
}

void CPPBuild::build(std::string target)
{
}

void CPPBuild::clean(std::string target)
{
}

void CPPBuild::rebuild(std::string target)
{
	clean(target);
	build(target);
}
