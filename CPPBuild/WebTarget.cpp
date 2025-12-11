
#include "Precomp.h"
#include "WebTarget.h"
#include "CSSTokenizer.h"
#include "BuildSetup.h"
#include "Process.h"
#include "IOData/FilePath.h"
#include "IOData/File.h"
#include "IOData/Directory.h"
#include "IOData/MemoryDevice.h"
#include "Zip/ZipWriter.h"
#include <iostream>
#include <future>

WebTarget::WebTarget(const std::string& workDir, const std::string& target, const std::string& configuration) : workDir(workDir), target(target), configuration(configuration)
{
	emcc = "emcc";
	emar = "emar";

	loadTargets();
}

void WebTarget::build()
{
	compile();
	link();
	linkCSS();
	package();
}

void WebTarget::clean()
{
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

	std::string outputCSS = getLibPrefix() + target + ".css";
	printLine("Cleaning " + outputCSS);
	File::tryDelete(FilePath::combine((targetType == WebTargetType::library) ? binDir : objDir, outputCSS));
}

void WebTarget::rebuild()
{
	clean();
	build();
}

void WebTarget::compile()
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
	if (compileFailed)
		throw std::runtime_error("Compile failed");
}

void WebTarget::compileThreadMain(int threadIndex, int numThreads)
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
					std::string commandline = emcc + " " + compileFlags + " -MD -c " + cppFile + " -o " + objFile;
					runCommand(commandline, "Could not compile " + filename);
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

void WebTarget::printLine(const std::string& text)
{
	std::unique_lock lock(mutex);
	std::cout << text.c_str() << std::endl;
}

void WebTarget::runCommand(const std::string& commandline, const std::string& errorMessage)
{
	int result = Process::runCommand(commandline, [=, this](const std::string& line) { printLine(line); });
	if (result != 0)
		throw std::runtime_error(errorMessage);
}

void WebTarget::link()
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

	std::string outputFile;
	if (targetType == WebTargetType::library)
		outputFile = "lib" + target + ".a";
	else if (targetType == WebTargetType::component)
		outputFile = target + ".js";
	else
		outputFile = target + ".html";

	std::string outputPath = (targetType == WebTargetType::library) ? binDir : objDir;

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
		if (targetType == WebTargetType::library)
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

			std::string cmdline = emar + " \"@" + responsefilename + "\"";
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

			std::string cmdline = emcc + " \"@" + responsefilename + "\"";
			runCommand(cmdline, "Could not link " + outputFile);
		}
	}
}

void WebTarget::linkCSS()
{
	std::string outputCSS = getLibPrefix() + target + ".css";
	std::string objFile = FilePath::combine((targetType == WebTargetType::library) ? binDir : objDir, outputCSS);
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

			if (!found)
				throw std::runtime_error("Could not find " + cssFilename);

			css += File::readAllText(depFilename);
#ifdef WIN32
			css += "\r\n";
#else
			css += "\n";
#endif
			includes.push_back(depFilename);
		}

		css += processCSSFile(cssFile, File::readAllText(cssFile), includes);

		writeDependencyFile(depFile, cssFile, includes);
		File::writeAllText(objFile, css);
	}
}

std::string WebTarget::processCSSFile(const std::string& filename, std::string text, std::vector<std::string>& includes, int level)
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

void WebTarget::package()
{
	if (targetType != WebTargetType::component && targetType != WebTargetType::website)
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
		if (targetType == WebTargetType::website)
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

void WebTarget::addFolder(ZipWriter* zip, std::string srcdir, std::string destdir)
{
	for (const std::string& filename : Directory::files(FilePath::combine(srcdir, "*.*")))
	{
		auto data = File::readAllBytes(FilePath::combine(srcdir, filename));
		zip->addFile(FilePath::combine(destdir, filename), true, data->data(), data->size());
	}

	for (const std::string& filename : Directory::folders(FilePath::combine(srcdir, "*.*")))
	{
		addFolder(zip, FilePath::combine(srcdir, filename), FilePath::combine(destdir, filename));
	}
}

void WebTarget::loadTargets()
{
	std::string cppbuildDir = FilePath::combine(workDir, ".cppbuild");
	BuildSetup setup = BuildSetup::fromJson(JsonValue::parse(File::readAllText(FilePath::combine(cppbuildDir, "config.json"))));

	const BuildConfiguration& configDef = setup.project.getConfiguration(configuration);
	const BuildTarget& targetDef = setup.project.getTarget(target);

	std::string sourcePath = FilePath::combine(setup.sourcePath, targetDef.subdirectory);

	binDir = FilePath::combine(workDir, { "Build", configuration, configDef.platform, "bin" });
	objDir = FilePath::combine(workDir, { "Build", configuration, configDef.platform, "obj", target });

	Directory::create(binDir);
	Directory::create(objDir);

	std::string type = targetDef.type;
	if (type == "webcomponent")
		targetType = WebTargetType::component;
	else if (type == "website")
		targetType = WebTargetType::website;
	else if (type == "weblibrary")
		targetType = WebTargetType::library;
	else if (type.empty())
		throw std::runtime_error("No project type specified");
	else
		throw std::runtime_error("Invalid project type '" + type + "'");

	linkLibraries = targetDef.linkLibraries;

	wwwrootDir = FilePath::combine(sourcePath, targetDef.wwwRootDir);

	for (const std::string& name : targetDef.files)
	{
		if (FilePath::hasExtension(name, "cpp") || FilePath::hasExtension(name, "cc"))
		{
			sourceFiles.push_back(FilePath::combine(sourcePath, name));
		}
	}

	cssFile = FilePath::forceSlash(FilePath::combine(sourcePath, targetDef.cssRootFile));
	shellFile = FilePath::forceSlash(FilePath::combine(sourcePath, targetDef.htmlShellFile));

	defines = targetDef.defines;

	for (const std::string& path: targetDef.includePaths)
		includePaths.push_back(FilePath::combine(sourcePath, path));

	for (const std::string& item : targetDef.libraryPaths)
		libraryPaths.push_back(FilePath::combine(sourcePath, item));

	auto itTargetConfig = targetDef.configurations.find(configuration);
	if (itTargetConfig != targetDef.configurations.end())
	{
		const BuildTargetConfiguration& targetConfigDef = itTargetConfig->second;

		defines.insert(defines.end(), targetConfigDef.defines.begin(), targetConfigDef.defines.end());
		linkLibraries.insert(linkLibraries.end(), targetConfigDef.linkLibraries.begin(), targetConfigDef.linkLibraries.end());

		for (const std::string& item : targetConfigDef.includePaths)
			includePaths.push_back(FilePath::combine(sourcePath, item));

		for (const std::string& item : targetConfigDef.libraryPaths)
			libraryPaths.push_back(FilePath::combine(sourcePath, item));
	}

	// To do: use -gseparate-dwarf[=FILENAME] maybe

	std::string flags = "-s DISABLE_EXCEPTION_CATCHING=0";
	compileFlags = flags + " --std=c++23 -Werror -Wno-deprecated-this-capture";

	for (const std::string& includePath : includePaths)
	{
		compileFlags += " -I \"" + includePath + "\"";
	}

	if (targetType == WebTargetType::component)
	{
		linkFlags = flags + " -s ALLOW_MEMORY_GROWTH=1 -s EXPORT_NAME=\"'" + target + "'\" -s MODULARIZE  -lembind";
	}
	else if (targetType == WebTargetType::website)
	{
		linkFlags = flags + " -s ALLOW_MEMORY_GROWTH=1 --shell-file " + shellFile + " -lembind";
	}

	compileFlags = "-O1 " + compileFlags;
	linkFlags = "-O1 " + linkFlags;
}

bool WebTarget::isCppFile(const std::string& filename)
{
	for (const char* ext : { "cpp", "cc", "c" })
	{
		if (FilePath::hasExtension(filename, ext))
		{
			return true;
		}
	}
	return false;
}

std::vector<std::string> WebTarget::readMakefileDependencyFile(const std::string& filename)
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

std::vector<WebTarget::MakeToken> WebTarget::parseMakefile(const std::string& text)
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

std::vector<std::string> WebTarget::readDependencyFile(const std::string& filename)
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

void WebTarget::writeDependencyFile(const std::string& filename, const std::string& inputFile, const std::vector<std::string>& includes)
{
	JsonValue depJson = JsonValue::object();
	if (!inputFile.empty())
		depJson["Source"] = JsonValue::string(inputFile);
	depJson["Includes"] = JsonValue::array(includes);
	File::writeAllText(filename, depJson.to_json());
}
