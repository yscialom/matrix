# Migration Guide — v0.x → v1.0.0

This document covers all breaking changes introduced in `ysc::matrix` v1.0.0 and explains
how to update your code.

## Stability Promise (since v1.0.0)

Starting with v1.0.0, `ysc::matrix` follows [Semantic Versioning](https://semver.org/):

- **Public API** = everything in namespace `ysc` *except* `ysc::detail::`.
  Breaking changes to the public API require a **major version bump**.
- **`ysc::detail::`** is internal implementation. It may change in any release, including
  patch releases. Do **not** depend on it directly.

## Breaking Changes

### 1. CMake target renamed: `matrix` → `ysc::matrix`

**Affected users:** anyone consuming the library via CMake `FetchContent` or `find_package`.

**Before (v0.x):**
```cmake
target_link_libraries(myapp PRIVATE matrix)
```

**After (v1.0.0):**
```cmake
target_link_libraries(myapp PRIVATE ysc::matrix)
```

The old unnamespaced target `matrix` is no longer exported. The canonical name is now
`ysc::matrix`, consistent with CMake namespacing conventions.

### 2. Hash values changed

**Affected users:** anyone using `ysc::matrix` as a key in `std::unordered_set`,
`std::unordered_map`, or any hash-based container **and** persisting those hash values
across program runs (e.g. written to disk or sent over the network).

**What changed:** The `std::hash<ysc::matrix<T, Dims...>>` specialization was updated to
use a 64-bit hash-combine algorithm for better distribution. Hash values computed by
v1.0.0 differ from those computed by v0.x.

**In-memory use is unaffected** — hash containers rehash automatically on construction.
Only serialized hash values are invalidated.

**Migration:** Recompute and replace any persisted hash values after upgrading.
