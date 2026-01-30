
#include "Precomp.h"
#include "Target.h"
#include "CSSTokenizer.h"
#include "BuildSetup.h"
#include "ConsoleProcess.h"
#include "PackageManager.h"
#include "IOData/FilePath.h"
#include "IOData/File.h"
#include "IOData/Directory.h"
#include "IOData/MemoryDevice.h"
#include "Zip/ZipWriter.h"
#include <iostream>
#include <future>
#include <algorithm>

Target::Target(BuildSetup& setup, PackageManager* packages, const std::string& workDir, const std::string& target, const std::string& configuration) : workDir(workDir), target(target), configuration(configuration)
{
	loadTarget(setup, packages);
}

int Target::build()
{
	if (targetType == TargetType::custom)
	{
		runCommand(buildCommand, "Could not run build command");
		return 0;
	}

	if (!compile())
		return 2;
	link();
	linkCSS();
	package();

	return 0;
}

void Target::clean()
{
	if (targetType == TargetType::custom)
	{
		runCommand(cleanCommand, "Could not run clean command");
		return;
	}

	for (const std::string& inputFile : sourceFiles)
	{
		if (isCppFile(inputFile))
		{
			std::string objFilename = FilePath::removeExtension(FilePath::lastComponent(inputFile)) + ".obj";
			printLine("Cleaning " + objFilename);
			std::string objFile = FilePath::combine(objDir, objFilename);
			File::tryDelete(objFile);
		}
	}

	for (const auto& filename : outputFiles)
	{
		printLine("Cleaning " + filename);
		File::tryDelete(FilePath::combine(binDir, filename));
	}

	if (targetType == TargetType::webComponent ||
		targetType == TargetType::webLibrary ||
		targetType == TargetType::website)
	{
		std::string outputCSS = getLibPrefix() + target + ".css";
		printLine("Cleaning " + outputCSS);
		if (targetType == TargetType::webLibrary)
		{
			File::tryDelete(FilePath::combine(binDir, outputCSS));
		}
		else
		{
			File::tryDelete(FilePath::combine(objDir, outputCSS));
		}
	}
}

int Target::rebuild()
{
	clean();
	return build();
}

bool Target::compile()
{
	std::vector<std::future<void>> results;

	int numThreads = std::max((int)(std::thread::hardware_concurrency() * 3) / 4, 2);
	for (int threadIndex = 0; threadIndex < numThreads; threadIndex++)
	{
		results.push_back(std::async([=,this]() { compileThreadMain(threadIndex, numThreads); }));
	}

	for (auto& result : results)
		result.get();

	std::unique_lock lock(mutex);
	return !compileFailed;
}

void Target::compileThreadMain(int threadIndex, int numThreads)
{
	try
	{
		int i = 0;
		for (const std::string& inputFile : sourceFiles)
		{
			std::string filename = FilePath::lastComponent(inputFile);
			if (isCppFile(filename))
			{
				if (i++ % numThreads != threadIndex)
					continue;

				std::unique_lock lock(mutex);
				if (compileFailed)
					break;
				lock.unlock();

				std::string cppFile = inputFile;
				std::string objFile = FilePath::combine(objDir, FilePath::removeExtension(filename) + ".obj");
				std::string depFile = FilePath::combine(objDir, FilePath::removeExtension(filename) + ".d");

				bool needsCompile = false;
				try
				{
					int64_t objTime = File::getLastWriteTime(objFile);
					for (const std::string& dependency : readMakefileDependencyFile(depFile))
					{
						int64_t depTime = File::getLastWriteTime(dependency);
						if (depTime > objTime)
						{
							needsCompile = true;
							break;
						}
					}
				}
				catch (...)
				{
					needsCompile = true;
				}

				if (needsCompile)
				{
					printLine(filename);

					if (FilePath::hasExtension(filename, "c"))
					{
						std::string commandline = cc + " " + cflags + " -MD -c " + cppFile + " -o " + objFile;
						runCommand(commandline, "Could not compile " + filename);
					}
					else
					{
						std::string commandline = ccpp + " " + cxxflags + " -MD -c " + cppFile + " -o " + objFile;
						runCommand(commandline, "Could not compile " + filename);
					}
				}
			}
		}
	}
	catch (const std::exception& e)
	{
		printLine(e.what());
		std::unique_lock lock(mutex);
		compileFailed = true;
	}
}

void Target::printLine(const std::string& text)
{
	std::unique_lock lock(mutex);
	std::cout << text.c_str() << std::endl;
}

void Target::runCommand(const std::string& commandline, const std::string& errorMessage)
{
	int result = ConsoleProcess::runCommand(commandline, [=, this](const std::string& line) { printLine(line); });
	if (result != 0)
		throw std::runtime_error(errorMessage);
}

void Target::link()
{
	std::vector<std::string> objFiles;
	for (const std::string& inputFile : sourceFiles)
	{
		if (isCppFile(inputFile))
		{
			std::string objFile = FilePath::combine(objDir, FilePath::removeExtension(FilePath::lastComponent(inputFile)) + ".obj");
			objFiles.push_back(objFile);
		}
	}

	std::string outputFile, outputPath;

	if (targetType == TargetType::application)
	{
		outputFile = target;
		outputPath = binDir;
	}
	else if (targetType == TargetType::console)
	{
		outputFile = target;
		outputPath = binDir;
	}
	else if (targetType == TargetType::lib)
	{
		outputFile = "lib" + target + ".a";
		outputPath = binDir;
	}
	else if (targetType == TargetType::dll)
	{
		outputFile = "lib" + target + ".so";
		outputPath = binDir;
	}
	else if (targetType == TargetType::webLibrary)
	{
		outputFile = "lib" + target + ".a";
		outputPath = binDir;
	}
	else if (targetType == TargetType::webComponent)
	{
		outputFile = target + ".js";
		outputPath = objDir;
	}
	else if (targetType == TargetType::website)
	{
		outputFile = target + ".html";
		outputPath = objDir;
	}

	bool needsLink = false;
	try
	{
		int64_t exeTime = File::getLastWriteTime(FilePath::combine(outputPath, outputFile));
		for (const std::string& dependency : objFiles)
		{
			int64_t depTime = File::getLastWriteTime(dependency);
			if (depTime > exeTime)
			{
				needsLink = true;
				break;
			}
		}

		for (const auto& dep : linkLibraries)
		{
			std::string depFilename = FilePath::combine(binDir, "lib" + dep + ".a");
			int64_t depTime = File::getLastWriteTime(depFilename);
			if (depTime > exeTime)
			{
				needsLink = true;
				break;
			}
		}
	}
	catch (...)
	{
		needsLink = true;
	}

	if (needsLink)
	{
		if (targetType == TargetType::webLibrary || targetType == TargetType::lib)
		{
			printLine("Creating static library " + outputFile);

			std::string responsefilename = FilePath::combine(objDir, "responsefile.1");
			std::string responsefile;
			responsefile.reserve(64 * 1024);
			responsefile += " rcs ";
			responsefile += FilePath::forceSlash(FilePath::combine(outputPath, outputFile));
			for (const auto& file : objFiles)
			{
				responsefile.push_back(' ');
				responsefile += FilePath::forceSlash(file);
			}
			File::writeAllText(responsefilename, responsefile);

			std::string cmdline = ar + " \"@" + responsefilename + "\"";
			runCommand(cmdline, "Could not link " + outputFile);
		}
		else
		{
			printLine("Linking " + outputFile);

			std::string responsefilename = FilePath::combine(objDir, "responsefile.1");
			std::string responsefile;
			responsefile.reserve(64 * 1024);
			responsefile += linkFlags;
			responsefile += " -o ";
			responsefile += FilePath::forceSlash(FilePath::combine(outputPath, outputFile));

			for (const auto& file : objFiles)
			{
				responsefile.push_back(' ');
				responsefile += FilePath::forceSlash(file);
			}

			responsefile += " -L \"" + FilePath::forceSlash(binDir) + "\"";

			for (const auto& path : libraryPaths)
				responsefile += " -L \"" + FilePath::forceSlash(path) + "\"";

			for (const auto& dep : linkLibraries)
				responsefile += " -l" + dep;

			File::writeAllText(responsefilename, responsefile);

			std::string cmdline = ccpp + " \"@" + responsefilename + "\"";
			runCommand(cmdline, "Could not link " + outputFile);
		}
	}
}

void Target::linkCSS()
{
	if (targetType != TargetType::webLibrary &&
		targetType != TargetType::webComponent &&
		targetType != TargetType::website)
	{
		return;
	}

	std::string outputCSS = getLibPrefix() + target + ".css";
	std::string objFile = FilePath::combine((targetType == TargetType::webLibrary) ? binDir : objDir, outputCSS);
	std::string depFile = FilePath::combine(objDir, FilePath::removeExtension(outputCSS) + ".cssdep");

	bool needsCompile = false;
	try
	{
		int64_t objTime = File::getLastWriteTime(objFile);
		for (const std::string& dependency : readDependencyFile(depFile))
		{
			int64_t depTime = File::getLastWriteTime(dependency);
			if (depTime > objTime)
			{
				needsCompile = true;
				break;
			}
		}
	}
	catch (...)
	{
		needsCompile = true;
	}

	if (needsCompile)
	{
		printLine(FilePath::lastComponent(cssFile));

		std::vector<std::string> includes;
		std::string css;

		for (const auto& dep : linkLibraries)
		{
			std::string cssFilename = "lib" + dep + ".css";
			std::string depFilename = FilePath::combine(binDir, cssFilename);
			bool found = File::exists(depFilename);
			if (!found)
			{
				for (const std::string& libPath : libraryPaths)
				{
					depFilename = FilePath::combine(libPath, cssFilename);
					found = File::exists(depFilename);
					if (found)
						break;
				}
			}

			if (found)
			{
				css += File::readAllText(depFilename);
#ifdef WIN32
				css += "\r\n";
#else
				css += "\n";
#endif
				includes.push_back(depFilename);
			}
		}

		css += processCSSFile(cssFile, File::readAllText(cssFile), includes);

		writeDependencyFile(depFile, cssFile, includes);
		File::writeAllText(objFile, css);
	}
}

std::string Target::processCSSFile(const std::string& filename, std::string text, std::vector<std::string>& includes, int level)
{
	auto tokenizer = CSSTokenizer::create(text);

	std::string includeCSS;

	size_t importEnd = 0;
	CSSToken token;
	while (true)
	{
		tokenizer->read(token, true);
		if (token.type == CSSTokenType::null)
		{
			importEnd = text.size();
			break;
		}

		importEnd = token.offset;
		if (token.type == CSSTokenType::atkeyword && token.value == "import")
		{
			tokenizer->read(token, true);
			if (token.type != CSSTokenType::string)
				throw std::runtime_error(FilePath::normalizePathDelimiters(filename) + ": error: expected string after @import");

			if (level >= 10)
				throw std::runtime_error(FilePath::normalizePathDelimiters(filename) + ": error: @import recursion too deep");

			std::string includeFilename = FilePath::combine(FilePath::removeLastComponent(filename), token.value);
			std::string includeText;
			try
			{
				includeText = File::readAllText(includeFilename);
			}
			catch (const std::exception&)
			{
				bool found = false;
				for (const std::string& includePath : includePaths)
				{
					try
					{
						includeFilename = FilePath::combine(includePath, token.value);
						includeText = File::readAllText(includeFilename);
						found = true;
					}
					catch (const std::exception&)
					{
					}
				}
				if (!found)
					throw std::runtime_error(FilePath::normalizePathDelimiters(filename) + ": error: could not include '" + token.value);
			}
			includes.push_back(includeFilename);
			includeCSS += processCSSFile(includeFilename, std::move(includeText), includes, level + 1);

			tokenizer->read(token, true);
			if (token.type != CSSTokenType::semi_colon)
				throw std::runtime_error(FilePath::normalizePathDelimiters(filename) + ": error: ';' expected after import directive");
		}
		else
		{
			break;
		}
	}

	return includeCSS + text.substr(importEnd);
}

void Target::package()
{
	if (targetType != TargetType::webComponent && targetType != TargetType::website)
		return;

	std::string outputHtml = target + ".html";
	std::string outputJS = target + ".js";
	std::string outputWasm = target + ".wasm";
	std::string outputCSS = target + ".css";
	std::string outputPackage = target + ".webpkg";

	std::string depFile = FilePath::combine(objDir, FilePath::removeExtension(target) + ".pkgdep");

	bool needsCompile = false;
	try
	{
		int64_t objTime = File::getLastWriteTime(FilePath::combine(binDir, outputPackage));
		for (const std::string& dependency : readDependencyFile(depFile))
		{
			int64_t depTime = File::getLastWriteTime(dependency);
			if (depTime > objTime)
			{
				needsCompile = true;
				break;
			}
		}
	}
	catch (...)
	{
		needsCompile = true;
	}

	if (needsCompile)
	{
		printLine("Building web package " + outputPackage);

		auto memdevice = MemoryDevice::create();
		auto zip = ZipWriter::create(memdevice);

		std::vector<std::string> outputFiles;
		outputFiles.push_back(outputCSS);
		if (targetType == TargetType::website)
			outputFiles.push_back(outputHtml);
		outputFiles.push_back(outputJS);
		outputFiles.push_back(outputWasm);

		std::vector<std::string> includes;
		for (std::string file : outputFiles)
		{
			std::string filename = FilePath::combine(objDir, file);
			includes.push_back(filename);
			auto data = File::readAllBytes(filename);
			zip->addFile(FilePath::lastComponent(filename), true, data->data(), data->size());
		}
		addFolder(zip.get(), wwwrootDir, "");

		zip->writeToc();
		zip.reset();
		writeDependencyFile(depFile, "", includes);
		File::writeAllBytes(FilePath::combine(binDir, outputPackage), memdevice->buffer());
	}
}

void Target::addFolder(ZipWriter* zip, std::string srcdir, std::string destdir)
{
	for (const std::string& filename : Directory::files(FilePath::combine(srcdir, "*")))
	{
		auto data = File::readAllBytes(FilePath::combine(srcdir, filename));
		zip->addFile(FilePath::combine(destdir, filename), true, data->data(), data->size());
	}

	for (const std::string& filename : Directory::folders(FilePath::combine(srcdir, "*")))
	{
		addFolder(zip, FilePath::combine(srcdir, filename), FilePath::combine(destdir, filename));
	}
}

std::string Target::getLibPrefix() const
{
#ifndef WIN32
	if (targetType == TargetType::lib || targetType == TargetType::dll)
	{
		return "lib";
	}
#endif
	if (targetType == TargetType::webLibrary)
	{
		return "lib";
	}
	return {};
}

void Target::loadTarget(BuildSetup& setup, PackageManager* packages)
{
	const BuildConfiguration& configDef = setup.project.getConfiguration(configuration);
	const BuildTarget& targetDef = setup.project.getTarget(target);

	std::string sourcePath = FilePath::combine(setup.sourcePath, targetDef.subdirectory);

	binDir = FilePath::combine(workDir, { "Build", configuration, "bin" });
	objDir = FilePath::combine(workDir, { "Build", configuration, "obj", target });

	Directory::create(binDir);
	Directory::create(objDir);

	std::string type = targetDef.type;
	if (type == "custom")
		targetType = TargetType::custom;
	else if (type == "application")
		targetType = TargetType::application;
	else if (type == "console")
		targetType = TargetType::console;
	else if (type == "lib")
		targetType = TargetType::lib;
	else if (type == "dll")
		targetType = TargetType::dll;
	else if (type == "webcomponent")
		targetType = TargetType::webComponent;
	else if (type == "website")
		targetType = TargetType::website;
	else if (type == "weblibrary")
		targetType = TargetType::webLibrary;
	else if (type.empty())
		throw std::runtime_error("No project type specified");
	else
		throw std::runtime_error("Invalid project type '" + type + "'");

	if (targetType == TargetType::custom)
	{
		buildCommand = targetDef.buildCommand;
		cleanCommand = targetDef.cleanCommand;

		auto itTargetConfig = targetDef.configurations.find(configuration);
		if (itTargetConfig != targetDef.configurations.end())
		{
			const BuildTargetConfiguration& targetConfigDef = itTargetConfig->second;
			if (!targetConfigDef.buildCommand.empty())
				buildCommand = targetConfigDef.buildCommand;
			if (!targetConfigDef.cleanCommand.empty())
				cleanCommand = targetConfigDef.cleanCommand;
		}

		if (!buildCommand.empty())
			buildCommand = addPathToCommand(buildCommand, setup);
		if (!cleanCommand.empty())
			cleanCommand = addPathToCommand(cleanCommand, setup);

		return;
	}

	if (targetType == TargetType::webComponent ||
		targetType == TargetType::webLibrary ||
		targetType == TargetType::website)
	{
		cc = "emcc";
		ccpp = "emcc";
		ar = "emar";
	}
	else
	{
#ifdef WIN32
		cc = "cl";
		ccpp = "cl";
		ar = "link";
#elif __APPLE__
		cc = "clang";
		ccpp = "clang";
		ar = "ar";
#else
		cc = "gcc";
		ccpp = "g++";
		ar = "ar";
#endif
	}

	wwwrootDir = FilePath::combine(sourcePath, targetDef.wwwRootDir);

	for (const std::string& name : targetDef.files)
	{
		if (FilePath::hasExtension(name, "cpp") || FilePath::hasExtension(name, "cc") || FilePath::hasExtension(name, "c"))
		{
			sourceFiles.push_back(FilePath::combine(sourcePath, name));
		}
	}

	for (const std::string pkgName : targetDef.packages)
	{
		const Package& package = packages->getPackage(pkgName);
		std::string pkgBasePath = packages->getPackagePath(pkgName);

		for (const std::string& define : package.defines)
			defines.push_back(define);

		for (const std::string& opt : package.cCompileOptions)
			cCompileOptions.push_back(opt);

		for (const std::string& opt : package.cxxCompileOptions)
			cxxCompileOptions.push_back(opt);

		for (const std::string& opt : package.linkOptions)
			linkOptions.push_back(opt);

		for (const std::string& linkLibrary : package.linkLibraries)
			linkLibraries.push_back(linkLibrary);

		for (const std::string& path : package.includePaths)
			includePaths.push_back(FilePath::combine(pkgBasePath, path));

		for (const std::string& item : package.libraryPaths)
			libraryPaths.push_back(FilePath::combine(pkgBasePath, item));

		auto itPackageConfig = package.configurations.find(configuration);
		if (itPackageConfig != package.configurations.end())
		{
			const PackageConfiguration& packageConfigDef = itPackageConfig->second;

			for (const std::string& define : packageConfigDef.defines)
				defines.push_back(define);

			for (const std::string& opt : packageConfigDef.cCompileOptions)
				cCompileOptions.push_back(opt);

			for (const std::string& opt : packageConfigDef.cxxCompileOptions)
				cxxCompileOptions.push_back(opt);

			for (const std::string& opt : packageConfigDef.linkOptions)
				linkOptions.push_back(opt);

			for (const std::string& linkLibrary : packageConfigDef.linkLibraries)
				linkLibraries.push_back(linkLibrary);

			for (const std::string& item : packageConfigDef.includePaths)
				includePaths.push_back(FilePath::combine(pkgBasePath, item));

			for (const std::string& item : packageConfigDef.libraryPaths)
				libraryPaths.push_back(FilePath::combine(pkgBasePath, item));
		}
	}

	for (const std::string& define : targetDef.defines)
		defines.push_back(define);

	for (const std::string& opt : targetDef.cCompileOptions)
		cCompileOptions.push_back(opt);

	for (const std::string& opt : targetDef.cxxCompileOptions)
		cxxCompileOptions.push_back(opt);

	for (const std::string& opt : targetDef.linkOptions)
		linkOptions.push_back(opt);

	for (const std::string& linkLibrary : targetDef.linkLibraries)
		linkLibraries.push_back(linkLibrary);

	for (const std::string& path: targetDef.includePaths)
		includePaths.push_back(FilePath::combine(sourcePath, path));

	for (const std::string& item : targetDef.libraryPaths)
		libraryPaths.push_back(FilePath::combine(sourcePath, item));

	auto itTargetConfig = targetDef.configurations.find(configuration);
	if (itTargetConfig != targetDef.configurations.end())
	{
		const BuildTargetConfiguration& targetConfigDef = itTargetConfig->second;

		for (const std::string& define : targetConfigDef.defines)
			defines.push_back(define);

		for (const std::string& opt : targetConfigDef.cCompileOptions)
			cCompileOptions.push_back(opt);

		for (const std::string& opt : targetConfigDef.cxxCompileOptions)
			cxxCompileOptions.push_back(opt);

		for (const std::string& opt : targetConfigDef.linkOptions)
			linkOptions.push_back(opt);

		for (const std::string& linkLibrary : targetConfigDef.linkLibraries)
			linkLibraries.push_back(linkLibrary);

		for (const std::string& item : targetConfigDef.includePaths)
			includePaths.push_back(FilePath::combine(sourcePath, item));

		for (const std::string& item : targetConfigDef.libraryPaths)
			libraryPaths.push_back(FilePath::combine(sourcePath, item));
	}

	bool cLangVersion = isOptionSpecified("--std=", cCompileOptions);
	bool cOptimizeSet = isOptionSpecified("-O", cCompileOptions);
	bool cDebugOutput = isOptionSpecified("-g", cCompileOptions);
	bool cxxLangVersion = isOptionSpecified("--std=", cxxCompileOptions);
	bool cxxOptimizeSet = isOptionSpecified("-O", cxxCompileOptions);
	bool cxxDebugOutput = isOptionSpecified("-g", cxxCompileOptions);

	if (targetType == TargetType::webComponent ||
		targetType == TargetType::webLibrary ||
		targetType == TargetType::website)
	{
		bool cxxDisableExceptions = isOptionSpecified("-s DISABLE_EXCEPTION_CATCHING=", cxxCompileOptions);
		bool linkAllowMemGrowth = isOptionSpecified("-s ALLOW_MEMORY_GROWTH=", linkOptions);
		bool linkShellFile = isOptionSpecified("--shell-file", linkOptions);
		bool linkExportName = isOptionSpecified("-s EXPORT_NAME=", linkOptions);
		bool linkModularize = isOptionSpecified("-s MODULARIZE", linkOptions);

		cssFile = FilePath::forceSlash(FilePath::combine(sourcePath, targetDef.cssRootFile));
		shellFile = FilePath::forceSlash(FilePath::combine(sourcePath, targetDef.htmlShellFile));

		for (std::string& path : includePaths)
			path = FilePath::forceSlash(path);
		for (std::string& path : libraryPaths)
			path = FilePath::forceSlash(path);

		// Emscripten defaults to dinosaur versions of the languages
		if (!cLangVersion)
			cCompileOptions.push_back("--std=c17");
		if (!cxxLangVersion)
			cxxCompileOptions.push_back("--std=c++23");

		// Debugging webassembly in the browser is currently borderline useless. Better to optimize for size always for now.
		if (!cOptimizeSet)
			cCompileOptions.push_back("-O1");
		if (!cxxOptimizeSet)
			cxxCompileOptions.push_back("-O1");

		// To do: use -gseparate-dwarf[=FILENAME] maybe

		// Emscripten defaults are bad. Make them standards compliant C++
		if (!cxxDisableExceptions)
			cxxCompileOptions.push_back("-s DISABLE_EXCEPTION_CATCHING=0");
		if (!linkAllowMemGrowth)
			linkOptions.push_back("-s ALLOW_MEMORY_GROWTH=1");
		if (!linkShellFile)
			linkOptions.push_back("--shell-file \"" + shellFile + "\"");

		// WebCPP always needs embind
		linkLibraries.push_back("embind");

		if (targetType == TargetType::webComponent)
		{
			if (!linkExportName)
				linkOptions.push_back("-s EXPORT_NAME=\"'" + target + "'\"");
			if (!linkModularize)
				linkOptions.push_back("-s MODULARIZE");
		}
	}
	else
	{
		for (std::string& path : includePaths)
			path = FilePath::normalizePathDelimiters(path);
		for (std::string& path : libraryPaths)
			path = FilePath::normalizePathDelimiters(path);

		// GCC defaults to dinosaur versions of the languages
		if (!cLangVersion)
			cCompileOptions.push_back("--std=gnu17");
		if (!cxxLangVersion)
			cxxCompileOptions.push_back("--std=gnu++20");

		if (configuration == "Debug" || configuration == "debug")
		{
			if (!cOptimizeSet)
				cCompileOptions.push_back("-O0");
			if (!cDebugOutput)
				cCompileOptions.push_back("-g");
			if (!cxxOptimizeSet)
				cxxCompileOptions.push_back("-O0");
			if (!cxxDebugOutput)
				cxxCompileOptions.push_back("-g");
		}
		else
		{
			if (!cOptimizeSet)
				cCompileOptions.push_back("-O2");
			if (!cxxOptimizeSet)
				cxxCompileOptions.push_back("-O2");
		}
	}

	for (const std::string& opt : cCompileOptions)
		addArg(cflags, opt);

	for (const std::string& opt : cxxCompileOptions)
		addArg(cxxflags, opt);

	for (const std::string& opt : linkOptions)
		addArg(linkFlags, opt);

	for (const std::string& includePath : includePaths)
	{
		addArg(cflags, " -I \"" + includePath + "\"");
		addArg(cxxflags, " -I \"" + includePath + "\"");
	}

	for (const std::string& define : defines)
	{
		addArg(cflags, " -D \"" + define + "\"");
		addArg(cxxflags, " -D \"" + define + "\"");
	}
}

std::string Target::addPathToCommand(std::string cmdline, const BuildSetup& setup)
{
	std::string tool;
	size_t pos = cmdline.find(' ');
	if (pos == std::string::npos)
	{
		tool = cmdline;
		pos = cmdline.size();
	}
	else
	{
		tool = cmdline.substr(0, pos);
	}

	for (const BuildTarget& target : setup.project.targets)
	{
		if (target.name == tool)
		{
			return FilePath::combine(binDir, tool) + cmdline.substr(pos);
		}
	}

	return cmdline;
}

void Target::addArg(std::string& args, const std::string& arg)
{
	if (!arg.empty() && !args.empty())
		args += ' ';
	args += arg;
}

bool Target::isOptionSpecified(const std::string& opt, std::vector<std::string> options)
{
	// Check if any option begins with opt
	for (const std::string& v : options)
	{
		if (v.size() >= opt.size() && std::equal(opt.begin(), opt.end(), v.begin()))
			return true;
	}
	return false;
}

bool Target::isCppFile(const std::string& filename)
{
	for (const char* ext : { "cpp", "cc", "c"})
	{
		if (FilePath::hasExtension(filename, ext))
		{
			return true;
		}
	}
	return false;
}

std::vector<std::string> Target::readMakefileDependencyFile(const std::string& filename)
{
	std::vector<MakeToken> tokens = parseMakefile(File::readAllText(filename));

	if (tokens.size() < 2 || tokens[0].type != MakeTokenType::string || tokens[1].type != MakeTokenType::colon)
		throw std::runtime_error("Invalid or empty dependency file");

	std::vector<std::string> files;

	for (size_t i = 2; i < tokens.size(); i++)
	{
		if (tokens[i].type == MakeTokenType::newline)
			break;
		else if (tokens[i].type != MakeTokenType::string)
			throw std::runtime_error("Parse error reading dependency target");
		files.push_back(tokens[i].value);
	}

	return files;
}

std::vector<Target::MakeToken> Target::parseMakefile(const std::string& text)
{
	std::vector<MakeToken> tokens;

	size_t pos = 0;
	while (pos < text.size())
	{
		pos = text.find_first_not_of(" \t", pos);
		if (pos == std::string::npos)
			break;

		char c, c2, c3;

		c = text[pos];
		c2 = (pos + 1 < text.size()) ? text[pos + 1] : 0;
		c3 = (pos + 2 < text.size()) ? text[pos + 2] : 0;

		if (c == ':')
		{
			MakeToken token;
			token.type = MakeTokenType::colon;
			tokens.push_back(std::move(token));
			pos++;
		}
		else if (c == '\n')
		{
			MakeToken token;
			token.type = MakeTokenType::newline;
			tokens.push_back(std::move(token));
			pos++;
		}
		else if (c == '\r' && c2 == '\n')
		{
			MakeToken token;
			token.type = MakeTokenType::newline;
			tokens.push_back(std::move(token));
			pos += 2;
		}
		else if (c == '\\' && c2 == '\n')
		{
			pos += 2;
		}
		else if (c == '\\' && c2 == '\r' && c3 == '\n')
		{
			pos += 3;
		}
		else
		{
			MakeToken token;
			token.type = MakeTokenType::string;
			while (pos < text.size())
			{
				c = text[pos];
				c2 = (pos + 1 < text.size()) ? text[pos + 1] : 0;
				c3 = (pos + 2 < text.size()) ? text[pos + 2] : 0;

				if (c == ' ' || c == '\t' || c == '\r' || c == '\n' || (c == ':' && c2 == ' '))
				{
					break;
				}
				else if (c == '\\')
				{
					if (pos + 1 >= text.size())
						throw std::runtime_error("Invalid escape encountered");

					if (c2 == '\\')
					{
						token.value.push_back('\\');
						pos += 2;
					}
					else if (c2 == '\n')
					{
						pos += 2;
						break;
					}
					else if (c2 == '\r' && c3 == '\n')
					{
						pos += 3;
						break;
					}
					else
					{
						token.value.push_back('\\');
						pos++;
					}
				}
				else
				{
					token.value.push_back(text[pos]);
					pos++;
				}
			}

			tokens.push_back(std::move(token));
		}
	}
	return tokens;
}

std::vector<std::string> Target::readDependencyFile(const std::string& filename)
{
	std::vector<std::string> files;

	JsonValue depJson = JsonValue::parse(File::readAllText(filename));

	if (!depJson["Source"].is_undefined())
		files.push_back(depJson["Source"].to_string());

	for (const JsonValue& includeFile : depJson["Includes"].items())
	{
		files.push_back(includeFile.to_string());
	}

	return files;
}

void Target::writeDependencyFile(const std::string& filename, const std::string& inputFile, const std::vector<std::string>& includes)
{
	JsonValue depJson = JsonValue::object();
	if (!inputFile.empty())
		depJson["Source"] = JsonValue::string(inputFile);
	depJson["Includes"] = JsonValue::array(includes);
	File::writeAllText(filename, depJson.to_json());
}
