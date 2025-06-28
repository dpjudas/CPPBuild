import { Targets } from "cppbuild";

var files = [
	"Configure.js",
	"Main.cpp",
	"Main.h",
	"Main.css",
	"Shell.html"
];

var website = Targets.addWebsite("Website");
website.setCSSRootFile("Main.css");
website.addFiles(files);
