#pragma once

#include "Json/JsonValue.h"

class BuildConfiguration
{
public:
	std::string name;
	std::string platform;

	static BuildConfiguration fromJson(const JsonValue& json);
};

class BuildTargetConfiguration
{
public:
	std::vector<std::string> defines;
	std::vector<std::string> includePaths;
	std::vector<std::string> linkLibraries;
	std::vector<std::string> libraryPaths;

	static BuildTargetConfiguration fromJson(const JsonValue& json);
};

class BuildTarget
{
public:
	std::string subdirectory;
	std::string type;
	std::string name;
	std::vector<std::string> defines;
	std::vector<std::string> includePaths;
	std::vector<std::string> files;
	std::vector<std::string> filters;
	std::vector<std::string> linkLibraries;
	std::vector<std::string> libraryPaths;
	std::string wwwRootDir;
	std::string cssRootFile;
	std::string htmlShellFile;
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

class BuildProject
{
public:
	std::string name;
	std::vector<BuildTarget> targets;
	std::vector<BuildConfiguration> configurations;
	std::vector<BuildInstaller> installers;

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
