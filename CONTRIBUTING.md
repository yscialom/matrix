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

## Commit messages

This project follows [Conventional Commits](https://www.conventionalcommits.org/). Each commit
message must have the form:

```
<type>[optional scope]: <description>
```

Recognised types:

| Type       | When to use                                    |
|------------|------------------------------------------------|
| `feat`     | New feature visible to users                   |
| `fix`      | Bug fix                                        |
| `perf`     | Performance improvement                        |
| `refactor` | Code restructuring without behaviour change    |
| `docs`     | Documentation only                             |
| `test`     | Tests only                                     |
| `chore`    | Tooling, build, CI, dependencies               |
| `ci`       | CI/CD pipeline changes                         |
| `build`    | Build system changes                           |
| `revert`   | Reverts a previous commit                      |

Breaking changes must be noted with a `!` after the type (e.g. `feat!: remove deprecated API`) or
with a `BREAKING CHANGE:` footer.

The CHANGELOG is generated automatically from these messages on each release via
[git-cliff](https://git-cliff.org/) (configuration in `cliff.toml`).

## Releasing

Releases are automated via `.github/workflows/release.yml`. To cut a release:

1. Update `VERSION_MAJOR`, `VERSION_MINOR`, and `VERSION_PATCH` in `CMakeLists.txt`.
2. Merge the version bump PR into `develop`.
3. Push a tag matching `vMAJOR.MINOR.PATCH`:

```bash
git tag v1.2.3
git push origin v1.2.3
```

The CI will verify that the tag matches the version declared in `CMakeLists.txt`, run the tests,
generate the CHANGELOG, and publish the GitHub Release automatically.
