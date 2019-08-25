#pragma once

#include <optional>
#include <string_view>
#include <type_traits>

namespace results {

// todo: move to dedicated file/namespace
[[noreturn]] void panic(std::string_view msg);

// c++17 magic...
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

template <typename T>
class option {
public:
    option()
    {}
    option(T value)
        : d_value(std::move(value))
    {}

    bool is_none() const {
        return !d_value.has_value();
    }

    bool is_some() const {
        return d_value.has_value();
    }

    const T & unwrap() const {
        return expect("unwrapping none");
    }

    const T & expect(std::string_view msg) const {
        if (!is_some()) {
            panic(msg);
        }
        return *d_value;
    }

    template <typename F>
    auto and_then(F && f) const -> option<decltype(f(unwrap()))> {
        using U = decltype(f(unwrap()));
        if (is_none()) {
            return option<U>();
        }
        return option<U>(f(*d_value));
    }

    template <typename F1, typename F2>
    auto match(F1 && on_some, F2 && on_none) -> decltype(on_some(unwrap())) {
        static_assert(std::is_constructible<decltype(on_none()), decltype(on_some(unwrap()))>::value,
                      "return value of on_none() must be equal or convertible to the return value of on_some()");

        if (is_some()) {
            return on_some(*d_value);
        }
        return on_none();
    }
private:
    std::optional<T> d_value;
};

template <typename T>
option<std::decay_t<T>> make_none() {
    return option<std::decay_t<T>>();
}

template <typename T, typename... Args>
option<std::decay_t<T>> make_some(Args&&... args) {
    return option<std::decay_t<T>>(T(std::forward<Args>(args)...));
}

}

