
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
		std::vector<std::string> cCompileOptions;
		std::vector<std::string> cxxCompileOptions;
		std::vector<std::string> linkOptions;
		std::vector<std::string> includePaths;
		std::vector<std::string> libraryPaths;
		std::vector<std::string> linkLibraries;
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
			else if (projectType == "application")
			{
				projConfig->general.configurationType = "Application";
				projConfig->clCompile.preprocessorDefinitions = configDefines;
				projConfig->clCompile.additionalIncludeDirectories = configIncludes;
				projConfig->clCompile.warningLevel = "Level3";
				projConfig->clCompile.functionLevelLinking = "true";
				projConfig->clCompile.intrinsicFunctions = "true";
				projConfig->clCompile.runtimeLibrary = "MultiThreaded";
				projConfig->clCompile.multiProcessorCompilation = "true";
				projConfig->clCompile.sdlCheck = "true";
				projConfig->clCompile.conformanceMode = "true";
				projConfig->clCompile.languageStandard = "stdcpp20";
				projConfig->link.subSystem = "Windows";
				projConfig->link.enableCOMDATFolding = "true";
				projConfig->link.optimizeReferences = "true";
				projConfig->link.generateDebugInformation = "true";
				projConfig->link.additionalLibraryDirectories = configLibraryPaths;
				projConfig->link.additionalDependencies = configDependencies;
			}
			else if (projectType == "console")
			{
				projConfig->general.configurationType = "Application";
				projConfig->clCompile.preprocessorDefinitions = configDefines;
				projConfig->clCompile.additionalIncludeDirectories = configIncludes;
				projConfig->clCompile.warningLevel = "Level3";
				projConfig->clCompile.functionLevelLinking = "true";
				projConfig->clCompile.intrinsicFunctions = "true";
				projConfig->clCompile.runtimeLibrary = "MultiThreaded";
				projConfig->clCompile.multiProcessorCompilation = "true";
				projConfig->clCompile.sdlCheck = "true";
				projConfig->clCompile.conformanceMode = "true";
				projConfig->clCompile.languageStandard = "stdcpp20";
				projConfig->link.subSystem = "Console";
				projConfig->link.enableCOMDATFolding = "true";
				projConfig->link.optimizeReferences = "true";
				projConfig->link.generateDebugInformation = "true";
				projConfig->link.additionalLibraryDirectories = configLibraryPaths;
				projConfig->link.additionalDependencies = configDependencies;
			}
			else if (projectType == "lib")
			{
				projConfig->general.configurationType = "StaticLibrary";
				projConfig->clCompile.preprocessorDefinitions = configDefines;
				projConfig->clCompile.additionalIncludeDirectories = configIncludes;
				projConfig->clCompile.warningLevel = "Level3";
				projConfig->clCompile.functionLevelLinking = "true";
				projConfig->clCompile.intrinsicFunctions = "true";
				projConfig->clCompile.runtimeLibrary = "MultiThreaded";
				projConfig->clCompile.multiProcessorCompilation = "true";
				projConfig->clCompile.sdlCheck = "true";
				projConfig->clCompile.conformanceMode = "true";
				projConfig->clCompile.languageStandard = "stdcpp20";
				projConfig->link.subSystem = "Windows";
				projConfig->link.enableCOMDATFolding = "true";
				projConfig->link.optimizeReferences = "true";
				projConfig->link.generateDebugInformation = "true";
				projConfig->link.additionalLibraryDirectories = configLibraryPaths;
				projConfig->link.additionalDependencies = configDependencies;
			}
			else if (projectType == "dll")
			{
				projConfig->general.configurationType = "DynamicLibrary";
				projConfig->clCompile.preprocessorDefinitions = configDefines;
				projConfig->clCompile.additionalIncludeDirectories = configIncludes;
				projConfig->clCompile.warningLevel = "Level3";
				projConfig->clCompile.functionLevelLinking = "true";
				projConfig->clCompile.intrinsicFunctions = "true";
				projConfig->clCompile.runtimeLibrary = "MultiThreaded";
				projConfig->clCompile.multiProcessorCompilation = "true";
				projConfig->clCompile.sdlCheck = "true";
				projConfig->clCompile.conformanceMode = "true";
				projConfig->clCompile.languageStandard = "stdcpp20";
				projConfig->link.subSystem = "Windows";
				projConfig->link.enableCOMDATFolding = "true";
				projConfig->link.optimizeReferences = "true";
				projConfig->link.generateDebugInformation = "true";
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

			// TBD: what should the defaults be? MSBuild defaults will bomb us decades back in time and build insecure executables
			// To do: deal with configCCompileOptions - apply it to every C file encountered?

			applyCompileOptions(projConfig.get(), configCxxCompileOptions);
			if (projectType == "lib")
				applyLibOptions(projConfig.get(), configLinkOptions);
			else
				applyLinkOptions(projConfig.get(), configLinkOptions);

			project->configurations.push_back(std::move(projConfig));
		}

		solution->projects.push_back(std::move(project));
	}

	solution->generate();
	saveSolutionGuids(guids, cppbuildDir);
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

void VSWorkspace::applyCompileOptions(VSCppProjectConfiguration* projConfig, const std::vector<std::string>& options)
{
	std::map<std::string, SimpleOption> simpleOptions =
	{
		{ "/FA", { "AssemblyCode", &projConfig->clCompile.assemblerOutput }},
		{ "/FAc", { "AssemblyAndMachineCode", &projConfig->clCompile.assemblerOutput }},
		{ "/FAs", { "AssemblyAndSourceCode", &projConfig->clCompile.assemblerOutput }},
		{ "/FAcs", { "All", &projConfig->clCompile.assemblerOutput }},
		{ "/RTC-", { "Default", &projConfig->clCompile.assemblerOutput }},
		{ "/RTCs", { "StackFrameRuntimeCheck", &projConfig->clCompile.assemblerOutput }},
		{ "/RTCu", { "UninitializedLocalUsageCheck", &projConfig->clCompile.assemblerOutput }},
		{ "/RTC1", { "EnableFastChecks", &projConfig->clCompile.assemblerOutput }},
		{ "/FR", { "true", &projConfig->clCompile.browseInformation }},
		{ "/Fr", { "true", &projConfig->clCompile.browseInformation }},
		{ "/GS", { "true", &projConfig->clCompile.bufferSecurityCheck }},
		{ "/GS-", { "false", &projConfig->clCompile.bufferSecurityCheck }},
		{ "/Gd", { "Cdecl", &projConfig->clCompile.callingConvention }},
		{ "/Gr", { "FastCall", &projConfig->clCompile.callingConvention }},
		{ "/Gz", { "StdCall", &projConfig->clCompile.callingConvention }},
		{ "/TC", { "CompileAsC", &projConfig->clCompile.compileAs }},
		{ "/TP", { "CompileAsCpp", &projConfig->clCompile.compileAs }},
		{ "/interface", { "CompileAsCppModule", &projConfig->clCompile.compileAs }},
		{ "/internalPartition", { "CompileAsCppModuleInternalPartition", &projConfig->clCompile.compileAs }},
		{ "/exportHeader", { "CompileAsHeaderUnit", &projConfig->clCompile.compileAs }},
		{ "/clr", { "true", &projConfig->clCompile.compileAsManaged }},
		{ "/clr:pure", { "Pure", &projConfig->clCompile.compileAsManaged }},
		{ "/clr:safe", { "Safe", &projConfig->clCompile.compileAsManaged }},
		{ "/clr:oldSyntax", { "OldSyntax", &projConfig->clCompile.compileAsManaged }},
		{ "/hotpatch", { "true", &projConfig->clCompile.createHotpatchableImage }},
		{ "/Z7", { "OldStyle", &projConfig->clCompile.debugInformationFormat }},
		{ "/Zi", { "ProgramDatabase", &projConfig->clCompile.debugInformationFormat }},
		{ "/ZI", { "EditAndContinue", &projConfig->clCompile.debugInformationFormat }},
		{ "/arch:IA32", { "NoExtensions", &projConfig->clCompile.enableEnhancedInstructionSet }},
		{ "/arch:SSE", { "StreamingSIMDExtensions", &projConfig->clCompile.enableEnhancedInstructionSet }},
		{ "/arch:SSE2", { "StreamingSIMDExtensions2", &projConfig->clCompile.enableEnhancedInstructionSet }},
		{ "/arch:AVX", { "AdvancedVectorExtensions", &projConfig->clCompile.enableEnhancedInstructionSet }},
		{ "/arch:AVX2", { "AdvancedVectorExtensions2", &projConfig->clCompile.enableEnhancedInstructionSet }},
		{ "/arch:AVX512", { "AdvancedVectorExtensions512", &projConfig->clCompile.enableEnhancedInstructionSet }},
		{ "/GT", { "true", &projConfig->clCompile.enableFiberSafeOptimizations }},
		{ "/GT-", { "false", &projConfig->clCompile.enableFiberSafeOptimizations }},
		{ "/analyze", { "true", &projConfig->clCompile.enablePREfast }},
		{ "/analyze-", { "false", &projConfig->clCompile.enablePREfast }},
		{ "/errorReport:none", { "None", &projConfig->clCompile.errorReporting }},
		{ "/errorReport:prompt", { "Prompt", &projConfig->clCompile.errorReporting }},
		{ "/errorReport:queue", { "Queue", &projConfig->clCompile.errorReporting }},
		{ "/errorReport:send", { "Send", &projConfig->clCompile.errorReporting }},
		{ "/EH-", { "false", &projConfig->clCompile.exceptionHandling }},
		{ "/EHa", { "Async", &projConfig->clCompile.exceptionHandling }},
		{ "/EHsc", { "Sync", &projConfig->clCompile.exceptionHandling }},
		{ "/EHs", { "SyncCThrow", &projConfig->clCompile.exceptionHandling }},
		{ "/Fx", { "true", &projConfig->clCompile.expandAttributedSource }},
		{ "/O-", { "Neither", &projConfig->clCompile.favorSizeOrSpeed }},
		{ "/Os", { "Size", &projConfig->clCompile.favorSizeOrSpeed }},
		{ "/Ot", { "Speed", &projConfig->clCompile.favorSizeOrSpeed }},
		{ "/fp:except", { "true", &projConfig->clCompile.floatingPointExceptions }},
		{ "/fp:except-", { "false", &projConfig->clCompile.floatingPointExceptions }},
		{ "/fp:precise", { "Precise", &projConfig->clCompile.floatingPointModel }},
		{ "/fp:strict", { "Strict", &projConfig->clCompile.floatingPointModel }},
		{ "/fp:fast", { "Fast", &projConfig->clCompile.floatingPointModel }},
		{ "/Zc:forScope", { "true", &projConfig->clCompile.forceConformanceInForLoopScope }},
		{ "/Zc:forScope-", { "false", &projConfig->clCompile.forceConformanceInForLoopScope }},
		{ "/Gy", { "true", &projConfig->clCompile.functionLevelLinking }},
		{ "/Gy-", { "false", &projConfig->clCompile.functionLevelLinking }},
		{ "/doc", { "true", &projConfig->clCompile.generateXMLDocumentationFiles }},
		{ "/X", { "true", &projConfig->clCompile.ignoreStandardIncludePath }},
		{ "/Ob-", { "Default", &projConfig->clCompile.inlineFunctionExpansion }},
		{ "/Ob0", { "Disabled", &projConfig->clCompile.inlineFunctionExpansion }},
		{ "/Ob1", { "OnlyExplicitInline", &projConfig->clCompile.inlineFunctionExpansion }},
		{ "/Ob2", { "AnySuitable", &projConfig->clCompile.inlineFunctionExpansion }},
		{ "/Oi", { "true", &projConfig->clCompile.intrinsicFunctions }},
		{ "/Oi-", { "false", &projConfig->clCompile.intrinsicFunctions }},
		{ "/Gm", { "true", &projConfig->clCompile.minimalRebuild }},
		{ "/Zl", { "true", &projConfig->clCompile.omitDefaultLibName }},
		{ "/Oy", { "true", &projConfig->clCompile.omitFramePointers }},
		{ "/openmp", { "true", &projConfig->clCompile.openMPSupport }},
		{ "/Od", { "Disabled", &projConfig->clCompile.optimization }},
		{ "/O1", { "MinSpace", &projConfig->clCompile.optimization }},
		{ "/O2", { "MaxSpeed", &projConfig->clCompile.optimization }},
		{ "/Ox", { "Full", &projConfig->clCompile.optimization }},
		{ "/Y-", { "NotUsing", &projConfig->clCompile.precompiledHeader }},
		{ "/Yc", { "Create", &projConfig->clCompile.precompiledHeader }},
		{ "/Yu", { "Use", &projConfig->clCompile.precompiledHeader }},
		{ "/C", { "true", &projConfig->clCompile.preprocessKeepComments }},
		{ "/EP", { "true", &projConfig->clCompile.preprocessSuppressLineNumbers }},
		{ "/P", { "true", &projConfig->clCompile.preprocessToFile }},
		{ "/MT", { "MultiThreaded", &projConfig->clCompile.runtimeLibrary }},
		{ "/MTd", { "MultiThreadedDebug", &projConfig->clCompile.runtimeLibrary }},
		{ "/MD", { "MultiThreadedDLL", &projConfig->clCompile.runtimeLibrary }},
		{ "/MDd", { "MultiThreadedDebugDLL", &projConfig->clCompile.runtimeLibrary }},
		{ "/GR", { "true", &projConfig->clCompile.runtimeTypeInfo }},
		{ "/GR-", { "false", &projConfig->clCompile.runtimeTypeInfo }},
		{ "/showIncludes", { "true", &projConfig->clCompile.showIncludes }},
		{ "/RTCc", { "true", &projConfig->clCompile.smallerTypeCheck }},
		{ "/GF", { "true", &projConfig->clCompile.stringPooling }},
		{ "/Zp1", { "1Byte", &projConfig->clCompile.structMemberAlignment }},
		{ "/Zp2", { "2Bytes", &projConfig->clCompile.structMemberAlignment }},
		{ "/Zp4", { "4Bytes", &projConfig->clCompile.structMemberAlignment }},
		{ "/Zp8", { "8Bytes", &projConfig->clCompile.structMemberAlignment }},
		{ "/Zp16", { "16Bytes", &projConfig->clCompile.structMemberAlignment }},
		{ "/nologo", { "true", &projConfig->clCompile.suppressStartupBanner }},
		{ "/WX", { "true", &projConfig->clCompile.treatWarningAsError }},
		{ "/Zc:wchar_t", { "true", &projConfig->clCompile.treatWChar_tAsBuiltInType }},
		{ "/Zc:wchar_t-", { "false", &projConfig->clCompile.treatWChar_tAsBuiltInType }},
		{ "/u", { "true", &projConfig->clCompile.undefineAllPreprocessorDefinitions }},
		{ "/FC", { "true", &projConfig->clCompile.useFullPaths }},
		{ "/FAu", { "true", &projConfig->clCompile.useUnicodeForAssemblerListing }},
		{ "/W0", { "TurnOffAllWarnings", &projConfig->clCompile.warningLevel }},
		{ "/W1", { "Level1", &projConfig->clCompile.warningLevel }},
		{ "/W2", { "Level2", &projConfig->clCompile.warningLevel }},
		{ "/W3", { "Level3", &projConfig->clCompile.warningLevel }},
		{ "/W4", { "Level4", &projConfig->clCompile.warningLevel }},
		{ "/Wall", { "EnableAllWarnings", &projConfig->clCompile.warningLevel }},
		{ "/GL", { "true", &projConfig->clCompile.wholeProgramOptimization }},
	};

	// To do: handle the undocumented sdlCheck, conformanceMode and languageStandard properties

	for (const std::string& opt : options)
	{
		if (opt.empty())
			continue;

		auto it = simpleOptions.find(opt);
		if (it != simpleOptions.end())
		{
			const SimpleOption& simple = it->second;
			*simple.prop = simple.value;
		}
		else if (opt.starts_with("/I "))
		{
			projConfig->clCompile.additionalIncludeDirectories.push_back(opt.substr(3));
		}
		else if (opt.starts_with("/I"))
		{
			projConfig->clCompile.additionalIncludeDirectories.push_back(opt.substr(2));
		}
		else if (opt.starts_with("/AI"))
		{
			projConfig->clCompile.additionalUsingDirectories.push_back(opt.substr(3));
		}
		else if (opt.starts_with("/Fa"))
		{
			projConfig->clCompile.assemblerListingLocation = opt.substr(3);
		}
		else if (opt.starts_with("/FR") || opt.starts_with("/Fr"))
		{
			projConfig->clCompile.browseInformation = "true";
			projConfig->clCompile.browseInformationFile = opt.substr(3);
		}
		else if (opt.starts_with("/wd"))
		{
			projConfig->clCompile.disableSpecificWarnings.push_back(opt.substr(3));
		}
		else if (opt.starts_with("/FI "))
		{
			projConfig->clCompile.forcedIncludeFiles.push_back(opt.substr(4));
		}
		else if (opt.starts_with("/FI"))
		{
			projConfig->clCompile.forcedIncludeFiles.push_back(opt.substr(3));
		}
		else if (opt.starts_with("/Fo "))
		{
			projConfig->clCompile.objectFileName = opt.substr(4);
		}
		else if (opt.starts_with("/Fo"))
		{
			projConfig->clCompile.objectFileName = opt.substr(3);
		}
		else if (opt.starts_with("/Yc"))
		{
			projConfig->clCompile.precompiledHeader = "Create";
			projConfig->clCompile.precompiledHeaderFile = opt.substr(3);
		}
		else if (opt.starts_with("/Yu"))
		{
			projConfig->clCompile.precompiledHeader = "Use";
			projConfig->clCompile.precompiledHeaderFile = opt.substr(3);
		}
		else if (opt.starts_with("/Fp"))
		{
			projConfig->clCompile.precompiledHeaderOutputFile = opt.substr(3);
		}
		else if (opt.starts_with("/Fi"))
		{
			projConfig->clCompile.preprocessOutputPath = opt.substr(3);
		}
		else if (opt.starts_with("/MP"))
		{
			projConfig->clCompile.multiProcessorCompilation = "true";
			projConfig->clCompile.processorNumber = opt.substr(3);
		}
		else if (opt.starts_with("/Fd"))
		{
			projConfig->clCompile.programDataBaseFileName = opt.substr(3);
		}
		else if (opt.starts_with("/we"))
		{
			projConfig->clCompile.treatSpecificWarningsAsErrors.push_back(opt.substr(3));
		}
		else if (opt.starts_with("/U "))
		{
			projConfig->clCompile.undefinePreprocessorDefinitions.push_back(opt.substr(3));
		}
		else if (opt.starts_with("/U"))
		{
			projConfig->clCompile.undefinePreprocessorDefinitions.push_back(opt.substr(2));
		}
		else if (opt.starts_with("/doc"))
		{
			projConfig->clCompile.generateXMLDocumentationFiles = "true";
			projConfig->clCompile.xmlDocumentationFileName = opt.substr(4);
		}
		else
		{
			if (!projConfig->clCompile.additionalOptions.empty())
				projConfig->clCompile.additionalOptions += ' ';
			projConfig->clCompile.additionalOptions += opt;
		}
	}
}

void VSWorkspace::applyLinkOptions(VSCppProjectConfiguration* projConfig, const std::vector<std::string>& options)
{
	std::map<std::string, SimpleOption> simpleOptions =
	{
		{ "/ALLOWISOLATION", { "true", &projConfig->link.allowIsolation }},
		{ "/ALLOWISOLATION:NO", { "false", &projConfig->link.allowIsolation }},
		{ "/ASSEMBLYDEBUG", { "true", &projConfig->link.assemblyDebug }},
		{ "/ASSEMBLYDEBUG:DISABLE", { "false", &projConfig->link.assemblyDebug }},
		{ "/CLRIMAGETYPE:IJW", { "ForceIJWImage", &projConfig->link.clrImageType }},
		{ "/CLRIMAGETYPE:PURE", { "ForcePureILImage", &projConfig->link.clrImageType }},
		{ "/CLRIMAGETYPE:SAFE", { "ForceSafeILImage", &projConfig->link.clrImageType }},
		{ "/CLRSUPPORTLASTERROR", { "Enabled", &projConfig->link.clrSupportLastError }},
		{ "/CLRSUPPORTLASTERROR:NO", { "Disabled", &projConfig->link.clrSupportLastError }},
		{ "/CLRSUPPORTLASTERROR:SYSTEMDLL", { "SystemDlls", &projConfig->link.clrSupportLastError }},
		{ "/CLRTHREADATTRIBUTE:NONE", { "DefaultThreadingAttribute", &projConfig->link.clrThreadAttribute }},
		{ "/CLRTHREADATTRIBUTE:MTA", { "MTAThreadingAttribute", &projConfig->link.clrThreadAttribute }},
		{ "/CLRTHREADATTRIBUTE:STA", { "STAThreadingAttribute", &projConfig->link.clrThreadAttribute }},
		{ "/CLRUNMANAGEDCODECHECK", { "true", &projConfig->link.clrUnmanagedCodeCheck }},
		{ "/CLRUNMANAGEDCODECHECK:NO", { "false", &projConfig->link.clrUnmanagedCodeCheck }},
		{ "/FUNCTIONPADMIN", { "Enabled", &projConfig->link.createHotPatchableImage }},
		{ "/FUNCTIONPADMIN:5", { "X86Image", &projConfig->link.createHotPatchableImage }},
		{ "/FUNCTIONPADMIN:6", { "X64Image", &projConfig->link.createHotPatchableImage }},
		{ "/FUNCTIONPADMIN:16", { "ItaniumImage", &projConfig->link.createHotPatchableImage }},
		{ "/NXCOMPAT", { "true", &projConfig->link.dataExecutionPrevention }},
		{ "/NXCOMPAT:NO", { "false", &projConfig->link.dataExecutionPrevention }},
		{ "/DELAYSIGN", { "true", &projConfig->link.delaySign }},
		{ "/DELAYSIGN:NO", { "false", &projConfig->link.delaySign }},
		{ "/DRIVER", { "Driver", &projConfig->link.driver }},
		{ "/DRIVER:UPONLY", { "UpOnly", &projConfig->link.driver }},
		{ "/DRIVER:WDM", { "WDM", &projConfig->link.driver }},
		{ "/OPT:ICF", { "true", &projConfig->link.enableCOMDATFolding }},
		{ "/OPT:NOICF", { "false", &projConfig->link.enableCOMDATFolding }},
		{ "/MANIFESTUAC", { "true", &projConfig->link.enableUAC }},
		{ "/MANIFESTUAC:NO", { "false", &projConfig->link.enableUAC }},
		{ "/FIXED", { "true", &projConfig->link.fixedBaseAddress }},
		{ "/FIXED:NO", { "false", &projConfig->link.fixedBaseAddress }},
		{ "/FORCE", { "Enabled", &projConfig->link.forceFileOutput }},
		{ "/FORCE:MULTIPLE", { "MultiplyDefinedSymbolOnly", &projConfig->link.forceFileOutput }},
		{ "/FORCE:UNRESOLVED", { "UndefinedSymbolOnly", &projConfig->link.forceFileOutput }},
		{ "/DEBUG", { "true", &projConfig->link.generateDebugInformation }},
		{ "/DEBUG:NONE", { "false", &projConfig->link.generateDebugInformation }},
		{ "/MAP", { "true", &projConfig->link.generateMapFile }},
		{ "/NODEFAULTLIB", { "true", &projConfig->link.ignoreAllDefaultLibraries }},
		{ "/IGNOREIDL", { "true", &projConfig->link.ignoreEmbeddedIDL }},
		{ "/SAFESEH", { "true", &projConfig->link.imageHasSafeExceptionHandlers }},
		{ "/SAFESEH:NO", { "false", &projConfig->link.imageHasSafeExceptionHandlers }},
		{ "/LARGEADDRESSAWARE", { "true", &projConfig->link.largeAddressAware }},
		{ "/LARGEADDRESSAWARE:NO", { "false", &projConfig->link.largeAddressAware }},
		{ "/DLL", { "true", &projConfig->link.linkDLL }},
		{ "/ERRORREPORT:NONE", { "NoErrorReport", &projConfig->link.linkErrorReporting }},
		{ "/ERRORREPORT:PROMPT", { "PromptImmediately", &projConfig->link.linkErrorReporting }},
		{ "/ERRORREPORT:QUEUE", { "QueueForNextLogin", &projConfig->link.linkErrorReporting }},
		{ "/ERRORREPORT:SEND", { "SendErrorReport", &projConfig->link.linkErrorReporting }},
		{ "/INCREMENTAL", { "true", &projConfig->link.linkIncremental }},
		{ "/INCREMENTAL:NO", { "false", &projConfig->link.linkIncremental }},
		{ "/LTCG:STATUS", { "true", &projConfig->link.linkStatus }},
		{ "/LTCG:OFF", { "Default", &projConfig->link.linkTimeCodeGeneration }},
		{ "/LTCG", { "UseLinkTimeCodeGeneration", &projConfig->link.linkTimeCodeGeneration }},
		{ "/LTCG:PGInstrument", { "PGInstrument", &projConfig->link.linkTimeCodeGeneration }},
		{ "/LTCG:PGOptimize", { "PGOptimization", &projConfig->link.linkTimeCodeGeneration }},
		{ "/LTCG:PGUpdate", { "PGUpdate", &projConfig->link.linkTimeCodeGeneration }},
		{ "/MAPINFO:EXPORTS", { "true", &projConfig->link.mapExports }},
		{ "/NOENTRY", { "true", &projConfig->link.noEntryPoint }},
		{ "/OPT:REF", { "true", &projConfig->link.optimizeReferences }},
		{ "/OPT:NOREF", { "false", &projConfig->link.optimizeReferences }},
		{ "/ALLOWBIND", { "true", &projConfig->link.preventDllBinding }},
		{ "/ALLOWBIND:NO", { "false", &projConfig->link.preventDllBinding }},
		{ "/PROFILE", { "true", &projConfig->link.profile }},
		{ "/DYNAMICBASE", { "true", &projConfig->link.randomizedBaseAddress }},
		{ "/DYNAMICBASE:NO", { "false", &projConfig->link.randomizedBaseAddress }},
		{ "/RELEASE", { "true", &projConfig->link.setChecksum }},
		{ "/VERBOSE", { "LinkVerbose", &projConfig->link.showProgress }},
		{ "/VERBOSE:LIB", { "LinkVerboseLib", &projConfig->link.showProgress }},
		{ "/VERBOSE:ICF", { "LinkVerboseICF", &projConfig->link.showProgress }},
		{ "/VERBOSE:REF", { "LinkVerboseREF", &projConfig->link.showProgress }},
		{ "/VERBOSE:SAFESEH", { "LinkVerboseSAFESEH", &projConfig->link.showProgress }},
		{ "/VERBOSE:CLR", { "LinkVerboseCLR", &projConfig->link.showProgress }},
		{ "/VERBOSE:NO", { "NotSet", &projConfig->link.showProgress }},
		{ "/SUBSYSTEM:CONSOLE", { "Console", &projConfig->link.subSystem }},
		{ "/SUBSYSTEM:WINDOWS", { "Windows", &projConfig->link.subSystem }},
		{ "/SUBSYSTEM:NATIVE", { "Native", &projConfig->link.subSystem }},
		{ "/SUBSYSTEM:EFI_APPLICATION", { "EFI Application", &projConfig->link.subSystem }},
		{ "/SUBSYSTEM:EFI_BOOT_SERVICE_DRIVER", { "EFI Boot Service Driver", &projConfig->link.subSystem }},
		{ "/SUBSYSTEM:EFI_ROM", { "EFI ROM", &projConfig->link.subSystem }},
		{ "/SUBSYSTEM:EFI_RUNTIME_DRIVER", { "EFI Runtime", &projConfig->link.subSystem }},
		{ "/SUBSYSTEM:WINDOWSCE", { "WindowsCE", &projConfig->link.subSystem }},
		{ "/SUBSYSTEM:POSIX", { "POSIX", &projConfig->link.subSystem }},
		{ "/DELAY:NOBIND", { "true", &projConfig->link.supportNobindOfDelayLoadedDLL }},
		{ "/DELAY:UNLOAD", { "true", &projConfig->link.supportUnloadOfDelayLoadedDLL }},
		{ "/NOLOGO", { "", &projConfig->link.suppressStartupBanner }},
		{ "/SWAPRUN:CD", { "true", &projConfig->link.swapRunFromCD }},
		{ "/SWAPRUN:NET", { "true", &projConfig->link.swapRunFromNET }},
		{ "/MACHINE:ARM", { "MachineARM", &projConfig->link.targetMachine }},
		{ "/MACHINE:ARM64", { "MachineARM64", &projConfig->link.targetMachine }},
		{ "/MACHINE:ARMEC", { "MachineARM64EC", &projConfig->link.targetMachine }},
		{ "/MACHINE:EBC", { "MachineEBC", &projConfig->link.targetMachine }},
		{ "/MACHINE:IA64", { "MachineIA64", &projConfig->link.targetMachine }},
		{ "/MACHINE:MIPS", { "MachineMIPS", &projConfig->link.targetMachine }},
		{ "/MACHINE:MIPS16", { "MachineMIPS16", &projConfig->link.targetMachine }},
		{ "/MACHINE:MIPSFPU", { "MachineMIPSFPU", &projConfig->link.targetMachine }},
		{ "/MACHINE:MIPSFPU16", { "MachineMIPSFPU16", &projConfig->link.targetMachine }},
		{ "/MACHINE:SH4", { "MachineSH4", &projConfig->link.targetMachine }},
		{ "/MACHINE:THUMB", { "MachineTHUMB", &projConfig->link.targetMachine }},
		{ "/MACHINE:X64", { "MachineX64", &projConfig->link.targetMachine }},
		{ "/MACHINE:X86", { "MachineX86", &projConfig->link.targetMachine }},
		{ "/TSAWARE", { "true", &projConfig->link.terminalServerAware }},
		{ "/TSAWARE:NO", { "false", &projConfig->link.terminalServerAware } },
		{ "/WX", { "true", &projConfig->link.treatLinkerWarningAsErrors } },
		{ "/WX:NO", { "false", &projConfig->link.treatLinkerWarningAsErrors } },
		{ "/NOASSEMBLY", { "true", &projConfig->link.turnOffAssemblyGeneration } },
	};

	for (const std::string& opt : options)
	{
		if (opt.empty())
			continue;

		std::string upper = opt;
		for (char& c : upper)
		{
			if (c >= 'a' && c <= 'Z')
				c += 'A' - 'a';
		}

		auto it = simpleOptions.find(upper);
		if (it != simpleOptions.end())
		{
			const SimpleOption& simple = it->second;
			*simple.prop = simple.value;
		}
		else if (upper.starts_with("/LIBPATH:"))
		{
			projConfig->link.additionalLibraryDirectories.push_back(opt.substr(9));
		}
		else if (upper.starts_with("/MANIFESTDEPENDENCY:"))
		{
			projConfig->link.additionalManifestDependencies.push_back(opt.substr(20));
		}
		else if (upper.starts_with("/ASSEMBLYMODULE:"))
		{
			projConfig->link.addModuleNamesToAssembly.push_back(opt.substr(16));
		}
		else if (upper.starts_with("/ASSEMBLYLINKRESOURCE:"))
		{
			projConfig->link.assemblyLinkResource.push_back(opt.substr(22));
		}
		else if (upper.starts_with("/BASE:"))
		{
			projConfig->link.baseAddress = opt.substr(6);
		}
		else if (upper.starts_with("/DELAYLOAD:"))
		{
			projConfig->link.delayLoadDLLs.push_back(opt.substr(11));
		}
		else if (upper.starts_with("/ASSEMBLYRESOURCE:"))
		{
			projConfig->link.embedManagedResourceFile.push_back(opt.substr(18));
		}
		else if (upper.starts_with("/ENTRY:"))
		{
			projConfig->link.entryPointSymbol = opt.substr(7);
		}
		else if (upper.starts_with("/INCLUDE:"))
		{
			projConfig->link.forceSymbolReferences.push_back(opt.substr(9));
		}
		else if (upper.starts_with("/ORDER:"))
		{
			projConfig->link.functionOrder = opt.substr(7);
		}
		else if (upper.starts_with("/MAP:"))
		{
			projConfig->link.generateMapFile = "true";
			projConfig->link.mapFileName = opt.substr(5);
		}
		else if (upper.starts_with("/HEAP:"))
		{
			size_t split = opt.find(',', 6);
			if (split != std::string::npos)
			{
				projConfig->link.heapReserveSize = opt.substr(6);
			}
			else
			{
				projConfig->link.heapReserveSize = opt.substr(6, split - 6);
				projConfig->link.heapCommitSize = opt.substr(split + 1);
			}
		}
		else if (upper.starts_with("/NODEFAULTLIB:"))
		{
			projConfig->link.ignoreSpecificDefaultLibraries.push_back(opt.substr(14));
		}
		else if (upper.starts_with("/IMPLIB:"))
		{
			projConfig->link.importLibrary = opt.substr(8);
		}
		else if (upper.starts_with("/KEYCONTAINER:"))
		{
			projConfig->link.keyContainer = opt.substr(14);
		}
		else if (upper.starts_with("/KEYFILE:"))
		{
			projConfig->link.keyFile = opt.substr(9);
		}
		else if (upper.starts_with("/MANIFESTFILE:"))
		{
			projConfig->link.manifestFile = opt.substr(14);
		}
		else if (upper.starts_with("/IDLOUT:"))
		{
			projConfig->link.mergedIDLBaseFileName = opt.substr(8);
		}
		else if (upper.starts_with("/MIDL:"))
		{
			projConfig->link.midlCommandFile = opt.substr(6);
		}
		else if (upper.starts_with("/DEF:"))
		{
			projConfig->link.moduleDefinitionFile = opt.substr(5);
		}
		else if (upper.starts_with("/STUB:"))
		{
			projConfig->link.msdosStubFileName = opt.substr(6);
		}
		else if (upper.starts_with("/OUT:"))
		{
			projConfig->link.outputFile = opt.substr(5);
		}
		else if (upper.starts_with("/PGD:"))
		{
			projConfig->link.profileGuidedDatabase = opt.substr(5);
		}
		else if (upper.starts_with("/PDB:"))
		{
			projConfig->link.programDatabaseFile = opt.substr(5);
		}
		else if (upper.starts_with("/ALIGN:"))
		{
			projConfig->link.sectionAlignment = opt.substr(7);
		}
		else if (upper.starts_with("/SECTION:"))
		{
			projConfig->link.specifySectionAttributes.push_back(opt.substr(9));
		}
		else if (upper.starts_with("/STACK:"))
		{
			size_t split = opt.find(',', 7);
			if (split != std::string::npos)
			{
				projConfig->link.stackReserveSize = opt.substr(7);
			}
			else
			{
				projConfig->link.stackReserveSize = opt.substr(7, split - 7);
				projConfig->link.stackCommitSize = opt.substr(split + 1);
			}
		}
		else if (upper.starts_with("/PDBSTRIPPED:"))
		{
			projConfig->link.stripPrivateSymbols = opt.substr(13);
		}
		else if (upper.starts_with("/TLBOUT:"))
		{
			projConfig->link.typeLibraryFile = opt.substr(8);
		}
		else if (upper.starts_with("/TLBID:"))
		{
			projConfig->link.typeLibraryResourceID = opt.substr(7);
		}
		else if (upper.starts_with("/MANIFESTUAC:"))
		{
			std::string arg = opt.substr(13);
			if (arg == "level='asInvoker'")
			{
				projConfig->link.uacExecutionLevel = "AsInvoker";
			}
			else if (arg == "level='highestAvailable'")
			{
				projConfig->link.uacExecutionLevel = "HighestAvailable";
			}
			else if (arg == "level='requireAdministrator'")
			{
				projConfig->link.uacExecutionLevel = "RequireAdministrator";
			}
			else if (arg == "uiAccess='false'")
			{
				projConfig->link.uacUIAccess = "false";
			}
			else if (arg == "uiAccess='true'")
			{
				projConfig->link.uacUIAccess = "true";
			}
			else
			{
				if (!projConfig->link.additionalOptions.empty())
					projConfig->link.additionalOptions += ' ';
				projConfig->link.additionalOptions += opt;
			}
		}
		else if (upper.starts_with("/VERSION:"))
		{
			projConfig->link.version = opt.substr(9);
		}
		else
		{
			if (!projConfig->link.additionalOptions.empty())
				projConfig->link.additionalOptions += ' ';
			projConfig->link.additionalOptions += opt;
		}
	}
}

void VSWorkspace::applyLibOptions(VSCppProjectConfiguration* projConfig, const std::vector<std::string>& options)
{
	std::map<std::string, SimpleOption> simpleOptions =
	{
		{ "/NODEFAULTLIB", { "true", &projConfig->lib.ignoreAllDefaultLibraries }},
		{ "/ERRORREPORT:NONE", { "NoErrorReport", &projConfig->lib.errorReporting }},
		{ "/ERRORREPORT:PROMPT", { "PromptImmediately", &projConfig->lib.errorReporting }},
		{ "/ERRORREPORT:QUEUE", { "QueueForNextLogin", &projConfig->lib.errorReporting }},
		{ "/ERRORREPORT:SEND", { "SendErrorReport", &projConfig->lib.errorReporting }},
		{ "/LTCG", { "true", &projConfig->lib.linkTimeCodeGeneration }},
		{ "/SUBSYSTEM:CONSOLE", { "Console", &projConfig->lib.subSystem }},
		{ "/SUBSYSTEM:WINDOWS", { "Windows", &projConfig->lib.subSystem }},
		{ "/SUBSYSTEM:NATIVE", { "Native", &projConfig->lib.subSystem }},
		{ "/SUBSYSTEM:EFI_APPLICATION", { "EFI Application", &projConfig->lib.subSystem }},
		{ "/SUBSYSTEM:EFI_BOOT_SERVICE_DRIVER", { "EFI Boot Service Driver", &projConfig->lib.subSystem }},
		{ "/SUBSYSTEM:EFI_ROM", { "EFI ROM", &projConfig->lib.subSystem }},
		{ "/SUBSYSTEM:EFI_RUNTIME_DRIVER", { "EFI Runtime", &projConfig->lib.subSystem }},
		{ "/SUBSYSTEM:WINDOWSCE", { "WindowsCE", &projConfig->lib.subSystem }},
		{ "/SUBSYSTEM:POSIX", { "POSIX", &projConfig->lib.subSystem }},
		{ "/NOLOGO", { "", &projConfig->lib.suppressStartupBanner }},
		{ "/MACHINE:ARM", { "MachineARM", &projConfig->lib.targetMachine }},
		{ "/MACHINE:ARM64", { "MachineARM64", &projConfig->lib.targetMachine }},
		{ "/MACHINE:ARMEC", { "MachineARM64EC", &projConfig->lib.targetMachine }},
		{ "/MACHINE:EBC", { "MachineEBC", &projConfig->lib.targetMachine }},
		{ "/MACHINE:IA64", { "MachineIA64", &projConfig->lib.targetMachine }},
		{ "/MACHINE:MIPS", { "MachineMIPS", &projConfig->lib.targetMachine }},
		{ "/MACHINE:MIPS16", { "MachineMIPS16", &projConfig->lib.targetMachine }},
		{ "/MACHINE:MIPSFPU", { "MachineMIPSFPU", &projConfig->lib.targetMachine }},
		{ "/MACHINE:MIPSFPU16", { "MachineMIPSFPU16", &projConfig->lib.targetMachine }},
		{ "/MACHINE:SH4", { "MachineSH4", &projConfig->lib.targetMachine }},
		{ "/MACHINE:THUMB", { "MachineTHUMB", &projConfig->lib.targetMachine }},
		{ "/MACHINE:X64", { "MachineX64", &projConfig->lib.targetMachine }},
		{ "/MACHINE:X86", { "MachineX86", &projConfig->lib.targetMachine }},
		{ "/WX", { "true", &projConfig->lib.treatLibWarningAsErrors }},
		{ "/VERBOSE", { "true", &projConfig->lib.verbose }},
	};

	for (const std::string& opt : options)
	{
		if (opt.empty())
			continue;

		std::string upper = opt;
		for (char& c : upper)
		{
			if (c >= 'a' && c <= 'Z')
				c += 'A' - 'a';
		}

		auto it = simpleOptions.find(upper);
		if (it != simpleOptions.end())
		{
			const SimpleOption& simple = it->second;
			*simple.prop = simple.value;
		}
		else if (upper.starts_with("/LIBPATH:"))
		{
			projConfig->lib.additionalLibraryDirectories.push_back(opt.substr(9));
		}
		else if (upper.starts_with("/LIST:"))
		{
			projConfig->lib.displayLibrary = opt.substr(6);
		}
		else if (upper.starts_with("/INCLUDE:"))
		{
			projConfig->lib.forceSymbolReferences.push_back(opt.substr(9));
		}
		else if (upper.starts_with("/NODEFAULTLIB:"))
		{
			projConfig->lib.ignoreSpecificDefaultLibraries.push_back(opt.substr(14));
		}
		else if (upper.starts_with("/DEF:"))
		{
			projConfig->lib.moduleDefinitionFile = opt.substr(5);
		}
		else if (upper.starts_with("/NAME:"))
		{
			projConfig->lib.name = opt.substr(6);
		}
		else if (upper.starts_with("/OUT:"))
		{
			projConfig->lib.outputFile = opt.substr(5);
		}
		else if (upper.starts_with("/REMOVE:"))
		{
			projConfig->lib.removeObjects.push_back(opt.substr(8));
		}
		else if (upper.starts_with("/EXPORT:"))
		{
			projConfig->lib.exportNamedFunctions.push_back(opt.substr(8));
		}
		else
		{
			if (!projConfig->lib.additionalOptions.empty())
				projConfig->lib.additionalOptions += ' ';
			projConfig->lib.additionalOptions += opt;
		}
	}
}
