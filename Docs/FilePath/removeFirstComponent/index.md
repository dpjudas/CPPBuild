# FilePath: removeFirstComponent() static method

The **`removeFirstComponent()`** static method of the `FilePath` object removes the first path component and returns the result.

## Syntax

```js
FilePath.removeFirstComponent(path)
```

### Parameters

- `path`

    A string with the file path.

### Return value

A string with the first component removed.

## Description

**`removeFirstComponent()`** removes the first path component and returns the result. If there is only a single component
an empty string is returned.
