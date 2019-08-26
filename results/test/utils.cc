#include <gtest/gtest.h>
#include <results/utils.hh>

namespace results {
namespace internal {
namespace {

TEST(panic, throws_panicked)
{
  try
  {
    panic("booh!");
    FAIL() << "above should have thrown";
  }
  catch(const panicked& p)
  {
    EXPECT_STREQ("booh!", p.what());
  }
}

} // namespace
} // namespace internal
} // namespace results
