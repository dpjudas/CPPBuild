
#include "Precomp.h"
#include "CPPBuild.h"
#include "VSGenerator.h"
#include "WebBuild.h"
#include "IOData/Directory.h"
#include "IOData/FilePath.h"
#include "IOData/File.h"
#include "Javascript/ScriptContext.h"
#include "Json/JsonValue.h"
#include <iostream>

CPPBuild::CPPBuild(std::string workDir) : workDir(workDir)
{
	cppbuildDir = FilePath::combine(workDir, ".cppbuild");
}

void CPPBuild::configure(std::string sourcePath)
{
	if (sourcePath.empty())
		sourcePath = Directory::currentDirectory();

	JsonValue config = JsonValue::object();
	config["version"] = JsonValue::number(1);
	config["sourcePath"] = JsonValue::string(sourcePath);
	config["project"] = runConfigureScript(sourcePath);
	validateConfig(config);

	Directory::create(cppbuildDir);
	Directory::trySetHidden(cppbuildDir);
	File::writeAllText(FilePath::combine(cppbuildDir, "config.json"), config.to_json());

	generateWorkspace();
}

void CPPBuild::validateConfig(const JsonValue& config)
{
	std::string name = config["project"]["name"].to_string();
	if (name.empty())
		throw std::runtime_error("No project name specified");

	if (config["project"]["configurations"].items().empty())
		throw std::runtime_error("No targets specified");

	if (config["project"]["targets"].items().empty())
		throw std::runtime_error("No targets specified");
}

JsonValue CPPBuild::runConfigureScript(const std::string& sourcePath)
{
	std::string scriptFilename = FilePath::combine(sourcePath, "CPPBuild.js");
	std::string configureScript = File::readAllText(scriptFilename);

	ScriptContext context;
	ScriptValue result = context.eval(configureScript, scriptFilename, JS_EVAL_TYPE_MODULE | JS_EVAL_FLAG_STRICT);

	if (result.isException())
	{
		ScriptValue exception = context.getException();
		throw std::runtime_error(exception.toString() + "\n" + exception.getExceptionStack());
	}

	return JsonValue::parse(context.generateConfiguration());
}

void CPPBuild::generateWorkspace()
{
	std::string cppbuildexe = "\"" + FilePath::combine(Directory::exePath(), "cppbuild.exe") + "\"";

	JsonValue config = JsonValue::parse(File::readAllText(FilePath::combine(cppbuildDir, "config.json")));

	std::string sourcePath = config["sourcePath"].to_string();
	std::string solutionName = config["project"]["name"].to_string();

	auto solution = std::make_unique<VSSolution>(solutionName, workDir);

	for (const JsonValue& configDef : config["project"]["configurations"].items())
		solution->configurations.push_back(std::make_unique<VSSolutionConfiguration>(configDef["name"].to_string(), configDef["platform"].to_string()));

	for (const JsonValue& targetDef : config["project"]["targets"].items())
	{
		std::vector<std::string> sourceFiles;
		std::vector<std::string> headerFiles;
		std::vector<std::string> extraFiles;
		std::vector<std::string> defines;
		std::vector<std::string> includes;

		for (const JsonValue& item : targetDef["files"].items())
		{
			std::string name = FilePath::combine(sourcePath, item.to_string());
			if (FilePath::hasExtension(name, "cpp") || FilePath::hasExtension(name, "cc") || FilePath::hasExtension(name, "c"))
			{
				sourceFiles.push_back(name);
			}
			else if (FilePath::hasExtension(name, "h") || FilePath::hasExtension(name, "hpp"))
			{
				headerFiles.push_back(name);
			}
			else
			{
				extraFiles.push_back(name);
			}
		}

		for (const JsonValue& item : targetDef["defines"].items())
			defines.push_back(item.to_string());

		for (const JsonValue& item : targetDef["includes"].items())
			includes.push_back(FilePath::combine(sourcePath, item.to_string()));

		std::string projectName = targetDef["name"].to_string();
		std::string outputExe = projectName + ".exe";

		auto project = std::make_unique<VSCppProject>(projectName, cppbuildDir);
		project->sourceFiles = sourceFiles;
		project->headerFiles = headerFiles;
		project->extraFiles = extraFiles;

		for (const JsonValue& configDef : config["project"]["configurations"].items())
		{
			std::string configName = configDef["name"].to_string();
			std::string platform = configDef["platform"].to_string();

			auto projConfig = std::make_unique<VSCppProjectConfiguration>(configName, platform);
			projConfig->general.configurationType = "Makefile";
			projConfig->general.nmakePreprocessorDefinitions = defines;
			projConfig->general.nmakeIncludeSearchPath = includes;
			projConfig->general.nmakeOutput = outputExe;
			projConfig->general.nmakeBuildCommandLine = cppbuildexe + " -workdir $(SolutionDir) build " + projectName + " " + configName;
			projConfig->general.nmakeCleanCommandLine = cppbuildexe + " -workdir $(SolutionDir) clean " + projectName + " " + configName;
			projConfig->general.nmakeReBuildCommandLine = cppbuildexe + " -workdir $(SolutionDir) rebuild " + projectName + " " + configName;
			project->configurations.push_back(std::move(projConfig));
		}

		solution->projects.push_back(std::move(project));
	}

	solution->generate();
}

void CPPBuild::build(std::string target, std::string configuration)
{
	WebBuild webTarget(workDir, target, configuration);
	webTarget.build();
}

void CPPBuild::clean(std::string target, std::string configuration)
{
	WebBuild webTarget(workDir, target, configuration);
	webTarget.clean();
}

void CPPBuild::rebuild(std::string target, std::string configuration)
{
	WebBuild webTarget(workDir, target, configuration);
	webTarget.rebuild();
}
