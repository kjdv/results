#pragma once

#include "utils.hh"
#include <string>
#include <string_view>
#include <variant>
#include <stdexcept>

namespace results {

struct error
{
  std::string msg;
  error(std::string_view m = "") noexcept;
};

template <typename T, typename E = error>
class result
{
public:
  using value_type = T;
  using error_type = E;

  // contstruct
  template <typename... Args>
  constexpr static result<T, E> ok(Args&&... args) noexcept;

  template <typename... Args>
  constexpr static result<T, E> err(Args&&... args) noexcept;

  // info
  constexpr bool is_ok() const noexcept;

  constexpr bool is_err() const noexcept;

  // raw access
  constexpr const T& expect(std::string_view msg) const;

  constexpr const E& expect_err(std::string_view msg) const;

  constexpr const T& unwrap() const;

  constexpr const E& unwrap_err() const;

  constexpr const T& unwrap_or(const T& other) noexcept;

  template <typename F>
  T unwrap_or_else(F&& f) const;

  // boolean logic
  constexpr const result<T, E>& and_(const result<T, E>& other) const noexcept;

  constexpr const result<T, E>& or_(const result<T, E>& other) const noexcept;

  template <typename F>
  result<T, E> or_else(F&& f) const;

  // match
  template <typename F1, typename F2>
  auto match(F1&& on_ok, F2&& on_err) const -> decltype(on_ok(unwrap()));

  // chaining
  template <typename F>
  auto and_then(F&& f) const -> decltype(f(unwrap()));

  template <typename F>
  auto map(F&& f) const -> result<return_wrapper_t<decltype(f(unwrap()))>, error_type>;

  template <typename F>
  auto map_err(F&& f) const -> result<value_type, std::decay_t<decltype(f(unwrap_err()))>>;

  template <typename F1, typename F2>
  auto map_or_else(F1&& f, const F2& def) const -> decltype(f(unwrap()));

private:
  template <typename... Args>
  constexpr result(Args&&... args);

  constexpr const T& get_ok() const noexcept;

  constexpr const E& get_err() const noexcept;

  enum {
    OK  = 0,
    ERR = 1,
  };

  std::variant<T, E> d_value;
};

template <typename T, typename E = error, typename... Args>
constexpr result<T, E> make_ok(Args&&... args) noexcept
{
  return result<T, E>::ok(std::forward<Args>(args)...);
}

template <typename T, typename E = error, typename... Args>
constexpr result<T, E> make_err(Args&&... args) noexcept
{
  return result<T, E>::err(std::forward<Args>(args)...);
}

template <typename F, typename E = error>
auto make_from_throwable(F && f) noexcept -> result<std::decay_t<decltype(f())>, E>
{
  using R = result<std::decay_t<decltype(f())>, E>;
  try
  {
    return R::ok(f());
  }
  catch(const std::exception &e)
  {
    return R::err(e.what());
  }
  catch(...)
  {
    return R::err("non-std exception");
  }
}


template <typename T, typename E>
template <typename... Args>
constexpr result<T, E> result<T, E>::ok(Args&&... args) noexcept
{
  return result<T, E>(std::in_place_index<OK>, std::forward<Args>(args)...);
}

template <typename T, typename E>
template <typename... Args>
constexpr result<T, E> result<T, E>::err(Args&&... args) noexcept
{
  return result<T, E>(std::in_place_index<ERR>, std::forward<Args>(args)...);
}

template <typename T, typename E>
template <typename... Args>
constexpr result<T, E>::result(Args&&... args)
  : d_value(std::forward<Args>(args)...)
{
}

template <typename T, typename E>
constexpr bool result<T, E>::is_ok() const noexcept
{
  return d_value.index() == OK;
}

template <typename T, typename E>
constexpr bool result<T, E>::is_err() const noexcept
{
  return d_value.index() == ERR;
}

template <typename T, typename E>
constexpr const T& result<T, E>::expect(std::string_view msg) const
{
  if(!is_ok())
  {
    internal::panic(msg);
  }
  return get_ok();
}

template <typename T, typename E>
constexpr const T& result<T, E>::unwrap() const
{
  return expect("unwrapping err");
}

template <typename T, typename E>
constexpr const T& result<T, E>::get_ok() const noexcept
{
  return std::get<OK>(d_value);
}

template <typename T, typename E>
constexpr const E& result<T, E>::get_err() const noexcept
{
  return std::get<ERR>(d_value);
}

template <typename T, typename E>
constexpr const result<T, E>& result<T, E>::and_(const result<T, E>& other) const noexcept
{
  return is_ok() ? other : *this;
}

template <typename T, typename E>
constexpr const result<T, E>& result<T, E>::or_(const result<T, E>& other) const noexcept
{
  return is_err() ? other : *this;
}

template <typename T, typename E>
template <typename F>
result<T, E> result<T, E>::or_else(F&& f) const
{
  return is_ok() ? *this : f();
}

template <typename T, typename E>
constexpr const E& result<T, E>::expect_err(std::string_view msg) const
{
  if(!is_err())
  {
    internal::panic(msg);
  }
  return get_err();
}

template <typename T, typename E>
constexpr const E& result<T, E>::unwrap_err() const
{
  return expect_err("unwrapping ok");
}

template <typename T, typename E>
constexpr const T& result<T, E>::unwrap_or(const T& other) noexcept
{
  return is_ok() ? get_ok() : other;
}

template <typename T, typename E>
template <typename F>
T result<T, E>::unwrap_or_else(F&& f) const
{
  return is_ok() ? get_ok() : f();
}

template <typename T, typename E>
template <typename F>
auto result<T, E>::and_then(F&& f) const -> decltype(f(unwrap()))
{
  using U = decltype(f(unwrap()));
  return match(
      [&](auto&& ok) { return f(ok); },
      [](auto&& err) { return make_err<typename U::value_type, typename U::error_type>(err); });
}

template <typename T, typename E>
template <typename F1, typename F2>
auto result<T, E>::match(F1&& on_ok, F2&& on_err) const -> decltype(on_ok(unwrap()))
{
  static_assert(std::is_convertible<decltype(on_err(get_err())), decltype(on_ok(get_ok()))>::value,
                "return value of on_err() must be equal or convertible to the return value of on_ok()");
  return is_ok() ? on_ok(get_ok()) : on_err(get_err());
}

template <typename T, typename E>
template <typename F>
auto result<T, E>::map(F&& f) const -> result<return_wrapper_t<decltype(f(unwrap()))>, error_type>
{
  using R = return_wrapper<decltype(f(unwrap()))>;
  using U = typename R::type;
  return match(
      [&](auto&& ok) { return make_ok<U, E>(R::call(f, ok)); },
      [](auto&& err) { return make_err<U, E>(err); });
}

template <typename T, typename E>
template <typename F>
auto result<T, E>::map_err(F&& f) const -> result<value_type, std::decay_t<decltype(f(unwrap_err()))>>
{
  using U = decltype(f(unwrap_err()));
  return match(
      [](auto&& ok) { return make_ok<T, U>(ok); },
      [&](auto&& err) { return make_err<T, U>(f(err)); });
}

template <typename T, typename E>
template <typename F1, typename F2>
auto result<T, E>::map_or_else(F1&& f, const F2& def) const -> decltype(f(unwrap()))
{
  return match(f, def);
}

} // namespace results
