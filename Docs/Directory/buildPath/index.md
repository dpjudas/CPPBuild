# Directory: buildPath() static method

The **`buildPath()`** static method of the `Directory` object
returns the path to the build directory.

## Syntax

```js
Directory.buildPath()
Directory.buildPath(relativePath)
```

### Parameters

- `relativePath` - optional

    A path relative to the build path that it is combined with.
    
    This is short form for `FilePath.combine(Directory.buildPath(), relativePath)`.

### Return value

A string with a path.

## Description

The **`buildPath()`** method returns the path to the build output directory
as specified when CPPBuild configure was run.
