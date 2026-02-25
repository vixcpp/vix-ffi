#include <array>
#include <cstring>
#include <string>
#include <vector>

#include <base64/base64_ffi.h>

static vix_str make_str_lit(const char *s)
{
  vix_str out;
  out.ptr = s;
  out.len = std::strlen(s);
  return out;
}

static void set_ok(vix_status *st)
{
  if (st)
    *st = vix_status_ok();
}

static void set_error(vix_status *st, int32_t code, const char *msg)
{
  if (!st)
    return;
  st->code = code;
  st->message = make_str_lit(msg);
}

/* Minimal base64 (standard alphabet, no newlines). For demo only. */
static const char *kAlphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static std::string b64_encode_bytes(const uint8_t *data, size_t len)
{
  std::string out;
  out.reserve(((len + 2) / 3) * 4);

  size_t i = 0;
  while (i + 3 <= len)
  {
    uint32_t v = (uint32_t(data[i]) << 16) | (uint32_t(data[i + 1]) << 8) | uint32_t(data[i + 2]);
    out.push_back(kAlphabet[(v >> 18) & 63]);
    out.push_back(kAlphabet[(v >> 12) & 63]);
    out.push_back(kAlphabet[(v >> 6) & 63]);
    out.push_back(kAlphabet[v & 63]);
    i += 3;
  }

  const size_t rem = len - i;
  if (rem == 1)
  {
    uint32_t v = (uint32_t(data[i]) << 16);
    out.push_back(kAlphabet[(v >> 18) & 63]);
    out.push_back(kAlphabet[(v >> 12) & 63]);
    out.push_back('=');
    out.push_back('=');
  }
  else if (rem == 2)
  {
    uint32_t v = (uint32_t(data[i]) << 16) | (uint32_t(data[i + 1]) << 8);
    out.push_back(kAlphabet[(v >> 18) & 63]);
    out.push_back(kAlphabet[(v >> 12) & 63]);
    out.push_back(kAlphabet[(v >> 6) & 63]);
    out.push_back('=');
  }

  return out;
}

static int b64_value(unsigned char c)
{
  if (c >= 'A' && c <= 'Z')
    return int(c - 'A');
  if (c >= 'a' && c <= 'z')
    return int(c - 'a') + 26;
  if (c >= '0' && c <= '9')
    return int(c - '0') + 52;
  if (c == '+')
    return 62;
  if (c == '/')
    return 63;
  return -1;
}

static bool b64_decode_string(const uint8_t *in_ptr, size_t in_len, std::vector<uint8_t> &out)
{
  // Accept only valid length multiple of 4
  if (in_len % 4 != 0)
    return false;

  out.clear();
  out.reserve((in_len / 4) * 3);

  size_t i = 0;
  while (i < in_len)
  {
    const unsigned char c0 = (unsigned char)in_ptr[i + 0];
    const unsigned char c1 = (unsigned char)in_ptr[i + 1];
    const unsigned char c2 = (unsigned char)in_ptr[i + 2];
    const unsigned char c3 = (unsigned char)in_ptr[i + 3];

    const int v0 = b64_value(c0);
    const int v1 = b64_value(c1);
    if (v0 < 0 || v1 < 0)
      return false;

    int v2 = (c2 == '=') ? -2 : b64_value(c2);
    int v3 = (c3 == '=') ? -2 : b64_value(c3);
    if (v2 == -1 || v3 == -1)
      return false;

    uint32_t triple = (uint32_t(v0) << 18) | (uint32_t(v1) << 12);
    if (v2 >= 0)
      triple |= (uint32_t(v2) << 6);
    if (v3 >= 0)
      triple |= uint32_t(v3);

    out.push_back(uint8_t((triple >> 16) & 0xFF));
    if (c2 != '=')
      out.push_back(uint8_t((triple >> 8) & 0xFF));
    if (c3 != '=')
      out.push_back(uint8_t(triple & 0xFF));

    // padding rules
    if (c2 == '=' && c3 != '=')
      return false;
    if (c2 == '=' || c3 == '=')
    {
      // must be last block
      if (i + 4 != in_len)
        return false;
    }

    i += 4;
  }

  return true;
}

vix_str VIX_FFI_CALL base64_ffi_version(vix_status *out_status)
{
  set_ok(out_status);
  return make_str_lit("example-1.0.0");
}

int VIX_FFI_CALL base64_encode(
    const uint8_t *in_ptr,
    size_t in_len,
    uint8_t *out_ptr,
    size_t out_cap,
    size_t *out_len,
    vix_status *out_status)
{
  set_ok(out_status);

  if (!out_len)
  {
    set_error(out_status, VIX_STATUS_INVALID_ARG, "out_len is null");
    return 1;
  }
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

  const std::string enc = b64_encode_bytes(in_ptr, in_len);

  if (out_cap < enc.size())
  {
    *out_len = enc.size();
    set_error(out_status, VIX_STATUS_BUFFER_TOO_SMALL, "output buffer too small");
    return 1;
  }

  if (!enc.empty())
    std::memcpy(out_ptr, enc.data(), enc.size());
  *out_len = enc.size();
  return 0;
}

int VIX_FFI_CALL base64_decode(
    const uint8_t *in_ptr,
    size_t in_len,
    uint8_t *out_ptr,
    size_t out_cap,
    size_t *out_len,
    vix_status *out_status)
{
  set_ok(out_status);

  if (!out_len)
  {
    set_error(out_status, VIX_STATUS_INVALID_ARG, "out_len is null");
    return 1;
  }
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

  std::vector<uint8_t> dec;
  if (!b64_decode_string(in_ptr, in_len, dec))
  {
    set_error(out_status, VIX_STATUS_INVALID_ARG, "invalid base64 input");
    return 1;
  }

  if (out_cap < dec.size())
  {
    *out_len = dec.size();
    set_error(out_status, VIX_STATUS_BUFFER_TOO_SMALL, "output buffer too small");
    return 1;
  }

  if (!dec.empty())
    std::memcpy(out_ptr, dec.data(), dec.size());
  *out_len = dec.size();
  return 0;
}
