
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
			output.writeLine("    <ClCompile>");
			configuration->clCompile.writeProperties(output, 6);
			output.writeLine("    </ClCompile>");

			if (configuration->general.configurationType == "StaticLibrary")
			{
				output.writeLine("    <Lib>");
				configuration->lib.writeProperties(output, 6);
				output.writeLine("    </Lib>");
			}
			else
			{
				output.writeLine("    <Link>");
				configuration->link.writeProperties(output, 6);
				output.writeLine("    </Link>");

				output.writeLine("    <ResourceCompile>");
				configuration->rc.writeProperties(output, 6);
				output.writeLine("    </ResourceCompile>");

				output.writeLine("    <Manifest>");
				configuration->manifest.writeProperties(output, 6);
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

void VSCompileTask::writeProperties(VSLineWriter& output, int indent)
{
	output.writeProperty(indent, "AdditionalIncludeDirectories", additionalIncludeDirectories);
	output.writeProperty(indent, "AdditionalOptions", additionalOptions);
	output.writeProperty(indent, "AdditionalUsingDirectories", additionalUsingDirectories);
	output.writeProperty(indent, "AlwaysAppend", alwaysAppend);
	output.writeProperty(indent, "AssemblerListingLocation", assemblerListingLocation);
	output.writeProperty(indent, "AssemblerOutput", assemblerOutput);
	output.writeProperty(indent, "BasicRuntimeChecks", basicRuntimeChecks);
	output.writeProperty(indent, "BrowseInformation", browseInformation);
	output.writeProperty(indent, "BrowseInformationFile", browseInformationFile);
	output.writeProperty(indent, "BufferSecurityCheck", bufferSecurityCheck);
	output.writeProperty(indent, "BuildingInIDE", buildingInIDE);
	output.writeProperty(indent, "CallingConvention", callingConvention);
	output.writeProperty(indent, "CompileAs", compileAs);
	output.writeProperty(indent, "CompileAsManaged", compileAsManaged);
	output.writeProperty(indent, "CreateHotpatchableImage", createHotpatchableImage);
	output.writeProperty(indent, "DebugInformationFormat", debugInformationFormat);
	output.writeProperty(indent, "DisableLanguageExtensions", disableLanguageExtensions);
	output.writeProperty(indent, "DisableSpecificWarnings", disableSpecificWarnings);
	output.writeProperty(indent, "EnableEnhancedInstructionSet", enableEnhancedInstructionSet);
	output.writeProperty(indent, "EnableFiberSafeOptimizations", enableFiberSafeOptimizations);
	output.writeProperty(indent, "EnablePREfast", enablePREfast);
	output.writeProperty(indent, "ErrorReporting", errorReporting);
	output.writeProperty(indent, "ExceptionHandling", exceptionHandling);
	output.writeProperty(indent, "ExpandAttributedSource", expandAttributedSource);
	output.writeProperty(indent, "FavorSizeOrSpeed", favorSizeOrSpeed);
	output.writeProperty(indent, "FloatingPointExceptions", floatingPointExceptions);
	output.writeProperty(indent, "FloatingPointModel", floatingPointModel);
	output.writeProperty(indent, "ForceConformanceInForLoopScope", forceConformanceInForLoopScope);
	output.writeProperty(indent, "ForcedIncludeFiles", forcedIncludeFiles);
	output.writeProperty(indent, "ForcedUsingFiles", forcedUsingFiles);
	output.writeProperty(indent, "FunctionLevelLinking", functionLevelLinking);
	output.writeProperty(indent, "GenerateXMLDocumentationFiles", generateXMLDocumentationFiles);
	output.writeProperty(indent, "IgnoreStandardIncludePath", ignoreStandardIncludePath);
	output.writeProperty(indent, "InlineFunctionExpansion", inlineFunctionExpansion);
	output.writeProperty(indent, "IntrinsicFunctions", intrinsicFunctions);
	output.writeProperty(indent, "SDLCheck", sdlCheck);
	output.writeProperty(indent, "MinimalRebuild", minimalRebuild);
	output.writeProperty(indent, "MultiProcessorCompilation", multiProcessorCompilation);
	output.writeProperty(indent, "ObjectFileName", objectFileName);
	output.writeProperty(indent, "ObjectFiles", objectFiles);
	output.writeProperty(indent, "OmitDefaultLibName", omitDefaultLibName);
	output.writeProperty(indent, "OmitFramePointers", omitFramePointers);
	output.writeProperty(indent, "OpenMPSupport", openMPSupport);
	output.writeProperty(indent, "Optimization", optimization);
	output.writeProperty(indent, "PrecompiledHeader", precompiledHeader);
	output.writeProperty(indent, "PrecompiledHeaderFile", precompiledHeaderFile);
	output.writeProperty(indent, "PrecompiledHeaderOutputFile", precompiledHeaderOutputFile);
	output.writeProperty(indent, "PreprocessKeepComments", preprocessKeepComments);
	output.writeProperty(indent, "PreprocessorDefinitions", preprocessorDefinitions);
	output.writeProperty(indent, "PreprocessOutput", preprocessOutput);
	output.writeProperty(indent, "PreprocessOutputPath", preprocessOutputPath);
	output.writeProperty(indent, "PreprocessSuppressLineNumbers", preprocessSuppressLineNumbers);
	output.writeProperty(indent, "PreprocessToFile", preprocessToFile);
	output.writeProperty(indent, "ProcessorNumber", processorNumber);
	output.writeProperty(indent, "ProgramDataBaseFileName", programDataBaseFileName);
	output.writeProperty(indent, "ConformanceMode", conformanceMode);
	output.writeProperty(indent, "RuntimeLibrary", runtimeLibrary);
	output.writeProperty(indent, "RuntimeTypeInfo", runtimeTypeInfo);
	output.writeProperty(indent, "LanguageStandard", languageStandard);
	output.writeProperty(indent, "ShowIncludes", showIncludes);
	output.writeProperty(indent, "SmallerTypeCheck", smallerTypeCheck);
	output.writeProperty(indent, "StringPooling", stringPooling);
	output.writeProperty(indent, "StructMemberAlignment", structMemberAlignment);
	output.writeProperty(indent, "SuppressStartupBanner", suppressStartupBanner);
	output.writeProperty(indent, "TrackerLogDirectory", trackerLogDirectory);
	output.writeProperty(indent, "TreatSpecificWarningsAsErrors", treatSpecificWarningsAsErrors);
	output.writeProperty(indent, "TreatWarningAsError", treatWarningAsError);
	output.writeProperty(indent, "TreatWChar_tAsBuiltInType", treatWChar_tAsBuiltInType);
	output.writeProperty(indent, "UndefineAllPreprocessorDefinitions", undefineAllPreprocessorDefinitions);
	output.writeProperty(indent, "UndefinePreprocessorDefinitions", undefinePreprocessorDefinitions);
	output.writeProperty(indent, "UseFullPaths", useFullPaths);
	output.writeProperty(indent, "UseUnicodeForAssemblerListing", useUnicodeForAssemblerListing);
	output.writeProperty(indent, "WarningLevel", warningLevel);
	output.writeProperty(indent, "WholeProgramOptimization", wholeProgramOptimization);
	output.writeProperty(indent, "XMLDocumentationFileName", xmlDocumentationFileName);
	output.writeProperty(indent, "MinimalRebuildFromTracking", minimalRebuildFromTracking);
	output.writeProperty(indent, "TrackFileAccess", trackFileAccess);
}

/////////////////////////////////////////////////////////////////////////////

void VSLinkTask::writeProperties(VSLineWriter& output, int indent)
{
	output.writeProperty(indent, "AdditionalDependencies", additionalDependencies);
	output.writeProperty(indent, "AdditionalLibraryDirectories", additionalLibraryDirectories);
	output.writeProperty(indent, "AdditionalManifestDependencies", additionalManifestDependencies);
	output.writeProperty(indent, "AdditionalOptions", additionalOptions);
	output.writeProperty(indent, "AddModuleNamesToAssembly", addModuleNamesToAssembly);
	output.writeProperty(indent, "AllowIsolation", allowIsolation);
	output.writeProperty(indent, "AssemblyDebug", assemblyDebug);
	output.writeProperty(indent, "AssemblyLinkResource", assemblyLinkResource);
	output.writeProperty(indent, "AttributeFileTracking", attributeFileTracking);
	output.writeProperty(indent, "BaseAddress", baseAddress);
	output.writeProperty(indent, "BuildingInIDE", buildingInIDE);
	output.writeProperty(indent, "CLRImageType", clrImageType);
	output.writeProperty(indent, "CLRSupportLastError", clrSupportLastError);
	output.writeProperty(indent, "CLRThreadAttribute", clrThreadAttribute);
	output.writeProperty(indent, "CLRUnmanagedCodeCheck", clrUnmanagedCodeCheck);
	output.writeProperty(indent, "CreateHotPatchableImage", createHotPatchableImage);
	output.writeProperty(indent, "DataExecutionPrevention", dataExecutionPrevention);
	output.writeProperty(indent, "DelayLoadDLLs", delayLoadDLLs);
	output.writeProperty(indent, "DelaySign", delaySign);
	output.writeProperty(indent, "Driver", driver);
	output.writeProperty(indent, "EmbedManagedResourceFile", embedManagedResourceFile);
	output.writeProperty(indent, "EnableCOMDATFolding", enableCOMDATFolding);
	output.writeProperty(indent, "EnableUAC", enableUAC);
	output.writeProperty(indent, "EntryPointSymbol", entryPointSymbol);
	output.writeProperty(indent, "FixedBaseAddress", fixedBaseAddress);
	output.writeProperty(indent, "ForceFileOutput", forceFileOutput);
	output.writeProperty(indent, "ForceSymbolReferences", forceSymbolReferences);
	output.writeProperty(indent, "FunctionOrder", functionOrder);
	output.writeProperty(indent, "GenerateDebugInformation", generateDebugInformation);
	output.writeProperty(indent, "GenerateManifest", generateManifest);
	output.writeProperty(indent, "GenerateMapFile", generateMapFile);
	output.writeProperty(indent, "HeapCommitSize", heapCommitSize);
	output.writeProperty(indent, "HeapReserveSize", heapReserveSize);
	output.writeProperty(indent, "IgnoreAllDefaultLibraries", ignoreAllDefaultLibraries);
	output.writeProperty(indent, "IgnoreEmbeddedIDL", ignoreEmbeddedIDL);
	output.writeProperty(indent, "IgnoreImportLibrary", ignoreImportLibrary);
	output.writeProperty(indent, "IgnoreSpecificDefaultLibraries", ignoreSpecificDefaultLibraries);
	output.writeProperty(indent, "ImageHasSafeExceptionHandlers", imageHasSafeExceptionHandlers);
	output.writeProperty(indent, "ImportLibrary", importLibrary);
	output.writeProperty(indent, "KeyContainer", keyContainer);
	output.writeProperty(indent, "KeyFile", keyFile);
	output.writeProperty(indent, "LargeAddressAware", largeAddressAware);
	output.writeProperty(indent, "LinkDLL", linkDLL);
	output.writeProperty(indent, "LinkErrorReporting", linkErrorReporting);
	output.writeProperty(indent, "LinkIncremental", linkIncremental);
	output.writeProperty(indent, "LinkLibraryDependencies", linkLibraryDependencies);
	output.writeProperty(indent, "LinkStatus", linkStatus);
	output.writeProperty(indent, "LinkTimeCodeGeneration", linkTimeCodeGeneration);
	output.writeProperty(indent, "ManifestFile", manifestFile);
	output.writeProperty(indent, "MapExports", mapExports);
	output.writeProperty(indent, "MapFileName", mapFileName);
	output.writeProperty(indent, "MergedIDLBaseFileName", mergedIDLBaseFileName);
	output.writeProperty(indent, "MergeSections", mergeSections);
	output.writeProperty(indent, "MidlCommandFile", midlCommandFile);
	output.writeProperty(indent, "MinimumRequiredVersion", minimumRequiredVersion);
	output.writeProperty(indent, "ModuleDefinitionFile", moduleDefinitionFile);
	output.writeProperty(indent, "MSDOSStubFileName", msdosStubFileName);
	output.writeProperty(indent, "NoEntryPoint", noEntryPoint);
	output.writeProperty(indent, "ObjectFiles", objectFiles);
	output.writeProperty(indent, "OptimizeReferences", optimizeReferences);
	output.writeProperty(indent, "OutputFile", outputFile);
	output.writeProperty(indent, "PerUserRedirection", perUserRedirection);
	output.writeProperty(indent, "PreventDllBinding", preventDllBinding);
	output.writeProperty(indent, "Profile", profile);
	output.writeProperty(indent, "ProfileGuidedDatabase", profileGuidedDatabase);
	output.writeProperty(indent, "ProgramDatabaseFile", programDatabaseFile);
	output.writeProperty(indent, "RandomizedBaseAddress", randomizedBaseAddress);
	output.writeProperty(indent, "RegisterOutput", registerOutput);
	output.writeProperty(indent, "SectionAlignment", sectionAlignment);
	output.writeProperty(indent, "SetChecksum", setChecksum);
	output.writeProperty(indent, "ShowProgress", showProgress);
	output.writeProperty(indent, "SpecifySectionAttributes", specifySectionAttributes);
	output.writeProperty(indent, "StackCommitSize", stackCommitSize);
	output.writeProperty(indent, "StackReserveSize", stackReserveSize);
	output.writeProperty(indent, "StripPrivateSymbols", stripPrivateSymbols);
	output.writeProperty(indent, "SubSystem", subSystem);
	output.writeProperty(indent, "SupportNobindOfDelayLoadedDLL", supportNobindOfDelayLoadedDLL);
	output.writeProperty(indent, "SupportUnloadOfDelayLoadedDLL", supportUnloadOfDelayLoadedDLL);
	output.writeProperty(indent, "SuppressStartupBanner", suppressStartupBanner);
	output.writeProperty(indent, "SwapRunFromCD", swapRunFromCD);
	output.writeProperty(indent, "SwapRunFromNET", swapRunFromNET);
	output.writeProperty(indent, "TargetMachine", targetMachine);
	output.writeProperty(indent, "TerminalServerAware", terminalServerAware);
	output.writeProperty(indent, "TrackerLogDirectory", trackerLogDirectory);
	output.writeProperty(indent, "TreatLinkerWarningAsErrors", treatLinkerWarningAsErrors);
	output.writeProperty(indent, "TurnOffAssemblyGeneration", turnOffAssemblyGeneration);
	output.writeProperty(indent, "TypeLibraryFile", typeLibraryFile);
	output.writeProperty(indent, "TypeLibraryResourceID", typeLibraryResourceID);
	output.writeProperty(indent, "UACExecutionLevel", uacExecutionLevel);
	output.writeProperty(indent, "UACUIAccess", uacUIAccess);
	output.writeProperty(indent, "UseLibraryDependencyInputs", useLibraryDependencyInputs);
	output.writeProperty(indent, "Version", version);
}

/////////////////////////////////////////////////////////////////////////////

void VSLibTask::writeProperties(VSLineWriter& output, int indent)
{
	output.writeProperty(indent, "AdditionalDependencies", additionalDependencies);
	output.writeProperty(indent, "AdditionalLibraryDirectories", additionalLibraryDirectories);
	output.writeProperty(indent, "AdditionalOptions", additionalOptions);
	output.writeProperty(indent, "DisplayLibrary", displayLibrary);
	output.writeProperty(indent, "ErrorReporting", errorReporting);
	output.writeProperty(indent, "ExportNamedFunctions", exportNamedFunctions);
	output.writeProperty(indent, "ForceSymbolReferences", forceSymbolReferences);
	output.writeProperty(indent, "IgnoreAllDefaultLibraries", ignoreAllDefaultLibraries);
	output.writeProperty(indent, "IgnoreSpecificDefaultLibraries", ignoreSpecificDefaultLibraries);
	output.writeProperty(indent, "LinkLibraryDependencies", linkLibraryDependencies);
	output.writeProperty(indent, "LinkTimeCodeGeneration", linkTimeCodeGeneration);
	output.writeProperty(indent, "MinimumRequiredVersion", minimumRequiredVersion);
	output.writeProperty(indent, "ModuleDefinitionFile", moduleDefinitionFile);
	output.writeProperty(indent, "Name", name);
	output.writeProperty(indent, "OutputFile", outputFile);
	output.writeProperty(indent, "RemoveObjects", removeObjects);
	output.writeProperty(indent, "SubSystem", subSystem);
	output.writeProperty(indent, "SuppressStartupBanner", suppressStartupBanner);
	output.writeProperty(indent, "TargetMachine", targetMachine);
	output.writeProperty(indent, "TrackerLogDirectory", trackerLogDirectory);
	output.writeProperty(indent, "TreatLibWarningAsErrors", treatLibWarningAsErrors);
	output.writeProperty(indent, "UseUnicodeResponseFiles", useUnicodeResponseFiles);
	output.writeProperty(indent, "Verbose", verbose);
}

/////////////////////////////////////////////////////////////////////////////

void VSResourceTask::writeProperties(VSLineWriter& output, int indent)
{
	output.writeProperty(indent, "AdditionalIncludeDirectories", additionalIncludeDirectories);
	output.writeProperty(indent, "AdditionalOptions", additionalOptions);
	output.writeProperty(indent, "Culture", culture);
	output.writeProperty(indent, "IgnoreStandardIncludePath", ignoreStandardIncludePath);
	output.writeProperty(indent, "NullTerminateStrings", nullTerminateStrings);
	output.writeProperty(indent, "PreprocessorDefinitions", preprocessorDefinitions);
	output.writeProperty(indent, "ResourceOutputFileName", resourceOutputFileName);
	output.writeProperty(indent, "ShowProgress", showProgress);
	output.writeProperty(indent, "SuppressStartupBanner", suppressStartupBanner);
	output.writeProperty(indent, "TrackerLogDirectory", trackerLogDirectory);
	output.writeProperty(indent, "UndefinePreprocessorDefinitions", undefinePreprocessorDefinitions);
}

/////////////////////////////////////////////////////////////////////////////

void VSManifestTask::writeProperties(VSLineWriter& output, int indent)
{
	output.writeProperty(indent, "AdditionalManifestFiles", additionalManifestFiles);
	output.writeProperty(indent, "AdditionalOptions", additionalOptions);
	output.writeProperty(indent, "AssemblyIdentity", assemblyIdentity);
	output.writeProperty(indent, "ComponentFileName", componentFileName);
	output.writeProperty(indent, "DependencyInformationFile", dependencyInformationFile);
	output.writeProperty(indent, "EmbedManifest", embedManifest);
	output.writeProperty(indent, "EnableDPIAwareness", enableDPIAwareness);
	output.writeProperty(indent, "GenerateCatalogFiles", generateCatalogFiles);
	output.writeProperty(indent, "GenerateCategoryTags", generateCategoryTags);
	output.writeProperty(indent, "InputResourceManifests", inputResourceManifests);
	output.writeProperty(indent, "ManifestFromManagedAssembly", manifestFromManagedAssembly);
	output.writeProperty(indent, "OutputManifestFile", outputManifestFile);
	output.writeProperty(indent, "OutputResourceManifests", outputResourceManifests);
	output.writeProperty(indent, "RegistrarScriptFile", registrarScriptFile);
	output.writeProperty(indent, "ReplacementsFile", replacementsFile);
	output.writeProperty(indent, "ResourceOutputFileName", resourceOutputFileName);
	output.writeProperty(indent, "SuppressDependencyElement", suppressDependencyElement);
	output.writeProperty(indent, "SuppressStartupBanner", suppressStartupBanner);
	output.writeProperty(indent, "TrackerLogDirectory", trackerLogDirectory);
	output.writeProperty(indent, "TypeLibraryFile", typeLibraryFile);
	output.writeProperty(indent, "UpdateFileHashes", updateFileHashes);
	output.writeProperty(indent, "UpdateFileHashesSearchPath", updateFileHashesSearchPath);
	output.writeProperty(indent, "VerboseOutput", verboseOutput);
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
