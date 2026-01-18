# PackageInstaller: addDefines() method

**`PackageInstaller.addIncludePaths()`** adds include paths to targets that use the package.

## Syntax

```js
addIncludePaths(paths)
addIncludePaths(paths, options)
```

### Parameters

- `paths`

    An array of strings. Each string is an include path to add.

- `options` - optional

    An object with additional parameters. See below.

### Additional parameters

- `configuration`

    Configuration name to apply the include paths to. Applies to all if not specified.

### Return value

None.
