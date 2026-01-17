# FilePath: removeLastComponent() static method

The **`removeLastComponent()`** static method of the `FilePath` object removes the last path component and returns the result.

## Syntax

```js
FilePath.removeLastComponent(path)
```

### Parameters

- `path`

    A string with the file path.

### Return value

A string with the last component removed.

## Description

**`removeLastComponent()`** removes the last path component and returns the result. If there is only a single component
an empty string is returned.
