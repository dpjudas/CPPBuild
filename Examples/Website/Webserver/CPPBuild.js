import { Targets } from "cppbuild";

var files = [
	"CPPBuild.js",
	"Main.cpp",
	"Main.h"
];

var webserver = Targets.addExecutable("Webserver");
webserver.addFiles(files);
