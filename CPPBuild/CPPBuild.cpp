
#include "Precomp.h"
#include "CPPBuild.h"
#include "VSGenerator.h"
#include "IOData/Directory.h"
#include "IOData/FilePath.h"
#include "IOData/File.h"
#include "Javascript/ScriptContext.h"
#include "Json/JsonValue.h"
#include <iostream>

CPPBuild::CPPBuild()
{
}

void CPPBuild::generate(std::string sourcePath)
{
	std::string cppbuildexe = "\"" + FilePath::combine(Directory::exePath(), "cppbuild.exe") + "\"";

	std::string scriptFilename = FilePath::combine(sourcePath, "CPPBuild.js");
	std::string configureScript = File::readAllText(scriptFilename);

	ScriptContext context;
	ScriptValue result = context.eval(configureScript, scriptFilename, JS_EVAL_TYPE_MODULE | JS_EVAL_FLAG_STRICT);

	if (result.isException())
	{
		ScriptValue exception = context.getException();
		std::cout << exception.toString().c_str() << std::endl;
		std::cout << exception.getExceptionStack().c_str() << std::endl;
		return;
	}

	JsonValue config = JsonValue::parse(context.generateConfiguration());

	std::string solutionName = config["name"].to_string();
	if (solutionName.empty())
		throw std::runtime_error("No solution name specified");

	auto solution = std::make_unique<VSSolution>(solutionName);

	for (const JsonValue& configDef : config["configurations"].items())
		solution->configurations.push_back(std::make_unique<VSSolutionConfiguration>(configDef["name"].to_string(), configDef["platform"].to_string()));

	for (const JsonValue& targetDef : config["targets"].items())
	{
		std::vector<std::string> sourceFiles;
		std::vector<std::string> headerFiles;
		std::vector<std::string> extraFiles;
		std::vector<std::string> defines;
		std::vector<std::string> includes;

		for (const JsonValue& item : targetDef["files"].items())
			sourceFiles.push_back(item.to_string());

		for (const JsonValue& item : targetDef["defines"].items())
			defines.push_back(item.to_string());

		for (const JsonValue& item : targetDef["includes"].items())
			includes.push_back(item.to_string());

		std::string projectName = targetDef["name"].to_string();
		std::string outputExe = projectName + ".exe";

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
		solution->projects.push_back(std::move(project));
	}

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
