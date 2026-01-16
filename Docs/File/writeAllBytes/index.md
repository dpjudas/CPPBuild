# File: writeAllBytes() static method

The **`writeAllBytes()`** static method of the `File` object
writes a byte array to disk.

## Syntax

```js
File.writeAllBytes(filename, data)
```

### Parameters

- `filename`

    A string with the path to the file.

- `data`

    A byte array of data to be written.

### Return value

None.

## Description

**`writeAllBytes()`** writes a byte array to disk. If the file already exists it
is overwritten with the new file.
