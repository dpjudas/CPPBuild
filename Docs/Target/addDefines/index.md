# Target: addDefines() method

**`Target.addDefines()`** adds preprocessor defines.

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

- `files`

    An array of files to apply the defines to. Applies to all if not specified.

- `configuration`

    Configuration name to apply the defines to. Applies to all if not specified.

- `lang`

    Language the options apply to. Can be `c` or `c++`. Applies to all if not specified.

### Return value

None.
