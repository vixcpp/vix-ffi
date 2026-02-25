#ifndef VIX_FFI_TYPES_H
#define VIX_FFI_TYPES_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

  typedef struct vix_str
  {
    const char *ptr;
    size_t len;
  } vix_str;

  typedef struct vix_buf
  {
    const uint8_t *ptr;
    size_t len;
  } vix_buf;

  typedef struct vix_mut_buf
  {
    uint8_t *ptr;
    size_t cap;
  } vix_mut_buf;

#ifdef __cplusplus
}
#endif

#endif
