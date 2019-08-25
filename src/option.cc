#include "option.hh"
#include <stdexcept>
#include <string>

namespace results {

void panic(std::string_view msg) {
    throw std::runtime_error(std::string(msg));
}

}
