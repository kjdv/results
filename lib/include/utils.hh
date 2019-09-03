#pragma once

#include <type_traits>
#include <stdexcept>
#include <string>
#include <string_view>

namespace results {

class panicked : public std::exception
{
public:
  explicit panicked(std::string_view msg);

  const char* what() const noexcept;

private:
  std::string d_msg;
};

namespace internal {

[[noreturn]] void panic(std::string_view msg);

}

template <typename T>
struct return_wrapper
{
  using type = std::decay_t<T>;

  template <typename F, typename... Args>
  static auto call(F && f, Args&&... args) -> decltype(f(args...))
  {
    return f(std::forward<Args>(args)...);
  }
};

template <>
struct return_wrapper<void>
{
  using type = int;

  template <typename F, typename... Args>
  static type call(F && f, Args&&... args)
  {
    f(std::forward<Args>(args)...);
    return 0;
  }
};

template <typename T>
using return_wrapper_t = typename return_wrapper<T>::type;



} // namespace results
