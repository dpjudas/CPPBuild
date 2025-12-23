# CPPBuild

CPPBuild is a build system and a project generator for C++. It is licensed under a ZLib style license, see license.md for the details.

## Usage

```
cppbuild configure [source path]
cppbuild [-workdir <path>] build <target> <configuration>
cppbuild [-workdir <path>] clean <target> <configuration>
cppbuild [-workdir <path>] rebuild <target> <configuration>
cppbuild [-workdir <path>] create-installer
```

CPPBuild is configured by a script called Configure.js located at the root of the project. The script creates targets that can be used
to build the project.

`cppbuild configure` runs the script and outputs makefiles for the platform: a Visual Studio solution on Windows and a Makefile for
Linux. After this the project can be built either by running cppbuild directly (`cppbuild build <target> <configuration>`),
opening the Visual Studio, or via the Makefile (`make all`).

## Configure.js

A basic configure script may look like this:

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
	basic.addCompileOptions(["/YuPrecomp.h"]);
	basic.addCompileOptions(["/YcPrecomp.h"], { files: ["Precomp.cpp"], configuration: "Debug", platform: "x64" });
	basic.addCompileOptions(["/YcPrecomp.h"], { files: ["Precomp.cpp"], configuration: "Release", platform: "x64" });
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
	static addConfiguration(name, platform);
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

## Package class

The Packages class holds packages that can be used by targets.

```js
class Packages
{
	static add(name);
}
```

The add function adds a package to the list of packages. It returns an instance of the Package class:

```js
class Package
{
	getConfiguration(name);
	addSource(source, options);
	addDefines(defines, options);
	addCompileOptions(opts, options);
	addLinkOptions(opts, options);
	addIncludePaths(paths, options);
	addLinkLibraries(libs, options);
	addLibraryPaths(paths, options);
}
```

When a package is referenced by a target it adds the compiler and linker options from the package.
The `addSource` function specifies where the package is located.

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

## Installer classes

The following classes are used for generating MSI installers.

```js
class Installers
{
	static addInstaller(name);
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
