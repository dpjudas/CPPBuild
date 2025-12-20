
#include "Precomp.h"
#include "CPPBuild.h"
#include "Target.h"
#include "VSWorkspace.h"
#include "MakefileWorkspace.h"
#include "BuildSetup.h"
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

void CPPBuild::configure(std::string sourcePath)
{
	if (sourcePath.empty())
		sourcePath = Directory::currentDirectory();

	JsonValue config = JsonValue::object();
	config["version"] = JsonValue::number(1);
	config["sourcePath"] = JsonValue::string(sourcePath);
	config["project"] = runConfigureScript(sourcePath);

	BuildSetup setup = BuildSetup::fromJson(config);
	validateConfig(setup);

	Directory::create(cppbuildDir);
	Directory::trySetHidden(cppbuildDir);
	File::writeAllText(FilePath::combine(cppbuildDir, "config.json"), config.to_json());

	if (!setup.project.targets.empty())
		generateWorkspace();
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
		int64_t makefileTime = File::getLastWriteTime(configFilename);
		BuildSetup setup = BuildSetup::fromJson(JsonValue::parse(File::readAllText(configFilename)));

		int64_t srcFileTime = File::getLastWriteTime(FilePath::combine(setup.sourcePath, "Configure.js"));
		if (makefileTime < srcFileTime)
		{
			needsUpdate = true;
		}

		for (const BuildTarget& target : setup.project.targets)
		{
			std::string sourcePath = FilePath::combine(setup.sourcePath, target.subdirectory);

			// To do: need to check all files that Configure.js included
			int64_t srcFileTime = File::getLastWriteTime(FilePath::combine(sourcePath, target.subdirectory + ".js"));
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

	if (!setup.project.installers.empty())
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

JsonValue CPPBuild::runConfigureScript(const std::string& sourcePath)
{
	std::string scriptFilename = FilePath::combine(sourcePath, "Configure.js");
	std::string configureScript = File::readAllText(scriptFilename);

	ScriptContext context(sourcePath);
	ScriptValue result = context.eval(configureScript, scriptFilename, JS_EVAL_TYPE_MODULE | JS_EVAL_FLAG_STRICT);

	if (result.isException())
	{
		ScriptValue exception = context.getException();
		throw std::runtime_error(exception.toString() + "\n" + exception.getExceptionStack());
	}

	return JsonValue::parse(context.generateConfiguration());
}

void CPPBuild::generateWorkspace()
{
#ifdef WIN32
	VSWorkspace workspace;
	workspace.generate(loadBuildSetup(), workDir, cppbuildDir);
#else
	MakefileWorkspace workspace;
	workspace.generate(loadBuildSetup(), workDir, cppbuildDir);
#endif
}

void CPPBuild::build(std::string targetName, std::string configuration)
{
	checkMakefile();

	BuildSetup setup = loadBuildSetup();
	for (std::string name : getBuildOrder(setup, targetName, configuration))
	{
		Target target(setup, workDir, name, configuration);
		target.build();
	}
}

void CPPBuild::clean(std::string targetName, std::string configuration)
{
	checkMakefile();

	BuildSetup setup = loadBuildSetup();
	for (std::string name : getBuildOrder(setup, targetName, configuration))
	{
		Target target(setup, workDir, name, configuration);
		target.clean();
	}
}

void CPPBuild::rebuild(std::string targetName, std::string configuration)
{
	checkMakefile();

	BuildSetup setup = loadBuildSetup();
	for (std::string name : getBuildOrder(setup, targetName, configuration))
	{
		Target target(setup, workDir, name, configuration);
		target.rebuild();
	}
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
