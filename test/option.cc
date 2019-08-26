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
    EXPECT_THROW(make_none<int>().unwrap(), panicked);
    EXPECT_EQ(2, make_some<int>(2).unwrap());
}

TEST(option, unwrap_or) {
    EXPECT_EQ(2, make_some<int>(2).unwrap_or(3));
    EXPECT_EQ(3, make_none<int>().unwrap_or(3));
}

TEST(option, unwrap_or_else) {
    auto f = []{ return 3; };
    EXPECT_EQ(2, make_some<int>(2).unwrap_or_else(f));
    EXPECT_EQ(3, make_none<int>().unwrap_or_else(f));
}


TEST(option, expect) {
    EXPECT_THROW(make_none<int>().expect("booh"), panicked);
    EXPECT_EQ(2, make_some<int>(2).expect("booh"));
}

TEST(option, and_then) {
    auto to_string = [](int i) { return make_some<std::string>(std::to_string(i)); };
    EXPECT_EQ("42", make_some<int>(42).and_then(to_string).unwrap());

    EXPECT_TRUE(make_none<int>().and_then(to_string).is_none());
}

TEST(option, match) {
    EXPECT_EQ(2, make_some<int>(2).match([](int i) { return i; }, []{ return -1; }));
    EXPECT_EQ(-1, make_none<int>().match([](int i) { return i; }, []{ return -1; }));
}

TEST(option, filter) {
    auto pred = [](int i) { return i == 3; };

    EXPECT_EQ(3, make_some<int>(3).filter(pred).unwrap());
    EXPECT_TRUE(make_some<int>(4).filter(pred).is_none());
    EXPECT_TRUE(make_none<int>().filter(pred).is_none());
}

TEST(option, or_else) {
    EXPECT_EQ(2, make_some<int>(2).or_(make_some<int>(3)).unwrap());
    EXPECT_EQ(2, make_none<int>().or_(make_some<int>(2)).unwrap());

    EXPECT_EQ(2, make_some<int>(2).or_else([]{ return make_some<int>(3); }).unwrap());
    EXPECT_EQ(2, make_none<int>().or_else([]{ return make_some<int>(2); }).unwrap());
}

TEST(option, and_) {
    auto some = make_some<int>(0);
    auto none = make_none<int>();

    EXPECT_TRUE(none.and_(some).is_none());
    EXPECT_TRUE(some.and_(none).is_none());
    EXPECT_TRUE(some.and_(make_some<int>(1)).is_some());
}

TEST(option, flatten) {
    auto some = make_some<option<int>>(make_some<int>(3));
    auto none = make_none<option<int>>();

    EXPECT_EQ(3, some.flatten().unwrap());
    EXPECT_TRUE(none.flatten().is_none());
}

TEST(option, get_or_insert) {
    auto some = make_some<int>(0);
    auto none = make_none<int>();

    EXPECT_EQ(0, some.get_or_insert(2));
    EXPECT_EQ(2, none.get_or_insert(2));

    none = make_none<int>();

    auto f = []{ return 2; };
    EXPECT_EQ(0, some.get_or_insert_with(f));
    EXPECT_EQ(2, none.get_or_insert_with(f));
}

TEST(option, map) {
    auto to_string = [](int i) { return std::to_string(i); };
    EXPECT_EQ("42", make_some<int>(42).map(to_string).unwrap());

    EXPECT_TRUE(make_none<int>().map(to_string).is_none());
}

TEST(option, map_or) {
    auto f = [](int i) { return i * 2; };
    EXPECT_EQ(2, make_none<int>().map_or(f, 2));
    EXPECT_EQ(6, make_some<int>(3).map_or(f, 2));
}

TEST(option, map_or_else) {
    auto f = [](int i) { return i * 2 ;};
    auto g = []{ return 42; };
    EXPECT_EQ(42, make_none<int>().map_or_else(f, g));
    EXPECT_EQ(6, make_some<int>(3).map_or_else(f, g));
}

TEST(option, replace) {
    auto some = make_some<int>(2);
    auto other = some.replace(5);

    EXPECT_EQ(2, other.unwrap());
    EXPECT_EQ(5, some.unwrap());
}

TEST(option, take) {
    auto some = make_some<int>(2);
    auto other = some.take();

    EXPECT_EQ(2, other.unwrap());
    EXPECT_TRUE(some.is_none());
}

TEST(option, xor_) {
    auto a = make_some<int>(1);
    auto b = make_some<int>(2);
    auto n = make_none<int>();

    EXPECT_TRUE(a.xor_(b).is_none());
    EXPECT_TRUE(n.xor_(n).is_none());
    EXPECT_EQ(1, a.xor_(n).unwrap());
    EXPECT_EQ(1, n.xor_(a).unwrap());
}

}
}
