
#include "Precomp.h"
#include "VSGenerator.h"
#include "IOData/File.h"
#include "IOData/FilePath.h"

VSSolution::VSSolution(const std::string& name, const std::string& location, const std::string& solutionGuid) : name(name), location(location), solutionGuid(solutionGuid)
{
}

void VSSolution::generate()
{
	VSGenerator generator;
	generator.writeSolution(this);
	for (const auto& project : projects)
	{
		generator.writeProject(project.get());
	}
}

/////////////////////////////////////////////////////////////////////////////

void VSGenerator::writeSolution(const VSSolution* solution)
{
	VSLineWriter output;
	output.writeLine("Microsoft Visual Studio Solution File, Format Version 12.00");
	output.writeLine("# Visual Studio Version 17");
	if (!solution->visualStudioVersion.empty())
		output.writeLine("VisualStudioVersion = " + solution->visualStudioVersion);
	if (!solution->minimumVisualStudioVersion.empty())
		output.writeLine("MinimumVisualStudioVersion = " + solution->minimumVisualStudioVersion);
	for (const auto& project : solution->projects)
	{
		output.writeLine("Project(\"" + project->typeGuid + "\") = \"" + project->name + "\", \"" + FilePath::combine(project->location, project->name + ".vcxproj") + "\", \"" + project->projectGuid + "\")");
		output.writeLine("EndProject");
	}
	output.writeLine("Global");
	output.writeLine("\tGlobalSection(SolutionConfigurationPlatforms) = preSolution");
	for (const auto& configuration : solution->configurations)
	{
		output.writeLine("\t\t" + configuration->name + "|" + configuration->platform + " = " + configuration->name + "|" + configuration->platform);
	}
	output.writeLine("\tEndGlobalSection");
	output.writeLine("\tGlobalSection(ProjectConfigurationPlatforms) = postSolution");
	for (const auto& project : solution->projects)
	{
		for (const auto& configuration : project->configurations)
		{
			output.writeLine("\t\t{" + project->projectGuid + "}." + configuration->name + "|" + configuration->platform + ".ActiveCfg = " + configuration->name + "|" + configuration->platform);
			output.writeLine("\t\t{" + project->projectGuid + "}." + configuration->name + "|" + configuration->platform + ".Build.0 = " + configuration->name + "|" + configuration->platform);
		}
	}
	output.writeLine("\tEndGlobalSection");
	output.writeLine("\tGlobalSection(SolutionProperties) = preSolution");
	output.writeLine("\t\tHideSolutionNode = FALSE");
	output.writeLine("\tEndGlobalSection");
	output.writeLine("\tGlobalSection(ExtensibilityGlobals) = postSolution");
	output.writeLine("\t\tSolutionGuid = {" + solution->solutionGuid + "}");
	output.writeLine("\tEndGlobalSection");
	output.writeLine("EndGlobal");
	output.save(FilePath::combine(solution->location, solution->name + ".sln"));
}

void VSGenerator::writeProject(const VSCppProject* project)
{
	VSLineWriter output;
	output.writeLine("<?xml version=\"1.0\" encoding=\"utf-8\"?>");
	output.writeLine("<Project DefaultTargets=\"Build\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">");
	output.writeLine("  <ItemGroup Label=\"ProjectConfigurations\">");
	for (const auto& configuration : project->configurations)
	{
		output.writeLine("    <ProjectConfiguration Include=\"" + configuration->name + "|" + configuration->platform + "\">");
		output.writeLine("      <Configuration>" + configuration->name + "</Configuration>");
		output.writeLine("      <Platform>" + configuration->platform + "</Platform>");
		output.writeLine("    </ProjectConfiguration>");
	}
	output.writeLine("  </ItemGroup>");
	output.writeLine("  <PropertyGroup Label=\"Globals\">");
	output.writeLine("    <VCProjectVersion>" + project->vcProjectVersion + "</VCProjectVersion>");
	output.writeLine("    <Keyword>Win32Proj</Keyword>");
	output.writeLine("    <ProjectGuid>{" + toLowerCase(project->projectGuid) + "}</ProjectGuid>");
	output.writeLine("    <RootNamespace>" + project->name + "</RootNamespace>");
	output.writeLine("    <WindowsTargetPlatformVersion>" + project->windowsTargetPlatformVersion + "</WindowsTargetPlatformVersion>");
	output.writeLine("  </PropertyGroup>");
	output.writeLine("  <Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.Default.props\" />");
	for (const auto& configuration : project->configurations)
	{
		output.writeLine("  <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='" + configuration->name + "|" + configuration->platform + "'\" Label=\"Configuration\">");
		output.writeLine("    <ConfigurationType>" + configuration->general.configurationType + "</ConfigurationType>");
		output.writeLine("    <UseDebugLibraries>" + configuration->general.useDebugLibraries + "</UseDebugLibraries>");
		output.writeLine("    <PlatformToolset>" + configuration->general.platformToolset + "</PlatformToolset>");
		if (configuration->general.configurationType != "Makefile" && configuration->general.configurationType != "Utility")
		{
			output.writeLine("    <WholeProgramOptimization>" + configuration->general.wholeProgramOptimization + "</WholeProgramOptimization>");
			output.writeLine("    <CharacterSet>" + configuration->general.characterSet + "</CharacterSet>");
		}
		output.writeLine("  </PropertyGroup>");
	}
	output.writeLine("  <Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.props\" />");
	output.writeLine("  <ImportGroup Label=\"ExtensionSettings\">");
	output.writeLine("  </ImportGroup>");
	output.writeLine("  <ImportGroup Label=\"Shared\">");
	output.writeLine("  </ImportGroup>");
	for (const auto& configuration : project->configurations)
	{
		output.writeLine("  <ImportGroup Label=\"PropertySheets\" Condition=\"'$(Configuration)|$(Platform)'=='" + configuration->name + "|" + configuration->platform + "'\">");
		output.writeLine("    <Import Project=\"$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props\" Condition=\"exists('$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props')\" Label=\"LocalAppDataPlatform\" />");
		output.writeLine("  </ImportGroup>");
	}
	output.writeLine("  <PropertyGroup Label=\"UserMacros\" />");
	for (const auto& configuration : project->configurations)
	{
		output.writeLine("  <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='" + configuration->name + "|" + configuration->platform + "'\">");
		if (configuration->general.configurationType != "Makefile" && configuration->general.configurationType != "Utility")
		{
			output.writeLine("    <LinkIncremental>" + configuration->general.linkIncremental + "</LinkIncremental>");
		}
		else
		{
			std::string nmakePreprocessorDefinitions;
			for (const auto& value : configuration->general.nmakePreprocessorDefinitions)
			{
				if (!nmakePreprocessorDefinitions.empty())
					nmakePreprocessorDefinitions += ";";
				nmakePreprocessorDefinitions += value;
			}
			if (!nmakePreprocessorDefinitions.empty())
			{
				nmakePreprocessorDefinitions += ";";
				nmakePreprocessorDefinitions += "$(NMakePreprocessorDefinitions)";
			}

			std::string nmakeIncludeSearchPath;
			for (const auto& value : configuration->general.nmakeIncludeSearchPath)
			{
				if (!nmakeIncludeSearchPath.empty())
					nmakeIncludeSearchPath += ";";
				nmakeIncludeSearchPath += value;
			}
			if (!nmakeIncludeSearchPath.empty())
			{
				nmakeIncludeSearchPath += ";";
				nmakeIncludeSearchPath += "$(NMakeIncludeSearchPath)";
			}

			output.writeLine("    <NMakeBuildCommandLine>" + configuration->general.nmakeBuildCommandLine + "</NMakeBuildCommandLine>");
			output.writeLine("    <NMakeOutput>" + configuration->general.nmakeOutput + "</NMakeOutput>");
			output.writeLine("    <NMakeCleanCommandLine>" + configuration->general.nmakeCleanCommandLine + "</NMakeCleanCommandLine>");
			output.writeLine("    <NMakeReBuildCommandLine>" + configuration->general.nmakeReBuildCommandLine + "</NMakeReBuildCommandLine>");
			output.writeLine("    <NMakePreprocessorDefinitions>" + nmakePreprocessorDefinitions + "</NMakePreprocessorDefinitions>");
			output.writeLine("    <NMakeIncludeSearchPath>" + nmakeIncludeSearchPath + "</NMakeIncludeSearchPath>");
			output.writeLine("    <AdditionalOptions>/std:c++20</AdditionalOptions>"); // For intellisense
		}
		output.writeLine("    <OutDir>" + configuration->general.outDir + "</OutDir>");
		output.writeLine("    <IntDir>" + configuration->general.intDir + "</IntDir>");
		output.writeLine("  </PropertyGroup>");
	}
	for (const auto& configuration : project->configurations)
	{
		output.writeLine("  <ItemDefinitionGroup Condition=\"'$(Configuration)|$(Platform)'=='" + configuration->name + "|" + configuration->platform + "'\">");
		if (configuration->general.configurationType != "Makefile" && configuration->general.configurationType != "Utility")
		{
			int indent = 6;
			output.writeLine("    <ClCompile>");
			output.writeProperty(indent, "AdditionalIncludeDirectories", configuration->clCompile.additionalIncludeDirectories);
			output.writeProperty(indent, "AdditionalOptions", configuration->clCompile.additionalOptions);
			output.writeProperty(indent, "AdditionalUsingDirectories", configuration->clCompile.additionalUsingDirectories);
			output.writeProperty(indent, "AlwaysAppend", configuration->clCompile.alwaysAppend);
			output.writeProperty(indent, "AssemblerListingLocation", configuration->clCompile.assemblerListingLocation);
			output.writeProperty(indent, "AssemblerOutput", configuration->clCompile.assemblerOutput);
			output.writeProperty(indent, "BasicRuntimeChecks", configuration->clCompile.basicRuntimeChecks);
			output.writeProperty(indent, "BrowseInformation", configuration->clCompile.browseInformation);
			output.writeProperty(indent, "BrowseInformationFile", configuration->clCompile.browseInformationFile);
			output.writeProperty(indent, "BufferSecurityCheck", configuration->clCompile.bufferSecurityCheck);
			output.writeProperty(indent, "BuildingInIDE", configuration->clCompile.buildingInIDE);
			output.writeProperty(indent, "CallingConvention", configuration->clCompile.callingConvention);
			output.writeProperty(indent, "CompileAs", configuration->clCompile.compileAs);
			output.writeProperty(indent, "CompileAsManaged", configuration->clCompile.compileAsManaged);
			output.writeProperty(indent, "CreateHotpatchableImage", configuration->clCompile.createHotpatchableImage);
			output.writeProperty(indent, "DebugInformationFormat", configuration->clCompile.debugInformationFormat);
			output.writeProperty(indent, "DisableLanguageExtensions", configuration->clCompile.disableLanguageExtensions);
			output.writeProperty(indent, "DisableSpecificWarnings", configuration->clCompile.disableSpecificWarnings);
			output.writeProperty(indent, "EnableEnhancedInstructionSet", configuration->clCompile.enableEnhancedInstructionSet);
			output.writeProperty(indent, "EnableFiberSafeOptimizations", configuration->clCompile.enableFiberSafeOptimizations);
			output.writeProperty(indent, "EnablePREfast", configuration->clCompile.enablePREfast);
			output.writeProperty(indent, "ErrorReporting", configuration->clCompile.errorReporting);
			output.writeProperty(indent, "ExceptionHandling", configuration->clCompile.exceptionHandling);
			output.writeProperty(indent, "ExpandAttributedSource", configuration->clCompile.expandAttributedSource);
			output.writeProperty(indent, "FavorSizeOrSpeed", configuration->clCompile.favorSizeOrSpeed);
			output.writeProperty(indent, "FloatingPointExceptions", configuration->clCompile.floatingPointExceptions);
			output.writeProperty(indent, "FloatingPointModel", configuration->clCompile.floatingPointModel);
			output.writeProperty(indent, "ForceConformanceInForLoopScope", configuration->clCompile.forceConformanceInForLoopScope);
			output.writeProperty(indent, "ForcedIncludeFiles", configuration->clCompile.forcedIncludeFiles);
			output.writeProperty(indent, "ForcedUsingFiles", configuration->clCompile.forcedUsingFiles);
			output.writeProperty(indent, "FunctionLevelLinking", configuration->clCompile.functionLevelLinking);
			output.writeProperty(indent, "GenerateXMLDocumentationFiles", configuration->clCompile.generateXMLDocumentationFiles);
			output.writeProperty(indent, "IgnoreStandardIncludePath", configuration->clCompile.ignoreStandardIncludePath);
			output.writeProperty(indent, "InlineFunctionExpansion", configuration->clCompile.inlineFunctionExpansion);
			output.writeProperty(indent, "IntrinsicFunctions", configuration->clCompile.intrinsicFunctions);
			output.writeProperty(indent, "SDLCheck", configuration->clCompile.sdlCheck);
			output.writeProperty(indent, "MinimalRebuild", configuration->clCompile.minimalRebuild);
			output.writeProperty(indent, "MultiProcessorCompilation", configuration->clCompile.multiProcessorCompilation);
			output.writeProperty(indent, "ObjectFileName", configuration->clCompile.objectFileName);
			output.writeProperty(indent, "ObjectFiles", configuration->clCompile.objectFiles);
			output.writeProperty(indent, "OmitDefaultLibName", configuration->clCompile.omitDefaultLibName);
			output.writeProperty(indent, "OmitFramePointers", configuration->clCompile.omitFramePointers);
			output.writeProperty(indent, "OpenMPSupport", configuration->clCompile.openMPSupport);
			output.writeProperty(indent, "Optimization", configuration->clCompile.optimization);
			output.writeProperty(indent, "PrecompiledHeader", configuration->clCompile.precompiledHeader);
			output.writeProperty(indent, "PrecompiledHeaderFile", configuration->clCompile.precompiledHeaderFile);
			output.writeProperty(indent, "PrecompiledHeaderOutputFile", configuration->clCompile.precompiledHeaderOutputFile);
			output.writeProperty(indent, "PreprocessKeepComments", configuration->clCompile.preprocessKeepComments);
			output.writeProperty(indent, "PreprocessorDefinitions", configuration->clCompile.preprocessorDefinitions);
			output.writeProperty(indent, "PreprocessOutput", configuration->clCompile.preprocessOutput);
			output.writeProperty(indent, "PreprocessOutputPath", configuration->clCompile.preprocessOutputPath);
			output.writeProperty(indent, "PreprocessSuppressLineNumbers", configuration->clCompile.preprocessSuppressLineNumbers);
			output.writeProperty(indent, "PreprocessToFile", configuration->clCompile.preprocessToFile);
			output.writeProperty(indent, "ProcessorNumber", configuration->clCompile.processorNumber);
			output.writeProperty(indent, "ProgramDataBaseFileName", configuration->clCompile.programDataBaseFileName);
			output.writeProperty(indent, "ConformanceMode", configuration->clCompile.conformanceMode);
			output.writeProperty(indent, "RuntimeLibrary", configuration->clCompile.runtimeLibrary);
			output.writeProperty(indent, "RuntimeTypeInfo", configuration->clCompile.runtimeTypeInfo);
			output.writeProperty(indent, "LanguageStandard", configuration->clCompile.languageStandard);
			output.writeProperty(indent, "ShowIncludes", configuration->clCompile.showIncludes);
			output.writeProperty(indent, "SmallerTypeCheck", configuration->clCompile.smallerTypeCheck);
			output.writeProperty(indent, "StringPooling", configuration->clCompile.stringPooling);
			output.writeProperty(indent, "StructMemberAlignment", configuration->clCompile.structMemberAlignment);
			output.writeProperty(indent, "SuppressStartupBanner", configuration->clCompile.suppressStartupBanner);
			output.writeProperty(indent, "TrackerLogDirectory", configuration->clCompile.trackerLogDirectory);
			output.writeProperty(indent, "TreatSpecificWarningsAsErrors", configuration->clCompile.treatSpecificWarningsAsErrors);
			output.writeProperty(indent, "TreatWarningAsError", configuration->clCompile.treatWarningAsError);
			output.writeProperty(indent, "TreatWChar_tAsBuiltInType", configuration->clCompile.treatWChar_tAsBuiltInType);
			output.writeProperty(indent, "UndefineAllPreprocessorDefinitions", configuration->clCompile.undefineAllPreprocessorDefinitions);
			output.writeProperty(indent, "UndefinePreprocessorDefinitions", configuration->clCompile.undefinePreprocessorDefinitions);
			output.writeProperty(indent, "UseFullPaths", configuration->clCompile.useFullPaths);
			output.writeProperty(indent, "UseUnicodeForAssemblerListing", configuration->clCompile.useUnicodeForAssemblerListing);
			output.writeProperty(indent, "WarningLevel", configuration->clCompile.warningLevel);
			output.writeProperty(indent, "WholeProgramOptimization", configuration->clCompile.wholeProgramOptimization);
			output.writeProperty(indent, "XMLDocumentationFileName", configuration->clCompile.xmlDocumentationFileName);
			output.writeProperty(indent, "MinimalRebuildFromTracking", configuration->clCompile.minimalRebuildFromTracking);
			output.writeProperty(indent, "TrackFileAccess", configuration->clCompile.trackFileAccess);
			output.writeLine("    </ClCompile>");

			if (configuration->general.configurationType == "StaticLibrary")
			{
				output.writeLine("    <Lib>");
				output.writeProperty(indent, "AdditionalDependencies", configuration->lib.additionalDependencies);
				output.writeProperty(indent, "AdditionalLibraryDirectories", configuration->lib.additionalLibraryDirectories);
				output.writeProperty(indent, "AdditionalOptions", configuration->lib.additionalOptions);
				output.writeProperty(indent, "DisplayLibrary", configuration->lib.displayLibrary);
				output.writeProperty(indent, "ErrorReporting", configuration->lib.errorReporting);
				output.writeProperty(indent, "ExportNamedFunctions", configuration->lib.exportNamedFunctions);
				output.writeProperty(indent, "ForceSymbolReferences", configuration->lib.forceSymbolReferences);
				output.writeProperty(indent, "IgnoreAllDefaultLibraries", configuration->lib.ignoreAllDefaultLibraries);
				output.writeProperty(indent, "IgnoreSpecificDefaultLibraries", configuration->lib.ignoreSpecificDefaultLibraries);
				output.writeProperty(indent, "LinkLibraryDependencies", configuration->lib.linkLibraryDependencies);
				output.writeProperty(indent, "LinkTimeCodeGeneration", configuration->lib.linkTimeCodeGeneration);
				output.writeProperty(indent, "MinimumRequiredVersion", configuration->lib.minimumRequiredVersion);
				output.writeProperty(indent, "ModuleDefinitionFile", configuration->lib.moduleDefinitionFile);
				output.writeProperty(indent, "Name", configuration->lib.name);
				output.writeProperty(indent, "OutputFile", configuration->lib.outputFile);
				output.writeProperty(indent, "RemoveObjects", configuration->lib.removeObjects);
				output.writeProperty(indent, "SubSystem", configuration->lib.subSystem);
				output.writeProperty(indent, "SuppressStartupBanner", configuration->lib.suppressStartupBanner);
				output.writeProperty(indent, "TargetMachine", configuration->lib.targetMachine);
				output.writeProperty(indent, "TrackerLogDirectory", configuration->lib.trackerLogDirectory);
				output.writeProperty(indent, "TreatLibWarningAsErrors", configuration->lib.treatLibWarningAsErrors);
				output.writeProperty(indent, "UseUnicodeResponseFiles", configuration->lib.useUnicodeResponseFiles);
				output.writeProperty(indent, "Verbose", configuration->lib.verbose);
				output.writeLine("    </Lib>");
			}
			else
			{
				output.writeLine("    <Link>");
				output.writeProperty(indent, "AdditionalDependencies", configuration->link.additionalDependencies);
				output.writeProperty(indent, "AdditionalLibraryDirectories", configuration->link.additionalLibraryDirectories);
				output.writeProperty(indent, "AdditionalManifestDependencies", configuration->link.additionalManifestDependencies);
				output.writeProperty(indent, "AdditionalOptions", configuration->link.additionalOptions);
				output.writeProperty(indent, "AddModuleNamesToAssembly", configuration->link.addModuleNamesToAssembly);
				output.writeProperty(indent, "AllowIsolation", configuration->link.allowIsolation);
				output.writeProperty(indent, "AssemblyDebug", configuration->link.assemblyDebug);
				output.writeProperty(indent, "AssemblyLinkResource", configuration->link.assemblyLinkResource);
				output.writeProperty(indent, "AttributeFileTracking", configuration->link.attributeFileTracking);
				output.writeProperty(indent, "BaseAddress", configuration->link.baseAddress);
				output.writeProperty(indent, "BuildingInIDE", configuration->link.buildingInIDE);
				output.writeProperty(indent, "CLRImageType", configuration->link.clrImageType);
				output.writeProperty(indent, "CLRSupportLastError", configuration->link.clrSupportLastError);
				output.writeProperty(indent, "CLRThreadAttribute", configuration->link.clrThreadAttribute);
				output.writeProperty(indent, "CLRUnmanagedCodeCheck", configuration->link.clrUnmanagedCodeCheck);
				output.writeProperty(indent, "CreateHotPatchableImage", configuration->link.createHotPatchableImage);
				output.writeProperty(indent, "DataExecutionPrevention", configuration->link.dataExecutionPrevention);
				output.writeProperty(indent, "DelayLoadDLLs", configuration->link.delayLoadDLLs);
				output.writeProperty(indent, "DelaySign", configuration->link.delaySign);
				output.writeProperty(indent, "Driver", configuration->link.driver);
				output.writeProperty(indent, "EmbedManagedResourceFile", configuration->link.embedManagedResourceFile);
				output.writeProperty(indent, "EnableCOMDATFolding", configuration->link.enableCOMDATFolding);
				output.writeProperty(indent, "EnableUAC", configuration->link.enableUAC);
				output.writeProperty(indent, "EntryPointSymbol", configuration->link.entryPointSymbol);
				output.writeProperty(indent, "FixedBaseAddress", configuration->link.fixedBaseAddress);
				output.writeProperty(indent, "ForceFileOutput", configuration->link.forceFileOutput);
				output.writeProperty(indent, "ForceSymbolReferences", configuration->link.forceSymbolReferences);
				output.writeProperty(indent, "FunctionOrder", configuration->link.functionOrder);
				output.writeProperty(indent, "GenerateDebugInformation", configuration->link.generateDebugInformation);
				output.writeProperty(indent, "GenerateManifest", configuration->link.generateManifest);
				output.writeProperty(indent, "GenerateMapFile", configuration->link.generateMapFile);
				output.writeProperty(indent, "HeapCommitSize", configuration->link.heapCommitSize);
				output.writeProperty(indent, "HeapReserveSize", configuration->link.heapReserveSize);
				output.writeProperty(indent, "IgnoreAllDefaultLibraries", configuration->link.ignoreAllDefaultLibraries);
				output.writeProperty(indent, "IgnoreEmbeddedIDL", configuration->link.ignoreEmbeddedIDL);
				output.writeProperty(indent, "IgnoreImportLibrary", configuration->link.ignoreImportLibrary);
				output.writeProperty(indent, "IgnoreSpecificDefaultLibraries", configuration->link.ignoreSpecificDefaultLibraries);
				output.writeProperty(indent, "ImageHasSafeExceptionHandlers", configuration->link.imageHasSafeExceptionHandlers);
				output.writeProperty(indent, "ImportLibrary", configuration->link.importLibrary);
				output.writeProperty(indent, "KeyContainer", configuration->link.keyContainer);
				output.writeProperty(indent, "KeyFile", configuration->link.keyFile);
				output.writeProperty(indent, "LargeAddressAware", configuration->link.largeAddressAware);
				output.writeProperty(indent, "LinkDLL", configuration->link.linkDLL);
				output.writeProperty(indent, "LinkErrorReporting", configuration->link.linkErrorReporting);
				output.writeProperty(indent, "LinkIncremental", configuration->link.linkIncremental);
				output.writeProperty(indent, "LinkLibraryDependencies", configuration->link.linkLibraryDependencies);
				output.writeProperty(indent, "LinkStatus", configuration->link.linkStatus);
				output.writeProperty(indent, "LinkTimeCodeGeneration", configuration->link.linkTimeCodeGeneration);
				output.writeProperty(indent, "ManifestFile", configuration->link.manifestFile);
				output.writeProperty(indent, "MapExports", configuration->link.mapExports);
				output.writeProperty(indent, "MapFileName", configuration->link.mapFileName);
				output.writeProperty(indent, "MergedIDLBaseFileName", configuration->link.mergedIDLBaseFileName);
				output.writeProperty(indent, "MergeSections", configuration->link.mergeSections);
				output.writeProperty(indent, "MidlCommandFile", configuration->link.midlCommandFile);
				output.writeProperty(indent, "MinimumRequiredVersion", configuration->link.minimumRequiredVersion);
				output.writeProperty(indent, "ModuleDefinitionFile", configuration->link.moduleDefinitionFile);
				output.writeProperty(indent, "MSDOSStubFileName", configuration->link.msdosStubFileName);
				output.writeProperty(indent, "NoEntryPoint", configuration->link.noEntryPoint);
				output.writeProperty(indent, "ObjectFiles", configuration->link.objectFiles);
				output.writeProperty(indent, "OptimizeReferences", configuration->link.optimizeReferences);
				output.writeProperty(indent, "OutputFile", configuration->link.outputFile);
				output.writeProperty(indent, "PerUserRedirection", configuration->link.perUserRedirection);
				output.writeProperty(indent, "PreventDllBinding", configuration->link.preventDllBinding);
				output.writeProperty(indent, "Profile", configuration->link.profile);
				output.writeProperty(indent, "ProfileGuidedDatabase", configuration->link.profileGuidedDatabase);
				output.writeProperty(indent, "ProgramDatabaseFile", configuration->link.programDatabaseFile);
				output.writeProperty(indent, "RandomizedBaseAddress", configuration->link.randomizedBaseAddress);
				output.writeProperty(indent, "RegisterOutput", configuration->link.registerOutput);
				output.writeProperty(indent, "SectionAlignment", configuration->link.sectionAlignment);
				output.writeProperty(indent, "SetChecksum", configuration->link.setChecksum);
				output.writeProperty(indent, "ShowProgress", configuration->link.showProgress);
				output.writeProperty(indent, "SpecifySectionAttributes", configuration->link.specifySectionAttributes);
				output.writeProperty(indent, "StackCommitSize", configuration->link.stackCommitSize);
				output.writeProperty(indent, "StackReserveSize", configuration->link.stackReserveSize);
				output.writeProperty(indent, "StripPrivateSymbols", configuration->link.stripPrivateSymbols);
				output.writeProperty(indent, "SubSystem", configuration->link.subSystem);
				output.writeProperty(indent, "SupportNobindOfDelayLoadedDLL", configuration->link.supportNobindOfDelayLoadedDLL);
				output.writeProperty(indent, "SupportUnloadOfDelayLoadedDLL", configuration->link.supportUnloadOfDelayLoadedDLL);
				output.writeProperty(indent, "SuppressStartupBanner", configuration->link.suppressStartupBanner);
				output.writeProperty(indent, "SwapRunFromCD", configuration->link.swapRunFromCD);
				output.writeProperty(indent, "SwapRunFromNET", configuration->link.swapRunFromNET);
				output.writeProperty(indent, "TargetMachine", configuration->link.targetMachine);
				output.writeProperty(indent, "TerminalServerAware", configuration->link.terminalServerAware);
				output.writeProperty(indent, "TrackerLogDirectory", configuration->link.trackerLogDirectory);
				output.writeProperty(indent, "TreatLinkerWarningAsErrors", configuration->link.treatLinkerWarningAsErrors);
				output.writeProperty(indent, "TurnOffAssemblyGeneration", configuration->link.turnOffAssemblyGeneration);
				output.writeProperty(indent, "TypeLibraryFile", configuration->link.typeLibraryFile);
				output.writeProperty(indent, "TypeLibraryResourceID", configuration->link.typeLibraryResourceID);
				output.writeProperty(indent, "UACExecutionLevel", configuration->link.uacExecutionLevel);
				output.writeProperty(indent, "UACUIAccess", configuration->link.uacUIAccess);
				output.writeProperty(indent, "UseLibraryDependencyInputs", configuration->link.useLibraryDependencyInputs);
				output.writeProperty(indent, "Version", configuration->link.version);
				output.writeLine("    </Link>");

				output.writeLine("    <ResourceCompile>");
				output.writeProperty(indent, "AdditionalIncludeDirectories", configuration->rc.additionalIncludeDirectories);
				output.writeProperty(indent, "AdditionalOptions", configuration->rc.additionalOptions);
				output.writeProperty(indent, "Culture", configuration->rc.culture);
				output.writeProperty(indent, "IgnoreStandardIncludePath", configuration->rc.ignoreStandardIncludePath);
				output.writeProperty(indent, "NullTerminateStrings", configuration->rc.nullTerminateStrings);
				output.writeProperty(indent, "PreprocessorDefinitions", configuration->rc.preprocessorDefinitions);
				output.writeProperty(indent, "ResourceOutputFileName", configuration->rc.resourceOutputFileName);
				output.writeProperty(indent, "ShowProgress", configuration->rc.showProgress);
				output.writeProperty(indent, "SuppressStartupBanner", configuration->rc.suppressStartupBanner);
				output.writeProperty(indent, "TrackerLogDirectory", configuration->rc.trackerLogDirectory);
				output.writeProperty(indent, "UndefinePreprocessorDefinitions", configuration->rc.undefinePreprocessorDefinitions);
				output.writeLine("    </ResourceCompile>");

				output.writeLine("    <Manifest>");
				output.writeProperty(indent, "AdditionalOptions", configuration->manifest.additionalOptions);
				output.writeLine("    </Manifest>");
			}
		}
		output.writeLine("  </ItemDefinitionGroup>");
	}

	if (!project->sourceFiles.empty())
	{
		output.writeLine("  <ItemGroup>");
		for (const auto& file : project->sourceFiles)
		{
			output.writeLine("    <ClCompile Include=\"" + file + "\" />");
		}
		output.writeLine("  </ItemGroup>");
	}

	if (!project->headerFiles.empty())
	{
		output.writeLine("  <ItemGroup>");
		for (const auto& file : project->headerFiles)
		{
			output.writeLine("    <ClInclude Include=\"" + file + "\" />");
		}
		output.writeLine("  </ItemGroup>");
	}

	if (!project->resourceFiles.empty())
	{
		output.writeLine("  <ItemGroup>");
		for (const auto& file : project->resourceFiles)
		{
			output.writeLine("    <ResourceCompile Include=\"" + file + "\" />");
		}
		output.writeLine("  </ItemGroup>");
	}

	if (!project->manifestFiles.empty())
	{
		output.writeLine("  <ItemGroup>");
		for (const auto& file : project->manifestFiles)
		{
			output.writeLine("    <Manifest Include=\"" + file + "\" />");
		}
		output.writeLine("  </ItemGroup>");
	}

	if (!project->customBuildFile.file.empty())
	{
		output.writeLine("  <ItemGroup>");
		output.writeLine("    <CustomBuild Include=\"" + project->customBuildFile.file + "\">");
		output.writeLine("      <UseUtf8Encoding>" + project->customBuildFile.useUtf8Encoding + "</UseUtf8Encoding>");
		for (const auto& configuration : project->configurations)
		{
			std::string additionalInputs, outputs;

			for (const auto& value : configuration->customBuildFile.additionalInputs)
			{
				if (!additionalInputs.empty())
					additionalInputs += ";";
				additionalInputs += value;
			}
			if (!additionalInputs.empty())
			{
				additionalInputs += ";";
				additionalInputs += "%(AdditionalInputs)";
			}

			for (const auto& value : configuration->customBuildFile.outputs)
			{
				if (!outputs.empty())
					outputs += ";";
				outputs += value;
			}

			output.writeLine("      <Message Condition=\"'$(Configuration)|$(Platform)'=='" + configuration->name + "|" + configuration->platform + "'\">" + configuration->customBuildFile.message + "</Message>");
			output.writeLine("      <Command Condition=\"'$(Configuration)|$(Platform)'=='" + configuration->name + "|" + configuration->platform + "'\">" + configuration->customBuildFile.command + "</Command>");
			output.writeLine("      <AdditionalInputs Condition=\"'$(Configuration)|$(Platform)'=='" + configuration->name + "|" + configuration->platform + "'\">" + additionalInputs + "</AdditionalInputs>");
			output.writeLine("      <Outputs Condition=\"'$(Configuration)|$(Platform)'=='" + configuration->name + "|" + configuration->platform + "'\">" + outputs + "</Outputs>");
			output.writeLine("      <LinkObjects Condition=\"'$(Configuration)|$(Platform)'=='" + configuration->name + "|" + configuration->platform + "'\">" + configuration->customBuildFile.linkObjects + "</LinkObjects>");
		}
		output.writeLine("    </CustomBuild>");
		output.writeLine("  </ItemGroup>");
	}

	if (!project->extraFiles.empty())
	{
		output.writeLine("  <ItemGroup>");
		for (const auto& file : project->extraFiles)
		{
			output.writeLine("    <None Include=\"" + file + "\" />");
		}
		output.writeLine("  </ItemGroup>");
	}

	if (!project->references.empty())
	{
		output.writeLine("  <ItemGroup>");
		for (const auto& ref : project->references)
		{
			output.writeLine("    <ProjectReference Include=\"" + FilePath::combine(ref.location, ref.name + ".vcxproj") + "\">");
			output.writeLine("      <Project>{" + ref.guid + "}</Project>");
			output.writeLine("    </ProjectReference>");
		}
		output.writeLine("  </ItemGroup>");
	}

	output.writeLine("  <Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.targets\" />");
	output.writeLine("  <ImportGroup Label=\"ExtensionTargets\">");
	output.writeLine("  </ImportGroup>");
	output.writeLine("</Project>");
	output.save(FilePath::combine(project->location, project->name + ".vcxproj"));

	if (!project->filters.empty())
	{
		writeProjectFilters(project);
	}
}

void VSGenerator::writeProjectFilters(const VSCppProject* project)
{
	VSLineWriter output;
	output.writeLine("<?xml version=\"1.0\" encoding=\"utf-8\"?>");
	output.writeLine("<Project ToolsVersion=\"4.0\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">");

	if (!project->sourceFiles.empty())
	{
		output.writeLine("  <ItemGroup>");
		for (const auto& filter : project->filters)
		{
			for (const auto& file : filter->sourceFiles)
			{
				if (filter->name == "")
				{
					output.writeLine("    <ClCompile Include=\"" + file + "\" />");
				}
				else
				{
					output.writeLine("    <ClCompile Include=\"" + file + "\">");
					output.writeLine("      <Filter>" + filter->name + "</Filter>");
					output.writeLine("    </ClCompile>");
				}
			}
		}
		output.writeLine("  </ItemGroup>");
	}

	if (!project->headerFiles.empty())
	{
		output.writeLine("  <ItemGroup>");
		for (const auto& filter : project->filters)
		{
			for (const auto& file : filter->headerFiles)
			{
				if (filter->name == "")
				{
					output.writeLine("    <ClInclude Include=\"" + file + "\" />");
				}
				else
				{
					output.writeLine("    <ClInclude Include=\"" + file + "\">");
					output.writeLine("      <Filter>" + filter->name + "</Filter>");
					output.writeLine("    </ClInclude>");
				}
			}
		}
		output.writeLine("  </ItemGroup>");
	}

	if (!project->resourceFiles.empty())
	{
		output.writeLine("  <ItemGroup>");
		for (const auto& filter : project->filters)
		{
			for (const auto& file : filter->resourceFiles)
			{
				if (filter->name == "")
				{
					output.writeLine("    <ResourceCompile Include=\"" + file + "\" />");
				}
				else
				{
					output.writeLine("    <ResourceCompile Include=\"" + file + "\">");
					output.writeLine("      <Filter>" + filter->name + "</Filter>");
					output.writeLine("    </ResourceCompile>");
				}
			}
		}
		output.writeLine("  </ItemGroup>");
	}

	if (!project->manifestFiles.empty())
	{
		output.writeLine("  <ItemGroup>");
		for (const auto& filter : project->filters)
		{
			for (const auto& file : filter->manifestFiles)
			{
				if (filter->name == "")
				{
					output.writeLine("    <Manifest Include=\"" + file + "\" />");
				}
				else
				{
					output.writeLine("    <Manifest Include=\"" + file + "\">");
					output.writeLine("      <Filter>" + filter->name + "</Filter>");
					output.writeLine("    </Manifest>");
				}
			}
		}
		output.writeLine("  </ItemGroup>");
	}

	if (!project->customBuildFile.file.empty())
	{
		output.writeLine("  <ItemGroup>");
		for (const auto& filter : project->filters)
		{
			if (!filter->customBuildFile.empty())
			{
				if (filter->name == "")
				{
					output.writeLine("    <CustomBuild Include=\"" + filter->customBuildFile + "\" />");
				}
				else
				{
					output.writeLine("    <CustomBuild Include=\"" + filter->customBuildFile + "\">");
					output.writeLine("      <Filter>" + filter->name + "</Filter>");
					output.writeLine("    </CustomBuild>");
				}
			}
		}
		output.writeLine("  </ItemGroup>");
	}

	if (!project->extraFiles.empty())
	{
		output.writeLine("  <ItemGroup>");
		for (const auto& filter : project->filters)
		{
			for (const auto& file : filter->extraFiles)
			{
				if (filter->name == "")
				{
					output.writeLine("    <None Include=\"" + file + "\" />");
				}
				else
				{
					output.writeLine("    <None Include=\"" + file + "\">");
					output.writeLine("      <Filter>" + filter->name + "</Filter>");
					output.writeLine("    </None>");
				}
			}
		}
		output.writeLine("  </ItemGroup>");
	}

	output.writeLine("  <ItemGroup>");
	for (const auto& filter : project->filters)
	{
		if (filter->name != "")
		{
			output.writeLine("    <Filter Include=\"" + filter->name + "\">");
			output.writeLine("      <UniqueIdentifier>{" + filter->guid + "}</UniqueIdentifier>");
			output.writeLine("    </Filter>");
		}
	}
	output.writeLine("  </ItemGroup>");

	output.writeLine("</Project>");
	output.save(FilePath::combine(project->location, project->name + ".vcxproj.filters"));
}

std::string VSGenerator::toLowerCase(std::string text)
{
	for (char& c : text)
	{
		if (c >= 'A' && c <= 'Z')
			c = c - 'A' + 'a';
	}
	return text;
}

/////////////////////////////////////////////////////////////////////////////

void VSLineWriter::writeLine(const std::string& text)
{
	lines += text;
	lines += "\r\n";
}

void VSLineWriter::writeProperty(int indent, const std::string& propName, const std::string& propValue)
{
	if (!propValue.empty())
	{
		line.clear();
		line.resize(indent, ' ');
		line += '<';
		line += propName;
		line += '>';
		line += propValue;
		line += '<';
		line += '/';
		line += propName;
		line += '>';
		writeLine(line);
	}
}

void VSLineWriter::writeProperty(int indent, const std::string& propName, const std::vector<std::string>& propValues)
{
	if (!propValues.empty())
	{
		line.clear();
		line.resize(indent, ' ');
		line += '<';
		line += propName;
		line += '>';
		for (const std::string& propValue : propValues)
		{
			line += propValue;
			line += ';';
		}
		line += '%';
		line += '(';
		line += propName;
		line += ')';
		line += '<';
		line += '/';
		line += propName;
		line += '>';
		writeLine(line);
	}
}

void VSLineWriter::save(const std::string& filename)
{
	try
	{
		// Don't touch file if nothing changed
		if (File::readAllText(filename) == lines)
			return;
	}
	catch (...)
	{
	}

	File::writeAllText(filename, lines);
}
