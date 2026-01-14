
#include "Precomp.h"
#include "Package.h"

PackageConfiguration PackageConfiguration::fromJson(const JsonValue& json)
{
	PackageConfiguration config;
	for (const JsonValue& item : json["defines"].items())
		config.defines.push_back(item.to_string());
	for (const JsonValue& item : json["cCompileOptions"].items())
		config.cCompileOptions.push_back(item.to_string());
	for (const JsonValue& item : json["cxxCompileOptions"].items())
		config.cxxCompileOptions.push_back(item.to_string());
	for (const JsonValue& item : json["linkOptions"].items())
		config.linkOptions.push_back(item.to_string());
	for (const JsonValue& item : json["includePaths"].items())
		config.includePaths.push_back(item.to_string());
	for (const JsonValue& item : json["linkLibraries"].items())
		config.linkLibraries.push_back(item.to_string());
	for (const JsonValue& item : json["libraryPaths"].items())
		config.libraryPaths.push_back(item.to_string());
	return config;
}

JsonValue PackageConfiguration::toJson() const
{
	auto obj = JsonValue::object();
	obj["defines"] = JsonValue::array(defines);
	obj["cCompileOptions"] = JsonValue::array(cCompileOptions);
	obj["cxxCompileOptions"] = JsonValue::array(cxxCompileOptions);
	obj["linkOptions"] = JsonValue::array(linkOptions);
	obj["includePaths"] = JsonValue::array(includePaths);
	obj["linkLibraries"] = JsonValue::array(linkLibraries);
	obj["libraryPaths"] = JsonValue::array(libraryPaths);
	return obj;
}

/////////////////////////////////////////////////////////////////////////////

Package Package::fromJson(const JsonValue& json)
{
	Package package;
	package.name = json["name"].to_string();
	for (const JsonValue& item : json["defines"].items())
		package.defines.push_back(item.to_string());
	for (const JsonValue& item : json["cCompileOptions"].items())
		package.cCompileOptions.push_back(item.to_string());
	for (const JsonValue& item : json["cxxCompileOptions"].items())
		package.cxxCompileOptions.push_back(item.to_string());
	for (const JsonValue& item : json["linkOptions"].items())
		package.linkOptions.push_back(item.to_string());
	for (const JsonValue& item : json["includePaths"].items())
		package.includePaths.push_back(item.to_string());
	for (const JsonValue& item : json["linkLibraries"].items())
		package.linkLibraries.push_back(item.to_string());
	for (const JsonValue& item : json["libraryPaths"].items())
		package.libraryPaths.push_back(item.to_string());
	for (const auto& it : json["configurations"].properties())
		package.configurations[it.first] = PackageConfiguration::fromJson(it.second);
	return package;
}

JsonValue Package::toJson() const
{
	auto obj = JsonValue::object();
	obj["defines"] = JsonValue::array(defines);
	obj["cCompileOptions"] = JsonValue::array(cCompileOptions);
	obj["cxxCompileOptions"] = JsonValue::array(cxxCompileOptions);
	obj["linkOptions"] = JsonValue::array(linkOptions);
	obj["includePaths"] = JsonValue::array(includePaths);
	obj["linkLibraries"] = JsonValue::array(linkLibraries);
	obj["libraryPaths"] = JsonValue::array(libraryPaths);
	auto configs = JsonValue::object();
	for (const auto& it : configurations)
		configs[it.first] = it.second.toJson();
	obj["configurations"] = std::move(configs);
	return obj;
}
