cmake_minimum_required(VERSION 3.20)
project(base64_ffi_example LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# vix-ffi include (repo root include/)
add_library(vix_ffi_headers INTERFACE)
target_include_directories(vix_ffi_headers INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../../include>
)

add_library(base64_ffi SHARED
  src/base64_ffi.cpp
)

target_include_directories(base64_ffi PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/include
)

target_link_libraries(base64_ffi PRIVATE vix_ffi_headers)

# Export macro for Windows
target_compile_definitions(base64_ffi PRIVATE VIX_FFI_BUILD_DLL=1)

set_target_properties(base64_ffi PROPERTIES
  OUTPUT_NAME base64
  POSITION_INDEPENDENT_CODE ON
)

# Optional: a tiny smoke test executable
add_executable(base64_ffi_smoke
  src/base64_ffi.cpp
)

target_include_directories(base64_ffi_smoke PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/include
)

target_link_libraries(base64_ffi_smoke PRIVATE vix_ffi_headers)
