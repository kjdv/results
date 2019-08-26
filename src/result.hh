#pragma once

#include <variant>
#include <string>
#include <string_view>
#include "utils.hh"

namespace results {

struct error {
    std::string msg;
    error(std::string_view m)
        : msg(m)
    {}
};

template <typename T, typename E = error>
class result {
public:
    explicit result(T ok);
    explicit result(E err);

    bool is_ok() const;

    bool is_err() const;

    const T &expect(std::string_view msg) const;

    const E &expect_err(std::string_view msg) const {
        if (!is_err()) {
            internal::panic(msg);
        }
        return get_err();
    }

    const T &unwrap() const;

    const E &unwrap_err() const {
        return expect_err("unwrapping ok");
    }

    const T &unwrap_or(const T &other) {
        return is_ok() ? get_ok() : other;
    }

    template <typename F>
    T unwrap_or_else(F && f) const {
        return is_ok() ? get_ok() : f();
    }

    const result<T, E> &and_(const result<T, E> &other) const;

    const result<T, E> &or_(const result<T, E> &other) const;

    template <typename F>
    result<T, E> or_else(F && f) const;

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


}
