#pragma once

#include "Json/JsonValue.h"

class PackageCopyFile
{
public:
	std::string src;
	std::string dest;

	static PackageCopyFile fromJson(const JsonValue& json);
	JsonValue toJson() const;
};

class PackageConfiguration
{
public:
	std::vector<PackageCopyFile> copyFiles;
	std::vector<std::string> defines;
	std::vector<std::string> cCompileOptions;
	std::vector<std::string> cxxCompileOptions;
	std::vector<std::string> linkOptions;
	std::vector<std::string> includePaths;
	std::vector<std::string> linkLibraries;
	std::vector<std::string> libraryPaths;

	static PackageConfiguration fromJson(const JsonValue& json);
	JsonValue toJson() const;
};

class Package
{
public:
	std::string name;
	std::vector<PackageCopyFile> copyFiles;
	std::vector<std::string> defines;
	std::vector<std::string> cCompileOptions;
	std::vector<std::string> cxxCompileOptions;
	std::vector<std::string> linkOptions;
	std::vector<std::string> includePaths;
	std::vector<std::string> linkLibraries;
	std::vector<std::string> libraryPaths;
	std::map<std::string, PackageConfiguration> configurations;

	static Package fromJson(const JsonValue& json);
	JsonValue toJson() const;
};
