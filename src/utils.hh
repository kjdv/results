#pragma once

#include <string_view>
#include <stdexcept>

namespace results {

class panicked : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

namespace internal {

[[noreturn]] void panic(std::string_view msg);

}

}
