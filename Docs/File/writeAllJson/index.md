# File: writeAllJson() static method

The **`writeAllJson()`** static method of the `File` object
writes a javascript value to disk as JSON.

## Syntax

```js
File.writeAllJson(filename, value)
```

### Parameters

- `filename`

    A string with the path to the file.

- `value`

    Object, array, string, number, boolean, or null value to be written.

### Return value

None.

## Description

**`writeAllJson()`** writes a javascript value to disk as JSON. If the file already exists it
is overwritten with the new file.
