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

`release.sh` at the root of the repository semi-automates the release process.
It requires `git` and `gh` (GitHub CLI, authenticated).

### Step 1 — Prepare the release

```bash
./release.sh prepare M.m.p
```

This:
- creates branch `release/vM.m.p` from the current branch
- bumps `VERSION_MAJOR/MINOR/PATCH` in `CMakeLists.txt`
- commits (title `Release vM.m.p`, body = epic dashboard from `user-stories.md`)
- opens a PR `release/vM.m.p` → `master`

### Step 2 — Merge the PR (manual)

Wait for CI to be green, then merge the PR into `master` (merge commit).

### Step 3 — Finalize the release

```bash
./release.sh finalize M.m.p
```

This:
- creates and pushes the signed tag `vM.m.p` on `master`
- opens a PR `master` → `develop` to bring the tag into `develop`'s history

### Step 4 — Merge the back-merge PR (manual)

Merge the `master` → `develop` PR (merge commit).

The CI triggered by the tag handles the rest: version verification, build,
CHANGELOG generation, and GitHub Release publication.
