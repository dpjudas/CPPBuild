#pragma once

class VSLineWriter;
class VSSolution;
class VSCppProject;
class VSSolutionConfiguration;
class VSGenerator;

class VSLineWriter
{
public:
	void writeLine(const std::string& text);
	void writeProperty(int indent, const std::string& propName, const std::string& propValue);
	void writeProperty(int indent, const std::string& propName, const std::vector<std::string>& propValues);
	void save(const std::string& filename);

private:
	std::string lines;
	std::string line;
};

class VSSolution
{
public:
	VSSolution(const std::string& name, const std::string& location, const std::string& solutionGuid);

	void generate();

	std::string name;
	std::string location;
	std::string visualStudioVersion;// = "17.14.36203.30 d17.14";
	std::string minimumVisualStudioVersion;// = "10.0.40219.1";
	std::string solutionGuid;
	std::vector<std::unique_ptr<VSCppProject>> projects;
	std::vector<std::unique_ptr<VSSolutionConfiguration>> configurations;
};

class VSSolutionConfiguration
{
public:
	VSSolutionConfiguration(const std::string& name = "Release", const std::string& platform = "x64") : name(name), platform(platform) { }

	std::string name;
	std::string platform;
};

class VSCppProjectConfiguration
{
public:
	VSCppProjectConfiguration(const std::string& name = "Release", const std::string& platform = "x64") : name(name), platform(platform) { }

	std::string name;
	std::string platform;

	struct 
	{
		std::string configurationType = "Application";
		std::string useDebugLibraries = "false";
		std::string platformToolset = "v143";
		std::string characterSet = "Unicode";
		std::string wholeProgramOptimization = "true";
		std::string linkIncremental = "false";
		std::string outDir = "$(SolutionDir)Build\\$(Configuration)\\$(Platform)\\bin\\";
		std::string intDir = "$(SolutionDir)Build\\$(Configuration)\\$(Platform)\\obj\\$(ProjectName)\\";
		std::string nmakeBuildCommandLine;
		std::string nmakeOutput;
		std::string nmakeCleanCommandLine;
		std::string nmakeReBuildCommandLine;
		std::vector<std::string> nmakePreprocessorDefinitions;
		std::vector<std::string> nmakeIncludeSearchPath;
	} general;

	// https://learn.microsoft.com/en-us/visualstudio/msbuild/cl-task
	struct
	{
		std::vector<std::string> additionalIncludeDirectories;
		std::string additionalOptions;
		std::vector<std::string> additionalUsingDirectories;
		std::string alwaysAppend;
		std::string assemblerListingLocation;
		std::string assemblerOutput;
		std::string basicRuntimeChecks;
		std::string browseInformation;
		std::string browseInformationFile;
		std::string bufferSecurityCheck;
		std::string buildingInIDE;
		std::string callingConvention;
		std::string compileAs;
		std::string compileAsManaged;
		std::string createHotpatchableImage;
		std::string debugInformationFormat;
		std::string disableLanguageExtensions;
		std::vector<std::string> disableSpecificWarnings;
		std::string enableEnhancedInstructionSet;
		std::string enableFiberSafeOptimizations;
		std::string enablePREfast;
		std::string errorReporting;
		std::string exceptionHandling;
		std::string expandAttributedSource;
		std::string favorSizeOrSpeed;
		std::string floatingPointExceptions;
		std::string floatingPointModel;
		std::string forceConformanceInForLoopScope;
		std::vector<std::string> forcedIncludeFiles;
		std::vector<std::string> forcedUsingFiles;
		std::string functionLevelLinking;
		std::string generateXMLDocumentationFiles;
		std::string ignoreStandardIncludePath;
		std::string inlineFunctionExpansion;
		std::string intrinsicFunctions;
		std::string sdlCheck;
		std::string minimalRebuild;
		std::string multiProcessorCompilation;
		std::string objectFileName;
		std::vector<std::string> objectFiles;
		std::string omitDefaultLibName;
		std::string omitFramePointers;
		std::string openMPSupport;
		std::string optimization;
		std::string precompiledHeader;
		std::string precompiledHeaderFile;
		std::string precompiledHeaderOutputFile;
		std::string preprocessKeepComments;
		std::vector<std::string> preprocessorDefinitions;
		std::string preprocessOutput;
		std::string preprocessOutputPath;
		std::string preprocessSuppressLineNumbers;
		std::string preprocessToFile;
		std::string processorNumber;
		std::string programDataBaseFileName;
		std::string conformanceMode;
		std::string runtimeLibrary;
		std::string runtimeTypeInfo;
		std::string languageStandard;
		std::string showIncludes;
		std::string smallerTypeCheck;
		std::string stringPooling;
		std::string structMemberAlignment;
		std::string suppressStartupBanner;
		std::string trackerLogDirectory;
		std::vector<std::string> treatSpecificWarningsAsErrors;
		std::string treatWarningAsError;
		std::string treatWChar_tAsBuiltInType;
		std::string undefineAllPreprocessorDefinitions;
		std::vector<std::string> undefinePreprocessorDefinitions;
		std::string useFullPaths;
		std::string useUnicodeForAssemblerListing;
		std::string warningLevel;
		std::string wholeProgramOptimization;
		std::string xmlDocumentationFileName;
		std::string minimalRebuildFromTracking;
		std::string trackFileAccess;
	} clCompile;

	// https://learn.microsoft.com/en-us/visualstudio/msbuild/link-task
	struct
	{
		std::vector<std::string> additionalDependencies;
		std::vector<std::string> additionalLibraryDirectories;
		std::vector<std::string> additionalManifestDependencies;
		std::string additionalOptions;
		std::vector<std::string> addModuleNamesToAssembly;
		std::string allowIsolation;
		std::string assemblyDebug;
		std::vector<std::string> assemblyLinkResource;
		std::string attributeFileTracking;
		std::string baseAddress;
		std::string buildingInIDE;
		std::string clrImageType;
		std::string clrSupportLastError;
		std::string clrThreadAttribute;
		std::string clrUnmanagedCodeCheck;
		std::string createHotPatchableImage;
		std::string dataExecutionPrevention;
		std::vector<std::string> delayLoadDLLs;
		std::string delaySign;
		std::string driver;
		std::vector<std::string> embedManagedResourceFile;
		std::string enableCOMDATFolding;
		std::string enableUAC;
		std::string entryPointSymbol;
		std::string fixedBaseAddress;
		std::string forceFileOutput;
		std::vector<std::string> forceSymbolReferences;
		std::string functionOrder;
		std::string generateDebugInformation;
		std::string generateManifest;
		std::string generateMapFile;
		std::string heapCommitSize;
		std::string heapReserveSize;
		std::string ignoreAllDefaultLibraries;
		std::string ignoreEmbeddedIDL;
		std::string ignoreImportLibrary;
		std::vector<std::string> ignoreSpecificDefaultLibraries;
		std::string imageHasSafeExceptionHandlers;
		std::string importLibrary;
		std::string keyContainer;
		std::string keyFile;
		std::string largeAddressAware;
		std::string linkDLL;
		std::string linkErrorReporting;
		std::string linkIncremental;
		std::string linkLibraryDependencies;
		std::string linkStatus;
		std::string linkTimeCodeGeneration;
		std::string manifestFile;
		std::string mapExports;
		std::string mapFileName;
		std::string mergedIDLBaseFileName;
		std::string mergeSections;
		std::string midlCommandFile;
		std::string minimumRequiredVersion;
		std::string moduleDefinitionFile;
		std::string msdosStubFileName;
		std::string noEntryPoint;
		std::vector<std::string> objectFiles;
		std::string optimizeReferences;
		std::string outputFile;
		std::string perUserRedirection;
		std::string preventDllBinding;
		std::string profile;
		std::string profileGuidedDatabase;
		std::string programDatabaseFile;
		std::string randomizedBaseAddress;
		std::string registerOutput;
		std::string sectionAlignment;
		std::string setChecksum;
		std::string showProgress;
		std::vector<std::string> specifySectionAttributes;
		std::string stackCommitSize;
		std::string stackReserveSize;
		std::string stripPrivateSymbols;
		std::string subSystem;
		std::string supportNobindOfDelayLoadedDLL;
		std::string supportUnloadOfDelayLoadedDLL;
		std::string suppressStartupBanner;
		std::string swapRunFromCD;
		std::string swapRunFromNET;
		std::string targetMachine;
		std::string terminalServerAware;
		std::string trackerLogDirectory;
		std::string treatLinkerWarningAsErrors;
		std::string turnOffAssemblyGeneration;
		std::string typeLibraryFile;
		std::string typeLibraryResourceID;
		std::string uacExecutionLevel;
		std::string uacUIAccess;
		std::string useLibraryDependencyInputs;
		std::string version;
	} link;

	struct
	{
		std::vector<std::string> additionalDependencies;
		std::vector<std::string> additionalLibraryDirectories;
		std::string additionalOptions;
		std::string displayLibrary;
		std::string errorReporting;
		std::vector<std::string> exportNamedFunctions;
		std::vector<std::string> forceSymbolReferences;
		std::string ignoreAllDefaultLibraries;
		std::vector<std::string> ignoreSpecificDefaultLibraries;
		std::string linkLibraryDependencies;
		std::string linkTimeCodeGeneration;
		std::string minimumRequiredVersion;
		std::string moduleDefinitionFile;
		std::string name;
		std::string outputFile;
		std::vector<std::string> removeObjects;
		std::string subSystem;
		std::string suppressStartupBanner;
		std::string targetMachine;
		std::string trackerLogDirectory;
		std::string treatLibWarningAsErrors;
		std::string useUnicodeResponseFiles;
		std::string verbose;
	} lib;

	struct
	{
		std::vector<std::string> additionalIncludeDirectories;
		std::string additionalOptions;
		std::string culture;
		std::string ignoreStandardIncludePath;
		std::string nullTerminateStrings;
		std::vector<std::string> preprocessorDefinitions;
		std::string resourceOutputFileName;
		std::string showProgress;
		std::string suppressStartupBanner;
		std::string trackerLogDirectory;
		std::vector<std::string> undefinePreprocessorDefinitions;
	} rc;

	struct
	{
		std::string message = "Running CPPBuild generate";
		std::string command;
		std::vector<std::string> additionalInputs;
		std::vector<std::string> outputs;
		std::string linkObjects = "false";
	} customBuildFile;
};

class VSCppProjectFilter
{
public:
	VSCppProjectFilter(const std::string& name, const std::string& guid) : name(name), guid(guid) { }

	std::string name;
	std::string guid;
	std::vector<std::string> sourceFiles;
	std::vector<std::string> headerFiles;
	std::vector<std::string> extraFiles;
	std::string customBuildFile;
};

class VSCppProjectReference
{
public:
	VSCppProjectReference(const std::string& name, const std::string& location, const std::string& guid) : name(name), location(location), guid(guid) {}

	std::string name;
	std::string location;
	std::string guid;
};

class VSCppProject
{
public:
	VSCppProject(const std::string& name, const std::string& location, std::string projectGuid) : name(name), location(location), projectGuid(projectGuid) { }

	std::string name = name;
	std::string location = location;
	std::string typeGuid = "8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942";
	std::string projectGuid;
	std::string vcProjectVersion = "17.0";
	std::string windowsTargetPlatformVersion = "10.0";
	std::vector<std::unique_ptr<VSCppProjectConfiguration>> configurations;
	std::vector<std::string> sourceFiles;
	std::vector<std::string> headerFiles;
	std::vector<std::string> extraFiles;
	std::vector<std::unique_ptr<VSCppProjectFilter>> filters;
	std::vector<VSCppProjectReference> references;

	struct
	{
		std::string file;
		std::string useUtf8Encoding = "Always";
	} customBuildFile;
};

class VSGenerator
{
public:
	void writeSolution(const VSSolution* solution);
	void writeProject(const VSCppProject* project);
	void writeProjectFilters(const VSCppProject* project);

private:
	static std::string toLowerCase(std::string text);
};
