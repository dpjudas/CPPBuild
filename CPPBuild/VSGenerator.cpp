
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
			VSCompileTask::writeProperties(output, 6, { { "", &configuration->clCompile } });
			output.writeLine("    </ClCompile>");

			if (configuration->general.configurationType == "StaticLibrary")
			{
				output.writeLine("    <Lib>");
				VSLibTask::writeProperties(output, 6, { { "", &configuration->lib } });
				output.writeLine("    </Lib>");
			}
			else
			{
				output.writeLine("    <Link>");
				VSLinkTask::writeProperties(output, 6, { { "", &configuration->link } });
				output.writeLine("    </Link>");

				output.writeLine("    <ResourceCompile>");
				VSResourceTask::writeProperties(output, 6, { { "", &configuration->rc } });
				output.writeLine("    </ResourceCompile>");

				output.writeLine("    <Manifest>");
				VSManifestTask::writeProperties(output, 6, { { "", &configuration->manifest } });
				output.writeLine("    </Manifest>");
			}
		}
		output.writeLine("  </ItemDefinitionGroup>");
	}

	writeItemGroup(output, "ClCompile", project->sourceFiles);
	writeItemGroup(output, "ClInclude", project->headerFiles);
	writeItemGroup(output, "ResourceCompile", project->resourceFiles);
	writeItemGroup(output, "Manifest", project->manifestFiles);

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

	writeItemGroup(output, "None", project->extraFiles);

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

template<typename T>
void VSGenerator::writeItemGroup(VSLineWriter& output, const std::string& taskName, const std::vector<VSFile<T>>& files)
{
	if (!files.empty())
	{
		output.writeLine("  <ItemGroup>");
		for (const auto& file : files)
		{
			if (!file.conditions.empty())
			{
				output.writeLine("    <" + taskName + " Include = \"" + file.file + "\">");
				T::writeProperties(output, 6, file.conditions);
				output.writeLine("    </" + taskName + ">");
			}
			else
			{
				output.writeLine("    <" + taskName + " Include=\"" + file.file + "\" />");
			}
		}
		output.writeLine("  </ItemGroup>");
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

void VSCompileTask::writeProperties(VSLineWriter& output, int indent, const std::map<std::string, std::shared_ptr<VSCompileTask>>& conditions)
{
	std::vector<std::pair<std::string, VSCompileTask*>> c;
	for (const auto& it : conditions)
		c.push_back({ it.first, it.second.get() });
	writeProperties(output, indent, c);
}

void VSCompileTask::writeProperties(VSLineWriter& output, int indent, const std::vector<std::pair<std::string, VSCompileTask*>>& conditions)
{
	for (const auto& c : conditions) output.writeProperty(indent, "AdditionalIncludeDirectories", c.first, c.second->additionalIncludeDirectories);
	for (const auto& c : conditions) output.writeProperty(indent, "AdditionalOptions", c.first, c.second->additionalOptions);
	for (const auto& c : conditions) output.writeProperty(indent, "AdditionalUsingDirectories", c.first, c.second->additionalUsingDirectories);
	for (const auto& c : conditions) output.writeProperty(indent, "AlwaysAppend", c.first, c.second->alwaysAppend);
	for (const auto& c : conditions) output.writeProperty(indent, "AssemblerListingLocation", c.first, c.second->assemblerListingLocation);
	for (const auto& c : conditions) output.writeProperty(indent, "AssemblerOutput", c.first, c.second->assemblerOutput);
	for (const auto& c : conditions) output.writeProperty(indent, "BasicRuntimeChecks", c.first, c.second->basicRuntimeChecks);
	for (const auto& c : conditions) output.writeProperty(indent, "BrowseInformation", c.first, c.second->browseInformation);
	for (const auto& c : conditions) output.writeProperty(indent, "BrowseInformationFile", c.first, c.second->browseInformationFile);
	for (const auto& c : conditions) output.writeProperty(indent, "BufferSecurityCheck", c.first, c.second->bufferSecurityCheck);
	for (const auto& c : conditions) output.writeProperty(indent, "BuildingInIDE", c.first, c.second->buildingInIDE);
	for (const auto& c : conditions) output.writeProperty(indent, "CallingConvention", c.first, c.second->callingConvention);
	for (const auto& c : conditions) output.writeProperty(indent, "CompileAs", c.first, c.second->compileAs);
	for (const auto& c : conditions) output.writeProperty(indent, "CompileAsManaged", c.first, c.second->compileAsManaged);
	for (const auto& c : conditions) output.writeProperty(indent, "CreateHotpatchableImage", c.first, c.second->createHotpatchableImage);
	for (const auto& c : conditions) output.writeProperty(indent, "DebugInformationFormat", c.first, c.second->debugInformationFormat);
	for (const auto& c : conditions) output.writeProperty(indent, "DisableLanguageExtensions", c.first, c.second->disableLanguageExtensions);
	for (const auto& c : conditions) output.writeProperty(indent, "DisableSpecificWarnings", c.first, c.second->disableSpecificWarnings);
	for (const auto& c : conditions) output.writeProperty(indent, "EnableEnhancedInstructionSet", c.first, c.second->enableEnhancedInstructionSet);
	for (const auto& c : conditions) output.writeProperty(indent, "EnableFiberSafeOptimizations", c.first, c.second->enableFiberSafeOptimizations);
	for (const auto& c : conditions) output.writeProperty(indent, "EnablePREfast", c.first, c.second->enablePREfast);
	for (const auto& c : conditions) output.writeProperty(indent, "ErrorReporting", c.first, c.second->errorReporting);
	for (const auto& c : conditions) output.writeProperty(indent, "ExceptionHandling", c.first, c.second->exceptionHandling);
	for (const auto& c : conditions) output.writeProperty(indent, "ExpandAttributedSource", c.first, c.second->expandAttributedSource);
	for (const auto& c : conditions) output.writeProperty(indent, "FavorSizeOrSpeed", c.first, c.second->favorSizeOrSpeed);
	for (const auto& c : conditions) output.writeProperty(indent, "FloatingPointExceptions", c.first, c.second->floatingPointExceptions);
	for (const auto& c : conditions) output.writeProperty(indent, "FloatingPointModel", c.first, c.second->floatingPointModel);
	for (const auto& c : conditions) output.writeProperty(indent, "ForceConformanceInForLoopScope", c.first, c.second->forceConformanceInForLoopScope);
	for (const auto& c : conditions) output.writeProperty(indent, "ForcedIncludeFiles", c.first, c.second->forcedIncludeFiles);
	for (const auto& c : conditions) output.writeProperty(indent, "ForcedUsingFiles", c.first, c.second->forcedUsingFiles);
	for (const auto& c : conditions) output.writeProperty(indent, "FunctionLevelLinking", c.first, c.second->functionLevelLinking);
	for (const auto& c : conditions) output.writeProperty(indent, "GenerateXMLDocumentationFiles", c.first, c.second->generateXMLDocumentationFiles);
	for (const auto& c : conditions) output.writeProperty(indent, "IgnoreStandardIncludePath", c.first, c.second->ignoreStandardIncludePath);
	for (const auto& c : conditions) output.writeProperty(indent, "InlineFunctionExpansion", c.first, c.second->inlineFunctionExpansion);
	for (const auto& c : conditions) output.writeProperty(indent, "IntrinsicFunctions", c.first, c.second->intrinsicFunctions);
	for (const auto& c : conditions) output.writeProperty(indent, "SDLCheck", c.first, c.second->sdlCheck);
	for (const auto& c : conditions) output.writeProperty(indent, "MinimalRebuild", c.first, c.second->minimalRebuild);
	for (const auto& c : conditions) output.writeProperty(indent, "MultiProcessorCompilation", c.first, c.second->multiProcessorCompilation);
	for (const auto& c : conditions) output.writeProperty(indent, "ObjectFileName", c.first, c.second->objectFileName);
	for (const auto& c : conditions) output.writeProperty(indent, "ObjectFiles", c.first, c.second->objectFiles);
	for (const auto& c : conditions) output.writeProperty(indent, "OmitDefaultLibName", c.first, c.second->omitDefaultLibName);
	for (const auto& c : conditions) output.writeProperty(indent, "OmitFramePointers", c.first, c.second->omitFramePointers);
	for (const auto& c : conditions) output.writeProperty(indent, "OpenMPSupport", c.first, c.second->openMPSupport);
	for (const auto& c : conditions) output.writeProperty(indent, "Optimization", c.first, c.second->optimization);
	for (const auto& c : conditions) output.writeProperty(indent, "PrecompiledHeader", c.first, c.second->precompiledHeader);
	for (const auto& c : conditions) output.writeProperty(indent, "PrecompiledHeaderFile", c.first, c.second->precompiledHeaderFile);
	for (const auto& c : conditions) output.writeProperty(indent, "PrecompiledHeaderOutputFile", c.first, c.second->precompiledHeaderOutputFile);
	for (const auto& c : conditions) output.writeProperty(indent, "PreprocessKeepComments", c.first, c.second->preprocessKeepComments);
	for (const auto& c : conditions) output.writeProperty(indent, "PreprocessorDefinitions", c.first, c.second->preprocessorDefinitions);
	for (const auto& c : conditions) output.writeProperty(indent, "PreprocessOutput", c.first, c.second->preprocessOutput);
	for (const auto& c : conditions) output.writeProperty(indent, "PreprocessOutputPath", c.first, c.second->preprocessOutputPath);
	for (const auto& c : conditions) output.writeProperty(indent, "PreprocessSuppressLineNumbers", c.first, c.second->preprocessSuppressLineNumbers);
	for (const auto& c : conditions) output.writeProperty(indent, "PreprocessToFile", c.first, c.second->preprocessToFile);
	for (const auto& c : conditions) output.writeProperty(indent, "ProcessorNumber", c.first, c.second->processorNumber);
	for (const auto& c : conditions) output.writeProperty(indent, "ProgramDataBaseFileName", c.first, c.second->programDataBaseFileName);
	for (const auto& c : conditions) output.writeProperty(indent, "ConformanceMode", c.first, c.second->conformanceMode);
	for (const auto& c : conditions) output.writeProperty(indent, "RuntimeLibrary", c.first, c.second->runtimeLibrary);
	for (const auto& c : conditions) output.writeProperty(indent, "RuntimeTypeInfo", c.first, c.second->runtimeTypeInfo);
	for (const auto& c : conditions) output.writeProperty(indent, "LanguageStandard", c.first, c.second->languageStandard);
	for (const auto& c : conditions) output.writeProperty(indent, "ShowIncludes", c.first, c.second->showIncludes);
	for (const auto& c : conditions) output.writeProperty(indent, "SmallerTypeCheck", c.first, c.second->smallerTypeCheck);
	for (const auto& c : conditions) output.writeProperty(indent, "StringPooling", c.first, c.second->stringPooling);
	for (const auto& c : conditions) output.writeProperty(indent, "StructMemberAlignment", c.first, c.second->structMemberAlignment);
	for (const auto& c : conditions) output.writeProperty(indent, "SuppressStartupBanner", c.first, c.second->suppressStartupBanner);
	for (const auto& c : conditions) output.writeProperty(indent, "TrackerLogDirectory", c.first, c.second->trackerLogDirectory);
	for (const auto& c : conditions) output.writeProperty(indent, "TreatSpecificWarningsAsErrors", c.first, c.second->treatSpecificWarningsAsErrors);
	for (const auto& c : conditions) output.writeProperty(indent, "TreatWarningAsError", c.first, c.second->treatWarningAsError);
	for (const auto& c : conditions) output.writeProperty(indent, "TreatWChar_tAsBuiltInType", c.first, c.second->treatWChar_tAsBuiltInType);
	for (const auto& c : conditions) output.writeProperty(indent, "UndefineAllPreprocessorDefinitions", c.first, c.second->undefineAllPreprocessorDefinitions);
	for (const auto& c : conditions) output.writeProperty(indent, "UndefinePreprocessorDefinitions", c.first, c.second->undefinePreprocessorDefinitions);
	for (const auto& c : conditions) output.writeProperty(indent, "UseFullPaths", c.first, c.second->useFullPaths);
	for (const auto& c : conditions) output.writeProperty(indent, "UseUnicodeForAssemblerListing", c.first, c.second->useUnicodeForAssemblerListing);
	for (const auto& c : conditions) output.writeProperty(indent, "WarningLevel", c.first, c.second->warningLevel);
	for (const auto& c : conditions) output.writeProperty(indent, "WholeProgramOptimization", c.first, c.second->wholeProgramOptimization);
	for (const auto& c : conditions) output.writeProperty(indent, "XMLDocumentationFileName", c.first, c.second->xmlDocumentationFileName);
	for (const auto& c : conditions) output.writeProperty(indent, "MinimalRebuildFromTracking", c.first, c.second->minimalRebuildFromTracking);
	for (const auto& c : conditions) output.writeProperty(indent, "TrackFileAccess", c.first, c.second->trackFileAccess);
}

/////////////////////////////////////////////////////////////////////////////

void VSLinkTask::writeProperties(VSLineWriter& output, int indent, const std::map<std::string, std::shared_ptr<VSLinkTask>>& conditions)
{
	std::vector<std::pair<std::string, VSLinkTask*>> c;
	for (const auto& it : conditions)
		c.push_back({ it.first, it.second.get() });
	writeProperties(output, indent, c);
}

void VSLinkTask::writeProperties(VSLineWriter& output, int indent, const std::vector<std::pair<std::string, VSLinkTask*>>& conditions)
{
	for (const auto& c : conditions) output.writeProperty(indent, "AdditionalDependencies", c.first, c.second->additionalDependencies);
	for (const auto& c : conditions) output.writeProperty(indent, "AdditionalLibraryDirectories", c.first, c.second->additionalLibraryDirectories);
	for (const auto& c : conditions) output.writeProperty(indent, "AdditionalManifestDependencies", c.first, c.second->additionalManifestDependencies);
	for (const auto& c : conditions) output.writeProperty(indent, "AdditionalOptions", c.first, c.second->additionalOptions);
	for (const auto& c : conditions) output.writeProperty(indent, "AddModuleNamesToAssembly", c.first, c.second->addModuleNamesToAssembly);
	for (const auto& c : conditions) output.writeProperty(indent, "AllowIsolation", c.first, c.second->allowIsolation);
	for (const auto& c : conditions) output.writeProperty(indent, "AssemblyDebug", c.first, c.second->assemblyDebug);
	for (const auto& c : conditions) output.writeProperty(indent, "AssemblyLinkResource", c.first, c.second->assemblyLinkResource);
	for (const auto& c : conditions) output.writeProperty(indent, "AttributeFileTracking", c.first, c.second->attributeFileTracking);
	for (const auto& c : conditions) output.writeProperty(indent, "BaseAddress", c.first, c.second->baseAddress);
	for (const auto& c : conditions) output.writeProperty(indent, "BuildingInIDE", c.first, c.second->buildingInIDE);
	for (const auto& c : conditions) output.writeProperty(indent, "CLRImageType", c.first, c.second->clrImageType);
	for (const auto& c : conditions) output.writeProperty(indent, "CLRSupportLastError", c.first, c.second->clrSupportLastError);
	for (const auto& c : conditions) output.writeProperty(indent, "CLRThreadAttribute", c.first, c.second->clrThreadAttribute);
	for (const auto& c : conditions) output.writeProperty(indent, "CLRUnmanagedCodeCheck", c.first, c.second->clrUnmanagedCodeCheck);
	for (const auto& c : conditions) output.writeProperty(indent, "CreateHotPatchableImage", c.first, c.second->createHotPatchableImage);
	for (const auto& c : conditions) output.writeProperty(indent, "DataExecutionPrevention", c.first, c.second->dataExecutionPrevention);
	for (const auto& c : conditions) output.writeProperty(indent, "DelayLoadDLLs", c.first, c.second->delayLoadDLLs);
	for (const auto& c : conditions) output.writeProperty(indent, "DelaySign", c.first, c.second->delaySign);
	for (const auto& c : conditions) output.writeProperty(indent, "Driver", c.first, c.second->driver);
	for (const auto& c : conditions) output.writeProperty(indent, "EmbedManagedResourceFile", c.first, c.second->embedManagedResourceFile);
	for (const auto& c : conditions) output.writeProperty(indent, "EnableCOMDATFolding", c.first, c.second->enableCOMDATFolding);
	for (const auto& c : conditions) output.writeProperty(indent, "EnableUAC", c.first, c.second->enableUAC);
	for (const auto& c : conditions) output.writeProperty(indent, "EntryPointSymbol", c.first, c.second->entryPointSymbol);
	for (const auto& c : conditions) output.writeProperty(indent, "FixedBaseAddress", c.first, c.second->fixedBaseAddress);
	for (const auto& c : conditions) output.writeProperty(indent, "ForceFileOutput", c.first, c.second->forceFileOutput);
	for (const auto& c : conditions) output.writeProperty(indent, "ForceSymbolReferences", c.first, c.second->forceSymbolReferences);
	for (const auto& c : conditions) output.writeProperty(indent, "FunctionOrder", c.first, c.second->functionOrder);
	for (const auto& c : conditions) output.writeProperty(indent, "GenerateDebugInformation", c.first, c.second->generateDebugInformation);
	for (const auto& c : conditions) output.writeProperty(indent, "GenerateManifest", c.first, c.second->generateManifest);
	for (const auto& c : conditions) output.writeProperty(indent, "GenerateMapFile", c.first, c.second->generateMapFile);
	for (const auto& c : conditions) output.writeProperty(indent, "HeapCommitSize", c.first, c.second->heapCommitSize);
	for (const auto& c : conditions) output.writeProperty(indent, "HeapReserveSize", c.first, c.second->heapReserveSize);
	for (const auto& c : conditions) output.writeProperty(indent, "IgnoreAllDefaultLibraries", c.first, c.second->ignoreAllDefaultLibraries);
	for (const auto& c : conditions) output.writeProperty(indent, "IgnoreEmbeddedIDL", c.first, c.second->ignoreEmbeddedIDL);
	for (const auto& c : conditions) output.writeProperty(indent, "IgnoreImportLibrary", c.first, c.second->ignoreImportLibrary);
	for (const auto& c : conditions) output.writeProperty(indent, "IgnoreSpecificDefaultLibraries", c.first, c.second->ignoreSpecificDefaultLibraries);
	for (const auto& c : conditions) output.writeProperty(indent, "ImageHasSafeExceptionHandlers", c.first, c.second->imageHasSafeExceptionHandlers);
	for (const auto& c : conditions) output.writeProperty(indent, "ImportLibrary", c.first, c.second->importLibrary);
	for (const auto& c : conditions) output.writeProperty(indent, "KeyContainer", c.first, c.second->keyContainer);
	for (const auto& c : conditions) output.writeProperty(indent, "KeyFile", c.first, c.second->keyFile);
	for (const auto& c : conditions) output.writeProperty(indent, "LargeAddressAware", c.first, c.second->largeAddressAware);
	for (const auto& c : conditions) output.writeProperty(indent, "LinkDLL", c.first, c.second->linkDLL);
	for (const auto& c : conditions) output.writeProperty(indent, "LinkErrorReporting", c.first, c.second->linkErrorReporting);
	for (const auto& c : conditions) output.writeProperty(indent, "LinkIncremental", c.first, c.second->linkIncremental);
	for (const auto& c : conditions) output.writeProperty(indent, "LinkLibraryDependencies", c.first, c.second->linkLibraryDependencies);
	for (const auto& c : conditions) output.writeProperty(indent, "LinkStatus", c.first, c.second->linkStatus);
	for (const auto& c : conditions) output.writeProperty(indent, "LinkTimeCodeGeneration", c.first, c.second->linkTimeCodeGeneration);
	for (const auto& c : conditions) output.writeProperty(indent, "ManifestFile", c.first, c.second->manifestFile);
	for (const auto& c : conditions) output.writeProperty(indent, "MapExports", c.first, c.second->mapExports);
	for (const auto& c : conditions) output.writeProperty(indent, "MapFileName", c.first, c.second->mapFileName);
	for (const auto& c : conditions) output.writeProperty(indent, "MergedIDLBaseFileName", c.first, c.second->mergedIDLBaseFileName);
	for (const auto& c : conditions) output.writeProperty(indent, "MergeSections", c.first, c.second->mergeSections);
	for (const auto& c : conditions) output.writeProperty(indent, "MidlCommandFile", c.first, c.second->midlCommandFile);
	for (const auto& c : conditions) output.writeProperty(indent, "MinimumRequiredVersion", c.first, c.second->minimumRequiredVersion);
	for (const auto& c : conditions) output.writeProperty(indent, "ModuleDefinitionFile", c.first, c.second->moduleDefinitionFile);
	for (const auto& c : conditions) output.writeProperty(indent, "MSDOSStubFileName", c.first, c.second->msdosStubFileName);
	for (const auto& c : conditions) output.writeProperty(indent, "NoEntryPoint", c.first, c.second->noEntryPoint);
	for (const auto& c : conditions) output.writeProperty(indent, "ObjectFiles", c.first, c.second->objectFiles);
	for (const auto& c : conditions) output.writeProperty(indent, "OptimizeReferences", c.first, c.second->optimizeReferences);
	for (const auto& c : conditions) output.writeProperty(indent, "OutputFile", c.first, c.second->outputFile);
	for (const auto& c : conditions) output.writeProperty(indent, "PerUserRedirection", c.first, c.second->perUserRedirection);
	for (const auto& c : conditions) output.writeProperty(indent, "PreventDllBinding", c.first, c.second->preventDllBinding);
	for (const auto& c : conditions) output.writeProperty(indent, "Profile", c.first, c.second->profile);
	for (const auto& c : conditions) output.writeProperty(indent, "ProfileGuidedDatabase", c.first, c.second->profileGuidedDatabase);
	for (const auto& c : conditions) output.writeProperty(indent, "ProgramDatabaseFile", c.first, c.second->programDatabaseFile);
	for (const auto& c : conditions) output.writeProperty(indent, "RandomizedBaseAddress", c.first, c.second->randomizedBaseAddress);
	for (const auto& c : conditions) output.writeProperty(indent, "RegisterOutput", c.first, c.second->registerOutput);
	for (const auto& c : conditions) output.writeProperty(indent, "SectionAlignment", c.first, c.second->sectionAlignment);
	for (const auto& c : conditions) output.writeProperty(indent, "SetChecksum", c.first, c.second->setChecksum);
	for (const auto& c : conditions) output.writeProperty(indent, "ShowProgress", c.first, c.second->showProgress);
	for (const auto& c : conditions) output.writeProperty(indent, "SpecifySectionAttributes", c.first, c.second->specifySectionAttributes);
	for (const auto& c : conditions) output.writeProperty(indent, "StackCommitSize", c.first, c.second->stackCommitSize);
	for (const auto& c : conditions) output.writeProperty(indent, "StackReserveSize", c.first, c.second->stackReserveSize);
	for (const auto& c : conditions) output.writeProperty(indent, "StripPrivateSymbols", c.first, c.second->stripPrivateSymbols);
	for (const auto& c : conditions) output.writeProperty(indent, "SubSystem", c.first, c.second->subSystem);
	for (const auto& c : conditions) output.writeProperty(indent, "SupportNobindOfDelayLoadedDLL", c.first, c.second->supportNobindOfDelayLoadedDLL);
	for (const auto& c : conditions) output.writeProperty(indent, "SupportUnloadOfDelayLoadedDLL", c.first, c.second->supportUnloadOfDelayLoadedDLL);
	for (const auto& c : conditions) output.writeProperty(indent, "SuppressStartupBanner", c.first, c.second->suppressStartupBanner);
	for (const auto& c : conditions) output.writeProperty(indent, "SwapRunFromCD", c.first, c.second->swapRunFromCD);
	for (const auto& c : conditions) output.writeProperty(indent, "SwapRunFromNET", c.first, c.second->swapRunFromNET);
	for (const auto& c : conditions) output.writeProperty(indent, "TargetMachine", c.first, c.second->targetMachine);
	for (const auto& c : conditions) output.writeProperty(indent, "TerminalServerAware", c.first, c.second->terminalServerAware);
	for (const auto& c : conditions) output.writeProperty(indent, "TrackerLogDirectory", c.first, c.second->trackerLogDirectory);
	for (const auto& c : conditions) output.writeProperty(indent, "TreatLinkerWarningAsErrors", c.first, c.second->treatLinkerWarningAsErrors);
	for (const auto& c : conditions) output.writeProperty(indent, "TurnOffAssemblyGeneration", c.first, c.second->turnOffAssemblyGeneration);
	for (const auto& c : conditions) output.writeProperty(indent, "TypeLibraryFile", c.first, c.second->typeLibraryFile);
	for (const auto& c : conditions) output.writeProperty(indent, "TypeLibraryResourceID", c.first, c.second->typeLibraryResourceID);
	for (const auto& c : conditions) output.writeProperty(indent, "UACExecutionLevel", c.first, c.second->uacExecutionLevel);
	for (const auto& c : conditions) output.writeProperty(indent, "UACUIAccess", c.first, c.second->uacUIAccess);
	for (const auto& c : conditions) output.writeProperty(indent, "UseLibraryDependencyInputs", c.first, c.second->useLibraryDependencyInputs);
	for (const auto& c : conditions) output.writeProperty(indent, "Version", c.first, c.second->version);
}

/////////////////////////////////////////////////////////////////////////////

void VSLibTask::writeProperties(VSLineWriter& output, int indent, const std::map<std::string, std::shared_ptr<VSLibTask>>& conditions)
{
	std::vector<std::pair<std::string, VSLibTask*>> c;
	for (const auto& it : conditions)
		c.push_back({ it.first, it.second.get() });
	writeProperties(output, indent, c);
}

void VSLibTask::writeProperties(VSLineWriter& output, int indent, const std::vector<std::pair<std::string, VSLibTask*>>& conditions)
{
	for (const auto& c : conditions) output.writeProperty(indent, "AdditionalDependencies", c.first, c.second->additionalDependencies);
	for (const auto& c : conditions) output.writeProperty(indent, "AdditionalLibraryDirectories", c.first, c.second->additionalLibraryDirectories);
	for (const auto& c : conditions) output.writeProperty(indent, "AdditionalOptions", c.first, c.second->additionalOptions);
	for (const auto& c : conditions) output.writeProperty(indent, "DisplayLibrary", c.first, c.second->displayLibrary);
	for (const auto& c : conditions) output.writeProperty(indent, "ErrorReporting", c.first, c.second->errorReporting);
	for (const auto& c : conditions) output.writeProperty(indent, "ExportNamedFunctions", c.first, c.second->exportNamedFunctions);
	for (const auto& c : conditions) output.writeProperty(indent, "ForceSymbolReferences", c.first, c.second->forceSymbolReferences);
	for (const auto& c : conditions) output.writeProperty(indent, "IgnoreAllDefaultLibraries", c.first, c.second->ignoreAllDefaultLibraries);
	for (const auto& c : conditions) output.writeProperty(indent, "IgnoreSpecificDefaultLibraries", c.first, c.second->ignoreSpecificDefaultLibraries);
	for (const auto& c : conditions) output.writeProperty(indent, "LinkLibraryDependencies", c.first, c.second->linkLibraryDependencies);
	for (const auto& c : conditions) output.writeProperty(indent, "LinkTimeCodeGeneration", c.first, c.second->linkTimeCodeGeneration);
	for (const auto& c : conditions) output.writeProperty(indent, "MinimumRequiredVersion", c.first, c.second->minimumRequiredVersion);
	for (const auto& c : conditions) output.writeProperty(indent, "ModuleDefinitionFile", c.first, c.second->moduleDefinitionFile);
	for (const auto& c : conditions) output.writeProperty(indent, "Name", c.first, c.second->name);
	for (const auto& c : conditions) output.writeProperty(indent, "OutputFile", c.first, c.second->outputFile);
	for (const auto& c : conditions) output.writeProperty(indent, "RemoveObjects", c.first, c.second->removeObjects);
	for (const auto& c : conditions) output.writeProperty(indent, "SubSystem", c.first, c.second->subSystem);
	for (const auto& c : conditions) output.writeProperty(indent, "SuppressStartupBanner", c.first, c.second->suppressStartupBanner);
	for (const auto& c : conditions) output.writeProperty(indent, "TargetMachine", c.first, c.second->targetMachine);
	for (const auto& c : conditions) output.writeProperty(indent, "TrackerLogDirectory", c.first, c.second->trackerLogDirectory);
	for (const auto& c : conditions) output.writeProperty(indent, "TreatLibWarningAsErrors", c.first, c.second->treatLibWarningAsErrors);
	for (const auto& c : conditions) output.writeProperty(indent, "UseUnicodeResponseFiles", c.first, c.second->useUnicodeResponseFiles);
	for (const auto& c : conditions) output.writeProperty(indent, "Verbose", c.first, c.second->verbose);
}

/////////////////////////////////////////////////////////////////////////////

void VSResourceTask::writeProperties(VSLineWriter& output, int indent, const std::map<std::string, std::shared_ptr<VSResourceTask>>& conditions)
{
	std::vector<std::pair<std::string, VSResourceTask*>> c;
	for (const auto& it : conditions)
		c.push_back({ it.first, it.second.get() });
	writeProperties(output, indent, c);
}

void VSResourceTask::writeProperties(VSLineWriter& output, int indent, const std::vector<std::pair<std::string, VSResourceTask*>>& conditions)
{
	for (const auto& c : conditions) output.writeProperty(indent, "AdditionalIncludeDirectories", c.first, c.second->additionalIncludeDirectories);
	for (const auto& c : conditions) output.writeProperty(indent, "AdditionalOptions", c.first, c.second->additionalOptions);
	for (const auto& c : conditions) output.writeProperty(indent, "Culture", c.first, c.second->culture);
	for (const auto& c : conditions) output.writeProperty(indent, "IgnoreStandardIncludePath", c.first, c.second->ignoreStandardIncludePath);
	for (const auto& c : conditions) output.writeProperty(indent, "NullTerminateStrings", c.first, c.second->nullTerminateStrings);
	for (const auto& c : conditions) output.writeProperty(indent, "PreprocessorDefinitions", c.first, c.second->preprocessorDefinitions);
	for (const auto& c : conditions) output.writeProperty(indent, "ResourceOutputFileName", c.first, c.second->resourceOutputFileName);
	for (const auto& c : conditions) output.writeProperty(indent, "ShowProgress", c.first, c.second->showProgress);
	for (const auto& c : conditions) output.writeProperty(indent, "SuppressStartupBanner", c.first, c.second->suppressStartupBanner);
	for (const auto& c : conditions) output.writeProperty(indent, "TrackerLogDirectory", c.first, c.second->trackerLogDirectory);
	for (const auto& c : conditions) output.writeProperty(indent, "UndefinePreprocessorDefinitions", c.first, c.second->undefinePreprocessorDefinitions);
}

/////////////////////////////////////////////////////////////////////////////

void VSManifestTask::writeProperties(VSLineWriter& output, int indent, const std::map<std::string, std::shared_ptr<VSManifestTask>>& conditions)
{
	std::vector<std::pair<std::string, VSManifestTask*>> c;
	for (const auto& it : conditions)
		c.push_back({ it.first, it.second.get() });
	writeProperties(output, indent, c);
}

void VSManifestTask::writeProperties(VSLineWriter& output, int indent, const std::vector<std::pair<std::string, VSManifestTask*>>& conditions)
{
	for (const auto& c : conditions) output.writeProperty(indent, "AdditionalManifestFiles", c.first, c.second->additionalManifestFiles);
	for (const auto& c : conditions) output.writeProperty(indent, "AdditionalOptions", c.first, c.second->additionalOptions);
	for (const auto& c : conditions) output.writeProperty(indent, "AssemblyIdentity", c.first, c.second->assemblyIdentity);
	for (const auto& c : conditions) output.writeProperty(indent, "ComponentFileName", c.first, c.second->componentFileName);
	for (const auto& c : conditions) output.writeProperty(indent, "DependencyInformationFile", c.first, c.second->dependencyInformationFile);
	for (const auto& c : conditions) output.writeProperty(indent, "EmbedManifest", c.first, c.second->embedManifest);
	for (const auto& c : conditions) output.writeProperty(indent, "EnableDPIAwareness", c.first, c.second->enableDPIAwareness);
	for (const auto& c : conditions) output.writeProperty(indent, "GenerateCatalogFiles", c.first, c.second->generateCatalogFiles);
	for (const auto& c : conditions) output.writeProperty(indent, "GenerateCategoryTags", c.first, c.second->generateCategoryTags);
	for (const auto& c : conditions) output.writeProperty(indent, "InputResourceManifests", c.first, c.second->inputResourceManifests);
	for (const auto& c : conditions) output.writeProperty(indent, "ManifestFromManagedAssembly", c.first, c.second->manifestFromManagedAssembly);
	for (const auto& c : conditions) output.writeProperty(indent, "OutputManifestFile", c.first, c.second->outputManifestFile);
	for (const auto& c : conditions) output.writeProperty(indent, "OutputResourceManifests", c.first, c.second->outputResourceManifests);
	for (const auto& c : conditions) output.writeProperty(indent, "RegistrarScriptFile", c.first, c.second->registrarScriptFile);
	for (const auto& c : conditions) output.writeProperty(indent, "ReplacementsFile", c.first, c.second->replacementsFile);
	for (const auto& c : conditions) output.writeProperty(indent, "ResourceOutputFileName", c.first, c.second->resourceOutputFileName);
	for (const auto& c : conditions) output.writeProperty(indent, "SuppressDependencyElement", c.first, c.second->suppressDependencyElement);
	for (const auto& c : conditions) output.writeProperty(indent, "SuppressStartupBanner", c.first, c.second->suppressStartupBanner);
	for (const auto& c : conditions) output.writeProperty(indent, "TrackerLogDirectory", c.first, c.second->trackerLogDirectory);
	for (const auto& c : conditions) output.writeProperty(indent, "TypeLibraryFile", c.first, c.second->typeLibraryFile);
	for (const auto& c : conditions) output.writeProperty(indent, "UpdateFileHashes", c.first, c.second->updateFileHashes);
	for (const auto& c : conditions) output.writeProperty(indent, "UpdateFileHashesSearchPath", c.first, c.second->updateFileHashesSearchPath);
	for (const auto& c : conditions) output.writeProperty(indent, "VerboseOutput", c.first, c.second->verboseOutput);
}

/////////////////////////////////////////////////////////////////////////////

void VSLineWriter::writeLine(const std::string& text)
{
	lines += text;
	lines += "\r\n";
}

void VSLineWriter::writeProperty(int indent, const std::string& propName, const std::string& condition, const std::string& propValue)
{
	if (!propValue.empty())
	{
		line.clear();
		line.resize(indent, ' ');
		line += '<';
		line += propName;
		if (!condition.empty())
		{
			line += " Condition=\"";
			line += condition;
			line += '"';
		}
		line += '>';
		line += propValue;
		line += '<';
		line += '/';
		line += propName;
		line += '>';
		writeLine(line);
	}
}

void VSLineWriter::writeProperty(int indent, const std::string& propName, const std::string& condition, const std::vector<std::string>& propValues)
{
	if (!propValues.empty())
	{
		line.clear();
		line.resize(indent, ' ');
		line += '<';
		line += propName;
		if (!condition.empty())
		{
			line += " Condition=\"";
			line += condition;
			line += '"';
		}
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
