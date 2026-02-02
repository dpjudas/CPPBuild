# Target: addCopyfiles() method

**`Target.addCopyfiles()`** adds files to copy to the output folder.

## Syntax

```js
addCopyfiles(files)
addCopyfiles(files, options)
```

### Parameters

- `files`

    An array of files. Each file is either a string with a path, or an object with two members: `src` and `dest`.

- `options` - optional

    An object with additional parameters. See below.

### Additional parameters

- `configuration`

    Configuration name to apply the action to. Applies to all if not specified.

### Return value

None.
