
#include "Precomp.h"
#include "VSWorkspace.h"
#include "VSGenerator.h"
#include "BuildSetup.h"
#include "Guid/Guid.h"
#include "IOData/Directory.h"
#include "IOData/FilePath.h"
#include "IOData/File.h"

void VSWorkspace::generate(const BuildSetup& setup, const std::string& workDir, const std::string& cppbuildDir)
{
	std::string cppbuildexe = "\"" + FilePath::combine(Directory::exePath(), "cppbuild.exe") + "\"";

	VSGuids guids = loadSolutionGuids(cppbuildDir);
	if (guids.solutionGuid.empty())
		guids.solutionGuid = Guid::makeGuid().toString();

	auto solution = std::make_unique<VSSolution>(setup.project.name, workDir, guids.solutionGuid);

#if defined(_M_X64) || defined(__x86_64__)
	std::string platform = "x64";
#elif defined(_M_ARM64) || defined(__aarch64__)
	std::string platform = "arm64";
#elif defined(_M_IX86) || defined(__i386__)
	std::string platform = "win32";
#elif defined(_M_ARM) || defined(__arm__)
	std::string platform = "arm32";
#endif

	for (const BuildConfiguration& configDef : setup.project.configurations)
		solution->configurations.push_back(std::make_unique<VSSolutionConfiguration>(configDef.name, platform));

	// Create CPPBuildCheck utility project that runs the cppbuild update check
	{
		std::string projectName = "CPPBuildCheck";
		auto& guid = guids.projectGuids[projectName];
		if (guid.empty())
			guid = Guid::makeGuid().toString();

		auto project = std::make_unique<VSCppProject>(projectName, cppbuildDir, guids.projectGuids[projectName]);

		for (const BuildConfiguration& configDef : setup.project.configurations)
		{
			auto projConfig = std::make_unique<VSCppProjectConfiguration>(configDef.name, platform);
			projConfig->general.configurationType = "Utility";
			project->configurations.push_back(std::move(projConfig));
		}

		VSFile<VSCustomBuildTask> customBuildFile;
		customBuildFile.file = FilePath::combine(setup.sourcePath, "Configure.js");
		for (const BuildConfiguration& configDef : setup.project.configurations)
		{
			auto props = customBuildFile.getTask(configDef.name, platform);
			props->message = "Running CPPBuild generate";
			props->useUtf8Encoding = "Always";
			props->linkObjects = "false";
			props->command = cppbuildexe + " -workdir $(SolutionDir) check-makefile";
			props->outputs.push_back(FilePath::combine(cppbuildDir, "Makefile.timestamp"));

			for (const BuildTarget& target : setup.project.targets)
			{
				std::string sourcePath = FilePath::combine(setup.sourcePath, target.subdirectory);
				std::string scriptFile = FilePath::lastComponent(target.subdirectory) + ".js";
				props->additionalInputs.push_back(FilePath::combine(sourcePath, scriptFile));
			}
			
			if (!props->additionalInputs.empty())
				props->additionalInputs.push_back("%(AdditionalInputs)");
		}
		project->customFiles.push_back(customBuildFile);
		//if (filter)
		//	filter->customFiles.push_back(customBuildFile.file);

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

		std::vector<VSFile<VSCompileTask>> sourceFiles;
		std::vector<VSFile<VSIncludeTask>> headerFiles;
		std::vector<VSFile<VSResourceTask>> resourceFiles;
		std::vector<VSFile<VSManifestTask>> manifestFiles;
		std::vector<VSFile<VSCustomBuildTask>> customFiles;
		std::vector<VSFile<VSNoneTask>> extraFiles;
		std::vector<std::string> defines;
		std::vector<std::string> cCompileOptions;
		std::vector<std::string> cxxCompileOptions;
		std::vector<std::string> linkOptions;
		std::vector<std::string> includePaths;
		std::vector<std::string> libraryPaths;
		std::vector<std::string> linkLibraries;
		std::vector<std::string> dependencies;

		std::map<std::string, std::unique_ptr<VSCppProjectFilter>> filters;

		std::unordered_map<std::string, std::vector<const BuildFilePropertySet*>> fileToSets;
		for (const BuildFilePropertySet& set : targetDef.filePropertySets)
		{
			for (const std::string& filename : set.files)
				fileToSets[filename].push_back(&set);
		}

		std::unordered_map<std::string, std::vector<const BuildCustomCommand*>> fileToCustomCmd;
		for (const BuildCustomCommand& cmd : targetDef.customCommands)
		{
			fileToCustomCmd[cmd.inputFile].push_back(&cmd);
		}

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

			auto itCustomCmd = fileToCustomCmd.find(item);
			if (itCustomCmd != fileToCustomCmd.end())
			{
				VSFile<VSCustomBuildTask> file = name;

				// Add commands targeting all configs
				for (const BuildCustomCommand* cmd : itCustomCmd->second)
				{
					if (cmd->configName.empty())
					{
						for (const BuildConfiguration& configDef : setup.project.configurations)
						{
							VSCustomBuildTask* task = file.getTask(configDef.name, platform);
							task->useUtf8Encoding = "true";
							task->linkObjects = "false";
							for (const std::string& cmdline : cmd->commands)
							{
								if (!task->command.empty())
									task->command += "\r\n";
								task->command += addPathToCommand(cmdline, configDef.name, setup, workDir);
							}
							task->outputs = cmd->outputFiles;
						}
					}
				}

				// Add commands for specific configs
				for (const BuildCustomCommand* cmd : itCustomCmd->second)
				{
					if (!cmd->configName.empty())
					{
						VSCustomBuildTask* task = file.getTask(cmd->configName, platform);
						task->useUtf8Encoding = "true";
						task->linkObjects = "false";
						for (const std::string& cmdline : cmd->commands)
						{
							if (!task->command.empty())
								task->command += "\r\n";
							task->command += addPathToCommand(cmdline, cmd->configName, setup, workDir);
						}
						task->outputs = cmd->outputFiles;
					}
				}

				customFiles.push_back(std::move(file));
				if (filter)
					filter->customFiles.push_back(name);
			}
			else if (FilePath::hasExtension(name, "cpp") || FilePath::hasExtension(name, "cc") || FilePath::hasExtension(name, "c"))
			{
				VSFile<VSCompileTask> file = name;
				for (const BuildFilePropertySet* set : fileToSets[item])
				{
					VSCompileTask* task = file.getTask(set->configName, platform);
					task->applyCompileOptions(set->compileOptions);
					task->preprocessorDefinitions = set->defines;
					task->additionalIncludeDirectories = set->includePaths;
				}
				sourceFiles.push_back(std::move(file));
				if (filter)
					filter->sourceFiles.push_back(name);
			}
			else if (FilePath::hasExtension(name, "h") || FilePath::hasExtension(name, "hpp"))
			{
				VSFile<VSIncludeTask> file = name;
				headerFiles.push_back(std::move(file));
				if (filter)
					filter->headerFiles.push_back(name);
			}
			else if (FilePath::hasExtension(name, "rc"))
			{
				VSFile<VSResourceTask> file = name;
				for (const BuildFilePropertySet* set : fileToSets[item])
				{
					VSResourceTask* task = file.getTask(set->configName, platform);
					task->preprocessorDefinitions = set->defines;
					task->additionalIncludeDirectories = set->includePaths;
				}
				resourceFiles.push_back(std::move(file));
				if (filter)
					filter->resourceFiles.push_back(name);
			}
			else if (FilePath::hasExtension(name, "manifest"))
			{
				VSFile<VSManifestTask> file = name;
				manifestFiles.push_back(std::move(file));
				if (filter)
					filter->manifestFiles.push_back(name);
			}
			else
			{
				VSFile<VSNoneTask> file = name;
				extraFiles.push_back(std::move(file));
				if (filter)
					filter->extraFiles.push_back(name);
			}
		}

		for (const std::string pkgName : targetDef.packages)
		{
			const BuildPackage& package = setup.project.getPackage(pkgName);
			std::string pkgBasePath = FilePath::combine(setup.sourcePath, package.subdirectory);

			for (const std::string& define : package.defines)
				defines.push_back(define);

			for (const std::string& opt : package.cCompileOptions)
				cCompileOptions.push_back(opt);

			for (const std::string& opt : package.cxxCompileOptions)
				cxxCompileOptions.push_back(opt);

			for (const std::string& opt : package.linkOptions)
				linkOptions.push_back(opt);

			for (const std::string& linkLibrary : package.linkLibraries)
				linkLibraries.push_back(linkLibrary);

			for (const std::string& source : package.sources)
			{
				std::string pkgSourcePath = FilePath::combine(pkgBasePath, source);

				for (const std::string& path : package.includePaths)
					includePaths.push_back(FilePath::combine(pkgSourcePath, path));

				for (const std::string& item : package.libraryPaths)
					libraryPaths.push_back(FilePath::combine(pkgSourcePath, item));
			}
		}

		for (const std::string& define : targetDef.defines)
			defines.push_back(define);

		for (const std::string& opt : targetDef.cCompileOptions)
			cCompileOptions.push_back(opt);

		for (const std::string& opt : targetDef.cxxCompileOptions)
			cxxCompileOptions.push_back(opt);

		for (const std::string& opt : targetDef.linkOptions)
			linkOptions.push_back(opt);

		for (const std::string& linkLibrary : targetDef.linkLibraries)
			linkLibraries.push_back(linkLibrary);

		for (const std::string& item : targetDef.includePaths)
			includePaths.push_back(FilePath::combine(sourcePath, item));

		for (const std::string& item : targetDef.libraryPaths)
			libraryPaths.push_back(FilePath::combine(sourcePath, item));

		std::string outputFile;
		if (projectType == "website" || projectType == "webcomponent" || projectType == "weblibrary" || projectType == "custom" || projectType == "utility")
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
		project->resourceFiles = resourceFiles;
		project->manifestFiles = manifestFiles;
		project->extraFiles = extraFiles;
		project->customFiles = customFiles;

		for (auto& it : filters)
			project->filters.push_back(std::move(it.second));

		project->references.emplace_back("CPPBuildCheck", cppbuildDir, guids.projectGuids["CPPBuildCheck"]);

		for (const std::string& libName : linkLibraries)
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
			std::vector<std::string> configCCompileOptions = cCompileOptions;
			std::vector<std::string> configCxxCompileOptions = cxxCompileOptions;
			std::vector<std::string> configLinkOptions = linkOptions;
			std::vector<std::string> configIncludes = includePaths;
			std::vector<std::string> configLibraryPaths = libraryPaths;
			std::vector<std::string> configDependencies = dependencies;
			std::vector<std::string> configLinkLibraries;

			for (const std::string pkgName : targetDef.packages)
			{
				const BuildPackage& package = setup.project.getPackage(pkgName);
				std::string pkgBasePath = FilePath::combine(setup.sourcePath, package.subdirectory);

				auto itPackageConfig = package.configurations.find(configName);
				if (itPackageConfig != package.configurations.end())
				{
					const BuildPackageConfiguration& packageConfigDef = itPackageConfig->second;

					for (const std::string& define : packageConfigDef.defines)
						configDefines.push_back(define);

					for (const std::string& opt : packageConfigDef.cCompileOptions)
						configCCompileOptions.push_back(opt);

					for (const std::string& opt : packageConfigDef.cxxCompileOptions)
						configCxxCompileOptions.push_back(opt);

					for (const std::string& opt : packageConfigDef.linkOptions)
						configLinkOptions.push_back(opt);

					for (const std::string& linkLibrary : packageConfigDef.linkLibraries)
						configLinkLibraries.push_back(linkLibrary);

					for (const std::string& source : package.sources)
					{
						std::string pkgSourcePath = FilePath::combine(pkgBasePath, source);

						for (const std::string& item : packageConfigDef.includePaths)
							configIncludes.push_back(FilePath::combine(pkgSourcePath, item));

						for (const std::string& item : packageConfigDef.libraryPaths)
							configLibraryPaths.push_back(FilePath::combine(pkgSourcePath, item));
					}
				}
			}

			auto itTargetConfig = targetDef.configurations.find(configName);
			if (itTargetConfig != targetDef.configurations.end())
			{
				const BuildTargetConfiguration& targetConfigDef = itTargetConfig->second;

				for (const std::string& define : targetConfigDef.defines)
					configDefines.push_back(define);

				for (const std::string& opt : targetConfigDef.cCompileOptions)
					configCCompileOptions.push_back(opt);

				for (const std::string& opt : targetConfigDef.cxxCompileOptions)
					configCxxCompileOptions.push_back(opt);

				for (const std::string& opt : targetConfigDef.linkOptions)
					configLinkOptions.push_back(opt);

				for (const std::string& item : targetConfigDef.includePaths)
					configIncludes.push_back(FilePath::combine(sourcePath, item));

				for (const std::string& item : targetConfigDef.libraryPaths)
					configLibraryPaths.push_back(FilePath::combine(sourcePath, item));

				for (const std::string& libName : targetConfigDef.linkLibraries)
					configLinkLibraries.push_back(libName);
			}

			for (const std::string& libName : configLinkLibraries)
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

			auto projConfig = std::make_unique<VSCppProjectConfiguration>(configName, platform);

			if (projectType == "website" || projectType == "webcomponent" || projectType == "weblibrary")
			{
				// Emscripten based projects always wants the emscripten headers for intellisense
				configIncludes.push_back("$(EMSDK)\\upstream\\emscripten\\system\\include");
				configDefines.push_back("__EMSCRIPTEN__");
				configDefines.push_back("INTELLISENSE");

				projConfig->general.configurationType = "Makefile";
				projConfig->general.nmakePreprocessorDefinitions = configDefines;
				projConfig->general.nmakeIncludeSearchPath = configIncludes;
				projConfig->general.nmakeOutput = outputFile;
				projConfig->general.nmakeBuildCommandLine = cppbuildexe + " -workdir $(SolutionDir) build " + projectName + " " + configName;
				projConfig->general.nmakeCleanCommandLine = cppbuildexe + " -workdir $(SolutionDir) clean " + projectName + " " + configName;
				projConfig->general.nmakeReBuildCommandLine = cppbuildexe + " -workdir $(SolutionDir) rebuild " + projectName + " " + configName;
			}
			else if (projectType == "custom")
			{
				projConfig->general.configurationType = "Makefile";
				projConfig->general.nmakePreprocessorDefinitions = configDefines;
				projConfig->general.nmakeIncludeSearchPath = configIncludes;
				projConfig->general.nmakeOutput = outputFile;
				projConfig->general.nmakeBuildCommandLine = cppbuildexe + " -workdir $(SolutionDir) build " + projectName + " " + configName;
				projConfig->general.nmakeCleanCommandLine = cppbuildexe + " -workdir $(SolutionDir) clean " + projectName + " " + configName;
				projConfig->general.nmakeReBuildCommandLine = cppbuildexe + " -workdir $(SolutionDir) rebuild " + projectName + " " + configName;
			}
			else if (projectType == "utility")
			{
				projConfig->general.configurationType = "Utility";
			}
			else if (projectType == "application")
			{
				projConfig->general.configurationType = "Application";
				projConfig->link.subSystem = "Windows";
			}
			else if (projectType == "console")
			{
				projConfig->general.configurationType = "Application";
				projConfig->link.subSystem = "Console";
			}
			else if (projectType == "lib")
			{
				projConfig->general.configurationType = "StaticLibrary";
				projConfig->link.subSystem = "Windows";
			}
			else if (projectType == "dll")
			{
				projConfig->general.configurationType = "DynamicLibrary";
				projConfig->link.subSystem = "Windows";
			}

			if (!configDefines.empty())
				configDefines.push_back("%(PreprocessorDefinitions)");
			if (!configIncludes.empty())
				configIncludes.push_back("%(AdditionalIncludeDirectories)");
			if (!configLibraryPaths.empty())
				configLibraryPaths.push_back("%(AdditionalLibraryDirectories)");
			if (!configDependencies.empty())
				configDependencies.push_back("%(AdditionalDependencies)");

			projConfig->clCompile.preprocessorDefinitions = configDefines;
			projConfig->clCompile.additionalIncludeDirectories = configIncludes;
			projConfig->link.additionalLibraryDirectories = configLibraryPaths;
			projConfig->link.additionalDependencies = configDependencies;
			projConfig->rc.preprocessorDefinitions = configDefines;
			projConfig->rc.additionalIncludeDirectories = configIncludes;

			// Apply defaults that do not produce compiler warnings, modern c++, uses multiple cores and enables safety features:

			projConfig->clCompile.warningLevel = "Level3";
			projConfig->clCompile.multiProcessorCompilation = "true";
			projConfig->clCompile.sdlCheck = "true";
			projConfig->clCompile.conformanceMode = "true";
			projConfig->clCompile.useStandardPreprocessor = "true";
			projConfig->clCompile.languageStandard = "stdcpp20";
			projConfig->clCompile.languageStandard_C = "stdc17";
			projConfig->clCompile.debugInformationFormat = "ProgramDatabase";
			projConfig->link.generateDebugInformation = "true";

			if (configName == "Debug" || configName == "debug")
			{
				projConfig->general.useDebugLibraries = "true";
				projConfig->general.wholeProgramOptimization = "false";
				projConfig->general.linkIncremental = "true";
				projConfig->clCompile.runtimeLibrary = "MultiThreadedDebug";
				projConfig->link.enableCOMDATFolding = "false";
				projConfig->link.optimizeReferences = "false";
			}
			else
			{
				projConfig->general.useDebugLibraries = "false";
				projConfig->general.wholeProgramOptimization = "true";
				projConfig->clCompile.runtimeLibrary = "MultiThreaded";
				projConfig->clCompile.intrinsicFunctions = "true";
				projConfig->clCompile.functionLevelLinking = "true";
				projConfig->link.enableCOMDATFolding = "true";
				projConfig->link.optimizeReferences = "true";
			}

			// To do: deal with configCCompileOptions - apply it to every C file encountered?

			projConfig->clCompile.applyCompileOptions(configCxxCompileOptions);
			if (projectType == "lib")
				projConfig->lib.applyLibOptions(configLinkOptions);
			else
				projConfig->link.applyLinkOptions(configLinkOptions);

			project->configurations.push_back(std::move(projConfig));
		}

		solution->projects.push_back(std::move(project));
	}

	solution->generate();
	saveSolutionGuids(guids, cppbuildDir);
}

std::string VSWorkspace::addPathToCommand(std::string cmdline, const std::string& configName, const BuildSetup& setup, const std::string& workDir)
{
	std::string tool;
	size_t pos = cmdline.find(' ');
	if (pos == std::string::npos)
	{
		tool = cmdline;
		pos = cmdline.size();
	}
	else
	{
		tool = cmdline.substr(0, pos);
	}

	for (const BuildTarget& target : setup.project.targets)
	{
		if (target.name == tool)
		{
			// std::string binPath = "$(SolutionDir)Build\\$(Configuration)\\bin\\";
			std::string binPath = FilePath::combine(workDir, { "Build", configName, "bin" });
			return FilePath::combine(binPath, tool) + cmdline.substr(pos);
		}
	}

	return cmdline;
}

VSGuids VSWorkspace::loadSolutionGuids(const std::string& cppbuildDir)
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

void VSWorkspace::saveSolutionGuids(const VSGuids& guids, const std::string& cppbuildDir)
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
