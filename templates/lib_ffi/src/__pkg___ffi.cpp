#include <cstring>
#include <string>

#include "__pkg__/__pkg___ffi.h"

static vix_str make_str_lit(const char *s)
{
  vix_str out;
  out.ptr = s;
  out.len = std::strlen(s);
  return out;
}

static void set_error(vix_status *st, int32_t code, const char *msg)
{
  if (!st)
    return;
  st->code = code;
  st->message = make_str_lit(msg);
}

vix_str VIX_FFI_CALL __pkg___ffi_version(vix_status *out_status)
{
  if (out_status)
    *out_status = vix_status_ok();

  // IMPORTANT:
  // This returns a string literal (static storage). Caller must not free it.
  return make_str_lit("1.0.0");
}

int VIX_FFI_CALL __pkg___ffi_process(
    const uint8_t *in_ptr,
    size_t in_len,
    uint8_t *out_ptr,
    size_t out_cap,
    size_t *out_len,
    vix_status *out_status)
{
  if (out_status)
    *out_status = vix_status_ok();

  if (!in_ptr && in_len != 0)
  {
    set_error(out_status, VIX_STATUS_INVALID_ARG, "in_ptr is null but in_len != 0");
    return 1;
  }

  if (!out_ptr && out_cap != 0)
  {
    set_error(out_status, VIX_STATUS_INVALID_ARG, "out_ptr is null but out_cap != 0");
    return 1;
  }

  if (!out_len)
  {
    set_error(out_status, VIX_STATUS_INVALID_ARG, "out_len is null");
    return 1;
  }

  try
  {
    // TODO: replace with real logic that calls your C++ library.
    // This skeleton just copies input -> output.
    if (out_cap < in_len)
    {
      *out_len = in_len;
      set_error(out_status, VIX_STATUS_BUFFER_TOO_SMALL, "output buffer too small");
      return 1;
    }

    if (in_len != 0)
      std::memcpy(out_ptr, in_ptr, in_len);

    *out_len = in_len;
    return 0;
  }
  catch (const std::exception &e)
  {
    set_error(out_status, VIX_STATUS_ERROR, e.what());
    return 1;
  }
  catch (...)
  {
    set_error(out_status, VIX_STATUS_ERROR, "unknown native exception");
    return 1;
  }
}
