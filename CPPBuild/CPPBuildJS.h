#pragma once

static const char* cppbuildJS = R"xxxx(

import { native } from "cppbuild/native";
export { Project, Targets, File, FilePath, Directory, Environment };

class Target
{
	constructor(subdirectory, type, name) {
		this.subdirectory = subdirectory;
		this.type = type;
		this.name = name;
		this.defines = [];
		this.includePaths = [];
		this.files = [];
		this.filters = [];
		this.linkLibraries = [];
		this.wwwRootDir = "wwwroot";
		this.cssRootFile = name + ".css";
		this.htmlShellFile = "shell.html";
	}

	addFiles(files) {
		var self = this;
		files.forEach(function(file) { self.addFile(file); });
	}

	addFile(file) {
		this.files.push(file);
	}

	addFilters(filters) {
		var self = this;
		filters.forEach(function(filter) { self.addFilter(filter); });
	}

	addFilter(filter) {
		this.filters.push(filter);
	}

	addDefines(defines) {
		var self = this;
		defines.forEach(function(define) { self.addDefine(define); });
	}

	addDefine(define) {
		this.defines.push(define);
	}

	addIncludePaths(paths) {
		var self = this;
		paths.forEach(function(path) { self.addIncludePath(path); });
	}

	addIncludePath(path) {
		this.includePaths.push(path);
	}

	addLinkLibraries(paths) {
		var self = this;
		paths.forEach(function(path) { self.addLinkLibrary(path); });
	}

	addLinkLibrary(lib) {
		this.linkLibraries.push(path);
	}

	setWebRootPath(path) {
		this.wwwrootDir = path;
	}

	setCSSRootFile(file) {
		this.cssRootFile = file;
	}

	setHtmlShellFile(file) {
		this.htmlShellFile = file;
	}

	toTargetDefinition() {
		return {
			subdirectory: this.subdirectory,
			type: this.type,
			name: this.name,
			defines: this.defines,
			includePaths: this.includePaths,
			files: this.files,
			filters: this.filters,
			linkLibraries: this.linkLibraries,
			wwwRootDir: this.wwwRootDir,
			cssRootFile: this.cssRootFile,
			htmlShellFile: this.htmlShellFile
		};
	}
}

class Configuration
{
	constructor(name, platform) {
		if (platform === undefined) {
			if (Environment.isX86()) {
				platform = "win32";
			}
			else if (Environment.isARM64()) {
				platform = "arm64";
			}
			else {
				platform = "x64";
			}
		}

		this.name = name;
		this.platform = platform;
	}

	toConfigDefinition() {
		return {
			name: this.name,
			platform: this.platform
		}
	}
}

var projectName = "";
var configList = [];
var targetList = [];

class Targets
{
	static addExecutable(name) {
		var target = new Target(native.subdirectory, "exe", name);
		targetList.push(target);
		return target;
	}

	static addWebsite(name) {
		var target = new Target(native.subdirectory, "website", name);
		targetList.push(target);
		return target;
	}

	static addWebComponent(name) {
		var target = new Target(native.subdirectory, "webcomponent", name);
		targetList.push(target);
		return target;
	}

	static addWebLibrary(name) {
		var target = new Target(native.subdirectory, "weblibrary", name);
		targetList.push(target);
		return target;
	}
}

class Project
{
	static setName(name) {
		projectName = name;
	}

	static addConfiguration(name, platform) {
		var config = new Configuration(name, platform);
		configList.push(config);
		return config;
	}

	static addSubdirectory(path) {
		native.addSubdirectory(path);
	}
}

class File
{
	static readAllText(filename) {
	}

	static readAllJson(filename) {
		return JSON.parse(File.readAllText());
	}

	static readAllBytes(filename) {
	}

	static writeAllText(filename, text) {
	}

	static writeAllJson(filename, obj) {
		File.writeAllText(filename, JSON.stringify(obj));
	}

	static writeAllBytes(filename, bytes) {
	}
}

class FilePath
{
	static hasExtension(filename, extension) {
		return false;
	}

	static extension(filename) {
	}

	static removeExtension(filename) {
	}

	static firstComponent(path) {
	}

	static removeFirstComponent(path) {
	}

	static lastComponent(path) {
	}

	static removeLastComponent(path) {
	}

	static combine(path1, path2) {
	}

	static forceSlash(path) {
	}

	static forceBackslash(path) {
	}
}

class Directory
{
	static files(filename) {
		return [];
	}

	static folders(filename) {
		return [];
	}

	static projectPath() {
	}

	static create(path) {
	}
}

class Environment
{
	static getVariable(name) {
	}

	static isMSVC() {
		return true;
	}

	static isClang() {
		return false;
	}

	static isGCC() {
		return false;
	}

	static isWindows() {
		return true;
	}

	static isUnix() {
		return false;
	}

	static isApple() {
		return false;
	}

	static isX86() {
		return false;
	}

	static isX64() {
		return true;
	}

	static isARM32() {
		return false;
	}

	static isARM64() {
		return false;
	}
}

native.generate = function() {
	return {
		name: projectName,
		targets: targetList.map(function(target) { return target.toTargetDefinition(); }),
		configurations: configList.map(function(config) { return config.toConfigDefinition(); })
	};
};

)xxxx";
