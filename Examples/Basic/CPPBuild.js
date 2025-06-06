import { Project, Targets, File, FilePath, Directory, Environment } from "cppbuild";

Project.setName("Basic");

Project.addConfiguration("Debug", "x64");
Project.addConfiguration("Release", "x64");

var files = [
	"Basic.cpp",
	"Basic.h"
];

var filters = [
];

var basic = Targets.addExecutable("Basic");
if (Environment.isWindows()) {
	basic.addDefines(["WIN32", "_WIN32", "UNICODE", "_UNICODE"]);
}
if (Environment.isMSVC()) {
	basic.addIncludePaths(["$(SolutionDir)"]);
}
basic.addFiles(files);
basic.addFilters(filters);
