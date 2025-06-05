
#include "Precomp.h"
#include "WebBuild.h"
#include "CSSTokenizer.h"
#include "IOData/FilePath.h"
#include "IOData/File.h"
#include "IOData/Directory.h"
#include "IOData/MemoryDevice.h"
#include "Zip/ZipWriter.h"
#include <iostream>
#include <future>

WebBuild::WebBuild(const std::string& projectDir, const std::string& platform, const std::string& configuration) : projectDir(projectDir), platform(platform), configuration(configuration)
{
	emcc = "emcc";
	emar = "emar";

	loadProjectFile();
}

void WebBuild::build()
{
	compile();
	link();
	linkCSS();
	package();
}

void WebBuild::clean()
{
	for (const std::string& inputFile : sourceFiles)
	{
		if (isCppFile(inputFile))
		{
			std::string objFilename = FilePath::removeExtension(FilePath::lastComponent(inputFile)) + ".obj";
			std::cout << "Cleaning " << objFilename << std::endl;
			std::string objFile = FilePath::combine(objDir, objFilename);
			File::tryDelete(objFile);
		}
	}

	for (const auto& filename : outputFiles)
	{
		std::cout << "Cleaning " << filename << std::endl;
		File::tryDelete(FilePath::combine(binDir, filename));
	}

	std::string outputCSS = getLibPrefix() + projectName + ".css";
	std::cout << "Cleaning " << outputCSS << std::endl;
	File::tryDelete(FilePath::combine((projectType == ProjectType::library) ? binDir : objDir, outputCSS));
}

void WebBuild::rebuild()
{
	clean();
	build();
}

void WebBuild::compile()
{
	std::vector<std::future<void>> results;

	int numThreads = std::max((int)(std::thread::hardware_concurrency() * 3) / 4, 2);
	for (int threadIndex = 0; threadIndex < numThreads; threadIndex++)
	{
		auto threadMain = [=]()
			{
				int i = 0;
				for (const std::string& inputFile : sourceFiles)
				{
					std::string filename = FilePath::lastComponent(inputFile);
					if (isCppFile(filename))
					{
						if (i++ % numThreads != threadIndex)
							continue;

						std::string cppFile = FilePath::combine(projectDir, inputFile);
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
							std::cout << filename << std::endl;
							std::string commandline = emcc + " " + compileFlags + " -MD -c " + cppFile + " -o " + objFile;
							std::string result = runCommand(commandline, "Could not compile " + filename);
							if (!result.empty())
								std::cout << result << std::endl;
						}
					}
				}
			};
		results.push_back(std::async(threadMain));
	}

	for (auto& result : results)
		result.get();
}

std::string WebBuild::runCommand(const std::string& commandline, const std::string& errorMessage)
{
	int result = std::system(commandline.c_str());
	if (result != 0)
		throw std::runtime_error(errorMessage);
	return std::string();
}

void WebBuild::link()
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

	std::string libraryPath = binDir;

	std::string outputFile;
	if (projectType == ProjectType::library)
		outputFile = "lib" + projectName + ".a";
	else if (projectType == ProjectType::component)
		outputFile = projectName + ".js";
	else
		outputFile = projectName + ".html";

	std::string outputPath = (projectType == ProjectType::library) ? binDir : objDir;

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

		for (const auto& dep : dependencies)
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
		if (projectType == ProjectType::library)
		{
			std::cout << "Creating static library " << outputFile << std::endl;

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

			std::string cmdline = emar + " @\"" + responsefilename + "\"";
			int result = std::system(("\"" + cmdline + "\"").c_str());
			if (result != 0)
				throw std::runtime_error("Could not link " + outputFile);
		}
		else
		{
			std::cout << "Linking " << outputFile << std::endl;

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

			responsefile += " -L " + FilePath::forceSlash(libraryPath);
			for (const auto& dep : dependencies)
			{
				responsefile += " -l" + dep;
			}

			File::writeAllText(responsefilename, responsefile);

			std::string cmdline = emcc + " @\"" + responsefilename + "\"";
			int result = std::system(("\"" + cmdline + "\"").c_str());
			if (result != 0)
				throw std::runtime_error("Could not link " + outputFile);
		}
	}
}

void WebBuild::linkCSS()
{
	std::string outputCSS = getLibPrefix() + projectName + ".css";
	std::string objFile = FilePath::combine((projectType == ProjectType::library) ? binDir : objDir, outputCSS);
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
		std::cout << FilePath::lastComponent(cssFile).c_str() << std::endl;

		std::vector<std::string> includes;
		std::string css;

		for (const auto& dep : dependencies)
		{
			std::string depFilename = FilePath::combine(binDir, "lib" + dep + ".css");
			css += File::readAllText(depFilename);
#ifdef WIN32
			css += "\r\n";
#else
			css += "\n";
#endif
			includes.push_back(depFilename);
		}

		css += processCSSFile(cssFile, File::readAllText(cssFile), includes, includePath);

		writeDependencyFile(depFile, cssFile, includes);
		File::writeAllText(objFile, css);
	}
}

std::string WebBuild::processCSSFile(const std::string& filename, std::string text, std::vector<std::string>& includes, const std::string& includePath, int level)
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
				try
				{
					includeFilename = FilePath::combine(includePath, token.value);
					includeText = File::readAllText(includeFilename);
				}
				catch (const std::exception&)
				{
					throw std::runtime_error(FilePath::normalizePathDelimiters(filename) + ": error: could not include '" + token.value);
				}
			}
			includes.push_back(includeFilename);
			includeCSS += processCSSFile(includeFilename, std::move(includeText), includes, includePath, level + 1);

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

void WebBuild::package()
{
	if (projectType != ProjectType::component && projectType != ProjectType::website)
		return;

	std::string outputHtml = projectName + ".html";
	std::string outputJS = projectName + ".js";
	std::string outputWasm = projectName + ".wasm";
	std::string outputCSS = projectName + ".css";
	std::string outputPackage = projectName + ".webpkg";

	std::string depFile = FilePath::combine(objDir, FilePath::removeExtension(projectName) + ".pkgdep");

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
		std::cout << "Building web package " << outputPackage.c_str() << std::endl;

		auto memdevice = MemoryDevice::create();
		auto zip = ZipWriter::create(memdevice);

		std::vector<std::string> outputFiles;
		outputFiles.push_back(outputCSS);
		if (projectType == ProjectType::website)
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

void WebBuild::addFolder(ZipWriter* zip, std::string srcdir, std::string destdir)
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

void WebBuild::loadProjectFile()
{
	JsonValue settings = JsonValue::parse(File::readAllText(FilePath::combine(projectDir, "WebBuild.json")));

	projectName = settings["name"].to_string();
	std::string type = settings["type"].to_string();
	std::string sources = substituteVars(settings["sources"].to_string());
	std::string resources = substituteVars(settings["resources"].to_string());
	binDir = FilePath::combine(projectDir, substituteVars(settings["bindir"].to_string()));
	objDir = FilePath::combine(projectDir, substituteVars(settings["objdir"].to_string()));

	Directory::create(binDir);
	Directory::create(objDir);

	if (type == "component")
		projectType = ProjectType::component;
	else if (type == "website")
		projectType = ProjectType::website;
	else if (type == "library")
		projectType = ProjectType::library;
	else if (type.empty())
		throw std::runtime_error("No project type specified in WebBuild.json");
	else
		throw std::runtime_error("Invalid project type '" + type + "' in WebBuild.json");

	for (const JsonValue& item : settings["dependencies"].items())
		dependencies.push_back(item.to_string());

	std::string searchPath = FilePath::combine(projectDir, sources);
	wwwrootDir = FilePath::combine(projectDir, resources);

	ignoreList = { projectName + ".vcxproj", projectName + ".vcxproj.filters", projectName + ".vcxproj.user" };

	findFiles(FilePath::combine(projectDir, sources));

	cssFile = FilePath::forceSlash(FilePath::combine(searchPath, projectName + ".css"));
	shellFile = FilePath::forceSlash(FilePath::combine(searchPath, "shell.html"));
	includePath = FilePath::forceSlash(FilePath::combine(projectDir, substituteVars(settings["includePath"].to_string())));

	// To do: use -gseparate-dwarf[=FILENAME] maybe

	std::string flags = "-s DISABLE_EXCEPTION_CATCHING=0";
	compileFlags = flags + " --std=c++23 -Werror -Wno-deprecated-this-capture -I " + includePath;

	if (projectType == ProjectType::component)
	{
		linkFlags = flags + " -s ALLOW_MEMORY_GROWTH=1 -s EXPORT_NAME=\"'" + projectName + "'\" -s MODULARIZE  -lembind";
	}
	else if (projectType == ProjectType::website)
	{
		linkFlags = flags + " -s ALLOW_MEMORY_GROWTH=1 --shell-file " + shellFile + " -lembind";
	}

	compileFlags = "-O1 " + compileFlags;
	linkFlags = "-O1 " + linkFlags;
}

void WebBuild::findFiles(const std::string& folder)
{
	findFiles(folder, {});
}

void WebBuild::findFiles(const std::string& folder, const std::string& relativeFolder)
{
	for (const std::string& name : Directory::folders(FilePath::combine(folder, "*")))
	{
		if (std::find(ignoreList.begin(), ignoreList.end(), name) == ignoreList.end())
		{
			findFiles(FilePath::combine(folder, name), FilePath::combine(relativeFolder, name));
		}
	}

	for (const std::string& name : Directory::files(FilePath::combine(folder, "*")))
	{
		if (std::find(ignoreList.begin(), ignoreList.end(), name) == ignoreList.end())
		{
			if (FilePath::hasExtension(name, "cpp") || FilePath::hasExtension(name, "cc"))
			{
				sourceFiles.push_back(FilePath::combine(relativeFolder, name));
			}
			else if (FilePath::hasExtension(name, "h") || FilePath::hasExtension(name, "hpp"))
			{
				headerFiles.push_back(FilePath::combine(relativeFolder, name));
			}
			else
			{
				extraFiles.push_back(FilePath::combine(relativeFolder, name));
			}
		}
	}
}

bool WebBuild::isCppFile(const std::string& filename)
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

std::vector<std::string> WebBuild::readMakefileDependencyFile(const std::string& filename)
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

std::vector<WebBuild::MakeToken> WebBuild::parseMakefile(const std::string& text)
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

std::vector<std::string> WebBuild::readDependencyFile(const std::string& filename)
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

void WebBuild::writeDependencyFile(const std::string& filename, const std::string& inputFile, const std::vector<std::string>& includes)
{
	JsonValue depJson = JsonValue::object();
	if (!inputFile.empty())
		depJson["Source"] = JsonValue::string(inputFile);
	depJson["Includes"] = JsonValue::array(includes);
	File::writeAllText(filename, depJson.to_json());
}

std::string WebBuild::substituteVars(std::string str)
{
	str = substituteVar(std::move(str), "$(Platform)", platform);
	str = substituteVar(std::move(str), "$(Configuration)", configuration);
	str = substituteVar(std::move(str), "$(ProjectName)", projectName);
	str = substituteVar(std::move(str), "$(ProjectDir)", projectDir);
	return str;
}

std::string WebBuild::substituteVar(std::string str, const std::string& name, const std::string& value)
{
	size_t i = 0;
	while (i < str.length())
	{
		size_t j = str.find(name, i);
		if (j == std::string::npos)
			break;
		str.replace(j, name.size(), value);
		i = j + value.size();
	}
	return str;
}
