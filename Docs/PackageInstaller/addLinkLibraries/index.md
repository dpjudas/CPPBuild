# PackageInstaller: addLinkLibraries() method

**`PackageInstaller.addLinkLibraries()`** adds libraries to link to targets that use the package.

## Syntax

```js
addLinkLibraries(libs)
addLinkLibraries(libs, options)
```

### Parameters

- `libs`

    An array of strings. Each string is an library to link against.

- `options` - optional

    An object with additional parameters. See below.

### Additional parameters

- `configuration`

    Configuration name to apply the link libraries. Applies to all if not specified.

### Return value

None.
