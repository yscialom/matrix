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

`.github/github-release.sh` at the root of the repository semi-automates the release process.
It requires `git`, `gh` (GitHub CLI, authenticated), and `git-cliff`.

### Step 1 — Prepare the release

```bash
./.github/github-release.sh prepare M.m.p
```

This:
- creates branch `release/vM.m.p` from the current branch
- bumps `VERSION_MAJOR/MINOR/PATCH` in `CMakeLists.txt`
- commits (title `Release vM.m.p`, body = epic dashboard from `doc/specs/README.md`)
- pushes the branch and opens a PR `release/vM.m.p` → `master`

### Step 2 — Wait for CI (manual)

Wait for CI to be green on the PR. **Do not merge yet.**

### Step 3 — Finalize the release

From the `release/vM.m.p` branch:

```bash
./.github/github-release.sh finalize M.m.p
```

This:
- generates `CHANGELOG.md` with `git-cliff` and commits it on the release branch
- creates and pushes the signed tag `vM.m.p` on the release branch
- the CI triggered by the tag handles: version verification, build, and GitHub Release publication

### Step 4 — Merge the release PR (manual)

Wait for the tag-triggered CI to be green, then merge the PR `release/vM.m.p` → `master`
(merge commit).

### Step 5 — Back-merge into develop (manual)

```bash
gh pr create --base develop --head master \
  --title "chore: back-merge vM.m.p into develop" \
  --body "Back-merge after release vM.m.p."
```

Merge this PR (merge commit) to bring the release into `develop`'s history.
