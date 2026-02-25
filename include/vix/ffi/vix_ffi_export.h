#ifndef VIX_FFI_EXPORT_H
#define VIX_FFI_EXPORT_H

#if defined(_WIN32) || defined(__CYGWIN__)
#if defined(VIX_FFI_BUILD_DLL)
#define VIX_FFI_API __declspec(dllexport)
#else
#define VIX_FFI_API __declspec(dllimport)
#endif
#define VIX_FFI_CALL __cdecl
#else
#if defined(__GNUC__) || defined(__clang__)
#define VIX_FFI_API __attribute__((visibility("default")))
#else
#define VIX_FFI_API
#endif
#define VIX_FFI_CALL
#endif

#ifdef __cplusplus
#define VIX_FFI_EXTERN_C extern "C"
#else
#define VIX_FFI_EXTERN_C
#endif

#endif
