
#include "Precomp.h"
#include "CPPBuild.h"
#include <iostream>

int main(int argc, char** argv)
{
	try
	{
		std::vector<std::string> args;
		for (int i = 0; i < argc; i++)
			args.push_back(argv[i]);

		std::string workdir;
		if (args.size() > 3 && args[1].substr(0, 8) == "-workdir")
		{
			workdir = args[2];
			args.erase(args.begin() + 1, args.begin() + 3);
		}
		else
		{
			workdir = "build";
		}

		CPPBuild app;
		if ((args.size() == 2 || args.size() == 3) && args[1] == "generate")
		{
			std::string sourcePath;
			if (args.size() == 3)
				sourcePath = args[2];
			app.generate(sourcePath);
			return 0;
		}
		else if (args.size() == 3 && args[1] == "build")
		{
			app.build(workdir, args[2]);
			return 0;
		}
		else if (args.size() == 3 && args[1] == "clean")
		{
			app.clean(workdir, args[2]);
			return 0;
		}
		else if (args.size() == 3 && args[1] == "rebuild")
		{
			app.rebuild(workdir, args[2]);
			return 0;
		}
		else
		{
			std::cout << "cppbuild generate [source path]" << std::endl;
			std::cout << "cppbuild [-workdir <path>] build [target]" << std::endl;
			std::cout << "cppbuild [-workdir <path>] clean [target]" << std::endl;
			std::cout << "cppbuild [-workdir <path>] rebuild [target]" << std::endl;
			return 1;
		}
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
		return 255;
	}
}
