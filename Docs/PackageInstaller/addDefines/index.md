# PackageInstaller: addDefines() method

**`PackageInstaller.addDefines()`** adds defines to targets that use the package.

## Syntax

```js
addDefines(defines)
addDefines(defines, options)
```

### Parameters

- `defines`

    An array of strings. Each string is a define to set.

- `options` - optional

    An object with additional parameters. See below.

### Additional parameters

- `configuration`

    Configuration name to apply the compile options to. Applies to all if not specified.

- `lang`

    Language the options apply to. Can be `c` or `c++`. Applies to all if not specified.

### Return value

None.
