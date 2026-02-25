# Python bindings (ctypes) for Vix FFI

This repository defines a stable C ABI for Vix Registry libraries and
provides a minimal Python binding workflow using `ctypes`.

The core idea is simple:

-   Each library exposes a small **C ABI** (a shared library:
    `.so/.dylib/.dll`)
-   Python loads it at runtime using `ctypes`
-   Errors and outputs follow a strict, deterministic contract (Vix FFI
    v1)

## Requirements

-   Python 3.8+
-   A C/C++ toolchain (to build the shared library)
-   CMake 3.20+

## How the Python loader finds the native library

The generated `_native.py` loader searches:

1)  Paths listed in `VIX_LIB_PATH`

    -   Linux/macOS: colon-separated
    -   Windows: semicolon-separated

2)  The Python package directory itself (next to `_native.py`)

3)  System dynamic linker search (fallback)

## Recommended FFI function shape (v1)

For best portability, return outputs into caller-allocated buffers:

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

-   Return 0 on success, non-zero on error.
-   If `out_cap` is too small:
    -   set `*out_len` to the required size
    -   set `out_status.code = VIX_STATUS_BUFFER_TOO_SMALL`
    -   return non-zero
-   Never throw exceptions across the ABI boundary.

## Quick start (example: base64_ffi)

From the repo root:

``` bash
cmake --preset dev-ninja
cmake --build build-ninja

cmake -S examples/base64_ffi -B build-base64-ffi -G Ninja
cmake --build build-base64-ffi
```

Run Python:

``` bash
export PYTHONPATH="$PWD/examples/base64_ffi/python:$PYTHONPATH"
export VIX_LIB_PATH="$PWD/build-base64-ffi"

python3 -c "from base64 import api; print(api.version()); s=api.encode(b'hello'); print(s); print(api.decode(s))"
```

### Expected output

-   a version string
-   a base64 string
-   the original decoded bytes

## Using the generators (recommended)

This repo includes tools to generate Python bindings from a small
manifest JSON.

### 1) Generate ctypes wrapper files

``` bash
python3 tools/gen_python_ctypes.py   --spec path/to/bindings.json   --out  path/to/output_dir
```

This produces:

-   `python/<package>/_native.py`
-   `python/<package>/api.py`
-   `python/<package>/__init__.py`

### 2) Create a Python package project (optional)

``` bash
python3 tools/gen_python_package.py   --package <package>   --src     path/to/output_dir   --out     dist/vix-<package>
```

This creates a small `pyproject.toml` project you can build or install
later.

## Notes

-   `ctypes` is the most portable first step. Later you can add faster
    options (CFFI, CPython extension) without changing the C ABI.
-   Keep the ABI stable.

