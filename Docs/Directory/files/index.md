# Directory: files() static method

The **`files()`** static method of the `Directory` object
scans a directory and returns the files contained within it.

## Syntax

```js
Directory.files(searchPattern)
Directory.files(searchPattern, options)
```

### Parameters

- `searchPattern`

    A string with the search pattern to use for scanning for files.

    The search pattern consists of a path followed by an asterisk (`*`) as
    the wildcard character. For example `Directory("/foobar/*")` will
    return all files in the `/foobar` directory.

- `options` - optional

    An object with additional parameters. See below.

### Additional parameters

- `recursive`

    If set to true, recursively scan for matches.

- `includeSearchPath`

    If set to true, includes the searchPattern path in the results. Defaults to true.

### Return value

A list of strings. Each string is a filename relative to the path from the
search pattern.

## Description

The **`files()`** method is used to scan a directory for files. The search
pattern allows for filtering. `/foobar/*.json` will list all json files from
the `foobar` directory.
