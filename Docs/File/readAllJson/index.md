# File: readAllJson() static method

The **`readAllJson()`** static method of the `File` object
reads an entire file from disk and parses it as JSON.

## Syntax

```js
File.readAllJson(filename)
```

### Parameters

- `filename`

    A string with the path to the file.

### Return value

The object, array, string, number, boolean, or null value corresponding to the JSON in the file.

## Description

**`readAllJson()`** reads an entire file from disk and parses it as JSON. The text encoding is UTF-8.
