# Target: addLibraryPaths() method

**`Target.addLibraryPaths()`** adds library search paths to target.

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

- `files`

    An array of files to apply the include paths to. Applies to all if not specified.

- `configuration`

    Configuration name to apply the library paths to. Applies to all if not specified.

### Return value

None.
