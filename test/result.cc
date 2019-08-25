#include <result.hh>
#include <gtest/gtest.h>
#include <string>
#include <type_traits>
#include <stdexcept>

namespace results {
namespace {

static_assert(std::is_move_constructible<result<std::string>>::value);
static_assert(std::is_copy_constructible<result<std::string>>::value);
static_assert(std::is_move_assignable<result<std::string>>::value);
static_assert(std::is_copy_assignable<result<std::string>>::value);


TEST(result, make_ok) {
    auto r = make_ok<int>(2);
    EXPECT_TRUE(r.is_ok());
    EXPECT_FALSE(r.is_err());
}

TEST(result, make_err) {
    auto r = make_err<int>("bad things!");
    EXPECT_FALSE(r.is_ok());
    EXPECT_TRUE(r.is_err());
}


}
}
