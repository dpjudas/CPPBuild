# FilePath: lastComponent() static method

The **`lastComponent()`** static method of the `FilePath` object returns the last path component.

## Syntax

```js
FilePath.lastComponent(path)
```

### Parameters

- `path`

    A string with the file path.

### Return value

A string with the component.

## Description

**`lastComponent()`** returns the last component for a path with multiple components. If the path only
has one component the entire path is returned.
