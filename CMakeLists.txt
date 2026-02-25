cmake_minimum_required(VERSION 3.20)
project(vix_ffi VERSION 0.1.0 LANGUAGES C CXX)

# ------------------------------------------------------------------------------
# Options
# ------------------------------------------------------------------------------
option(VIX_FFI_BUILD_TESTS "Build vix-ffi tests" ON)

# ------------------------------------------------------------------------------
# Library (header-only)
# ------------------------------------------------------------------------------
add_library(vix_ffi INTERFACE)
add_library(vix::ffi ALIAS vix_ffi)

target_include_directories(vix_ffi INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

# Compile flags (only for consumers that compile C/C++)
if (MSVC)
  target_compile_options(vix_ffi INTERFACE /W4 /permissive-)
else()
  target_compile_options(vix_ffi INTERFACE -Wall -Wextra -Wpedantic)
endif()

# ------------------------------------------------------------------------------
# Tests (optional)
# ------------------------------------------------------------------------------
if (VIX_FFI_BUILD_TESTS)
  enable_testing()

  add_executable(vix_ffi_test_abi_sizes tests/test_abi_sizes.cpp)
  target_link_libraries(vix_ffi_test_abi_sizes PRIVATE vix_ffi)
  add_test(NAME vix_ffi.abi_sizes COMMAND vix_ffi_test_abi_sizes)

  add_executable(vix_ffi_test_status tests/test_status.cpp)
  target_link_libraries(vix_ffi_test_status PRIVATE vix_ffi)
  add_test(NAME vix_ffi.status COMMAND vix_ffi_test_status)
endif()

# ------------------------------------------------------------------------------
# Install
# ------------------------------------------------------------------------------
include(GNUInstallDirs)

install(TARGETS vix_ffi
  EXPORT vix_ffiTargets
)

install(DIRECTORY include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT vix_ffiTargets
  NAMESPACE vix::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/vix-ffi
)
