# PackageInstaller: addLibraryPaths() method

**`PackageInstaller.addLibraryPaths()`** adds library paths to targets that use the package.

## Syntax

```js
addLibraryPaths(paths)
addLibraryPaths(paths, options)
```

### Parameters

- `paths`

    An array of strings. Each string is an library path to add.

- `options` - optional

    An object with additional parameters. See below.

### Additional parameters

- `configuration`

    Configuration name to apply the library paths to. Applies to all if not specified.

### Return value

None.
