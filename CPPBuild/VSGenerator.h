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
	void save(const std::string& filename);

private:
	std::string lines;
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

	struct
	{
		std::string warningLevel = "Level3";
		std::string functionLevelLinking = "true";
		std::string intrinsicFunctions = "true";
		std::string sdlCheck = "true";
		std::vector<std::string> preprocessorDefinitions;
		std::string conformanceMode = "true";
		std::vector<std::string> additionalIncludeDirectories;
		std::string runtimeLibrary = "MultiThreaded";
		std::string languageStandard = "stdcpp20";
		std::string multiProcessorCompilation = "true";
	} clCompile;

	struct
	{
		std::string subSystem = "Console";
		std::string enableCOMDATFolding = "true";
		std::string optimizeReferences = "true";
		std::string generateDebugInformation = "true";
	} link;

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
