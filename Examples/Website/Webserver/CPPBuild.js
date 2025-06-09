import { Targets } from "cppbuild";

var files = [
	"CPPBuild.js",
	"Precomp.cpp",
	"Precomp.h",
	"Main.cpp",
	"Main.h",
];

var webserver = Targets.addConsole("Webserver");
webserver.addFiles(files);
webserver.addIncludePaths([".", ".."]);
webserver.addDefines(["WIN32", "_WIN32", "UNICODE", "_UNICODE"]);
webserver.addLinkLibrary("Common");
