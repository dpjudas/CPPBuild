
#include "Precomp.h"
#include "MakefileWorkspace.h"
#include "BuildSetup.h"
#include "IOData/Directory.h"
#include "IOData/FilePath.h"
#include "IOData/File.h"

void MakefileWorkspace::generate(const BuildSetup& setup, const std::string& workDir, const std::string& cppbuildDir)
{
	std::string configName = "Release"; // To do: needs to be changable via cppbuild configure commandline

	std::string makefile = "\nall: ForceCppBuild\n\t@cppbuild build all " + configName + "\n\n";

	for (const BuildTarget& targetDef : setup.project.targets)
	{
		makefile += targetDef.name + ": ForceCppBuild\n";
		makefile += "\t@cppbuild build " + targetDef.name + " " + configName + "\n\n";
	}

	makefile += "clean: ForceCppBuild\n\t@cppbuild clean all " + configName + "\n\n";

	makefile += "ForceCppBuild:\n\n";

	File::writeAllText(FilePath::combine(workDir, "Makefile"), makefile);
}
