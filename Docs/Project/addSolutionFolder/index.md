# Project: addSolutionFolder() static method

**`Project.addSolutionFolder()`** adds a Visual Studio solution folder containing file items.

This is a Visual Studio-only feature and is ignored by other generators (e.g. Makefile).

## Syntax

```js
Project.addSolutionFolder(name, files)
```

### Parameters

- `name`

    A string with the folder name. Use a `/`-separated path to nest folders inside
    each other. The parent folder must also be registered with its own
    `addSolutionFolder()` call.

- `files`

    An array of strings with the file paths to list in the folder. Paths are
    relative to the root configure script. Wildcard patterns via
    [`Directory.files()`](../../Directory/files/index.md) are supported.

### Return value

None.

## Examples

### Flat folder

```js
import { Project, Directory } from "cppbuild";

Project.addSolutionFolder("Scripts", Directory.files("Scripts/*.ps1"));
```

### Nested folders

```js
import { Project, Directory } from "cppbuild";

Project.addSolutionFolder("Documentation", ["README.md"].concat(Directory.files("Docs/*.md")));
Project.addSolutionFolder("Documentation/API", Directory.files("Docs/API/*.yaml"));
```

This produces a `Documentation` folder in the solution containing the markdown files,
with an `API` subfolder inside it containing the YAML files.
