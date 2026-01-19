# CPPBuild Documentation

CPPBuild is a build system and a project generator for C and C++ projects.

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
Linux.

After this the project can be built either by running cppbuild directly, opening the Visual Studio solution, running `msbuild`, or via `make all`.

`cppbuild build` will build a specific target.

`cppbuild clean` will remove all intermediate and output files from earlier builds.

`cppbuild rebuild` is shorthand for first running clean followed by build.

`cppbuild create-package` creates packages described via `PackageInstaller.add()`.

`cppbuild create-installer` creates MSI installers described via `Installer.add()`.

## Configure.js

A basic configure script might look like this:

```js
import { Project, Target, File, FilePath, Directory, Environment } from "cppbuild";

Project.setName("Basic");

Project.addConfiguration("Debug");
Project.addConfiguration("Release");

var files = [
	"Precomp.cpp",
	"Precomp.h",
	"Basic.cpp",
	"Basic.h",
];

var basic = Target.addConsole("Basic");
basic.addFiles(files);

if (Environment.isWindows()) {
	basic.addDefines(["WIN32", "_WIN32", "UNICODE", "_UNICODE"]);
}

basic.addDefines(["DEBUG"], { configuration: "Debug" });
basic.addDefines(["NDEBUG"], { configuration: "Release" });

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

## Interfaces

Interfaces that can be imported from the `cppbuild` javascript module:

- [Project](Project/index.md)

    Project globals.

- [Target](Target/index.md)

    Build targets.

- [Directory](Directory/index.md)

    Directory access.

- [File](File/index.md)

    File access.

- [FilePath](FilePath/index.md)

    Utility functions for working with file paths.

- [Environment](Environment/index.md)

    Environment information.

- [PackageInstaller](PackageInstaller/index.md)

    Package installers.

- [Installer](Installer/index.md)

    MSI installers.

- [InstallerComponent](InstallerComponent/index.md)

    A component in a MSI installer.

- [InstallerFeature](InstallerFeature/index.md)

    A feature in a MSI installer.

