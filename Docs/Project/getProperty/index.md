# Project: getProperty() static method

The **`getProperty()`** static method of the `Project` object
returns the value of a variable stored using `cppbuild set`.

## Syntax

```js
Project.getProperty(name)
```

### Parameters

- `name`

    A string with the name of property variable.

### Return value

A string with the value of the property variable.

## Description

Returns the value of the property variable. If the variable does not exist `undefined` is returned.