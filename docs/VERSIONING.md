# Versioning Policy

This document defines the versioning and compatibility policy for the Vix FFI project.

The objective is to provide long-term stability for:

- C ABI consumers
- Language bindings (Python, Rust, Go, Node, etc.)
- Downstream packages in the Vix Registry
- Binary distributions

The C ABI is a public contract. It must be treated as stable infrastructure.

---

# 1. Versioning Model

Vix FFI follows Semantic Versioning (SemVer):

MAJOR.MINOR.PATCH

## MAJOR

Incremented when:

- A breaking change is introduced in the public C ABI
- Struct layouts are modified
- Function signatures are changed
- Calling conventions change
- Binary compatibility is broken

A MAJOR increment signals that existing compiled bindings may need to be rebuilt or adapted.

## MINOR

Incremented when:

- New functions are added in a backward-compatible way
- New status codes are introduced
- Optional fields are added in a non-breaking manner
- Internal improvements that do not break ABI

MINOR releases must preserve binary compatibility.

## PATCH

Incremented when:

- Bugs are fixed
- Documentation is updated
- Internal implementation changes occur without affecting ABI

PATCH releases must preserve binary compatibility.

---

# 2. ABI Compatibility Guarantees

The C ABI is considered stable across:

- All PATCH releases
- All MINOR releases within the same MAJOR version

Binary compatibility guarantees include:

- Struct size and alignment remain unchanged
- Existing function signatures remain unchanged
- Existing exported symbol names remain unchanged
- Calling conventions remain unchanged

---

# 3. What Constitutes a Breaking Change

The following changes require a MAJOR version increment:

- Reordering struct fields
- Changing struct field types
- Changing integer width (e.g., int32_t to int64_t)
- Removing an exported function
- Renaming an exported symbol
- Changing return type of an exported function
- Changing argument order or types
- Changing ownership rules for returned memory
- Changing calling convention macros

Even small ABI changes can break foreign language bindings and must be treated carefully.

---

# 4. Adding New Functions

Adding new exported functions is allowed in MINOR releases if:

- Existing symbols remain untouched
- Struct layouts remain unchanged
- No existing semantic behavior is altered

Bindings should tolerate the presence of additional symbols.

---

# 5. Struct Evolution Rules

Struct layout is part of the ABI.

Rules:

- Fields must not be reordered
- Field types must not change
- Padding assumptions must not change
- Alignment must remain consistent

If future extensibility is needed, it should be handled through:

- New functions
- Separate structs
- Opaque handles

Never extend existing structs in a way that alters size or layout without a MAJOR version increment.

---

# 6. Deprecation Policy

If a function needs to be deprecated:

- It must remain exported until the next MAJOR release
- Documentation must mark it as deprecated
- Replacement APIs should be provided before removal

Removal of deprecated APIs requires a MAJOR version increment.

---

# 7. Binary Distribution Considerations

When distributing compiled shared libraries:

- Linux: respect SONAME conventions
- macOS: maintain compatible dylib versioning
- Windows: avoid breaking exported symbols

SONAME or DLL compatibility must align with MAJOR version changes.

---

# 8. Binding Compatibility

Language bindings rely on:

- Stable symbol names
- Stable struct layout
- Stable error handling semantics

Any change that affects:

- Memory ownership
- Error reporting
- Buffer sizing rules

Must be evaluated as an ABI-level change.

---

# 9. Testing for Stability

The project includes:

- Compile-time layout checks
- ABI validation scripts
- Cross-language smoke tests

These must pass before any release.

---

# 10. Release Discipline

Before releasing a new version:

- Verify that no exported symbol has changed unintentionally
- Confirm struct sizes remain unchanged
- Confirm layout tests pass on all supported platforms
- Confirm bindings load and execute correctly

ABI stability is a core responsibility of this project.

---

# Summary

Versioning is not cosmetic.

It defines the trust contract between:

- C/C++ producers
- Language binding maintainers
- Application developers

The ABI must remain predictable, conservative, and stable across releases.
