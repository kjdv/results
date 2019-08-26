#include <gtest/gtest.h>
#include <results/result.hh>
#include <stdexcept>
#include <string>
#include <type_traits>

namespace results {
namespace {

static_assert(std::is_move_constructible<result<std::string>>::value);
static_assert(std::is_copy_constructible<result<std::string>>::value);
static_assert(std::is_move_assignable<result<std::string>>::value);
static_assert(std::is_copy_assignable<result<std::string>>::value);

TEST(result, make_ok)
{
  auto r = make_ok<int>(2);
  EXPECT_TRUE(r.is_ok());
  EXPECT_FALSE(r.is_err());
}

TEST(result, make_err)
{
  auto r = make_err<int>("bad things!");
  EXPECT_FALSE(r.is_ok());
  EXPECT_TRUE(r.is_err());
}

TEST(result, expect)
{
  EXPECT_THROW(make_err<int>("booh!").expect("something"), panicked);
  EXPECT_EQ(1, make_ok<int>(1).expect("blah"));
}

TEST(result, expect_err)
{
  EXPECT_THROW(make_ok<int>(1).expect_err("something"), panicked);
  EXPECT_EQ("something", make_err<int>("something").expect_err("blah").msg);
}

TEST(result, unwrap)
{
  EXPECT_THROW(make_err<int>("booh").unwrap(), panicked);
  EXPECT_EQ(1, make_ok<int>(1).unwrap());
}

TEST(result, unwrap_err)
{
  EXPECT_THROW(make_ok<int>(1).unwrap_err(), panicked);
  EXPECT_EQ("something", make_err<int>("something").unwrap_err().msg);
}

TEST(result, unwrap_or)
{
  auto ok  = make_ok<int>(1);
  auto err = make_err<int>("e");

  EXPECT_EQ(1, ok.unwrap_or(2));
  EXPECT_EQ(2, err.unwrap_or(2));
}

TEST(result, unwrap_or_else)
{
  auto ok  = make_ok<int>(1);
  auto err = make_err<int>("e");
  auto f   = [] { return 2; };

  EXPECT_EQ(1, ok.unwrap_or_else(f));
  EXPECT_EQ(2, err.unwrap_or_else(f));
}

TEST(result, and_)
{
  auto ok1 = make_ok<int>(1);
  auto ok2 = make_ok<int>(2);
  auto err = make_err<int>("e");

  EXPECT_EQ(2, ok1.and_(ok2).unwrap());
  EXPECT_TRUE(ok1.and_(err).is_err());
  EXPECT_TRUE(err.and_(ok1).is_err());
}

TEST(result, or_)
{
  auto ok1 = make_ok<int>(1);
  auto ok2 = make_ok<int>(2);
  auto err = make_err<int>("e");

  EXPECT_EQ(1, ok1.or_(ok2).unwrap());
  EXPECT_EQ(1, ok1.or_(err).unwrap());
  EXPECT_EQ(2, err.or_(ok2).unwrap());
  EXPECT_TRUE(err.or_(err).is_err());
}

TEST(result, or_else)
{
  auto ok  = make_ok<int>(1);
  auto err = make_err<int>("e");
  auto f   = [] { return make_ok<int>(2); };

  EXPECT_EQ(1, ok.or_else(f).unwrap());
  EXPECT_EQ(2, err.or_else(f).unwrap());
}

TEST(result, and_then)
{
  auto f   = [](int i) { return make_ok<std::string>(std::to_string(i)); };
  auto ok  = make_ok<int>(42);
  auto err = make_err<int>();

  EXPECT_EQ("42", ok.and_then(f).unwrap());
  EXPECT_TRUE(err.and_then(f).is_err());
}

TEST(result, match)
{
  auto on_ok  = [](int i) { return i * 2; };
  auto on_err = [](const error&) { return 42; };

  auto ok  = make_ok<int>(2);
  auto err = make_err<int>();

  EXPECT_EQ(4, ok.match(on_ok, on_err));
  EXPECT_EQ(42, err.match(on_ok, on_err));
}

TEST(result, map)
{
  auto f   = [](int i) { return std::to_string(i); };
  auto ok  = make_ok<int>(42);
  auto err = make_err<int>();

  EXPECT_EQ("42", ok.map(f).unwrap());
  EXPECT_TRUE(err.map(f).is_err());
}

TEST(result, map_err)
{
  auto f   = [](const error& e) { return e.msg; };
  auto ok  = make_ok<int>(1);
  auto err = make_err<int>("booh!");

  EXPECT_EQ(1, ok.map_err(f).unwrap());
  EXPECT_EQ("booh!", err.map_err(f).unwrap_err());
}

TEST(result, map_or_else)
{
  auto f   = [](int i) { return i * 2; };
  auto g   = [](const error&) { return 42; };
  auto ok  = make_ok<int>(2);
  auto err = make_err<int>();

  EXPECT_EQ(4, ok.map_or_else(f, g));
  EXPECT_EQ(42, err.map_or_else(f, g));
}

TEST(result, same_ok_and_err_type)
{
  // edge case: what if someone has the same ok and err types
  auto ok  = make_ok<std::string, std::string>("ok");
  auto err = make_err<std::string, std::string>("not ok");

  EXPECT_TRUE(ok.is_ok());
  EXPECT_FALSE(err.is_ok());

  auto on_ok  = [](auto&&) { return 1; };
  auto on_err = [](auto&&) { return 2; };

  EXPECT_EQ(1, ok.match(on_ok, on_err));
  EXPECT_EQ(2, err.match(on_ok, on_err));
}

} // namespace
} // namespace results
