# Target: addCompileOptions() method

**`Target.addCompileOptions()`** adds compile options

## Syntax

```js
addCompileOptions(compileOptions)
addCompileOptions(compileOptions, opt)
```

### Parameters

- `compileOptions`

    An array of strings. Each string is a compile option.

- `opt` - optional

    An object with additional parameters. See below.

### Additional parameters

- `files`

    An array of files to apply the options to. Applies to all if not specified.

- `configuration`

    Configuration name to apply the compile options to. Applies to all if not specified.

- `lang`

    Language the options apply to. Can be `c` or `c++`. Applies to all if not specified.

### Return value

None.
