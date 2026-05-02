# Contributing

## Code style

This project uses [clang-format](https://clang.llvm.org/docs/ClangFormat.html) (LLVM-based style,
4-space indent, 100-column limit). The CI blocks any PR with unformatted code.

Format all source files locally with:

```bash
cmake --build build --target format
```

## Pre-commit hook (optional)

To catch formatting issues before committing, add this check to `.git/hooks/pre-commit`:

```sh
#!/bin/sh
clang-format --dry-run --Werror $(find src test -name '*.hpp' -o -name '*.cpp')
```

Make the hook executable:

```bash
chmod +x .git/hooks/pre-commit
```
