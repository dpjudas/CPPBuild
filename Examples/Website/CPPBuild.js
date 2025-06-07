import { Project, Targets, File, FilePath, Directory, Environment } from "cppbuild";

Project.setName("Website");

Project.addConfiguration("Debug");
Project.addConfiguration("Release");

var files = [
	"CPPBuild.js",
	"Main.cpp",
	"Main.h",
	"Main.css",
	"Shell.html"
];

var filters = [
];

var website = Targets.addWebsite("Website");
website.setCSSRootFile("Main.css");
website.addFiles(files);
website.addFilters(filters);
