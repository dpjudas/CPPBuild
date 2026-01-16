# Directory: folders() static method

The **`folders()`** static method of the `Directory` object
scans a directory and returns the subdirectories contained within it.

## Syntax

```js
Directory.folders(searchPattern)
```

### Parameters

- `searchPattern`

    A string with the search pattern to use for scanning for files.

    The search pattern consists of a path followed by an asterisk (`*`) as
    the wildcard character. For example `Directory("/foobar/*")` will
    return all subdirectories in the `/foobar` directory.

### Return value

A list of strings. Each string is a directory filename relative to the
path from the search pattern.

## Description

The **`folders()`** method is used to scan a directory for subdirectories.
The search pattern allows for filtering. `/foobar/Foo*` will return all
directores beginning with `Foo` in the `foobar` directory.
