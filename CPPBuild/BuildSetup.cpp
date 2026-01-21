
#include "Precomp.h"
#include "BuildSetup.h"

BuildConfiguration BuildConfiguration::fromJson(const JsonValue& json)
{
	BuildConfiguration config;
	config.name = json["name"].to_string();
	return config;
}

/////////////////////////////////////////////////////////////////////////////

BuildTargetConfiguration BuildTargetConfiguration::fromJson(const JsonValue& json)
{
	BuildTargetConfiguration config;
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
	for (const JsonValue& item : json["packages"].items())
		config.packages.push_back(item.to_string());
	config.buildCommand = json["buildCommand"].to_string();
	config.cleanCommand = json["cleanCommand"].to_string();
	return config;
}

/////////////////////////////////////////////////////////////////////////////

BuildCustomCommand BuildCustomCommand::fromJson(const JsonValue& json)
{
	BuildCustomCommand cmd;
	cmd.inputFile = json["inputFile"].to_string();
	for (const JsonValue& item : json["commands"].items())
		cmd.commands.push_back(item.to_string());
	for (const JsonValue& item : json["outputFiles"].items())
		cmd.outputFiles.push_back(item.to_string());
	cmd.configName = json["configName"].to_string();
	return cmd;
}

/////////////////////////////////////////////////////////////////////////////

BuildFilePropertySet BuildFilePropertySet::fromJson(const JsonValue& json)
{
	BuildFilePropertySet set;
	for (const JsonValue& item : json["files"].items())
		set.files.push_back(item.to_string());
	set.configName = json["configName"].to_string();
	for (const JsonValue& item : json["defines"].items())
		set.defines.push_back(item.to_string());
	for (const JsonValue& item : json["compileOptions"].items())
		set.compileOptions.push_back(item.to_string());
	for (const JsonValue& item : json["includePaths"].items())
		set.includePaths.push_back(item.to_string());
	return set;
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
	target.buildCommand = json["buildCommand"].to_string();
	target.cleanCommand = json["cleanCommand"].to_string();
	for (const JsonValue& item : json["customCommands"].items())
		target.customCommands.push_back(BuildCustomCommand::fromJson(item));
	for (const JsonValue& item : json["defines"].items())
		target.defines.push_back(item.to_string());
	for (const JsonValue& item : json["cCompileOptions"].items())
		target.cCompileOptions.push_back(item.to_string());
	for (const JsonValue& item : json["cxxCompileOptions"].items())
		target.cxxCompileOptions.push_back(item.to_string());
	for (const JsonValue& item : json["linkOptions"].items())
		target.linkOptions.push_back(item.to_string());
	for (const JsonValue& item : json["includePaths"].items())
		target.includePaths.push_back(item.to_string());
	for (const JsonValue& item : json["files"].items())
		target.files.push_back(item.to_string());
	for (const JsonValue& item : json["filePropertySets"].items())
		target.filePropertySets.push_back(BuildFilePropertySet::fromJson(item));
	for (const JsonValue& item : json["filters"].items())
		target.filters.push_back(item.to_string());
	for (const JsonValue& item : json["linkLibraries"].items())
		target.linkLibraries.push_back(item.to_string());
	for (const JsonValue& item : json["libraryPaths"].items())
		target.libraryPaths.push_back(item.to_string());
	for (const JsonValue& item : json["packages"].items())
		target.packages.push_back(item.to_string());
	for (const auto& it : json["configurations"].properties())
		target.configurations[it.first] = BuildTargetConfiguration::fromJson(it.second);
	return target;
}

/////////////////////////////////////////////////////////////////////////////

BuildPackageInstallerConfiguration BuildPackageInstallerConfiguration::fromJson(const JsonValue& json)
{
	BuildPackageInstallerConfiguration config;
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

/////////////////////////////////////////////////////////////////////////////

BuildPackageFile BuildPackageFile::fromJson(const JsonValue& json)
{
	BuildPackageFile file;
	if (json.type() == JsonType::string)
	{
		file.src = json.to_string();
		file.dest = file.src;
	}
	else
	{
		file.src = json["src"].to_string();
		file.dest = json["dest"].to_string();
	}
	return file;
}

/////////////////////////////////////////////////////////////////////////////

BuildPackageInstaller BuildPackageInstaller::fromJson(const JsonValue& json)
{
	BuildPackageInstaller package;
	package.subdirectory = json["subdirectory"].to_string();
	package.name = json["name"].to_string();
	for (const JsonValue& item : json["files"].items())
		package.files.push_back(BuildPackageFile::fromJson(item));
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
		package.configurations[it.first] = BuildPackageInstallerConfiguration::fromJson(it.second);
	return package;
}

/////////////////////////////////////////////////////////////////////////////

BuildPackage BuildPackage::fromJson(const JsonValue& json)
{
	BuildPackage package;
	package.subdirectory = json["subdirectory"].to_string();
	package.source = json["source"].to_string();
	return package;
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
	for (const JsonValue& installer : json["packageInstallers"].items())
		proj.packageInstallers.push_back(BuildPackageInstaller::fromJson(installer));
	for (const JsonValue& package : json["packages"].items())
		proj.packages.push_back(BuildPackage::fromJson(package));
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
