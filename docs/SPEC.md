# Vix FFI Specification (ABI v1.0)

This document defines the **stable C ABI contract** for Vix Registry
libraries.

The goal is:

-   Deterministic cross-language interoperability
-   No C++ types across the boundary
-   No exceptions across the boundary
-   Stable memory ownership rules
-   Long-term ABI compatibility

This spec applies to **ABI version 1.x**.

# 1. Design Principles

1.  C ABI only\
    All exported symbols must use `extern "C"`.

2.  No C++ types in the ABI\
    No `std::string`, `std::vector`, exceptions, templates, or
    references.

3.  No exception leakage\
    Exceptions must be caught and converted to `vix_status`.

4.  Deterministic memory\
    Memory ownership must be explicit and documented.

5.  Forward compatibility\
    ABI-breaking changes require a new major ABI version.

# 2. ABI Versioning

Defined in:

``` c
#define VIX_FFI_ABI_VERSION_MAJOR 1
#define VIX_FFI_ABI_VERSION_MINOR 0
```

## Rules

-   Minor version increments for backward-compatible additions.
-   Major version increments for ABI-breaking changes.
-   Bindings should check the major version if needed.

# 3. Core Types

Defined in `include/vix/ffi/`.

## 3.1 vix_str

``` c
typedef struct vix_str
{
  const char *ptr;
  size_t len;
} vix_str;
```

### Rules

-   `ptr` may be NULL if `len == 0`
-   String is not required to be null-terminated
-   Encoding must be UTF-8 if textual

## 3.2 vix_buf

``` c
typedef struct vix_buf
{
  const uint8_t *ptr;
  size_t len;
} vix_buf;
```

Binary data buffer.

## 3.3 vix_mut_buf

``` c
typedef struct vix_mut_buf
{
  uint8_t *ptr;
  size_t cap;
} vix_mut_buf;
```

Caller-allocated output buffer.

## 3.4 vix_status

``` c
typedef struct vix_status
{
  int32_t code;
  vix_str message;
} vix_status;
```

### Status codes

-   `VIX_STATUS_OK`
-   `VIX_STATUS_ERROR`
-   `VIX_STATUS_INVALID_ARG`
-   `VIX_STATUS_BUFFER_TOO_SMALL`
-   `VIX_STATUS_NOT_SUPPORTED`

### Rules

-   `code == 0` means success
-   `message` may reference static storage
-   Caller must not free message memory unless explicitly documented

# 4. Function Conventions

## 4.1 Export Macros

All exported functions must use:

-   `VIX_FFI_EXTERN_C`
-   `VIX_FFI_API`
-   `VIX_FFI_CALL`

Example:

``` c
VIX_FFI_API int VIX_FFI_CALL pkg_op(...);
```

## 4.2 Return Pattern (Recommended)

Preferred pattern:

``` c
int pkg_op(
  const uint8_t *in_ptr,
  size_t in_len,
  uint8_t *out_ptr,
  size_t out_cap,
  size_t *out_len,
  vix_status *out_status
);
```

### Rules

-   Return 0 on success
-   Return non-zero on error
-   Always set `out_status` if provided
-   Always set `*out_len` on success

On buffer too small:

-   set `*out_len` to required size
-   set status to `VIX_STATUS_BUFFER_TOO_SMALL`
-   return non-zero

## 4.3 Version Function

Each FFI module must provide:

``` c
vix_str pkg_ffi_version(vix_status *out_status);
```

### Rules

-   Return static string literal
-   Do not allocate dynamically
-   Never require freeing

# 5. Memory Rules

ABI v1 strongly prefers:

-   Caller-allocated output buffers

### Why

-   No cross-runtime free mismatch
-   No ownership ambiguity
-   Safe for Python ctypes, Rust FFI, Go cgo

If dynamic allocation is required:

-   Accept a `vix_allocator`
-   Allocate using provided allocator
-   Document ownership clearly

### Never

-   Return memory that must be freed by `free()` unless explicitly
    documented
-   Throw exceptions across boundary

# 6. Error Handling

### Rules

-   All internal C++ exceptions must be caught
-   Convert to:

``` c
set_error(out_status, VIX_STATUS_ERROR, e.what());
```

Unknown exceptions must map to generic error.

Example:

``` c
catch (...)
{
  set_error(out_status, VIX_STATUS_ERROR, "unknown native exception");
  return 1;
}
```

# 7. Symbol Naming

All symbols must be prefixed by package name.

Example:

-   `base64_encode`
-   `base64_decode`
-   `base64_ffi_version`

### Never export generic names like:

-   `encode`
-   `decode`
-   `process`

# 8. Alignment and Layout Guarantees

All structs must:

-   Be standard layout
-   Be trivially copyable
-   Contain only fixed-width integer and pointer types

Tested via:

-   `tests/test_abi_sizes.cpp`
-   `tools/abi_check.py`

# 9. What Is NOT Allowed in ABI v1

-   C++ references
-   Templates
-   Overloaded functions
-   `std::string` in signature
-   `std::vector` in signature
-   Exceptions across boundary
-   Implicit memory ownership

# 10. Future Evolution

Possible future extensions:

-   Async ABI pattern
-   Allocator-based return objects
-   Streaming interfaces
-   Structured error objects
-   Rust-style result structs

ABI v1 remains minimal by design.

# Philosophy

The ABI must be:

-   Boring
-   Predictable
-   Portable
-   Deterministic
-   Stable

Bindings should be simple enough that:

-   Python ctypes works
-   Rust bindgen works
-   Go cgo works
-   Node FFI works

Without per-language hacks.

