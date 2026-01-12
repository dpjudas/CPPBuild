#pragma once

#include "Json/JsonValue.h"

class BuildConfiguration
{
public:
	std::string name;

	static BuildConfiguration fromJson(const JsonValue& json);
};

class BuildTargetConfiguration
{
public:
	std::vector<std::string> defines;
	std::vector<std::string> cCompileOptions;
	std::vector<std::string> cxxCompileOptions;
	std::vector<std::string> linkOptions;
	std::vector<std::string> includePaths;
	std::vector<std::string> linkLibraries;
	std::vector<std::string> libraryPaths;
	std::vector<std::string> packages;
	std::string buildCommand;
	std::string cleanCommand;

	static BuildTargetConfiguration fromJson(const JsonValue& json);
};

class BuildCustomCommand
{
public:
	std::string inputFile;
	std::vector<std::string> commands;
	std::vector<std::string> outputFiles;
	std::string configName;

	static BuildCustomCommand fromJson(const JsonValue& json);
};

class BuildFilePropertySet
{
public:
	std::vector<std::string> files;

	std::string configName;
	std::vector<std::string> defines;
	std::vector<std::string> compileOptions;
	std::vector<std::string> includePaths;

	static BuildFilePropertySet fromJson(const JsonValue& json);
};

class BuildTarget
{
public:
	std::string subdirectory;
	std::string type;
	std::string name;
	std::vector<std::string> defines;
	std::vector<std::string> cCompileOptions;
	std::vector<std::string> cxxCompileOptions;
	std::vector<std::string> linkOptions;
	std::vector<std::string> includePaths;
	std::vector<std::string> files;
	std::vector<BuildCustomCommand> customCommands;
	std::vector<BuildFilePropertySet> filePropertySets;
	std::vector<std::string> filters;
	std::vector<std::string> linkLibraries;
	std::vector<std::string> libraryPaths;
	std::vector<std::string> packages;
	std::string wwwRootDir;
	std::string cssRootFile;
	std::string htmlShellFile;
	std::string buildCommand;
	std::string cleanCommand;
	std::map<std::string, BuildTargetConfiguration> configurations;

	static BuildTarget fromJson(const JsonValue& json);
};

class BuildInstallerComponent
{
public:
	std::string name;
	std::string msiComponentId;
	std::vector<std::string> files;

	static BuildInstallerComponent fromJson(const JsonValue& json);
};

class BuildInstallerFeature
{
public:
	std::string name;
	std::vector<std::string> components;

	static BuildInstallerFeature fromJson(const JsonValue& json);
};

class BuildInstaller
{
public:
	std::string subdirectory;
	std::string name;
	std::string installDir;
	std::vector<BuildInstallerComponent> components;
	std::vector<BuildInstallerFeature> features;
	std::string msiProductName;
	std::string msiProductVersion;
	std::string msiManufacturer;
	std::string msiProductCode;
	std::string msiUpgradeCode;
	std::string msiPackageCode;
	std::vector<std::string> msiProductKeywords;

	static BuildInstaller fromJson(const JsonValue& json);
};

class BuildPackageInstallerConfiguration
{
public:
	std::vector<std::string> defines;
	std::vector<std::string> cCompileOptions;
	std::vector<std::string> cxxCompileOptions;
	std::vector<std::string> linkOptions;
	std::vector<std::string> includePaths;
	std::vector<std::string> linkLibraries;
	std::vector<std::string> libraryPaths;

	static BuildPackageInstallerConfiguration fromJson(const JsonValue& json);
};

class BuildPackageInstaller
{
public:
	std::string subdirectory;
	std::string name;
	std::vector<std::string> defines;
	std::vector<std::string> cCompileOptions;
	std::vector<std::string> cxxCompileOptions;
	std::vector<std::string> linkOptions;
	std::vector<std::string> includePaths;
	std::vector<std::string> linkLibraries;
	std::vector<std::string> libraryPaths;
	std::map<std::string, BuildPackageInstallerConfiguration> configurations;

	static BuildPackageInstaller fromJson(const JsonValue& json);
};

class BuildPackage
{
public:
	std::string subdirectory;
	std::string source;

	static BuildPackage fromJson(const JsonValue& json);
};

class BuildProject
{
public:
	std::string name;
	std::vector<BuildTarget> targets;
	std::vector<BuildConfiguration> configurations;
	std::vector<BuildInstaller> installers;
	std::vector<BuildPackageInstaller> packageInstallers;
	std::vector<BuildPackage> packages;

	const BuildConfiguration& getConfiguration(const std::string& name) const;
	const BuildTarget& getTarget(const std::string& name) const;

	static BuildProject fromJson(const JsonValue& json);
};

class BuildSetup
{
public:
	int version = 1;
	std::string sourcePath;
	BuildProject project;

	static BuildSetup fromJson(const JsonValue& json);
};
