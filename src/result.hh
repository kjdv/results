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

private:
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
    return d_value.index() == 0;
}

template<typename T, typename E>
bool result<T, E>::is_err() const {
    return d_value.index() == 1;
}


}
