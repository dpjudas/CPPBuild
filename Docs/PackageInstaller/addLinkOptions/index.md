# PackageInstaller: addComponent() method

**`PackageInstaller.addLinkOptions()`** adds linker options to targets that use the package.

## Syntax

```js
addLinkOptions(linkerOptions)
addLinkOptions(linkerOptions, opt)
```

### Parameters

- `linkerOptions`

    An array of strings. Each string is a linker option.

- `opt` - optional

    An object with additional parameters. See below.

### Additional parameters

- `configuration`

    Configuration name to apply the linker options to. Applies to all if not specified.

### Return value

None.
