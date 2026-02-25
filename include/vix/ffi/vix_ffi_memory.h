#ifndef VIX_FFI_MEMORY_H
#define VIX_FFI_MEMORY_H

#include <stddef.h>

#include <vix/ffi/vix_ffi_export.h>

#ifdef __cplusplus
extern "C"
{
#endif

  typedef void *(*vix_alloc_fn)(size_t size, void *user);
  typedef void (*vix_free_fn)(void *ptr, void *user);

  typedef struct vix_allocator
  {
    vix_alloc_fn alloc;
    vix_free_fn free;
    void *user;
  } vix_allocator;

  /*
    ABI v1 rule:
    - Prefer caller allocated buffers (vix_mut_buf) for outputs.
    - If a library needs dynamic allocations for returned memory, it must
      explicitly accept a vix_allocator from the caller and use it.
  */

#ifdef __cplusplus
}
#endif

#endif
