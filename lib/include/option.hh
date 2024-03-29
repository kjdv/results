#pragma once

#include "utils.hh"
#include <optional>
#include <string_view>
#include <type_traits>

namespace results {

template <typename T>
class option
{
private:
  // todo: move somewhere else, just to support decltype() calls
  T value()
  {
    return d_value.value();
  }
public:
  using value_type = T;

  // construction:
  template <typename... Args>
  static constexpr option<T> some(Args&&... args) noexcept;

  static constexpr option<T> none() noexcept;

  // info
  bool constexpr is_none() const noexcept;

  bool constexpr is_some() const noexcept;

  // raw access
  constexpr const T& expect(std::string_view msg) const;

  constexpr const T& unwrap() const;

  constexpr const T& unwrap_or(const T& other) const noexcept;

  template <typename F>
  T unwrap_or_else(F&& f) const;

  // boolean logic:
  constexpr const option<T>& and_(const option<T>& other) const noexcept;

  constexpr const option<T>& or_(const option<T>& other) const noexcept;

  template <typename F>
  option<T> or_else(F&& f) const;

  constexpr option<T> xor_(const option<T>& other) const noexcept;

  // get
  constexpr T& get_or_insert(T value) noexcept;

  template <typename F>
  T& get_or_insert_with(F&& f);

  constexpr option<T> replace(T value) noexcept;

  constexpr option<T> take() noexcept;

  // match
  template <typename F1, typename F2>
  auto match(F1&& on_some, F2&& on_none) const -> decltype(on_some(unwrap()));

  // chaining
  template <typename F>
  auto and_then(F&& f) const -> decltype(f(unwrap()));

  template <typename P>
  option<T> filter(P&& predicate) const;

  template <typename F>
  auto map(F&& f) const -> option<return_wrapper_t<decltype(f(unwrap()))>>;

  template <typename F, typename U>
  auto map_or(F&& f, const U& def) const -> decltype(f(unwrap()));

  template <typename F1, typename F2>
  auto map_or_else(F1&& f, const F2& def) const -> decltype(f(unwrap()));

  // misc
  constexpr value_type flatten() const noexcept;

  template <typename F>
  auto consume(F && f) -> option<return_wrapper_t<decltype(f(value()))>>;

private:
  template <typename... Args>
  constexpr option(Args&&... args) noexcept;

  std::optional<T> d_value;
};

template <typename T>
constexpr option<std::decay_t<T>> make_none() noexcept
{
  return option<std::decay_t<T>>::none();
}

template <typename T, typename... Args>
constexpr option<std::decay_t<T>> make_some(Args&&... args) noexcept
{
  return option<std::decay_t<T>>::some(std::forward<Args>(args)...);
}

template <typename T>
template <typename... Args>
constexpr option<T> option<T>::some(Args&&... args) noexcept
{
  return option<T>(std::in_place, std::forward<Args>(args)...);
}

template <typename T>
constexpr option<T> option<T>::none() noexcept
{
  return option<T>(std::nullopt);
}

template <typename T>
template <typename... Args>
constexpr option<T>::option(Args&&... args) noexcept
  : d_value(std::forward<Args>(args)...)
{
}

template <typename T>
constexpr bool option<T>::is_none() const noexcept
{
  return !d_value.has_value();
}

template <typename T>
constexpr bool option<T>::is_some() const noexcept
{
  return d_value.has_value();
}

template <typename T>
constexpr const T& option<T>::unwrap() const
{
  return expect("unwrapping none");
}

template <typename T>
constexpr const T& option<T>::unwrap_or(const T& other) const noexcept
{
  return is_some() ? *d_value : other;
}

template <typename T>
template <typename F>
T option<T>::unwrap_or_else(F&& f) const
{
  return is_some() ? *d_value : f();
}

template <typename T>
constexpr const T& option<T>::expect(std::string_view msg) const
{
  if(!is_some())
  {
    internal::panic(msg);
  }
  return *d_value;
}

template <typename T>
constexpr const option<T>& option<T>::and_(const option<T>& other) const noexcept
{
  return is_none() ? *this : other;
}

template <typename T>
constexpr const option<T>& option<T>::or_(const option<T>& other) const noexcept
{
  return is_some() ? *this : other;
}

template <typename T>
constexpr T& option<T>::get_or_insert(T value) noexcept
{
  if(is_none())
  {
    d_value = std::move(value);
  }
  return *d_value;
}

template <typename T>
template <typename F>
T& option<T>::get_or_insert_with(F&& f)
{
  static_assert(std::is_convertible<decltype(f()), T>::value, "the return type of f() must be convertible to T");
  if(is_none())
  {
    d_value = f();
  }
  return *d_value;
}

template <typename T>
template <typename F>
option<T> option<T>::or_else(F&& f) const
{
  static_assert(std::is_convertible<option<T>, decltype(f())>::value, "the return type of f() must be convertible to T");
  return is_some() ? *this : f();
}

template <typename T>
template <typename F>
auto option<T>::and_then(F&& f) const -> decltype(f(unwrap()))
{
  using U = decltype(f(unwrap()));
  return match(
      [&](auto&& some) { return f(some); },
      []() { return make_none<typename U::value_type>(); });
}

template <typename T>
template <typename F1, typename F2>
auto option<T>::match(F1&& on_some, F2&& on_none) const -> decltype(on_some(unwrap()))
{
  static_assert(std::is_convertible<decltype(on_none()), decltype(on_some(unwrap()))>::value,
                "return value of on_none() must be equal or convertible to the return value of on_some()");

  if(is_some())
  {
    return on_some(*d_value);
  }
  return on_none();
}

template <typename T>
template <typename P>
option<T> option<T>::filter(P&& predicate) const
{
  if(is_none() || !predicate(*d_value))
  {
    return make_none<T>();
  }
  return make_some<T>(*d_value);
}

template <typename T>
template <typename F>
auto option<T>::map(F&& f) const -> option<return_wrapper_t<decltype(f(unwrap()))>>
{
  using R = return_wrapper<decltype(f(unwrap()))>;
  using U = typename R::type;
  return match(
      [&](auto&& some) { return make_some<U>(R::call(f, some)); },
      []() { return make_none<U>(); });
}

template <typename T>
template <typename F, typename U>
auto option<T>::map_or(F&& f, const U& def) const -> decltype(f(unwrap()))
{
  return match(
      [&](auto&& some) { return f(some); },
      [&]() { return def; });
}

template <typename T>
template <typename F1, typename F2>
auto option<T>::map_or_else(F1&& f, const F2& def) const -> decltype(f(unwrap()))
{
  return match(
      [&](auto&& some) { return f(some); },
      [&]() { return def(); });
}

template <typename T>
constexpr option<T> option<T>::replace(T value) noexcept
{
  option<T> other(std::move(value));
  d_value.swap(other.d_value);
  return other;
}

template <typename T>
constexpr option<T> option<T>::take() noexcept
{
  option<T> other;
  d_value.swap(other.d_value);
  return other;
}

template <typename T>
constexpr option<T> option<T>::xor_(const option<T>& other) const noexcept
{
  if(is_none() && !other.is_none())
  {
    return other;
  }
  else if(is_some() && !other.is_some())
  {
    return *this;
  }
  return make_none<T>();
}

template <typename T>
template <typename F>
auto option<T>::consume(F&& f) -> option<return_wrapper_t<decltype(f(value()))>>
{
  using R = return_wrapper<decltype(f(value()))>;
  using U = typename R::type;

  if (is_some())
    return make_some<U>(R::call(f, std::move(*d_value)));
  return make_none<U>();
}


template <typename T>
constexpr typename option<T>::value_type option<T>::flatten() const noexcept
{
  static_assert(std::is_same_v<option<typename T::value_type>, T>, "contained type must be an option itself");
  return is_some() ? unwrap() : make_none<typename T::value_type>();
}

} // namespace results
