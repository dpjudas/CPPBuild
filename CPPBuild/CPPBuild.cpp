
#include "Precomp.h"
#include "CPPBuild.h"
#include "VSGenerator.h"
#include "WebTarget.h"
#include "Msi/MSIGenerator.h"
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

	if (!config["project"]["targets"].items().empty())
		generateWorkspace();
}

void CPPBuild::updateMakefile()
{
	JsonValue config = JsonValue::parse(File::readAllText(FilePath::combine(cppbuildDir, "config.json")));
	configure(config["sourcePath"].to_string());
}

void CPPBuild::checkMakefile(std::string target, std::string configuration)
{
	bool needsUpdate = false;
	try
	{
		int64_t makefileTime = File::getLastWriteTime(FilePath::combine(cppbuildDir, "config.json"));
		JsonValue config = JsonValue::parse(File::readAllText(FilePath::combine(cppbuildDir, "config.json")));

		for (const JsonValue& targetDef : config["project"]["targets"].items())
		{
			std::string projectType = targetDef["type"].to_string();
			std::string projectName = targetDef["name"].to_string();
			std::string sourcePath = FilePath::combine(config["sourcePath"].to_string(), targetDef["subdirectory"].to_string());

			// To do: need to check all files that Configure.js included
			int64_t srcFileTime = File::getLastWriteTime(FilePath::combine(sourcePath, "Configure.js"));
			if (makefileTime < srcFileTime)
			{
				needsUpdate = true;
				break;
			}
		}
	}
	catch (...)
	{
		needsUpdate = true;
	}

	if (needsUpdate)
		updateMakefile();
}

void CPPBuild::validateConfig(const JsonValue& config)
{
	std::string name = config["project"]["name"].to_string();
	if (name.empty())
		throw std::runtime_error("No project name specified");

	if (!config["project"]["installers"].items().empty())
	{
		return;
	}
	if (!config["project"]["targets"].items().empty())
	{
		if (config["project"]["configurations"].items().empty())
			throw std::runtime_error("No configurations specified");
		return;
	}

	throw std::runtime_error("No targets or installers specified");
}

JsonValue CPPBuild::runConfigureScript(const std::string& sourcePath)
{
	std::string scriptFilename = FilePath::combine(sourcePath, "Configure.js");
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
		std::string projectName = targetDef["name"].to_string();
		auto& guid = guids.projectGuids[projectName];
		if (guid.empty())
			guid = Guid::makeGuid().toString();
	}

	for (const JsonValue& targetDef : config["project"]["targets"].items())
	{
		std::string projectType = targetDef["type"].to_string();
		std::string projectName = targetDef["name"].to_string();
		std::string sourcePath = FilePath::combine(config["sourcePath"].to_string(), targetDef["subdirectory"].to_string());

		bool isMakefileProject = projectType == "website" || projectType == "webcomponent" || projectType == "weblibrary";
		std::string customBuildFile;
		std::vector<std::string> sourceFiles;
		std::vector<std::string> headerFiles;
		std::vector<std::string> extraFiles;
		std::vector<std::string> defines;
		std::vector<std::string> includes;

		std::map<std::string, std::unique_ptr<VSCppProjectFilter>> filters;

		for (const JsonValue& item : targetDef["files"].items())
		{
			VSCppProjectFilter* filter = nullptr;
			std::string folder = FilePath::forceBackslash(FilePath::removeLastComponent(item.to_string()));
			if (!folder.empty())
			{
				auto& item = filters[folder];
				if (!item)
					item = std::make_unique<VSCppProjectFilter>(folder, Guid::makeGuid().toString());
				filter = item.get();

				std::string parent = folder;
				while (true)
				{
					parent = FilePath::removeLastComponent(parent);
					if (parent.empty())
						break;
					auto& parentitem = filters[parent];
					if (!parentitem)
						parentitem = std::make_unique<VSCppProjectFilter>(parent, Guid::makeGuid().toString());
				}
			}

			std::string name = FilePath::combine(sourcePath, item.to_string());
			if (!isMakefileProject && customBuildFile.empty() && FilePath::lastComponent(name) == "Configure.js")
			{
				customBuildFile = name;
				if (filter)
					filter->customBuildFile = name;
			}
			else if (FilePath::hasExtension(name, "cpp") || FilePath::hasExtension(name, "cc") || FilePath::hasExtension(name, "c"))
			{
				sourceFiles.push_back(name);
				if (filter)
					filter->sourceFiles.push_back(name);
			}
			else if (FilePath::hasExtension(name, "h") || FilePath::hasExtension(name, "hpp"))
			{
				headerFiles.push_back(name);
				if (filter)
					filter->headerFiles.push_back(name);
			}
			else
			{
				extraFiles.push_back(name);
				if (filter)
					filter->extraFiles.push_back(name);
			}
		}

		for (const JsonValue& item : targetDef["defines"].items())
			defines.push_back(item.to_string());

		for (const JsonValue& item : targetDef["includePaths"].items())
			includes.push_back(FilePath::combine(sourcePath, item.to_string()));

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

		auto project = std::make_unique<VSCppProject>(projectName, cppbuildDir, guids.projectGuids[projectName]);
		project->sourceFiles = sourceFiles;
		project->headerFiles = headerFiles;
		project->extraFiles = extraFiles;
		project->customBuildFile.file = customBuildFile;

		for (auto& it : filters)
			project->filters.push_back(std::move(it.second));

		for (const JsonValue& item : targetDef["linkLibraries"].items())
		{
			std::string libName = item.to_string();
			auto it = guids.projectGuids.find(libName);
			if (it != guids.projectGuids.end())
			{
				project->references.emplace_back(libName, cppbuildDir, it->second);
			}
			else
			{
				// External lib reference.
				// To do: add this to linker flags
			}
		}

		for (const JsonValue& configDef : config["project"]["configurations"].items())
		{
			std::string configName = configDef["name"].to_string();
			std::string platform = configDef["platform"].to_string();

			auto projConfig = std::make_unique<VSCppProjectConfiguration>(configName, platform);
			if (isMakefileProject)
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

			if (!project->customBuildFile.file.empty())
			{
				// To do: fill customBuildFile.additionalInputs with everything included by javascript
				projConfig->customBuildFile.command = cppbuildexe + " -workdir $(SolutionDir) update-makefile";
				projConfig->customBuildFile.outputs.push_back(FilePath::combine(cppbuildDir, "config.json"));
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
	checkMakefile(target, configuration);
	WebTarget webTarget(workDir, target, configuration);
	webTarget.build();
}

void CPPBuild::clean(std::string target, std::string configuration)
{
	checkMakefile(target, configuration);
	WebTarget webTarget(workDir, target, configuration);
	webTarget.clean();
}

void CPPBuild::rebuild(std::string target, std::string configuration)
{
	checkMakefile(target, configuration);
	WebTarget webTarget(workDir, target, configuration);
	webTarget.rebuild();
}

void CPPBuild::createInstaller()
{
	JsonValue config = JsonValue::parse(File::readAllText(FilePath::combine(cppbuildDir, "config.json")));

	std::string binDir = FilePath::combine(workDir, { "Build", "Installer" });
	Directory::create(binDir);

	auto msi = MSIGenerator::create();
	for (const JsonValue& defJson : config["project"]["installers"].items())
	{
		std::string sourcePath = FilePath::combine(config["sourcePath"].to_string(), defJson["subdirectory"].to_string());

		InstallerDefinition def;
		def.name = defJson["name"].to_string();
		def.installDir = defJson["installDir"].to_string();
		def.msiProductName = defJson["msiProductName"].to_string();
		def.msiProductVersion = defJson["msiProductVersion"].to_string();
		def.msiManufacturer = defJson["msiManufacturer"].to_string();
		def.msiProductCode = defJson["msiProductCode"].to_string();
		def.msiUpgradeCode = defJson["msiUpgradeCode"].to_string();
		def.msiPackageCode = defJson["msiPackageCode"].to_string();

		for (const JsonValue& keywordJson : defJson["msiProductKeywords"].items())
		{
			def.msiProductKeywords.push_back(keywordJson.to_string());
		}

		for (const JsonValue& defComponentJson : defJson["components"].items())
		{
			InstallerComponent component;
			component.name = defComponentJson["name"].to_string();
			component.msiComponentId = defComponentJson["msiComponentId"].to_string();
			for (const JsonValue& file : defComponentJson["files"].items())
				component.files.push_back(file.to_string());
			def.components.push_back(std::move(component));
		}

		for (const JsonValue& defFeatureJson : defJson["features"].items())
		{
			InstallerFeature feature;
			feature.name = defFeatureJson["name"].to_string();
			for (const JsonValue& file : defFeatureJson["components"].items())
				feature.components.push_back(file.to_string());
			def.features.push_back(std::move(feature));
		}

		msi->generate(binDir, sourcePath, def);
	}
}
