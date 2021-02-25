
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

		CPPBuild app;
		if (args.size() == 2 && args[1] == "generate")
		{
			app.generate();
			return 0;
		}
		else if (args.size() == 3 && args[1] == "build")
		{
			app.build(args[2]);
			return 0;
		}
		else if (args.size() == 3 && args[1] == "clean")
		{
			app.clean(args[2]);
			return 0;
		}
		else if (args.size() == 3 && args[1] == "rebuild")
		{
			app.rebuild(args[2]);
			return 0;
		}
		else
		{
			std::cout << "cppbuild generate" << std::endl;
			std::cout << "cppbuild build <target>" << std::endl;
			std::cout << "cppbuild clean <target>" << std::endl;
			std::cout << "cppbuild rebuild <target>" << std::endl;
			return 1;
		}
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
		return 255;
	}
}
