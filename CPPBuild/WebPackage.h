#pragma once

#include <string>
#include <vector>

class ZipWriter;

class WebPackage
{
public:
	static void build(std::string outputFilename, std::string sourceDir);
	static void clean(std::string outputFilename, std::string sourceDir);

private:
	static void compile_folder(const std::string& emcc, std::string srcdir, std::vector<std::string>& outputFiles);
	static void link_exe(const std::string& emcc, std::string outputExe, const std::vector<std::string>& objFiles);
	static void add_folder(ZipWriter* zip, std::string srcdir, std::string destdir);

	enum class MakeTokenType
	{
		string,
		colon,
		newline
	};

	struct MakeToken
	{
		MakeTokenType type = MakeTokenType::string;
		std::string value;
	};

	static std::vector<MakeToken> parse_makefile(const std::string& text);
	static std::vector<std::string> read_dependency_file(const std::string& filename);
	static std::string replace_all(const std::string& text, const std::string& search, const std::string& replace);
};
