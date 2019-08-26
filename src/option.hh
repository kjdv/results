#pragma once

#include <optional>
#include <string_view>
#include <type_traits>
#include "utils.hh"

namespace results {

template <typename T>
class option {
public:
    using value_type = T;

    template <typename... Args>
    static option<T> some(Args&&... args);

    static option<T> none();

    bool is_none() const;

    bool is_some() const;

    const T & unwrap() const;

    const T & unwrap_or(const T &other) const;

    template <typename F>
    T unwrap_or_else(F && f) const;

    const T & expect(std::string_view msg) const;

    const option<T> &and_(const option<T> &other) const;

    template <typename F>
    auto and_then(F && f) const -> decltype(f(unwrap()));

    template <typename F1, typename F2>
    auto match(F1 && on_some, F2 && on_none) const -> decltype(on_some(unwrap()));

    template <typename P>
    option<T> filter(P && predicate) const;

    const option<T> &or_(const option<T> &other) const;

    template <typename F>
    option<T> or_else(F && f) const;

    T &get_or_insert(T value);

    template <typename F>
    T &get_or_insert_with(F && f);

    template <typename F>
    auto map(F && f) const -> option<std::decay_t<decltype(f(unwrap()))>>;

    template <typename F, typename U>
    auto map_or(F && f, const U &def) const -> decltype(f(unwrap()));

    template <typename F1, typename F2>
    auto map_or_else(F1 && f, const F2 &def) const -> decltype(f(unwrap()));

    option<T> replace(T value);

    option<T> take();

    option<T> xor_(const option<T> &other) const;

private:
    template <typename... Args>
    option(Args&&... args);

    std::optional<T> d_value;
};

template <typename T>
option<std::decay_t<T>> make_none() {
    return option<std::decay_t<T>>::none();
}

template <typename T, typename... Args>
option<std::decay_t<T>> make_some(Args&&... args) {
    return option<std::decay_t<T>>::some(std::forward<Args>(args)...);
}

template <typename T>
option<T> flatten(const option<option<T>> &o) {
    return o.is_some() ? o.unwrap() : make_none<T>();
}

template<typename T>
template <typename... Args>
option<T> option<T>::some(Args&&... args) {
    return option<T>(std::in_place, std::forward<Args>(args)...);
}

template<typename T>
option<T> option<T>::none() {
    return option<T>(std::nullopt);
}

template<typename T>
template <typename... Args>
option<T>::option(Args&&... args)
    : d_value(std::forward<Args>(args)...)
{}

template<typename T>
bool option<T>::is_none() const {
    return !d_value.has_value();
}

template<typename T>
bool option<T>::is_some() const {
    return d_value.has_value();
}

template<typename T>
const T &option<T>::unwrap() const {
    return expect("unwrapping none");
}

template<typename T>
const T &option<T>::unwrap_or(const T &other) const {
    return is_some() ? *d_value : other;
}

template<typename T>
template<typename F>
T option<T>::unwrap_or_else(F && f) const {
    return is_some() ? *d_value : f();
}

template<typename T>
const T &option<T>::expect(std::string_view msg) const {
    if (!is_some()) {
        internal::panic(msg);
    }
    return *d_value;
}

template<typename T>
const option<T> &option<T>::and_(const option<T> &other) const {
    return is_none() ? *this : other;
}

template<typename T>
const option<T> &option<T>::or_(const option<T> &other) const {
    return is_some() ? *this : other;
}

template<typename T>
T &option<T>::get_or_insert(T value) {
    if (is_none()) {
        d_value = std::move(value);
    }
    return *d_value;
}


template<typename T>
template<typename F>
T &option<T>::get_or_insert_with(F &&f)
{
    static_assert(std::is_convertible<decltype(f()), T>::value, "the return type of f() must be convertible to T");
    if (is_none()) {
        d_value = f();
    }
    return *d_value;
}

template<typename T>
template <typename F>
option<T> option<T>::or_else(F && f) const {
    static_assert(std::is_convertible<option<T>, decltype (f())>::value, "the return type of f() must be convertible to T");
    return is_some() ? *this : f();
}

template<typename T>
template<typename F>
auto option<T>::and_then(F && f) const -> decltype(f(unwrap())) {
    using U = decltype(f(unwrap()));
    return match(
                [&](auto&& some) { return f(some); },
                []() { return make_none<typename U::value_type>(); }
    );
}

template <typename T>
template <typename F1, typename F2>
auto option<T>::match(F1 && on_some, F2 && on_none) const -> decltype(on_some(unwrap())) {
    static_assert(std::is_convertible<decltype(on_none()), decltype(on_some(unwrap()))>::value,
                  "return value of on_none() must be equal or convertible to the return value of on_some()");

    if (is_some()) {
        return on_some(*d_value);
    }
    return on_none();
}

template <typename T>
template <typename P>
option<T> option<T>::filter(P && predicate) const {
    if (is_none() || !predicate(*d_value)) {
        return make_none<T>();
    }
    return make_some<T>(*d_value);
}

template <typename T>
template <typename F>
auto option<T>::map(F && f) const -> option<std::decay_t<decltype(f(unwrap()))>> {
    using U = std::decay_t<decltype(f(unwrap()))>;
    return match(
             [&](auto&& some) { return make_some<U>(f(some)); },
             []() { return make_none<U>(); }
    );
}

template <typename T>
template <typename F, typename U>
auto option<T>::map_or(F && f, const U &def) const -> decltype(f(unwrap())) {
   return match(
             [&](auto&& some) { return f(some); },
             [&]() { return def; }
   );
}


template <typename T>
template <typename F1, typename F2>
auto option<T>::map_or_else(F1 && f, const F2 &def) const -> decltype(f(unwrap())) {
    return match(
              [&](auto&& some) { return f(some); },
              [&]() { return def(); }
    );
}

template <typename T>
option<T> option<T>::replace(T value) {
    option<T> other(std::move(value));
    d_value.swap(other.d_value);
    return other;
}

template <typename T>
option<T> option<T>::take() {
    option<T> other;
    d_value.swap(other.d_value);
    return other;
}

template<typename T>
option<T> option<T>::xor_(const option<T> &other) const {
    if (is_none() && !other.is_none()) {
        return other;
    } else if (is_some() && !other.is_some()) {
        return *this;
    }
    return make_none<T>();
}


}

