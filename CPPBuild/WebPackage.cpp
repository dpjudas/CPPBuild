
#include "Precomp.h"
#include "WebPackage.h"
#include "File.h"
#include "ZipWriter.h"
#include "JsonValue.h"
#include <string>
#include <iostream>
#include <cstdlib>

void WebPackage::build(std::string outputFilename, std::string sourceDir)
{
	std::string mapbase = "/";
	std::string shellfile = FilePath::combine(sourceDir, "Sources/Framework/shell.html");
	std::string preJS = FilePath::combine(sourceDir, "Sources/Framework/JSCallback.js");
	std::string includePath = FilePath::combine(sourceDir, "Sources");

	// To do: use -gseparate-dwarf[=FILENAME] maybe

	std::string flags = "--bind -s DISABLE_EXCEPTION_CATCHING=0";
	std::string compileFlags = flags + " -I " + includePath;
	std::string linkFlags = flags + " --pre-js=" + preJS + " --shell-file " + shellfile;

	std::string compileFlagsDebug = "-g -O0 " + compileFlags;
	std::string linkFlagsDebug = "-g4 -O0 --source-map-base " + mapbase + " " + linkFlags;

	std::string compileFlagsRelease = "-O2 " + compileFlags;
	std::string linkFlagsRelease = "-O2 " + linkFlags;

	std::string emcc = "C:\\Development\\musicplayer\\Thirdparty\\emsdk\\upstream\\emscripten\\emcc";

	std::string outputHtml = "index.html";
	std::string outputJS = "index.js";
	std::string outputWasm = "index.wasm";
	std::string outputMap = "index.wasm.map";

	std::vector<std::string> objFiles;
	compile_folder(emcc + " " + compileFlagsDebug, FilePath::combine(sourceDir, "Sources"), objFiles);
	link_exe(emcc + " " + linkFlagsDebug, outputHtml, objFiles);

	auto zip = ZipWriter::create(outputFilename);
	for (std::string filename : { outputHtml, outputJS, outputWasm })
	{
		std::vector<uint8_t> data = File::read_all_bytes(filename);
		zip->add_file(FilePath::last_component(filename), data.data(), data.size());
	}
	add_folder(zip.get(), FilePath::combine(sourceDir, "wwwroot"), "");

	// Add every file listed to the package and remap the filenames
	std::vector<std::pair<std::string, std::string>> sources;
	JsonValue mapJson = JsonValue::parse(File::read_all_text(outputMap));
	for (JsonValue& source : mapJson["sources"].items())
	{
		std::string srcfilename = source.to_string();
		std::string destfilename = replace_all(srcfilename, "../", "");
		source.set_string(destfilename);
		sources.emplace_back(srcfilename, destfilename);
	}
	// sourcemaps.info spec says the order of the properties matters:
	std::string outputMapJson = "{";
	std::vector<std::string> propOrder = { "version", "file", "sourceRoot", "sources", "sourcesContent", "names", "mappings" };
	bool firstprop = true;
	for (std::string prop : propOrder)
	{
		if (!mapJson[prop].is_undefined())
		{
			if (!firstprop)
				outputMapJson += ",";
			firstprop = false;
			outputMapJson += "\r\n\"" + prop + "\": ";
			outputMapJson += mapJson[prop].to_json();
		}
	}
	outputMapJson += "\r\n}";
	//File::write_all_text("index.wasm.map.txt", outputMapJson);
	zip->add_file(outputMap, outputMapJson.data(), outputMapJson.size());
	for (const auto& source : sources)
	{
		std::vector<uint8_t> data = File::read_all_bytes(source.first);
		zip->add_file(source.second, data.data(), data.size());
	}

	zip->finalize_archive();
	zip.reset();
}

void WebPackage::clean(std::string outputFilename, std::string sourceDir)
{
	// File::deleteFile("musicplayer.webpkg");
}

std::string WebPackage::replace_all(const std::string& text, const std::string& search, const std::string& replace)
{
	std::string result;
	size_t pos = 0;
	while (true)
	{
		size_t pos2 = text.find(search, pos);
		if (pos2 == std::string::npos)
		{
			result.append(&text[pos], text.size() - pos);
			return result;
		}
		else
		{
			result.append(&text[pos], pos2 - pos);
			result.append(replace);
			pos = pos2 + search.size();
		}
	}
}

void WebPackage::link_exe(const std::string& emcc, std::string outputExe, const std::vector<std::string>& objFiles)
{
	bool needsLink = false;
	try
	{
		int64_t exeTime = File::get_last_write_time(outputExe);
		for (const std::string& dependency : objFiles)
		{
			int64_t depTime = File::get_last_write_time(dependency);
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
		std::cout << "Linking " << outputExe << std::endl;
		std::string cmdline;
		cmdline.reserve(64 * 1024);
		cmdline += emcc;
		cmdline += " -o ";
		cmdline += outputExe;
		for (const auto& file : objFiles)
		{
			cmdline.push_back(' ');
			cmdline += file;
		}
		int result = std::system(cmdline.c_str());
		if (result != 0)
			throw std::runtime_error("Could not link " + outputExe);
	}
}

void WebPackage::compile_folder(const std::string& emcc, std::string srcdir, std::vector<std::string>& outputFiles)
{
	for (const std::string& filename : Directory::folders(FilePath::combine(srcdir, "*.*")))
	{
		compile_folder(emcc, FilePath::combine(srcdir, filename), outputFiles);
	}

	for (const std::string& filename : Directory::files(FilePath::combine(srcdir, "*.cpp")))
	{
		std::string cppFile = FilePath::combine(srcdir, filename);
		std::string objFile = FilePath::remove_extension(filename) + ".obj";
		std::string depFile = FilePath::remove_extension(filename) + ".d";

		bool needsCompile = false;
		try
		{
			int64_t objTime = File::get_last_write_time(objFile);
			for (const std::string& dependency : read_dependency_file(depFile))
			{
				int64_t depTime = File::get_last_write_time(dependency);
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
			int result = std::system((emcc + " -MD -c " + cppFile + " -o " + objFile).c_str());
			if (result != 0)
				throw std::runtime_error("Could not compile " + cppFile);
		}

		outputFiles.push_back(objFile);
	}
}

void WebPackage::add_folder(ZipWriter* zip, std::string srcdir, std::string destdir)
{
	for (const std::string& filename : Directory::files(FilePath::combine(srcdir, "*.*")))
	{
		std::vector<uint8_t> data = File::read_all_bytes(FilePath::combine(srcdir, filename));
		zip->add_file(FilePath::combine(destdir, filename), data.data(), data.size());
	}

	for (const std::string& filename : Directory::folders(FilePath::combine(srcdir, "*.*")))
	{
		add_folder(zip, FilePath::combine(srcdir, filename), FilePath::combine(destdir, filename));
	}
}

std::vector<std::string> WebPackage::read_dependency_file(const std::string& filename)
{
	std::vector<MakeToken> tokens = parse_makefile(File::read_all_text(filename));

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

std::vector<WebPackage::MakeToken> WebPackage::parse_makefile(const std::string& text)
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
