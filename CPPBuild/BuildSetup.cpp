
#include "Precomp.h"
#include "BuildSetup.h"

BuildConfiguration BuildConfiguration::fromJson(const JsonValue& json)
{
	BuildConfiguration config;
	config.name = json["name"].to_string();
	config.platform = json["platform"].to_string();
	return config;
}

/////////////////////////////////////////////////////////////////////////////

BuildTargetConfiguration BuildTargetConfiguration::fromJson(const JsonValue& json)
{
	BuildTargetConfiguration config;
	for (const JsonValue& item : json["defines"].items())
		config.defines.push_back(item.to_string());
	for (const JsonValue& item : json["includePaths"].items())
		config.includePaths.push_back(item.to_string());
	for (const JsonValue& item : json["linkLibraries"].items())
		config.linkLibraries.push_back(item.to_string());
	for (const JsonValue& item : json["libraryPaths"].items())
		config.libraryPaths.push_back(item.to_string());
	return config;
}

/////////////////////////////////////////////////////////////////////////////

BuildTarget BuildTarget::fromJson(const JsonValue& json)
{
	BuildTarget target;
	target.subdirectory = json["subdirectory"].to_string();
	target.type = json["type"].to_string();
	target.name = json["name"].to_string();
	target.wwwRootDir = json["wwwRootDir"].to_string();
	target.cssRootFile = json["cssRootFile"].to_string();
	target.htmlShellFile = json["htmlShellFile"].to_string();
	for (const JsonValue& item : json["defines"].items())
		target.defines.push_back(item.to_string());
	for (const JsonValue& item : json["includePaths"].items())
		target.includePaths.push_back(item.to_string());
	for (const JsonValue& item : json["files"].items())
		target.files.push_back(item.to_string());
	for (const JsonValue& item : json["filters"].items())
		target.filters.push_back(item.to_string());
	for (const JsonValue& item : json["linkLibraries"].items())
		target.linkLibraries.push_back(item.to_string());
	for (const JsonValue& item : json["libraryPaths"].items())
		target.libraryPaths.push_back(item.to_string());
	for (auto it : json["configurations"].properties())
		target.configurations[it.first] = BuildTargetConfiguration::fromJson(it.second);

	return target;
}

/////////////////////////////////////////////////////////////////////////////

BuildInstallerComponent BuildInstallerComponent::fromJson(const JsonValue& json)
{
	BuildInstallerComponent component;
	component.name = json["name"].to_string();
	component.msiComponentId = json["msiComponentId"].to_string();
	for (const JsonValue& item : json["files"].items())
		component.files.push_back(item.to_string());
	return component;
}

/////////////////////////////////////////////////////////////////////////////

BuildInstallerFeature BuildInstallerFeature::fromJson(const JsonValue& json)
{
	BuildInstallerFeature feature;
	feature.name = json["name"].to_string();
	for (const JsonValue& item : json["components"].items())
		feature.components.push_back(item.to_string());
	return feature;
}

/////////////////////////////////////////////////////////////////////////////

BuildInstaller BuildInstaller::fromJson(const JsonValue& json)
{
	BuildInstaller installer;
	installer.subdirectory = json["subdirectory"].to_string();
	installer.name = json["name"].to_string();
	installer.installDir = json["installDir"].to_string();
	installer.msiProductName = json["msiProductName"].to_string();
	installer.msiProductVersion = json["msiProductVersion"].to_string();
	installer.msiManufacturer = json["msiManufacturer"].to_string();
	installer.msiProductCode = json["msiProductCode"].to_string();
	installer.msiUpgradeCode = json["msiUpgradeCode"].to_string();
	installer.msiPackageCode = json["msiPackageCode"].to_string();
	for (const JsonValue& component : json["components"].items())
		installer.components.push_back(BuildInstallerComponent::fromJson(component));
	for (const JsonValue& feature : json["features"].items())
		installer.features.push_back(BuildInstallerFeature::fromJson(feature));
	for (const JsonValue& item : json["msiProductKeywords"].items())
		installer.msiProductKeywords.push_back(item.to_string());
	return installer;
}

/////////////////////////////////////////////////////////////////////////////

BuildProject BuildProject::fromJson(const JsonValue& json)
{
	BuildProject proj;
	proj.name = json["name"].to_string();
	for (const JsonValue& configuration : json["configurations"].items())
		proj.configurations.push_back(BuildConfiguration::fromJson(configuration));
	for (const JsonValue& target : json["targets"].items())
		proj.targets.push_back(BuildTarget::fromJson(target));
	for (const JsonValue& installer : json["installers"].items())
		proj.installers.push_back(BuildInstaller::fromJson(installer));
	return proj;
}

const BuildConfiguration& BuildProject::getConfiguration(const std::string& name) const
{
	for (const BuildConfiguration& config : configurations)
	{
		if (config.name == name)
			return config;
	}
	throw std::runtime_error("Configuration '" + name + "' not found");
}

const BuildTarget& BuildProject::getTarget(const std::string& name) const
{
	for (const BuildTarget& target : targets)
	{
		if (target.name == name)
			return target;
	}
	throw std::runtime_error("Target '" + name + "' not found");
}

/////////////////////////////////////////////////////////////////////////////

BuildSetup BuildSetup::fromJson(const JsonValue& json)
{
	BuildSetup setup;
	setup.version = json["version"].to_int();
	setup.sourcePath = json["sourcePath"].to_string();
	setup.project = BuildProject::fromJson(json["project"]);
	return setup;
}
