# Directory: currentPath() static method

The **`currentPath()`** static method of the `Directory` object
returns the path to the current configure script directory.

## Syntax

```js
Directory.currentPath()
Directory.currentPath(relativePath)
```

### Parameters

- `relativePath` - optional

    A path relative to the current path that it is combined with.
    
    This is short form for `FilePath.combine(Directory.currentPath(), relativePath)`.

### Return value

A string with a path.

## Description

The **`currentPath()`** method returns the path to the directory where the current configure script file is located.

The current configure script is either the root `Configure.js` script, or
the one currently running from a `Project.addSubdirectory()` call.
