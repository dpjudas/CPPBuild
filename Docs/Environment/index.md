# Environment

The **`Environment`** interface provides information about the environment CPPBuild is executing in.

## Static methods

- [`Environment.getVariable()`](getVariable/index.md)

    Return the value of a system environment variable.

- [`Environment.isMSVC()`](isMSVC/index.md)

    Returns true if the Visual C++ compiler is used.

- [`Environment.isClang()`](isClang/index.md)

    Returns true if the clang C++ compiler is used.

- [`Environment.isGCC()`](isGCC/index.md)

    Returns true if the GCC C++ compiler is used.

- [`Environment.isWindows()`](isWindows/index.md)

    Returns true if the platform is Windows.

- [`Environment.isUnix()`](isUnix/index.md)

    Returns true if the platform is posix compatible (Linux, BSD, Apple, etc).

- [`Environment.isApple()`](isApple/index.md)

    Returns true if the platform is Apple (macOS, iOS, etc).

- [`Environment.isX86()`](isX86/index.md)

    Returns true if the architecture is 32-bit x86.

- [`Environment.isX64()`](isX64/index.md)

    Returns true if the architecture is 64-bit x86.

- [`Environment.isARM32()`](isARM32/index.md)

    Returns true if the architecture is 32-bit ARM.

- [`Environment.isARM64()`](isARM64/index.md)

    Returns true if the architecture is 64-bit ARM.
