# FilePath: combine() static method

The **`combine()`** static method of the `FilePath` object combines multiple paths into one.

## Syntax

```js
FilePath.combine(path1, path2, ...)
FilePath.combine(paths)
```

### Parameters

- `path1`

    A string with the first path to combine.

- `path2`

    A string with the second path to combine.

- `paths`

    An array of strings. Each string is a path to combine.

-  `...`

    Each extra parameter is an additional string path to combine.

### Return value

A string with the combined path.

## Description

**`combine()`** combines multiple paths into one.

The combined path is constructed by using the first path as a base path. If the second path is relative it
is appended to the first. If the second path is absolute the first path is ignored.
