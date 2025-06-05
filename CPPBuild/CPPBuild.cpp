
#include "Precomp.h"
#include "CPPBuild.h"
#include "VSGenerator.h"
#include "IOData/Directory.h"
#include "IOData/FilePath.h"
#include "IOData/File.h"
#include "Javascript/quickjs.h"

CPPBuild::CPPBuild()
{
	sourceFiles = { "CPPBuild.cpp" };
	headerFiles = { "CPPBuild.h" };
	defines = { "WIN32", "_WIN32" };
	includes = { "$(SolutionDir)" };
}

void CPPBuild::generate(std::string sourcePath)
{
	std::string cppbuildexe = "\"" + FilePath::combine(Directory::exePath(), "cppbuild.exe") + "\"";

	std::string scriptFilename = FilePath::combine(sourcePath, "CPPBuild.js");
	std::string configureScript = File::readAllText(scriptFilename);
	auto runtime = JS_NewRuntime();
	auto context = JS_NewContext(runtime);
	JSValue result = JS_Eval(context, configureScript.c_str(), configureScript.size(), scriptFilename.c_str(), JS_EVAL_TYPE_GLOBAL | JS_EVAL_FLAG_STRICT);
	size_t strLength = 0;
	const char* strData = JS_ToCStringLen(context, &strLength, result);
	std::string str(strData, strLength);
	JS_FreeCString(context, strData);
	JS_FreeValue(context, result);
	JS_FreeContext(context);
	JS_FreeRuntime(runtime);

	std::string solutionName = str;
	std::string projectName = str;
	std::string outputExe = str + ".exe";
	if (solutionName.empty())
		throw std::runtime_error("No solution name specified");

	auto debugConfig = std::make_unique<VSCppProjectConfiguration>("Debug", "x64");
	debugConfig->general.configurationType = "Makefile";
	debugConfig->general.nmakePreprocessorDefinitions = defines;
	debugConfig->general.nmakeIncludeSearchPath = includes;
	debugConfig->general.nmakeOutput = outputExe;
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

	auto project = std::make_unique<VSCppProject>(projectName);
	project->sourceFiles = sourceFiles;
	project->headerFiles = headerFiles;
	project->extraFiles = extraFiles;
	project->configurations.push_back(std::move(debugConfig));
	project->configurations.push_back(std::move(releaseConfig));

	auto solution = std::make_unique<VSSolution>(solutionName);
	solution->projects.push_back(std::move(project));
	solution->configurations.push_back(std::make_unique<VSSolutionConfiguration>("Debug", "x64"));
	solution->configurations.push_back(std::make_unique<VSSolutionConfiguration>("Release", "x64"));

	solution->generate();
}

void CPPBuild::build(std::string workdir, std::string target)
{
}

void CPPBuild::clean(std::string workdir, std::string target)
{
}

void CPPBuild::rebuild(std::string workdir, std::string target)
{
	clean(workdir, target);
	build(workdir, target);
}
