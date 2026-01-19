# Target: addCustomCommand() method

**`Target.addCustomCommand()`** adds a custom command to target.

## Syntax

```js
addCustomCommand(description)
```

### Parameters

- `description`

    An object with additional parameters. See below.

### Additional parameters

- `inputFile`

    Input file that the command depends upon. If this file is modified the command will run again.

- `commands`

    An array of strings. Each string is a system command to execute.

- `outputFiles`

    An array of strings. Each string is an output file produced by the command.

- `configuration`

    Configuration name to apply the custom command. Applies to all if not specified.

### Return value

None.
