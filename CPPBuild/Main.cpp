
#include "Precomp.h"
#include "CPPBuild.h"
#include "IOData/Directory.h"
#include "IOData/FilePath.h"
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
			workdir = Directory::currentDirectory();
		}

		CPPBuild app(workdir);
		if ((args.size() == 2 || args.size() == 3) && args[1] == "configure")
		{
			std::string sourcePath;
			if (args.size() == 3)
				sourcePath = args[2];
			app.configure(sourcePath);
			return 0;
		}
		if (args.size() == 2 && args[1] == "check-makefile")
		{
			app.checkMakefile();
			return 0;
		}
		else if (args.size() == 4 && args[1] == "build")
		{
			return app.build(args[2], args[3]);
		}
		else if (args.size() == 4 && args[1] == "clean")
		{
			app.clean(args[2], args[3]);
			return 0;
		}
		else if (args.size() == 4 && args[1] == "rebuild")
		{
			return app.rebuild(args[2], args[3]);
		}
		else if (args.size() == 2 && args[1] == "create-installer")
		{
			app.createInstaller();
			return 0;
		}
		else if (args.size() == 2 && args[1] == "create-package")
		{
			app.createPackage();
			return 0;
		}
		else if (args.size() == 4 && args[1] == "postbuild")
		{
			return app.postBuild(args[2], args[3]);
		}
		else
		{
			std::cout << "cppbuild configure [source path]" << std::endl;
			std::cout << "cppbuild [-workdir <path>] build <target> <configuration>" << std::endl;
			std::cout << "cppbuild [-workdir <path>] clean <target> <configuration>" << std::endl;
			std::cout << "cppbuild [-workdir <path>] rebuild <target> <configuration>" << std::endl;
			std::cout << "cppbuild [-workdir <path>] postbuild <target> <configuration>" << std::endl;
			std::cout << "cppbuild [-workdir <path>] create-installer" << std::endl;
			std::cout << "cppbuild [-workdir <path>] create-package" << std::endl;
			return 1;
		}
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
		return 2;
	}
}
