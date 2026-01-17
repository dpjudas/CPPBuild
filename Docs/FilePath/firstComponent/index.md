# FilePath: firstComponent() static method

The **`firstComponent()`** static method of the `FilePath` object returns the first path component.

## Syntax

```js
FilePath.firstComponent(path)
```

### Parameters

- `path`

    A string with the file path.

### Return value

A string with the component, if any.

## Description

**`firstComponent()`** returns the first component for a path with multiple components. If the path only
has one component the entire path is returned.
