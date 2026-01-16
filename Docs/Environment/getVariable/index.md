# Environment: getVariable() static method

The **`getVariable()`** static method of the `Environment` object
returns the value of a system environment variable.

## Syntax

```js
Environment.getVariable(name)
```

### Parameters

- `name`

    A string with the name of environment variable.

### Return value

A string with the value of the environment variable.

## Description

Returns the value of the environment variable. If the variable does not exist an empty value is returned.