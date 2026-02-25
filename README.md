# vix-ffi

Stable C ABI infrastructure and binding toolkit for Vix Registry
libraries.

This project defines a strict, portable C interface for C++ libraries
and provides tooling to generate language bindings, starting with Python
via `ctypes`.

The objective is to make Vix libraries usable across languages without
compromising determinism, performance, or long-term binary stability.

# What This Project Provides

## 1. Stable C ABI

A minimal and predictable C interface defined in:

    include/vix/ffi/

It includes:

-   Export and calling convention macros
-   Fixed layout structs
-   Structured error handling
-   Deterministic memory rules

The ABI is designed to be:

-   Portable across platforms
-   Friendly to foreign language interfaces
-   Safe for dynamic loading
-   Resistant to accidental breakage

## 2. CMake Integration

Located in:

    cmake/VixFFI.cmake

Provides helpers to:

-   Build shared libraries for FFI
-   Apply correct export macros
-   Enforce consistent compile flags
-   Install FFI targets cleanly

Libraries in the Vix Registry can adopt FFI support with minimal
configuration.

## 3. Binding Generators

Located in:

    tools/

Includes:

-   `gen_python_ctypes.py`\
    Generates Python bindings using `ctypes`.

-   `gen_python_package.py`\
    Creates a Python package project structure.

-   `abi_check.py`\
    Performs compile-time ABI validation.

Python is the first supported target language.\
The ABI design allows future bindings for:

-   Rust
-   Go
-   Node.js
-   Other C-compatible runtimes

# Repository Structure

    include/     Core ABI headers
    cmake/       Build helpers
    tools/       Binding generators and validation tools
    templates/   Reusable FFI layer templates
    examples/    End-to-end example (base64_ffi)
    tests/       Layout and ABI validation tests
    docs/        Specification and policy documents

# Example: base64 FFI

An end-to-end example is provided:

    examples/base64_ffi/

It demonstrates:

-   A shared library exposing a C ABI
-   Python bindings loading the shared library
-   Error handling through `vix_status`
-   Buffer management rules

Build example:

``` bash
cmake --preset dev-ninja
cmake --build build-ninja

cmake -S examples/base64_ffi -B build-base64-ffi -G Ninja
cmake --build build-base64-ffi
```

Run Python test:

``` bash
export PYTHONPATH="$PWD/examples/base64_ffi/python:$PYTHONPATH"
export VIX_LIB_PATH="$PWD/build-base64-ffi"

python3 -c "from base64 import api; print(api.version())"
```

# Design Goals

-   No C++ types in the ABI
-   No exception leakage across boundary
-   Caller-controlled memory for outputs
-   Stable struct layouts
-   Explicit ownership rules
-   Cross-platform compatibility

The ABI is intentionally conservative.

It prioritizes stability over syntactic convenience.

# Who Should Use This

-   Authors of Vix Registry libraries who want cross-language support
-   Developers building Python bindings for high-performance C++ code
-   Teams requiring stable binary interfaces
-   Projects targeting multi-language interoperability

# Documentation

-   `docs/SPEC.md` -- ABI specification
-   `docs/VERSIONING.md` -- Versioning and compatibility policy
-   `docs/PYTHON.md` -- Python binding workflow

# Contributing

When contributing:

-   Do not modify struct layouts casually
-   Do not change exported function signatures without review
-   Run ABI validation tools before submitting changes
-   Respect the versioning policy

ABI stability is a responsibility shared by all contributors.

# License

See the `LICENSE` file.

vix-ffi provides the foundation for reliable multi-language integration
within the Vix ecosystem.

