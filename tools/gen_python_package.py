#!/usr/bin/env python3
import argparse
import os
import shutil
import sys
from pathlib import Path
from typing import Optional


def die(msg: str) -> None:
    print(f"error: {msg}", file=sys.stderr)
    sys.exit(1)


def ensure_dir(p: Path) -> None:
    p.mkdir(parents=True, exist_ok=True)


def copytree(src: Path, dst: Path) -> None:
    """Copy a directory tree without requiring dst to not exist."""
    if not src.exists():
        die(f"missing directory: {src}")
    ensure_dir(dst)
    for item in src.rglob("*"):
        rel = item.relative_to(src)
        out = dst / rel
        if item.is_dir():
            ensure_dir(out)
        else:
            ensure_dir(out.parent)
            shutil.copy2(item, out)


def write_text(p: Path, content: str) -> None:
    ensure_dir(p.parent)
    p.write_text(content, encoding="utf-8")


def default_version() -> str:
    # v1: allow version override via env, else a safe default
    return os.environ.get("VIX_PY_VERSION", "0.1.0")


def render_pyproject(package: str, dist_name: str, version: str, description: str) -> str:
    # setuptools backend for maximum compatibility
    return f"""[build-system]
requires = [\"setuptools>=64\", \"wheel\"]
build-backend = \"setuptools.build_meta\"

[project]
name = \"{dist_name}\"
version = \"{version}\"
description = \"{description}\"
readme = \"README.md\"
requires-python = \">=3.8\"
license = {{text = \"MIT\"}}
authors = [{{name = \"Vix.cpp contributors\"}}]
classifiers = [
  \"Programming Language :: Python :: 3\",
  \"License :: OSI Approved :: MIT License\",
  \"Operating System :: OS Independent\"
]

[tool.setuptools]
package-dir = {{\"\" = \"src\"}}

[tool.setuptools.packages.find]
where = [\"src\"]
include = [\"{package}*\"]
"""


def render_setup_cfg() -> str:
    # optional, but handy for some tooling
    return """[metadata]
license_files = LICENSE

[options]
include_package_data = True
"""


def render_readme(dist_name: str, package: str, lib_name: str) -> str:
    return f"""# {dist_name}

Python bindings for **{lib_name}** (Vix Registry FFI).

This package loads the native shared library at runtime using `ctypes`.

## Usage

```python
from {package} import api

# See api.py for available functions
```

## Native library resolution

The loader searches:

- `{package}` package directory (next to `_native.py`)
- paths listed in `VIX_LIB_PATH` (colon-separated, or semicolon on Windows)
- system dynamic linker paths
"""


def main() -> int:
    ap = argparse.ArgumentParser(
        description="Create a Python package skeleton for a Vix FFI binding."
    )
    ap.add_argument("--package", required=True, help="Python import package name, e.g. base64")
    ap.add_argument(
        "--dist",
        default="",
        help="Distribution name on PyPI, e.g. vix-base64 (default: vix-<package>)",
    )
    ap.add_argument(
        "--lib",
        default="",
        help="Native library base name, e.g. base64 (default: <package>)",
    )
    ap.add_argument("--description", default="Vix Registry FFI Python bindings.", help="Short description")
    ap.add_argument(
        "--version",
        default="",
        help="Version (default: env VIX_PY_VERSION or 0.1.0)",
    )
    ap.add_argument("--src", required=True, help="Directory containing generated python/<package>/ files")
    ap.add_argument("--out", required=True, help="Output directory for the package project")
    ap.add_argument("--copy-lib", default="", help="Optional path to native shared library to copy into the package")
    args = ap.parse_args()

    package = args.package.strip()
    if not package:
        die("package must not be empty")

    dist = args.dist.strip() or f"vix-{package}"
    lib_name = args.lib.strip() or package
    version = args.version.strip() or default_version()
    description = args.description.strip()

    src_root = Path(args.src).resolve()
    gen_pkg_dir = src_root / "python" / package
    if not gen_pkg_dir.exists():
        die(f"generated package dir not found: {gen_pkg_dir}")

    out_root = Path(args.out).resolve()
    ensure_dir(out_root)

    # Layout:
    # out/
    #   pyproject.toml
    #   README.md
    #   setup.cfg
    #   src/<package>/...
    #   (optional) native shared library copied into src/<package>/
    proj_pyproject = out_root / "pyproject.toml"
    proj_readme = out_root / "README.md"
    proj_setup_cfg = out_root / "setup.cfg"

    write_text(proj_pyproject, render_pyproject(package, dist, version, description))
    write_text(proj_setup_cfg, render_setup_cfg())
    write_text(proj_readme, render_readme(dist, package, lib_name))

    # Copy python sources into src/<package>
    dst_pkg_dir = out_root / "src" / package
    copytree(gen_pkg_dir, dst_pkg_dir)

    # Optionally copy native shared lib next to _native.py (simplest)
    if args.copy_lib:
        lib_path = Path(args.copy_lib).resolve()
        if not lib_path.exists():
            die(f"--copy-lib file not found: {lib_path}")
        shutil.copy2(lib_path, dst_pkg_dir / lib_path.name)

    print(f"created python package project: {out_root}")
    print(f"  dist   : {dist}")
    print(f"  import : {package}")
    print(f"  version: {version}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

