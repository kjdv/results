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

TEST(result, expect) {
    EXPECT_THROW(make_err<int>("booh!").expect("something"), panicked);
    EXPECT_EQ(1, make_ok<int>(1).expect("blah"));
}

TEST(result, unwrap) {
    EXPECT_THROW(make_err<int>("booh").unwrap(), panicked);
    EXPECT_EQ(1, make_ok<int>(1).unwrap());
}

TEST(result, and_) {
    auto ok1 = make_ok<int>(1);
    auto ok2 = make_ok<int>(2);
    auto err = make_err<int>("e");

    EXPECT_EQ(2, ok1.and_(ok2).unwrap());
    EXPECT_TRUE(ok1.and_(err).is_err());
    EXPECT_TRUE(err.and_(ok1).is_err());
}

TEST(result, or_) {
    auto ok1 = make_ok<int>(1);
    auto ok2 = make_ok<int>(2);
    auto err = make_err<int>("e");

    EXPECT_EQ(1, ok1.or_(ok2).unwrap());
    EXPECT_EQ(1, ok1.or_(err).unwrap());
    EXPECT_EQ(2, err.or_(ok2).unwrap());
    EXPECT_TRUE(err.or_(err).is_err());
}

TEST(result, or_else) {
    auto ok = make_ok<int>(1);
    auto err = make_err<int>("e");
    auto f = [] { return make_ok<int>(2); };

    EXPECT_EQ(1, ok.or_else(f).unwrap());
    EXPECT_EQ(2, err.or_else(f).unwrap());
}



}
}
