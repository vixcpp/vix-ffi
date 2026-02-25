#ifndef VIX_FFI_STATUS_H
#define VIX_FFI_STATUS_H

#include <stddef.h>
#include <stdint.h>

#include <vix/ffi/vix_ffi_types.h>

#ifdef __cplusplus
extern "C"
{
#endif

  enum vix_status_code
  {
    VIX_STATUS_OK = 0,
    VIX_STATUS_ERROR = 1,
    VIX_STATUS_INVALID_ARG = 2,
    VIX_STATUS_BUFFER_TOO_SMALL = 3,
    VIX_STATUS_NOT_SUPPORTED = 4
  };

  typedef struct vix_status
  {
    int32_t code;
    vix_str message;
  } vix_status;

  static inline vix_status vix_status_ok(void)
  {
    vix_status st;
    st.code = VIX_STATUS_OK;
    st.message.ptr = (const char *)0;
    st.message.len = 0;
    return st;
  }

#ifdef __cplusplus
}
#endif

#endif
