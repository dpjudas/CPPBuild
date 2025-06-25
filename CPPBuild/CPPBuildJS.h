#pragma once

static const char* cppbuildJS = R"xxxx(

import { native } from "cppbuild/native";
export { Project, Targets, Installers, File, FilePath, Directory, Environment };

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

	addLinkLibraries(libs) {
		var self = this;
		libs.forEach(function(lib) { self.addLinkLibrary(lib); });
	}

	addLinkLibrary(lib) {
		this.linkLibraries.push(lib);
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
var installerList = [];

class Targets
{
	static addApplication(name) {
		var target = new Target(native.subdirectory, "application", name);
		targetList.push(target);
		return target;
	}

	static addConsole(name) {
		var target = new Target(native.subdirectory, "console", name);
		targetList.push(target);
		return target;
	}

	static addStaticLibrary(name) {
		var target = new Target(native.subdirectory, "lib", name);
		targetList.push(target);
		return target;
	}

	static addDynamicLibrary(name) {
		var target = new Target(native.subdirectory, "dll", name);
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
		return native.readAllText(filename);
	}

	static readAllJson(filename) {
		return JSON.parse(File.readAllText());
	}

	static readAllBytes(filename) {
		return native.readAllBytes(filename);
	}

	static writeAllText(filename, text) {
		native.writeAllText(filename, text);
	}

	static writeAllJson(filename, obj) {
		File.writeAllText(filename, JSON.stringify(obj));
	}

	static writeAllBytes(filename, bytes) {
		native.writeAllBytes(filename, bytes);
	}
}

class FilePath
{
	static hasExtension(filename, ext) {
		return FilePath.extension(filename).toLowerCase() == ext;
	}

	static extension(filename) {
		var file = FilePath.lastComponent(filename);
		var pos = file.lastIndexOf(".");
		if (pos == -1) {
			return "";
		}
		else {
			return filename.substring(pos + 1);
		}
	
	}

	static removeExtension(filename) {
		var file = FilePath.lastComponent(filename);
		var pos = file.lastIndexOf(".");
		if (pos == -1) {
			return filename;
		}
		else {
			return filename.substring(0, filename.length - file.length + pos);
		}
	}

	static firstComponent(path) {
		var firstSlash = path.indexOf("/");
		var firstBackslash = path.indexOf("\\");
		if (firstBackslash != -1 && (firstSlash == -1 || firstSlash > firstBackslash)) {
			firstSlash = firstBackslash;
		}

		if (firstSlash != -1) {
			return path.substring(0, firstSlash);
		}
		else {
			return path;
		}
	}

	static removeFirstComponent(path) {
		var firstSlash = path.indexOf("/");
		var firstBackslash = path.indexOf("\\");
		if (firstBackslash != -1 && (firstSlash == -1 || firstSlash > firstBackslash)) {
			firstSlash = firstBackslash;
		}

		if (firstSlash != -1) {
			return path.substring(firstSlash + 1);
		}
		else {
			return "";
		}
	}

	static lastComponent(path) {
		var lastSlash = path.lastIndexOf("/");
		var lastBackslash = path.lastIndexOf("\\");
		if (lastSlash != -1 && lastSlash >= lastBackslash) {
			return path.substring(lastSlash + 1);
		}
		else if (lastBackslash != -1 && lastBackslash >= lastSlash) {
			return path.substring(lastBackslash + 1);
		}
		else {
			return path;
		}
	}

	static removeLastComponent(path) {
		var lastSlash = path.lastIndexOf("/");
		var lastBackslash = path.lastIndexOf("\\");
		if (lastSlash != -1 && lastSlash >= lastBackslash) {
			return path.substring(0, lastSlash);
		}
		else if (lastBackslash != -1 && lastBackslash >= lastSlash) {
			return path.substring(0, lastBackslash);
		}
		else {
			return "";
		}
	}

	static combine(path1, path2) {
		if (path1 == "") {
			return FilePath.forceSlash(path1);
		}
		else if (path2 == "") {
			return FilePath.forceSlash(path1);
		}
		else if (path2.startsWith("\\") || path2.startsWith("/")) {
			return FilePath.forceSlash(path2);
		}
		else if (path2.length >= 2 && path2[1] == ':') {
			return FilePath.forceSlash(path2);
		}
		else if (!path2.endsWith("\\") && !path2.endsWith("/")) {
			return FilePath.forceSlash(path1 + "/" + path2);
		}
		else {
			return FilePath.forceSlash(path1 + path2);
		}
	}

	static forceSlash(path) {
		return path.replaceAll("\\", "/");
	}

	static forceBackslash(path) {
		return path.replaceAll("/", "\\");
	}
}

class Directory
{
	static files(filename) {
		return native.getFiles(filename);
	}

	static folders(filename) {
		return native.getFolders(filename);
	}

	static projectPath() {
		return native.subdirectory;
	}

	static create(path) {
		return native.createDirectory(path);
	}
}

class Environment
{
	static getVariable(name) {
		return native.getEnvironmentVar(name);
	}

	static isMSVC() {
		return native.environmentMsvc;
	}

	static isClang() {
		return native.environmentClang;
	}

	static isGCC() {
		return native.environmentGCC;
	}

	static isWindows() {
		return native.environmentWindows;
	}

	static isUnix() {
		return native.environmentUnix;
	}

	static isApple() {
		return native.environmentApple;
	}

	static isX86() {
		return native.environmentX86;
	}

	static isX64() {
		return native.environmentX64;
	}

	static isARM32() {
		return native.environmentARM32;
	}

	static isARM64() {
		return native.environmentARM64;
	}
}

class Installer
{
	constructor(subdirectory, name) {
		this.subdirectory = subdirectory;
		this.name = name;
		this.installDir = name;
		this.components = [];
		this.features = [];
		this.msiProductName = name;
		this.msiProductVersion = "01.01.0000";
		this.msiManufacturer = "";
		this.msiProductCode = "";
		this.msiUpgradeCode = "";
		this.msiPackageCode = "";
		this.msiProductKeywords = [ "Installer" ];
	}

	setInstallDir(path) {
		this.installDir = path;
	}

	setMsiProductName(name) {
		this.msiProductName = name;
	}

	setMsiProductVersion(version) {
		this.msiProductVersion = version;
	}

	setMsiManufacturer(manufacturer) {
		this.msiManufacturer = manufacturer;
	}

	setMsiProductCode(productCode) {
		this.msiProductCode = productCode;
	}

	setMsiUpgradeCode(upgradeCode) {
		this.msiUpgradeCode = upgradeCode;
	}

	setMsiPackageCode(packageCode) {
		this.msiPackageCode = packageCode;
	}

	addMsiProductKeywords(keywords) {
		var self = this;
		keywords.forEach(function(keyword) { self.addMsiProductKeyword(keyword); });
	}

	addMsiProductKeyword(keyword) {
		this.msiProductKeywords.push(keyword);
	}

	addComponent(name) {
		var component = new InstallerComponent(name);
		this.components.push(component);
		return component;
	}

	addFeature(name) {
		var feature = new InstallerFeature(name);
		this.features.push(feature);
		return feature;
	}

	toInstallerDefinition() {
		return {
			subdirectory: this.subdirectory,
			name: this.name,
			installDir: this.installDir,
			components: this.components.map(function(component) { return component.toInstallerDefinition(); }),
			features: this.features.map(function(feature) { return feature.toInstallerDefinition(); }),
			msiProductName: this.msiProductName,
			msiProductVersion: this.msiProductVersion,
			msiManufacturer: this.msiManufacturer,
			msiProductCode: this.msiProductCode,
			msiUpgradeCode: this.msiUpgradeCode,
			msiPackageCode: this.msiPackageCode,
			msiProductKeywords: this.msiProductKeywords
		};
	}
}

class InstallerComponent
{
	constructor(name) {
		this.name = name;
		this.msiComponentId = "";
		this.files = [];
	}

	setMsiComponentId(id) {
		this.msiComponentId = id;
	}

	addFiles(files) {
		var self = this;
		files.forEach(function(file) { self.addFile(file); });
	}

	addFile(file) {
		this.files.push(file);
	}

	toInstallerDefinition() {
		return {
			name: this.name,
			msiComponentId: this.msiComponentId,
			files: this.files
		};
	}
}

class InstallerFeature
{
	constructor(name) {
		this.name = name;
		this.components = [];
	}

	addComponents(components) {
		var self = this;
		components.forEach(function(component) { self.addComponent(component); });
	}

	addComponent(component) {
		this.components.push(component);
	}

	toInstallerDefinition() {
		return {
			name: this.name,
			components: this.components.map(function(component) { return component.name; })
		};
	}
}

class Installers
{
	static addInstaller(name) {
		var installer = new Installer(native.subdirectory, name);
		installerList.push(installer);
		return installer;
	}
}

native.generate = function() {
	return {
		name: projectName,
		targets: targetList.map(function(target) { return target.toTargetDefinition(); }),
		configurations: configList.map(function(config) { return config.toConfigDefinition(); }),
		installers: installerList.map(function(installer) { return installer.toInstallerDefinition(); })
	};
};

)xxxx";
