# File: writeAllText() static method

The **`writeAllText()`** static method of the `File` object
writes a string to disk as text.

## Syntax

```js
File.writeAllText(filename, text)
```

### Parameters

- `filename`

    A string with the path to the file.

- `text`

    A string to be written.

### Return value

None.

## Description

**`writeAllText()`** writes a string to disk. The text is encoded as UTF-8. If the file already exists it
is overwritten with the new file.
