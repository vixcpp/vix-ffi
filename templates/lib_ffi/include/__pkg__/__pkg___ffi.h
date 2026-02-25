#ifndef __PKG___FFI_H
#define __PKG___FFI_H

#include <vix/ffi/vix_ffi.h>

VIX_FFI_EXTERN_C
{

  VIX_FFI_API vix_str VIX_FFI_CALL __pkg___ffi_version(vix_status * out_status);

  /*
    Pattern: caller-allocated output buffer.

    Returns:
      0 on success
      non-zero on error (and out_status filled)
  */
  VIX_FFI_API int VIX_FFI_CALL __pkg___ffi_process(
      const uint8_t *in_ptr,
      size_t in_len,
      uint8_t *out_ptr,
      size_t out_cap,
      size_t *out_len,
      vix_status *out_status);

} /* extern "C" */

#endif
