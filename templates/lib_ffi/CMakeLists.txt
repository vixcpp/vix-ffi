cmake_minimum_required(VERSION 3.20)

# This template is meant to be included from a package CMakeLists.
# It builds a shared library exposing a stable C ABI for bindings.

# Expected variables (set by the parent project):
#   VIX_PKG_NAME            (e.g. base64)
#   VIX_PKG_VERSION         (e.g. 0.1.0)
#   VIX_FFI_OUTPUT_NAME     (e.g. base64)   # optional, defaults to VIX_PKG_NAME
#   VIX_FFI_SOURCES         (list of .cpp)
#   VIX_FFI_PUBLIC_INCLUDES (list of include dirs)
#   VIX_FFI_PUBLIC_LIBS     (list of libs to link publicly)  # optional
#   VIX_FFI_PRIVATE_LIBS    (list of libs to link privately) # optional

if (NOT DEFINED VIX_PKG_NAME)
  message(FATAL_ERROR "lib_ffi template: missing VIX_PKG_NAME")
endif()

set(_out_name "${VIX_PKG_NAME}")
if (DEFINED VIX_FFI_OUTPUT_NAME)
  set(_out_name "${VIX_FFI_OUTPUT_NAME}")
endif()

# vix-ffi helpers (repo providing vix/ffi headers + cmake/VixFFI.cmake)
# Parent project should make vix-ffi available (via vix registry or FetchContent).
if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/../../cmake/VixFFI.cmake")
  include("${CMAKE_CURRENT_LIST_DIR}/../../cmake/VixFFI.cmake")
elseif (EXISTS "${CMAKE_CURRENT_LIST_DIR}/../../../cmake/VixFFI.cmake")
  include("${CMAKE_CURRENT_LIST_DIR}/../../../cmake/VixFFI.cmake")
else()
  message(FATAL_ERROR "lib_ffi template: cannot find VixFFI.cmake (expected from vix-ffi)")
endif()

if (NOT DEFINED VIX_FFI_SOURCES)
  message(FATAL_ERROR "lib_ffi template: missing VIX_FFI_SOURCES")
endif()

vix_add_ffi_library(
  NAME ${VIX_PKG_NAME}_ffi
  OUTPUT_NAME "${_out_name}"
  SOURCES ${VIX_FFI_SOURCES}
  PUBLIC_INCLUDES ${VIX_FFI_PUBLIC_INCLUDES}
  PUBLIC_LIBS ${VIX_FFI_PUBLIC_LIBS}
  PRIVATE_LIBS ${VIX_FFI_PRIVATE_LIBS}
  VERSION ${VIX_PKG_VERSION}
  SOVERSION 1
)

# Install shared lib + headers for this ffi layer
vix_install_ffi_library(${VIX_PKG_NAME}_ffi)

# Install the ffi headers for consumers (optional)
# Parent can override, but template provides sane default.
install(DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/include/"
  DESTINATION include
)
