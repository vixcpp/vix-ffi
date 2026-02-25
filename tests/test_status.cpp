#include <cstdint>

#include <vix/ffi/vix_ffi.h>

static int test_status_ok()
{
  const vix_status st = vix_status_ok();

  if (st.code != VIX_STATUS_OK)
    return 1;

  if (st.message.ptr != nullptr)
    return 2;

  if (st.message.len != 0)
    return 3;

  return 0;
}

int main()
{
  return test_status_ok();
}
