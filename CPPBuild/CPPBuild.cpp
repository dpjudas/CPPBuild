
#include "Precomp.h"
#include "CPPBuild.h"
#include "VSGenerator.h"
#include "WebTarget.h"
#include "Guid/Guid.h"
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

	ScriptContext context(sourcePath);
	ScriptValue result = context.eval(configureScript, scriptFilename, JS_EVAL_TYPE_MODULE | JS_EVAL_FLAG_STRICT);

	if (result.isException())
	{
		ScriptValue exception = context.getException();
		throw std::runtime_error(exception.toString() + "\n" + exception.getExceptionStack());
	}

	return JsonValue::parse(context.generateConfiguration());
}

VSGuids CPPBuild::loadSolutionGuids()
{
	VSGuids guids;
	try
	{
		JsonValue file = JsonValue::parse(File::readAllText(FilePath::combine(cppbuildDir, "vsguids.json")));
		guids.solutionGuid = file["solutionGuid"].to_string();
		for (JsonValue& item : file["projectGuids"].items())
		{
			std::string name = item["name"].to_string();
			std::string guid = item["guid"].to_string();
			guids.projectGuids[name] = guid;
		}
	}
	catch (...)
	{
	}
	return guids;
}

void CPPBuild::saveSolutionGuids(const VSGuids& guids)
{
	auto projectGuids = JsonValue::array();
	for (auto& it : guids.projectGuids)
	{
		auto item = JsonValue::object();
		item["name"] = JsonValue::string(it.first);
		item["guid"] = JsonValue::string(it.second);
		projectGuids.items().push_back(std::move(item));
	}

	auto file = JsonValue::object();
	file["solutionGuid"] = JsonValue::string(guids.solutionGuid);
	file["projectGuids"] = std::move(projectGuids);
	File::writeAllText(FilePath::combine(cppbuildDir, "vsguids.json"), file.to_json());
}

void CPPBuild::generateWorkspace()
{
	std::string cppbuildexe = "\"" + FilePath::combine(Directory::exePath(), "cppbuild.exe") + "\"";

	JsonValue config = JsonValue::parse(File::readAllText(FilePath::combine(cppbuildDir, "config.json")));

	std::string solutionName = config["project"]["name"].to_string();

	VSGuids guids = loadSolutionGuids();
	if (guids.solutionGuid.empty())
		guids.solutionGuid = Guid::makeGuid().toString();

	auto solution = std::make_unique<VSSolution>(solutionName, workDir, guids.solutionGuid);

	for (const JsonValue& configDef : config["project"]["configurations"].items())
		solution->configurations.push_back(std::make_unique<VSSolutionConfiguration>(configDef["name"].to_string(), configDef["platform"].to_string()));

	for (const JsonValue& targetDef : config["project"]["targets"].items())
	{
		std::string sourcePath = FilePath::combine(config["sourcePath"].to_string(), targetDef["subdirectory"].to_string());

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

		for (const JsonValue& item : targetDef["includePaths"].items())
			includes.push_back(FilePath::combine(sourcePath, item.to_string()));

		std::string projectType = targetDef["type"].to_string();
		std::string projectName = targetDef["name"].to_string();

		std::string outputFile;
		if (projectType == "website" || projectType == "webcomponent" || projectType == "weblibrary")
		{
			// These project types can't be launched by visual studio.
		}
		else if (projectType == "application" || projectType == "console")
		{
			outputFile = projectName + ".exe";
		}
		else if (projectType == "lib")
		{
			outputFile = projectName + ".lib";
		}
		else if (projectType == "dll")
		{
			outputFile = projectName + ".dll";
		}

		auto& guid = guids.projectGuids[projectName];
		if (guid.empty())
			guid = Guid::makeGuid().toString();

		auto project = std::make_unique<VSCppProject>(projectName, cppbuildDir, guid);
		project->sourceFiles = sourceFiles;
		project->headerFiles = headerFiles;
		project->extraFiles = extraFiles;

		for (const JsonValue& configDef : config["project"]["configurations"].items())
		{
			std::string configName = configDef["name"].to_string();
			std::string platform = configDef["platform"].to_string();

			auto projConfig = std::make_unique<VSCppProjectConfiguration>(configName, platform);
			if (projectType == "website" || projectType == "webcomponent" || projectType == "weblibrary")
			{
				projConfig->general.configurationType = "Makefile";
				projConfig->general.nmakePreprocessorDefinitions = defines;
				projConfig->general.nmakeIncludeSearchPath = includes;
				projConfig->general.nmakeOutput = outputFile;
				projConfig->general.nmakeBuildCommandLine = cppbuildexe + " -workdir $(SolutionDir) build " + projectName + " " + configName;
				projConfig->general.nmakeCleanCommandLine = cppbuildexe + " -workdir $(SolutionDir) clean " + projectName + " " + configName;
				projConfig->general.nmakeReBuildCommandLine = cppbuildexe + " -workdir $(SolutionDir) rebuild " + projectName + " " + configName;
			}
			else if(projectType == "application")
			{
				projConfig->general.configurationType = "Application";
				projConfig->clCompile.preprocessorDefinitions = defines;
				projConfig->clCompile.additionalIncludeDirectories = includes;
				projConfig->link.subSystem = "Windows";
			}
			else if (projectType == "console")
			{
				projConfig->general.configurationType = "Application";
				projConfig->clCompile.preprocessorDefinitions = defines;
				projConfig->clCompile.additionalIncludeDirectories = includes;
				projConfig->link.subSystem = "Console";
			}
			else if (projectType == "lib")
			{
				projConfig->general.configurationType = "StaticLibrary";
				projConfig->clCompile.preprocessorDefinitions = defines;
				projConfig->clCompile.additionalIncludeDirectories = includes;
				projConfig->link.subSystem = "Windows";
			}
			else if (projectType == "dll")
			{
				projConfig->general.configurationType = "DynamicLibrary";
				projConfig->clCompile.preprocessorDefinitions = defines;
				projConfig->clCompile.additionalIncludeDirectories = includes;
				projConfig->link.subSystem = "Windows";
			}

			if (configName == "Debug")
			{
				projConfig->general.useDebugLibraries = "true";
				projConfig->general.linkIncremental = "true";
				projConfig->general.wholeProgramOptimization = "false";
				projConfig->clCompile.runtimeLibrary = "MultiThreadedDebug";
				projConfig->clCompile.intrinsicFunctions = "false";
			}
			else
			{
				projConfig->general.useDebugLibraries = "false";
				projConfig->general.wholeProgramOptimization = "true";
				projConfig->clCompile.runtimeLibrary = "MultiThreaded";
				projConfig->clCompile.intrinsicFunctions = "true";
			}

			project->configurations.push_back(std::move(projConfig));
		}

		solution->projects.push_back(std::move(project));
	}

	solution->generate();
	saveSolutionGuids(guids);
}

void CPPBuild::build(std::string target, std::string configuration)
{
	WebTarget webTarget(workDir, target, configuration);
	webTarget.build();
}

void CPPBuild::clean(std::string target, std::string configuration)
{
	WebTarget webTarget(workDir, target, configuration);
	webTarget.clean();
}

void CPPBuild::rebuild(std::string target, std::string configuration)
{
	WebTarget webTarget(workDir, target, configuration);
	webTarget.rebuild();
}
