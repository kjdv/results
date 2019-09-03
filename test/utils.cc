#include <gtest/gtest.h>
#include <string>
#include "utils.hh"

namespace results {
namespace internal {
namespace {

template <typename F>
auto use_return_wrapper(F && f, int arg1, int arg2) -> return_wrapper_t<decltype(f(arg1, arg2))>
{
  using R = return_wrapper<decltype(f(arg1, arg2))>;
  return R::call(f, arg1, arg2);
}

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

TEST(return_wrapper, non_void)
{
  auto f = [](int i, int j) { return std::to_string(i) + std::to_string(j); };
  EXPECT_EQ("42", use_return_wrapper(f, 4, 2));
}

TEST(return_wrapper, void)
{
  int k = 0;
  auto f = [&](int i, int j) { k = i + j; };

  use_return_wrapper(f, 4, 2);
  EXPECT_EQ(6, k);
}


} // namespace
} // namespace internal
} // namespace results
