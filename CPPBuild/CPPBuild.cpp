
#include "Precomp.h"
#include "CPPBuild.h"
#include "VSGenerator.h"
#include "WebTarget.h"
#include "BuildSetup.h"
#include "Msi/MSIGenerator.h"
#include "Guid/Guid.h"
#include "IOData/Directory.h"
#include "IOData/FilePath.h"
#include "IOData/File.h"
#include "Javascript/ScriptContext.h"
#include "Json/JsonValue.h"
#include <iostream>
#include <chrono>

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

	BuildSetup setup = BuildSetup::fromJson(config);
	validateConfig(setup);

	Directory::create(cppbuildDir);
	Directory::trySetHidden(cppbuildDir);
	File::writeAllText(FilePath::combine(cppbuildDir, "config.json"), config.to_json());

	if (!setup.project.targets.empty())
		generateWorkspace();
}

void CPPBuild::updateMakefile()
{
	JsonValue config = JsonValue::parse(File::readAllText(FilePath::combine(cppbuildDir, "config.json")));
	configure(config["sourcePath"].to_string());
}

void CPPBuild::checkMakefile()
{
	bool needsUpdate = false;
	try
	{
		std::string configFilename = FilePath::combine(cppbuildDir, "config.json");
		int64_t makefileTime = File::getLastWriteTime(configFilename);
		BuildSetup setup = BuildSetup::fromJson(JsonValue::parse(File::readAllText(configFilename)));

		int64_t srcFileTime = File::getLastWriteTime(FilePath::combine(setup.sourcePath, "Configure.js"));
		if (makefileTime < srcFileTime)
		{
			needsUpdate = true;
		}

		for (const BuildTarget& target : setup.project.targets)
		{
			std::string sourcePath = FilePath::combine(setup.sourcePath, target.subdirectory);

			// To do: need to check all files that Configure.js included
			int64_t srcFileTime = File::getLastWriteTime(FilePath::combine(sourcePath, target.subdirectory + ".js"));
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

	// msbuild wants an output file that always gets updated, so we give it that.
	int64_t timestamp = std::chrono::system_clock::now().time_since_epoch().count();
	File::writeAllText(FilePath::combine(cppbuildDir, "Makefile.timestamp"), std::to_string(timestamp));
}

void CPPBuild::validateConfig(const BuildSetup& setup)
{
	if (setup.project.name.empty())
		throw std::runtime_error("No project name specified");

	if (!setup.project.installers.empty())
	{
		return;
	}
	if (!setup.project.targets.empty())
	{
		if (setup.project.configurations.empty())
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

	BuildSetup setup = BuildSetup::fromJson(JsonValue::parse(File::readAllText(FilePath::combine(cppbuildDir, "config.json"))));

	VSGuids guids = loadSolutionGuids();
	if (guids.solutionGuid.empty())
		guids.solutionGuid = Guid::makeGuid().toString();

	auto solution = std::make_unique<VSSolution>(setup.project.name, workDir, guids.solutionGuid);

	for (const BuildConfiguration& configDef : setup.project.configurations)
		solution->configurations.push_back(std::make_unique<VSSolutionConfiguration>(configDef.name, configDef.platform));

	// Create CPPBuildCheck utility project that runs the cppbuild update check
	{
		std::string projectName = "CPPBuildCheck";
		auto& guid = guids.projectGuids[projectName];
		if (guid.empty())
			guid = Guid::makeGuid().toString();

		std::string customBuildFile = FilePath::combine(setup.sourcePath, "Configure.js");
		//if (filter)
		//	filter->customBuildFile = customBuildFile;

		auto project = std::make_unique<VSCppProject>(projectName, cppbuildDir, guids.projectGuids[projectName]);
		project->customBuildFile.file = customBuildFile;

		for (const BuildConfiguration& configDef : setup.project.configurations)
		{
			auto projConfig = std::make_unique<VSCppProjectConfiguration>(configDef.name, configDef.platform);
			projConfig->general.configurationType = "Utility";

			for (const BuildTarget& target : setup.project.targets)
			{
				std::string sourcePath = FilePath::combine(setup.sourcePath, target.subdirectory);
				projConfig->customBuildFile.additionalInputs.push_back(FilePath::combine(sourcePath, target.subdirectory + ".js"));
			}

			projConfig->customBuildFile.command = cppbuildexe + " -workdir $(SolutionDir) check-makefile";
			projConfig->customBuildFile.outputs.push_back(FilePath::combine(cppbuildDir, "Makefile.timestamp"));

			project->configurations.push_back(std::move(projConfig));
		}

		solution->projects.push_back(std::move(project));
	}

	for (const BuildTarget& targetDef : setup.project.targets)
	{
		auto& guid = guids.projectGuids[targetDef.name];
		if (guid.empty())
			guid = Guid::makeGuid().toString();
	}

	for (const BuildTarget& targetDef : setup.project.targets)
	{
		std::string projectType = targetDef.type;
		std::string projectName = targetDef.name;
		std::string sourcePath = FilePath::combine(setup.sourcePath, targetDef.subdirectory);

		bool isMakefileProject = projectType == "website" || projectType == "webcomponent" || projectType == "weblibrary";
		std::vector<std::string> sourceFiles;
		std::vector<std::string> headerFiles;
		std::vector<std::string> extraFiles;
		std::vector<std::string> defines;
		std::vector<std::string> includes;
		std::vector<std::string> libraryPaths;
		std::vector<std::string> dependencies;

		std::map<std::string, std::unique_ptr<VSCppProjectFilter>> filters;

		for (const std::string& item : targetDef.files)
		{
			VSCppProjectFilter* filter = nullptr;
			std::string folder = FilePath::forceBackslash(FilePath::removeLastComponent(item));
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

			std::string name = FilePath::combine(sourcePath, item);
			if (FilePath::hasExtension(name, "cpp") || FilePath::hasExtension(name, "cc") || FilePath::hasExtension(name, "c"))
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

		defines = targetDef.defines;

		for (const std::string& item : targetDef.includePaths)
			includes.push_back(FilePath::combine(sourcePath, item));

		for (const std::string& item : targetDef.libraryPaths)
			libraryPaths.push_back(FilePath::combine(sourcePath, item));

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

		for (auto& it : filters)
			project->filters.push_back(std::move(it.second));

		project->references.emplace_back("CPPBuildCheck", cppbuildDir, guids.projectGuids["CPPBuildCheck"]);

		for (const std::string& libName : targetDef.linkLibraries)
		{
			auto it = guids.projectGuids.find(libName);
			if (it != guids.projectGuids.end())
			{
				project->references.emplace_back(libName, cppbuildDir, it->second);
			}
			else
			{
				// External lib reference.
				if (FilePath::extension(libName).empty())
					dependencies.push_back(libName + ".lib");
				else
					dependencies.push_back(libName);
			}
		}

		for (const BuildConfiguration& configDef : setup.project.configurations)
		{
			std::string configName = configDef.name;

			std::vector<std::string> configDefines = defines;
			std::vector<std::string> configIncludes = includes;
			std::vector<std::string> configLibraryPaths = libraryPaths;
			std::vector<std::string> configDependencies = dependencies;

			auto itTargetConfig = targetDef.configurations.find(configName);
			if (itTargetConfig != targetDef.configurations.end())
			{
				const BuildTargetConfiguration& targetConfigDef = itTargetConfig->second;

				configDefines.insert(configDefines.end(), targetConfigDef.defines.begin(), targetConfigDef.defines.end());

				for (const std::string& item : targetConfigDef.includePaths)
					configIncludes.push_back(FilePath::combine(sourcePath, item));

				for (const std::string& item : targetConfigDef.libraryPaths)
					configLibraryPaths.push_back(FilePath::combine(sourcePath, item));

				for (const std::string& libName : targetConfigDef.linkLibraries)
				{
					auto it = guids.projectGuids.find(libName);
					if (it != guids.projectGuids.end())
					{
						throw std::runtime_error("Project link references must apply for all configurations");
					}
					else
					{
						// External lib reference.
						if (FilePath::extension(libName).empty())
							configDependencies.push_back(libName + ".lib");
						else
							configDependencies.push_back(libName);
					}
				}
			}

			auto projConfig = std::make_unique<VSCppProjectConfiguration>(configName, configDef.platform);
			if (isMakefileProject)
			{
				// Emscripten based projects always wants the emscripten headers for intellisense
				if (projectType == "website" || projectType == "webcomponent" || projectType == "weblibrary")
				{
					configIncludes.push_back("$(EMSDK)\\upstream\\emscripten\\system\\include");
					configDefines.push_back("__EMSCRIPTEN__");
					configDefines.push_back("INTELLISENSE");
				}

				projConfig->general.configurationType = "Makefile";
				projConfig->general.nmakePreprocessorDefinitions = configDefines;
				projConfig->general.nmakeIncludeSearchPath = configIncludes;
				projConfig->general.nmakeOutput = outputFile;
				projConfig->general.nmakeBuildCommandLine = cppbuildexe + " -workdir $(SolutionDir) build " + projectName + " " + configName;
				projConfig->general.nmakeCleanCommandLine = cppbuildexe + " -workdir $(SolutionDir) clean " + projectName + " " + configName;
				projConfig->general.nmakeReBuildCommandLine = cppbuildexe + " -workdir $(SolutionDir) rebuild " + projectName + " " + configName;
			}
			else if(projectType == "application")
			{
				projConfig->general.configurationType = "Application";
				projConfig->clCompile.preprocessorDefinitions = configDefines;
				projConfig->clCompile.additionalIncludeDirectories = configIncludes;
				projConfig->link.subSystem = "Windows";
				projConfig->link.additionalLibraryDirectories = configLibraryPaths;
				projConfig->link.additionalDependencies = configDependencies;
			}
			else if (projectType == "console")
			{
				projConfig->general.configurationType = "Application";
				projConfig->clCompile.preprocessorDefinitions = configDefines;
				projConfig->clCompile.additionalIncludeDirectories = configIncludes;
				projConfig->link.subSystem = "Console";
				projConfig->link.additionalLibraryDirectories = configLibraryPaths;
				projConfig->link.additionalDependencies = configDependencies;
			}
			else if (projectType == "lib")
			{
				projConfig->general.configurationType = "StaticLibrary";
				projConfig->clCompile.preprocessorDefinitions = configDefines;
				projConfig->clCompile.additionalIncludeDirectories = configIncludes;
				projConfig->link.subSystem = "Windows";
				projConfig->link.additionalLibraryDirectories = configLibraryPaths;
				projConfig->link.additionalDependencies = configDependencies;
			}
			else if (projectType == "dll")
			{
				projConfig->general.configurationType = "DynamicLibrary";
				projConfig->clCompile.preprocessorDefinitions = configDefines;
				projConfig->clCompile.additionalIncludeDirectories = configIncludes;
				projConfig->link.subSystem = "Windows";
				projConfig->link.additionalLibraryDirectories = configLibraryPaths;
				projConfig->link.additionalDependencies = configDependencies;
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
	checkMakefile();
	WebTarget webTarget(workDir, target, configuration);
	webTarget.build();
}

void CPPBuild::clean(std::string target, std::string configuration)
{
	checkMakefile();
	WebTarget webTarget(workDir, target, configuration);
	webTarget.clean();
}

void CPPBuild::rebuild(std::string target, std::string configuration)
{
	checkMakefile();
	WebTarget webTarget(workDir, target, configuration);
	webTarget.rebuild();
}

void CPPBuild::createInstaller()
{
	BuildSetup setup = BuildSetup::fromJson(JsonValue::parse(File::readAllText(FilePath::combine(cppbuildDir, "config.json"))));

	std::string binDir = FilePath::combine(workDir, { "Build", "Installer" });
	Directory::create(binDir);

	auto msi = MSIGenerator::create();
	for (const BuildInstaller& def : setup.project.installers)
	{
		std::string sourcePath = FilePath::combine(setup.sourcePath, def.subdirectory);
		msi->generate(binDir, sourcePath, def);
	}
}
