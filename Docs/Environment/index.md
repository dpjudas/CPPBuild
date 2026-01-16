# Environment

The **`Environment`** interface provides information about the environment CPPBuild is executing in.

## Static methods

- [`getVariable()`](getVariable/index.md)

    Return the value of a system environment variable.

- [`isMSVC()`](isMSVC/index.md)

    Returns true if the Visual C++ compiler is used.

- [`isClang()`](isClang/index.md)

    Returns true if the clang C++ compiler is used.

- [`isGCC()`](isGCC/index.md)

    Returns true if the GCC C++ compiler is used.

- [`isWindows()`](isWindows/index.md)

    Returns true if the platform is Windows.

- [`isUnix()`](isUnix/index.md)

    Returns true if the platform is posix compatible (Linux, BSD, Apple, etc).

- [`isApple()`](isApple/index.md)

    Returns true if the platform is Apple (macOS, iOS, etc).

- [`isX86()`](isX86/index.md)

    Returns true if the architecture is 32-bit x86.

- [`isX64()`](isX64/index.md)

    Returns true if the architecture is 64-bit x86.

- [`isARM32()`](isARM32/index.md)

    Returns true if the architecture is 32-bit ARM.

- [`isARM64()`](isARM64/index.md)

    Returns true if the architecture is 64-bit ARM.
