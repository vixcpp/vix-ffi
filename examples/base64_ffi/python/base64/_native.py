from __future__ import annotations

import ctypes
import os
import sys
from pathlib import Path
from typing import List, Optional


class vix_str(ctypes.Structure):
    _fields_ = [("ptr", ctypes.c_char_p), ("len", ctypes.c_size_t)]


class vix_status(ctypes.Structure):
    _fields_ = [("code", ctypes.c_int32), ("message", vix_str)]


def _decode_vix_str(s: vix_str) -> str:
    if not s.ptr or s.len == 0:
        return ""
    raw = ctypes.string_at(s.ptr, s.len)
    return raw.decode("utf-8", errors="replace")


def _default_lib_filename() -> str:
    name = "base64"
    if sys.platform.startswith("win"):
        return name + ".dll"
    if sys.platform == "darwin":
        return "lib" + name + ".dylib"
    return "lib" + name + ".so"


def _candidate_paths() -> List[Path]:
    out: List[Path] = []
    env = os.environ.get("VIX_LIB_PATH", "")
    if env:
        sep = ";" if sys.platform.startswith("win") else ":"
        for part in env.split(sep):
            part = part.strip()
            if part:
                out.append(Path(part))

    out.append(Path(__file__).resolve().parent)
    return out


def load_library() -> ctypes.CDLL:
    fname = _default_lib_filename()
    errors: List[str] = []
    for base in _candidate_paths():
        cand = base / fname
        if cand.exists():
            try:
                return ctypes.CDLL(str(cand))
            except Exception as e:
                errors.append(f"failed to load: {cand}: {e}")

    try:
        return ctypes.CDLL(fname)
    except Exception as e:
        errors.append(f"failed to load by name: {fname}: {e}")

    raise RuntimeError("could not load native library\n" + "\n".join(errors))


_lib: Optional[ctypes.CDLL] = None


def _get_lib() -> ctypes.CDLL:
    global _lib
    if _lib is None:
        _lib = load_library()
    return _lib


def _bind(name: str, restype, argtypes):
    lib = _get_lib()
    fn = getattr(lib, name)
    fn.restype = restype
    fn.argtypes = argtypes
    return fn


base64_ffi_version = _bind(
    "base64_ffi_version",
    vix_str,
    [ctypes.POINTER(vix_status)],
)

base64_encode = _bind(
    "base64_encode",
    ctypes.c_int,
    [
        ctypes.POINTER(ctypes.c_uint8),
        ctypes.c_size_t,
        ctypes.POINTER(ctypes.c_uint8),
        ctypes.c_size_t,
        ctypes.POINTER(ctypes.c_size_t),
        ctypes.POINTER(vix_status),
    ],
)

base64_decode = _bind(
    "base64_decode",
    ctypes.c_int,
    [
        ctypes.POINTER(ctypes.c_uint8),
        ctypes.c_size_t,
        ctypes.POINTER(ctypes.c_uint8),
        ctypes.c_size_t,
        ctypes.POINTER(ctypes.c_size_t),
        ctypes.POINTER(vix_status),
    ],
)
