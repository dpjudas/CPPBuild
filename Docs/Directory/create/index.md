# Directory: create() static method

The **`create()`** static method of the `Directory` object
creates a directory if it does not exist.

## Syntax

```js
Directory.create(path)
```

### Parameters

- `path`

    A string with the path to the directory to be created.

### Return value

None.

## Description

The full path will be created. If any of the parent directories do not exist
they will also be created.

If the directory already exists no action will be taken.
