#include <option.hh>
#include <gtest/gtest.h>
#include <string>
#include <type_traits>
#include <stdexcept>

namespace results {
namespace {

static_assert(std::is_move_constructible<option<std::string>>::value);
static_assert(std::is_copy_constructible<option<std::string>>::value);
static_assert(std::is_move_assignable<option<std::string>>::value);
static_assert(std::is_copy_assignable<option<std::string>>::value);

TEST(option, is_none) {
  EXPECT_TRUE(make_none<int>().is_none());
  EXPECT_FALSE(make_some<int>(1).is_none());
}

TEST(option, is_some) {
  EXPECT_FALSE(make_none<int>().is_some());
  EXPECT_TRUE(make_some<int>(1).is_some());
}

TEST(option, unwrap) {
    EXPECT_THROW(make_none<int>().unwrap(), std::runtime_error);
    EXPECT_EQ(2, make_some<int>(2).unwrap());
}

TEST(option, expect) {
    EXPECT_THROW(make_none<int>().expect("booh"), std::runtime_error);
    EXPECT_EQ(2, make_some<int>(2).expect("booh"));
}

TEST(option, and_then) {
    auto to_string = [](int i) { return std::to_string(i); };
    EXPECT_EQ("42", make_some<int>(42).and_then(to_string).unwrap());

    EXPECT_TRUE(make_none<int>().and_then(to_string).is_none());
}

TEST(option, match) {
    EXPECT_EQ(2, make_some<int>(2).match([](int i) { return i; }, []{ return -1; }));
    EXPECT_EQ(-1, make_none<int>().match([](int i) { return i; }, []{ return -1; }));
}

}
}
