#pragma once

#include <variant>
#include <string>
#include <string_view>
#include "utils.hh"

namespace results {

struct error {
    std::string msg;

    error()
    {}

    error(std::string_view m)
        : msg(m)
    {}
};

template <typename T, typename E = error>
class result {
public:
    using value_type = T;
    using error_type = E;

    explicit result(T ok);
    explicit result(E err);

    bool is_ok() const;

    bool is_err() const;

    const T &expect(std::string_view msg) const;

    const E &expect_err(std::string_view msg) const;

    const T &unwrap() const;

    const E &unwrap_err() const;

    const T &unwrap_or(const T &other);

    template <typename F>
    T unwrap_or_else(F && f) const;

    const result<T, E> &and_(const result<T, E> &other) const;

    const result<T, E> &or_(const result<T, E> &other) const;

    template <typename F>
    result<T, E> or_else(F && f) const;

    template <typename F>
    auto and_then(F && f) const -> decltype(f(unwrap()));

    template <typename F1, typename F2>
    auto match(F1 && on_ok, F2 && on_err) const -> decltype(on_ok(unwrap()));

    template <typename F>
    auto map(F && f) const -> result<std::decay_t<decltype(f(unwrap()))>, error_type>;

    template <typename F>
    auto map_err(F && f) const -> result<value_type, std::decay_t<decltype(f(unwrap_err()))>>;

    template <typename F1, typename F2>
    auto map_or_else(F1 && f, const F2 &def) const -> decltype(f(unwrap()));

private:
    const T & get_ok() const;

    const E & get_err() const;

    enum {
        OK = 0,
        ERR = 1,
    };

    std::variant<T, E> d_value;
};


template <typename T, typename E = error, typename... Args>
result<T, E> make_ok(Args&&... args) {
    return result<T, E>(T(std::forward<Args>(args)...));
}

template <typename T, typename E = error, typename... Args>
result<T, E> make_err(Args&&... args) {
    return result<T, E>(E(std::forward<Args>(args)...));
}


template<typename T, typename E>
result<T, E>::result(T ok)
    : d_value(std::move(ok))
{}

template<typename T, typename E>
result<T, E>::result(E err)
    : d_value(std::move(err))
{}

template<typename T, typename E>
bool result<T, E>::is_ok() const {
    return d_value.index() == OK;
}

template<typename T, typename E>
bool result<T, E>::is_err() const {
    return d_value.index() == ERR;
}

template<typename T, typename E>
const T &result<T, E>::expect(std::string_view msg) const {
    if (!is_ok()) {
        internal::panic(msg);
    }
    return get_ok();
}

template<typename T, typename E>
const T &result<T, E>::unwrap() const {
    return expect("unwrapping err");
}

template<typename T, typename E>
const T &result<T, E>::get_ok() const {
    return std::get<OK>(d_value);
}

template<typename T, typename E>
const E &result<T, E>::get_err() const {
    return std::get<ERR>(d_value);
}

template <typename T, typename E>
const result<T, E> &result<T, E>::and_(const result<T, E> &other) const {
    return is_ok() ? other : *this;
}

template <typename T, typename E>
const result<T, E> &result<T, E>::or_(const result<T, E> &other) const {
    return is_err() ? other : *this;
}

template <typename T, typename E>
template <typename F>
result<T, E> result<T, E>::or_else(F && f) const {
    return is_ok() ? *this : f();
}

template <typename T, typename E>
const E &result<T, E>::expect_err(std::string_view msg) const {
    if (!is_err()) {
        internal::panic(msg);
    }
    return get_err();
}

template <typename T, typename E>
const E &result<T, E>::unwrap_err() const {
    return expect_err("unwrapping ok");
}

template <typename T, typename E>
const T &result<T, E>::unwrap_or(const T &other) {
    return is_ok() ? get_ok() : other;
}

template <typename T, typename E>
template <typename F>
T result<T, E>::unwrap_or_else(F && f) const {
    return is_ok() ? get_ok() : f();
}

template <typename T, typename E>
template <typename F>
auto result<T, E>::and_then(F && f) const -> decltype(f(unwrap())) {
    using U = decltype(f(unwrap()));
    return match(
                [&](auto&& ok) { return f(ok); },
                [](auto&& err) { return make_err<typename U::value_type, typename U::error_type>(err); }
    );
}

template <typename T, typename E>
template <typename F1, typename F2>
auto result<T, E>::match(F1 && on_ok, F2 && on_err) const -> decltype(on_ok(unwrap())) {
    static_assert(std::is_convertible<decltype(on_err(get_err())), decltype(on_ok(get_ok()))>::value,
                  "return value of on_err() must be equal or convertible to the return value of on_ok()");
    return is_ok() ? on_ok(get_ok()) : on_err(get_err());
}

template <typename T, typename E>
template <typename F>
auto result<T, E>::map(F && f) const -> result<std::decay_t<decltype(f(unwrap()))>, error_type> {
    using U = decltype(f(unwrap()));
    return match(
                [&](auto&& ok) { return make_ok<U, E>(f(ok)); },
                [](auto&& err) { return make_err<U, E>(err); }
    );
}

template <typename T, typename E>
template <typename F>
auto result<T, E>::map_err(F && f) const -> result<value_type, std::decay_t<decltype(f(unwrap_err()))>> {
    using U = decltype(f(unwrap_err()));
    return match(
                [](auto&& ok) { return make_ok<T, U>(ok); },
                [&](auto&& err) { return make_err<T, U>(f(err)); }
    );
}

template <typename T, typename E>
template <typename F1, typename F2>
auto result<T, E>::map_or_else(F1 && f, const F2 &def) const -> decltype(f(unwrap())) {
    return match(f, def);
}


}
