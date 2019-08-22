#include "optional.hh"
#include <stdexcept>

namespace results {

void panic(std::string_view msg) {
    throw std::runtime_error(std::string(msg));
}

}
