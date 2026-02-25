#include <cstddef>
#include <cstdint>
#include <type_traits>

#include <vix/ffi/vix_ffi.h>

static void check_basic_layout()
{
  static_assert(std::is_standard_layout_v<vix_str>, "vix_str must be standard layout");
  static_assert(std::is_trivially_copyable_v<vix_str>, "vix_str must be trivially copyable");
  static_assert(std::is_standard_layout_v<vix_buf>, "vix_buf must be standard layout");
  static_assert(std::is_trivially_copyable_v<vix_buf>, "vix_buf must be trivially copyable");
  static_assert(std::is_standard_layout_v<vix_mut_buf>, "vix_mut_buf must be standard layout");
  static_assert(std::is_trivially_copyable_v<vix_mut_buf>, "vix_mut_buf must be trivially copyable");
  static_assert(std::is_standard_layout_v<vix_status>, "vix_status must be standard layout");
  static_assert(std::is_trivially_copyable_v<vix_status>, "vix_status must be trivially copyable");
  static_assert(std::is_standard_layout_v<vix_allocator>, "vix_allocator must be standard layout");
  static_assert(std::is_trivially_copyable_v<vix_allocator>, "vix_allocator must be trivially copyable");
}

static void check_expected_field_sizes()
{
  static_assert(sizeof(char) == 1, "char must be 1 byte");
  static_assert(sizeof(uint8_t) == 1, "uint8_t must be 1 byte");
  static_assert(sizeof(int32_t) == 4, "int32_t must be 4 bytes");
  static_assert(sizeof(size_t) == sizeof(void *), "size_t should match pointer size on this platform");
}

static void check_struct_shapes()
{
  static_assert(sizeof(vix_str) == sizeof(void *) + sizeof(size_t), "vix_str must be {ptr,len}");
  static_assert(alignof(vix_str) == alignof(void *), "vix_str alignment should match pointer alignment");

  static_assert(sizeof(vix_buf) == sizeof(void *) + sizeof(size_t), "vix_buf must be {ptr,len}");
  static_assert(alignof(vix_buf) == alignof(void *), "vix_buf alignment should match pointer alignment");

  static_assert(sizeof(vix_mut_buf) == sizeof(void *) + sizeof(size_t), "vix_mut_buf must be {ptr,cap}");
  static_assert(alignof(vix_mut_buf) == alignof(void *), "vix_mut_buf alignment should match pointer alignment");

  // vix_status: int32 + padding + vix_str
  static_assert(alignof(vix_status) >= alignof(vix_str), "vix_status alignment must support vix_str");

  // vix_allocator: 2 function pointers + user pointer
  static_assert(sizeof(vix_allocator) == 3 * sizeof(void *), "vix_allocator must be 3 pointers");
  static_assert(alignof(vix_allocator) == alignof(void *), "vix_allocator alignment should match pointer alignment");
}

int main()
{
  check_basic_layout();
  check_expected_field_sizes();
  check_struct_shapes();
  return 0;
}
