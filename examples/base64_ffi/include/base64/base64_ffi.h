#ifndef BASE64_FFI_H
#define BASE64_FFI_H

#include <vix/ffi/vix_ffi.h>

VIX_FFI_EXTERN_C
{

  VIX_FFI_API vix_str VIX_FFI_CALL base64_ffi_version(vix_status * out_status);

  VIX_FFI_API int VIX_FFI_CALL base64_encode(
      const uint8_t *in_ptr,
      size_t in_len,
      uint8_t *out_ptr,
      size_t out_cap,
      size_t *out_len,
      vix_status *out_status);

  VIX_FFI_API int VIX_FFI_CALL base64_decode(
      const uint8_t *in_ptr,
      size_t in_len,
      uint8_t *out_ptr,
      size_t out_cap,
      size_t *out_len,
      vix_status *out_status);

} /* extern "C" */

#endif
