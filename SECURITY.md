# Security Policy

## Scope

`ysc::matrix` is a header-only C++20 library with no network I/O, no file I/O, and no runtime
dependencies. Its attack surface is limited to:

- Memory safety: `at()` performs bounds checking and throws `std::out_of_range` on violation;
  `operator()` is unchecked by design (performance path) and exhibits UB on out-of-bounds access.
- Template instantiation: malformed template parameters may trigger compile-time errors, not
  runtime vulnerabilities.

## Reporting a Vulnerability

Please **do not** open a public GitHub issue for security vulnerabilities.

Report them via [GitHub Security Advisories](../../security/advisories/new) so they can be
triaged privately before public disclosure.

We aim to:
- Acknowledge the report within **72 hours**
- Publish a fix within **14 days** for confirmed vulnerabilities

## Supported Versions

Only the latest release on the `develop` branch receives security fixes.
