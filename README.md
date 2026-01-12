# CPPBuild

CPPBuild is a build system and a project generator for C++. It is licensed under a ZLib style license, see license.md for the details.

## Usage

```
cppbuild configure [source path]
cppbuild [-workdir <path>] build <target> <configuration>
cppbuild [-workdir <path>] clean <target> <configuration>
cppbuild [-workdir <path>] rebuild <target> <configuration>
cppbuild [-workdir <path>] create-package
cppbuild [-workdir <path>] create-installer
```

`cppbuild configure` runs `Configure.js` and outputs makefiles for the platform: a Visual Studio solution on Windows and a Makefile for
Linux. After this the project can be built either by running cppbuild directly, opening the Visual Studio solution, or via `make all`.

`cppbuild build` will build a specific target. `clean` will remove all intermediate and output files from earlier builds. And `rebuild`
is shorthand for run clean first and then build.

`cppbuild create-package` creates packages described by the configure script. `create-installer` creates MSI installers.

## Configure.js

A basic configure script might look like this:

```js
import { Project, Targets, File, FilePath, Directory, Environment } from "cppbuild";

Project.setName("Basic");

Project.addConfiguration("Debug", "x64");
Project.addConfiguration("Release", "x64");

var files = [
	"Precomp.cpp",
	"Precomp.h",
	"Basic.cpp",
	"Basic.h",
];

var basic = Targets.addConsole("Basic");
basic.addFiles(files);

if (Environment.isWindows()) {
	basic.addDefines(["WIN32", "_WIN32", "UNICODE", "_UNICODE"]);
}

if (Environment.isMSVC()) {
	basic.addCompileOptions(["/YuPrecomp.h"]);
	basic.addCompileOptions(["/YcPrecomp.h"], { files: ["Precomp.cpp"], configuration: "Debug" });
	basic.addCompileOptions(["/YcPrecomp.h"], { files: ["Precomp.cpp"], configuration: "Release" });
}
```

The script is using standard javascript, which means it can import other javascripts via the import clause. It can also run additional
scripts using `Project.addSubdirectory(subdirectoryName)`, which will then use paths relative to the subdirectory.

The `cppbuild` javascript module is special. It is the cppbuild interface available to the script. See below for a description of each
class exported by the cppbuild module.

## Project class

The project description for what is being configured. It is required to set the name of the project and add at least one configuration.

Targets are added via the `Targets` class. The `addSubdirectory` function runs additional configure scripts in subdirectories. Each script
uses the name of the directory it is placed in. For example, if the subdirectory is named "Foobar" then the script name is "Foobar.js"

```js
class Project
{
	static setName(name);
	static addConfiguration(name);
	static addSubdirectory(path);
}
```

## Targets class

The Targets class is used to add targets to the project. There are currently 7 target types:

```js
class Targets
{
	static addApplication(name);    // UI application (/subsystem:windows, app bundle)
	static addConsole(name);        // Console application (/subsystem:console)
	static addStaticLibrary(name);  // Static library (.lib or .a)
	static addDynamicLibrary(name); // Dynamic library (.dll or .so)
	static addWebsite(name);        // Emscripten website project
	static addWebComponent(name);   // Emscripten web component project
	static addWebLibrary(name);     // Emscripten static library with CSS resources
}
```

Each of the add functions returns an instance of the Target class. It can be used to add files, defines, libraries, etc.

```js
class Target
{
	getConfiguration(name);
	addFiles(files);
	addFilters(filters);
	addDefines(defines, options);
	addCompileOptions(opts, options);
	addIncludePaths(paths, options);
	addLinkOptions(opts, options);
	addLinkLibraries(libs, options);
	addLibraryPaths(paths, options);
	addPackages(names, options);
	setWebRootPath(path);
	setCSSRootFile(file);
	setHtmlShellFile(file);
}
```

## Packages

The Packages class holds packages that can then be used by targets.

```js
class Packages
{
	static add(source);
}
```

When a package is referenced by a target it adds the compiler and linker options from the package.

## File class

The File class is used to read or write files from the file system.

```js
class File
{
	static readAllText(filename);
	static readAllJson(filename);
	static readAllBytes(filename);
	static writeAllText(filename, text);
	static writeAllJson(filename, obj);
	static writeAllBytes(filename, bytes);
}
```

## FilePath class

The FilePath class helps working with path strings.

```js
class FilePath
{
	static hasExtension(filename, ext);
	static extension(filename);
	static removeExtension(filename);

	static firstComponent(path);
	static removeFirstComponent(path);

	static lastComponent(path);
	static removeLastComponent(path);

	static combine(path1, path2);

	static forceSlash(path);
	static forceBackslash(path);
}
```

## Directory class

The Directory class lists files and folders.

```js
class Directory
{
	static files(filename);
	static folders(filename);
	static projectPath();
	static create(path);
}
```

## Environment class

The Environment class returns information about the environment cppbuild is executing from.

`getVariable` returns an environment variable by name. The other functions are for detecting common platforms and compilers.

```js
class Environment
{
	static getVariable(name);
	static isMSVC();
	static isClang();
	static isGCC();
	static isWindows();
	static isUnix();
	static isApple();
	static isX86();
	static isX64();
	static isARM32();
	static isARM64();
}
```

## Installers

Installers are used for deploying build artifacts.

```js
class Installers
{
	static addPackage(name);
	static addInstaller(name);
}
```

## Package Installer

`Installers.addPackage` returns an instance of the package installer class:

```js
class PackageInstaller
{
	getConfiguration(name);
	addDefines(defines, options);
	addCompileOptions(opts, options);
	addLinkOptions(opts, options);
	addIncludePaths(paths, options);
	addLinkLibraries(libs, options);
	addLibraryPaths(paths, options);
}
```

## MSI Installer

`Installers.addInstaller` returns an instance of the MSI installer class:

```js
class Installers
{
	static addInstaller(name);
	static addPackage(name);
}

class Installer
{
	setInstallDir(path);
	setMsiProductName(name);
	setMsiProductVersion(version);
	setMsiManufacturer(manufacturer);
	setMsiProductCode(productCode);
	setMsiUpgradeCode(upgradeCode);
	setMsiPackageCode(packageCode);
	addMsiProductKeywords(keywords);
	addMsiProductKeyword(keyword);

	addComponent(name);
	addFeature(name);
}

class InstallerComponent
{
	setMsiComponentId(id);
	addFiles(files);
	addFile(file);
}

class InstallerFeature
{
	addComponents(components);
	addComponent(component);
}
```
