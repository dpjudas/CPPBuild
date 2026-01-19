# Target

The **`Target`** interface is used to describe targets.

## Static methods

- [`Target.addApplication()`](addApplication/index.md)

    Adds an application target (/subsystem:windows, app bundle).

- [`Target.addConsole()`](addConsole/index.md)

    Adds a console target (/subsystem:console).

- [`Target.addStaticLibrary()`](addStaticLibrary/index.md)

    Adds a static library target (.lib, .a).

- [`Target.addDynamicLibrary()`](addDynamicLibrary/index.md)

    Adds a dynamic library target (.dll, .so, .dylib).

- [`Target.addWebsite()`](addWebsite/index.md)

    Adds an emscripten website target (.webpkg).

- [`Target.addWebComponent()`](addWebComponent/index.md)

    Adds an emscripten web component target.

- [`Target.addWebLibrary()`](addWebLibrary/index.md)

    Adds an emscripten static library with CSS resources target.

## Instance methods

- [`Target.addFiles()`](addFiles/index.md)

    Add input files.

- [`Target.addDefines()`](addDefines/index.md)

    Add preprocessor defines.

- [`Target.addCompileOptions()`](addCompileOptions/index.md)

    Add compiler options.

- [`Target.addIncludePaths()`](addIncludePaths/index.md)

    Add include paths.

- [`Target.addLinkOptions()`](addLinkOptions/index.md)

    Add linker options.

- [`Target.addLinkLibraries()`](addLinkLibraries/index.md)

    Add libraries to link against.

- [`Target.addLibraryPaths()`](addLibraryPaths/index.md)

    Add library search paths.

- [`Target.addPackages()`](addPackages/index.md)

    Add packages to use.

- [`Target.setWebRootPath()`](setWebRootPath/index.md)

    Set path to folder containing static files to be included.

- [`Target.setCSSRootFile()`](setCSSRootFile/index.md)

    Set main CSS input file.

- [`Target.setHtmlShellFile()`](setHtmlShellFile/index.md)

    Set HTML shell input file.

