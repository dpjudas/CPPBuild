
#include "Precomp.h"
#include "CPPBuild.h"
#include "Target.h"
#include "VSWorkspace.h"
#include "MakefileWorkspace.h"
#include "BuildSetup.h"
#include "PackageManager.h"
#include "FileTimeCache.h"
#include "Msi/MSIGenerator.h"
#include "Guid/Guid.h"
#include "IOData/Directory.h"
#include "IOData/FilePath.h"
#include "IOData/File.h"
#include "Javascript/ScriptContext.h"
#include "Json/JsonValue.h"
#include <iostream>
#include <chrono>
#include <unordered_map>
#include <unordered_set>

CPPBuild::CPPBuild(std::string workDir) : workDir(workDir)
{
	cppbuildDir = FilePath::combine(workDir, ".cppbuild");
}

std::string CPPBuild::getGlobalConfigDir()
{
	return FilePath::combine(Directory::localAppData(), "cppbuild");
}

std::string CPPBuild::getLocalConfigDir()
{
	return cppbuildDir;
}

JsonValue CPPBuild::loadProperties(const std::string& filename)
{
	std::string text;
	try
	{
		text = File::readAllText(filename);
	}
	catch (...)
	{
	}

	if (text.empty())
		return JsonValue::object();
	else
		return JsonValue::parse(text);
}

void CPPBuild::setProperty(std::string name, std::string value, bool global)
{
	if (name.empty())
		throw std::runtime_error("Property name is an empty string");

	std::string configDir = global ? getGlobalConfigDir() : getLocalConfigDir();

	Directory::create(configDir);
	Directory::trySetHidden(configDir);

	std::string filename = FilePath::combine(configDir, "properties.json");
	JsonValue properties = loadProperties(filename);
	properties[name] = JsonValue::string(value);
	File::writeAllText(filename, properties.to_json());
}

void CPPBuild::listProperties()
{
	JsonValue properties = loadProperties(FilePath::combine(getGlobalConfigDir(), "properties.json"));
	for (const auto& it : properties.properties())
	{
		std::cout << it.first.c_str() << " = " << it.second.to_string() << " [global]" << std::endl;
	}

	properties = loadProperties(FilePath::combine(getLocalConfigDir(), "properties.json"));
	for (const auto& it : properties.properties())
	{
		std::cout << it.first.c_str() << " = " << it.second.to_string() << " [local]" << std::endl;
	}
}

void CPPBuild::configure(std::string sourcePath)
{
	if (sourcePath.empty())
		sourcePath = Directory::currentDirectory();

	JsonValue properties = loadProperties(FilePath::combine(getGlobalConfigDir(), "properties.json"));
	JsonValue localProperties = loadProperties(FilePath::combine(getLocalConfigDir(), "properties.json"));
	for (const auto& it : localProperties.properties())
		properties[it.first] = it.second;

	JsonValue config = JsonValue::object();
	config["version"] = JsonValue::number(1);
	config["sourcePath"] = JsonValue::string(sourcePath);
	config["project"] = runConfigureScript(sourcePath, properties);

	BuildSetup setup = BuildSetup::fromJson(config);
	validateConfig(setup);

	Directory::create(cppbuildDir);
	Directory::trySetHidden(cppbuildDir);
	File::writeAllText(FilePath::combine(cppbuildDir, "config.json"), config.to_json());

	PackageManager packages(workDir);
	packages.update(setup);

	if (!setup.project.targets.empty())
		generateWorkspace(properties);
}

void CPPBuild::updateMakefile()
{
	JsonValue config = JsonValue::parse(File::readAllText(FilePath::combine(cppbuildDir, "config.json")));
	configure(config["sourcePath"].to_string());
}

void CPPBuild::checkMakefile()
{
	bool needsUpdate = false;
	try
	{
		std::string configFilename = FilePath::combine(cppbuildDir, "config.json");
		int64_t makefileTime = FileTimeCache::getLastWriteTime(configFilename);
		BuildSetup setup = BuildSetup::fromJson(JsonValue::parse(File::readAllText(configFilename)));

		int64_t srcFileTime = FileTimeCache::getLastWriteTime(FilePath::combine(setup.sourcePath, "Configure.js"));
		if (makefileTime < srcFileTime)
		{
			needsUpdate = true;
		}

		for (const BuildTarget& target : setup.project.targets)
		{
			std::string sourcePath = FilePath::combine(setup.sourcePath, target.subdirectory);

			// To do: need to check all files that Configure.js included
			int64_t srcFileTime = FileTimeCache::getLastWriteTime(FilePath::combine(sourcePath, target.subdirectory + ".js"));
			if (makefileTime < srcFileTime)
			{
				needsUpdate = true;
				break;
			}
		}
	}
	catch (...)
	{
		needsUpdate = true;
	}

	if (needsUpdate)
		updateMakefile();

	// msbuild wants an output file that always gets updated, so we give it that.
	int64_t timestamp = std::chrono::system_clock::now().time_since_epoch().count();
	File::writeAllText(FilePath::combine(cppbuildDir, "Makefile.timestamp"), std::to_string(timestamp));
}

void CPPBuild::validateConfig(const BuildSetup& setup)
{
	if (setup.project.name.empty())
		throw std::runtime_error("No project name specified");

	if (!setup.project.installers.empty() || !setup.project.packageInstallers.empty())
	{
		return;
	}
	if (!setup.project.targets.empty())
	{
		if (setup.project.configurations.empty())
			throw std::runtime_error("No configurations specified");
		return;
	}

	throw std::runtime_error("No targets or installers specified");
}

JsonValue CPPBuild::runConfigureScript(const std::string& sourcePath, const JsonValue& properties)
{
	std::string scriptFilename = FilePath::combine(sourcePath, "Configure.js");
	std::string configureScript = File::readAllText(scriptFilename);
	std::string buildDir = FilePath::combine(workDir, "Build");

	ScriptContext context(sourcePath, buildDir, properties.to_json());
	ScriptValue result = context.eval(configureScript, scriptFilename, JS_EVAL_TYPE_MODULE | JS_EVAL_FLAG_STRICT);

	if (result.isException())
	{
		ScriptValue exception = context.getException();
		throw std::runtime_error(exception.toString() + "\n" + exception.getExceptionStack());
	}

	return JsonValue::parse(context.generateConfiguration());
}

void CPPBuild::generateWorkspace(const JsonValue& properties)
{
#ifdef WIN32
	PackageManager packages(workDir);
	VSWorkspace::generate(loadBuildSetup(), &packages, workDir, cppbuildDir, properties["cppbuild.vsversion"].to_string());
#else
	MakefileWorkspace workspace;
	workspace.generate(loadBuildSetup(), workDir, cppbuildDir);
#endif
}

int CPPBuild::postBuild(std::string targetName, std::string configuration)
{
	BuildSetup setup = loadBuildSetup();
	PackageManager packages(workDir);
	for (std::string name : getBuildOrder(setup, targetName, configuration))
	{
		Target target(setup, &packages, workDir, name, configuration);
		int result = target.postBuild();
		if (result != 0)
			return result;
	}
	return 0;
}

int CPPBuild::build(std::string targetName, std::string configuration)
{
	BuildSetup setup = loadBuildSetup();
	PackageManager packages(workDir);
	for (std::string name : getBuildOrder(setup, targetName, configuration))
	{
		Target target(setup, &packages, workDir, name, configuration);
		int result = target.build();
		if (result != 0)
			return result;
	}
	return 0;
}

void CPPBuild::clean(std::string targetName, std::string configuration)
{
	BuildSetup setup = loadBuildSetup();
	PackageManager packages(workDir);
	for (std::string name : getBuildOrder(setup, targetName, configuration))
	{
		Target target(setup, &packages, workDir, name, configuration);
		target.clean();
	}
}

int CPPBuild::rebuild(std::string targetName, std::string configuration)
{
	BuildSetup setup = loadBuildSetup();
	PackageManager packages(workDir);
	for (std::string name : getBuildOrder(setup, targetName, configuration))
	{
		Target target(setup, &packages, workDir, name, configuration);
		int result = target.rebuild();
		if (result != 0)
			return result;
	}
	return 0;
}

void CPPBuild::createInstaller()
{
	BuildSetup setup = loadBuildSetup();

	std::string binDir = FilePath::combine(workDir, { "Build", "Installer" });
	Directory::create(binDir);

	auto msi = MSIGenerator::create();
	for (const BuildInstaller& def : setup.project.installers)
	{
		std::string sourcePath = FilePath::combine(setup.sourcePath, def.subdirectory);
		msi->generate(binDir, sourcePath, def);
	}
}

void CPPBuild::createPackage()
{
	BuildSetup setup = loadBuildSetup();
	PackageManager packages(workDir);
	packages.createPackage(setup);
}

BuildSetup CPPBuild::loadBuildSetup()
{
	return BuildSetup::fromJson(JsonValue::parse(File::readAllText(FilePath::combine(cppbuildDir, "config.json"))));
}

std::vector<std::string> CPPBuild::getBuildOrder(BuildSetup& setup, std::string targetName, std::string configuration)
{
	std::unordered_map<std::string, const BuildTarget*> allTargets;
	for (const BuildTarget& target : setup.project.targets)
		allTargets[target.name] = &target;

	std::vector<std::string> buildOrder;
	std::unordered_set<std::string> added;

	if (targetName == "all")
	{
		for (const BuildTarget& target : setup.project.targets)
		{
			addTarget(buildOrder, &target, allTargets, added, 0);
		}
	}
	else
	{
		addTarget(buildOrder, &setup.project.getTarget(targetName), allTargets, added, 0);
	}

	return buildOrder;
}

void CPPBuild::addTarget(std::vector<std::string>& buildOrder, const BuildTarget* target, std::unordered_map<std::string, const BuildTarget*>& allTargets, std::unordered_set<std::string>& added, int depth)
{
	if (depth > 16)
		throw std::runtime_error("Target dependency depth too large");

	if (added.insert(target->name).second)
	{
		for (const std::string& lib : target->linkLibraries)
		{
			auto it = allTargets.find(lib);
			if (it != allTargets.end())
			{
				addTarget(buildOrder, it->second, allTargets, added, depth + 1);
			}
		}
		buildOrder.push_back(target->name);
	}
}
