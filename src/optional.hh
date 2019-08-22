#pragma once
#include <optional>
#include <string_view>

namespace results {

// todo: move to dedicated file/namespace
[[noreturn]] void panic(std::string_view msg);

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
    auto and_then(F && f) const -> option<decltype(f(T()))> {
        using U = decltype(f(T()));
        if (is_none()) {
            return option<U>();
        }
        return option<U>(f(*d_value));
    }

private:
    std::optional<T> d_value;
};

template <typename T>
option <T> none() {
    return option<T>();
}

template <typename T>
option <T> some(T value) {
    return option<T>(std::move(value));
}

}

