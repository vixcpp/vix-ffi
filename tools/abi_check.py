#!/usr/bin/env python3
import argparse
import os
import shlex
import subprocess
import sys
import tempfile
from pathlib import Path
from typing import List, Optional


def die(msg: str) -> None:
    print(f"error: {msg}", file=sys.stderr)
    sys.exit(1)


def run(cmd: List[str], cwd: Optional[Path] = None) -> int:
    print("+", " ".join(shlex.quote(x) for x in cmd))
    try:
        p = subprocess.run(cmd, cwd=str(cwd) if cwd else None)
        return int(p.returncode)
    except FileNotFoundError:
        die(f"compiler not found: {cmd[0]}")
    except Exception as e:
        die(f"failed to run command: {e}")
    return 1


C_TU = r"""
#include <stddef.h>
#include <stdint.h>

#include <vix/ffi/vix_ffi.h>

/* C mode check: ensure the header is C-friendly */
int main(void)
{
  /* sanity */
  (void)VIX_FFI_ABI_VERSION_MAJOR;
  (void)VIX_FFI_ABI_VERSION_MINOR;

  /* basic size checks */
  _Static_assert(sizeof(vix_str) == sizeof(void*) + sizeof(size_t), "vix_str shape");
  _Static_assert(sizeof(vix_buf) == sizeof(void*) + sizeof(size_t), "vix_buf shape");
  _Static_assert(sizeof(vix_mut_buf) == sizeof(void*) + sizeof(size_t), "vix_mut_buf shape");
  _Static_assert(sizeof(vix_allocator) == 3 * sizeof(void*), "vix_allocator shape");

  return 0;
}
"""

CPP_TU = r"""
#include <cstddef>
#include <cstdint>
#include <type_traits>

#include <vix/ffi/vix_ffi.h>

static void check_layout()
{
  static_assert(std::is_standard_layout_v<vix_str>, "vix_str must be standard layout");
  static_assert(std::is_trivially_copyable_v<vix_str>, "vix_str must be trivially copyable");
  static_assert(std::is_standard_layout_v<vix_status>, "vix_status must be standard layout");
  static_assert(std::is_trivially_copyable_v<vix_status>, "vix_status must be trivially copyable");
}

static void check_sizes()
{
  static_assert(sizeof(vix_str) == sizeof(void*) + sizeof(size_t), "vix_str shape");
  static_assert(sizeof(vix_buf) == sizeof(void*) + sizeof(size_t), "vix_buf shape");
  static_assert(sizeof(vix_mut_buf) == sizeof(void*) + sizeof(size_t), "vix_mut_buf shape");
  static_assert(sizeof(vix_allocator) == 3 * sizeof(void*), "vix_allocator shape");
}

int main()
{
  check_layout();
  check_sizes();
  return 0;
}
"""


def main() -> int:
    ap = argparse.ArgumentParser(description="Compile-time ABI sanity check for vix-ffi headers.")
    ap.add_argument("--root", default=".", help="Repo root (default: .)")
    ap.add_argument("--cc", default="", help="C compiler (default: cc)")
    ap.add_argument("--cxx", default="", help="C++ compiler (default: c++)")
    ap.add_argument("--mode", choices=["c", "cxx", "both"], default="both", help="Which compile mode to check")
    ap.add_argument("--std", default="c11", help="C standard (default: c11)")
    ap.add_argument("--cxx-std", default="c++17", help="C++ standard (default: c++17)")
    ap.add_argument("--extra", default="", help="Extra flags (quoted string)")
    args = ap.parse_args()

    root = Path(args.root).resolve()
    include_dir = root / "include"
    if not include_dir.exists():
        die(f"include/ not found at: {include_dir}")

    cc = args.cc.strip() or "cc"
    cxx = args.cxx.strip() or "c++"
    extra = shlex.split(args.extra) if args.extra else []

    with tempfile.TemporaryDirectory(prefix="vix_ffi_abi_") as td:
        td_path = Path(td)

        rc_total = 0

        if args.mode in ("c", "both"):
            c_file = td_path / "abi_check.c"
            c_file.write_text(C_TU, encoding="utf-8")
            out = td_path / "abi_check_c.out"
            cmd = [cc, f"-std={args.std}", "-I", str(include_dir), "-o", str(out), str(c_file)] + extra
            rc = run(cmd, cwd=root)
            if rc != 0:
                return rc
            rc_total |= rc

        if args.mode in ("cxx", "both"):
            cpp_file = td_path / "abi_check.cpp"
            cpp_file.write_text(CPP_TU, encoding="utf-8")
            out = td_path / "abi_check_cpp.out"
            cmd = [cxx, f"-std={args.cxx_std}", "-I", str(include_dir), "-o", str(out), str(cpp_file)] + extra
            rc = run(cmd, cwd=root)
            if rc != 0:
                return rc
            rc_total |= rc

    print("ok: vix-ffi ABI headers compile cleanly")
    return int(rc_total)


if __name__ == "__main__":
    raise SystemExit(main())
