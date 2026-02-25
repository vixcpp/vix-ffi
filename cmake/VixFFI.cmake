include_guard(GLOBAL)

include(GNUInstallDirs)

# ------------------------------------------------------------------------------
# vix_ffi_setup(<target>)
#
# Configure an existing target as a Vix FFI shared library with sane defaults.
# - Forces PIC where needed
# - Adds export macro define on Windows (VIX_FFI_BUILD_DLL)
# - Sets output name and version
# ------------------------------------------------------------------------------
function(vix_ffi_setup target)
  if (NOT TARGET ${target})
    message(FATAL_ERROR "vix_ffi_setup: target not found: ${target}")
  endif()

  set(options)
  set(oneValueArgs OUTPUT_NAME VERSION SOVERSION)
  set(multiValueArgs)
  cmake_parse_arguments(VIXFFI "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  # Ensure shared libs are built with PIC on platforms that need it
  set_target_properties(${target} PROPERTIES
    POSITION_INDEPENDENT_CODE ON
  )

  # Windows export define
  target_compile_definitions(${target} PRIVATE VIX_FFI_BUILD_DLL=1)

  # Default output name = target
  if (DEFINED VIXFFI_OUTPUT_NAME)
    set_target_properties(${target} PROPERTIES OUTPUT_NAME "${VIXFFI_OUTPUT_NAME}")
  endif()

  if (DEFINED VIXFFI_VERSION)
    set_target_properties(${target} PROPERTIES VERSION "${VIXFFI_VERSION}")
  endif()

  if (DEFINED VIXFFI_SOVERSION)
    set_target_properties(${target} PROPERTIES SOVERSION "${VIXFFI_SOVERSION}")
  endif()

  # Good warnings for FFI code
  if (MSVC)
    target_compile_options(${target} PRIVATE /W4 /permissive-)
  else()
    target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
  endif()
endfunction()

# ------------------------------------------------------------------------------
# vix_add_ffi_library(
#   NAME <name>
#   OUTPUT_NAME <output_name>           # optional
#   SOURCES <src1> <src2> ...
#   PUBLIC_INCLUDES <dir1> <dir2> ...   # optional
#   PUBLIC_LIBS <lib1> <lib2> ...       # optional
#   PRIVATE_LIBS <lib1> <lib2> ...      # optional
#   VERSION <x.y.z>                     # optional
#   SOVERSION <n>                       # optional
# )
#
# Creates a shared library intended to be loaded from other languages.
# It does NOT assume anything about the underlying C++ library; you link it in.
# ------------------------------------------------------------------------------
function(vix_add_ffi_library)
  set(options)
  set(oneValueArgs NAME OUTPUT_NAME VERSION SOVERSION)
  set(multiValueArgs SOURCES PUBLIC_INCLUDES PUBLIC_LIBS PRIVATE_LIBS)
  cmake_parse_arguments(VIXFFI "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  if (NOT VIXFFI_NAME)
    message(FATAL_ERROR "vix_add_ffi_library: missing NAME")
  endif()

  if (NOT VIXFFI_SOURCES)
    message(FATAL_ERROR "vix_add_ffi_library: missing SOURCES")
  endif()

  add_library(${VIXFFI_NAME} SHARED ${VIXFFI_SOURCES})

  if (VIXFFI_PUBLIC_INCLUDES)
    target_include_directories(${VIXFFI_NAME} PUBLIC ${VIXFFI_PUBLIC_INCLUDES})
  endif()

  if (VIXFFI_PUBLIC_LIBS)
    target_link_libraries(${VIXFFI_NAME} PUBLIC ${VIXFFI_PUBLIC_LIBS})
  endif()

  if (VIXFFI_PRIVATE_LIBS)
    target_link_libraries(${VIXFFI_NAME} PRIVATE ${VIXFFI_PRIVATE_LIBS})
  endif()

  # If vix-ffi headers are available as a target, use it.
  # This lets packages do: target_link_libraries(my_ffi PRIVATE vix::ffi)
  # but doesn't force it.
  if (TARGET vix::ffi)
    target_link_libraries(${VIXFFI_NAME} PRIVATE vix::ffi)
  endif()

  vix_ffi_setup(${VIXFFI_NAME}
    OUTPUT_NAME "${VIXFFI_OUTPUT_NAME}"
    VERSION "${VIXFFI_VERSION}"
    SOVERSION "${VIXFFI_SOVERSION}"
  )

  add_library(vix::${VIXFFI_NAME} ALIAS ${VIXFFI_NAME})
endfunction()

# ------------------------------------------------------------------------------
# vix_install_ffi_library(<target>)
#
# Installs shared library + headers (if any) using standard dirs.
# This is intentionally minimal; each lib can add extra install rules.
# ------------------------------------------------------------------------------
function(vix_install_ffi_library target)
  if (NOT TARGET ${target})
    message(FATAL_ERROR "vix_install_ffi_library: target not found: ${target}")
  endif()

  install(TARGETS ${target}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}   # Windows .dll
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}   # .so/.dylib
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}   # import libs on Windows
  )
endfunction()
