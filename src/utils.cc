#include "utils.hh"
#include <stdexcept>
#include <string>

namespace results {

namespace internal {

void panic(std::string_view msg) {
    throw panicked(std::string(msg));
}

}
}
