from __future__ import annotations

import ctypes

from . import _native


def _raise_status(st: _native.vix_status) -> None:
    if int(st.code) == 0:
        return
    msg = _native._decode_vix_str(st.message)
    if not msg:
        msg = f"native error code={int(st.code)}"
    raise RuntimeError(msg)


def version() -> str:
    st = _native.vix_status()
    s = _native.base64_ffi_version(ctypes.byref(st))
    _raise_status(st)
    return _native._decode_vix_str(s)


def encode(data: bytes, *, out_cap: int = 8192) -> str:
    if not isinstance(data, (bytes, bytearray, memoryview)):
        raise TypeError("data must be bytes-like")

    b = bytes(data)
    out = (ctypes.c_uint8 * int(out_cap))()
    out_len = ctypes.c_size_t(0)
    st = _native.vix_status()

    rc = _native.base64_encode(
        ctypes.cast(ctypes.c_char_p(b), ctypes.POINTER(ctypes.c_uint8)),
        ctypes.c_size_t(len(b)),
        ctypes.cast(out, ctypes.POINTER(ctypes.c_uint8)),
        ctypes.c_size_t(int(out_cap)),
        ctypes.byref(out_len),
        ctypes.byref(st),
    )

    if int(rc) != 0:
        _raise_status(st)

    raw = bytes(out[: int(out_len.value)])
    return raw.decode("utf-8", errors="strict")


def decode(text: str, *, out_cap: int = 8192) -> bytes:
    if not isinstance(text, str):
        raise TypeError("text must be str")

    b = text.encode("utf-8")
    out = (ctypes.c_uint8 * int(out_cap))()
    out_len = ctypes.c_size_t(0)
    st = _native.vix_status()

    rc = _native.base64_decode(
        ctypes.cast(ctypes.c_char_p(b), ctypes.POINTER(ctypes.c_uint8)),
        ctypes.c_size_t(len(b)),
        ctypes.cast(out, ctypes.POINTER(ctypes.c_uint8)),
        ctypes.c_size_t(int(out_cap)),
        ctypes.byref(out_len),
        ctypes.byref(st),
    )

    if int(rc) != 0:
        _raise_status(st)

    return bytes(out[: int(out_len.value)])
