#include "utils.hh"
#include <stdexcept>
#include <string>

namespace results {

panicked::panicked(std::string_view msg)
  : d_msg(msg)
{
}

const char* panicked::what() const noexcept
{
  return d_msg.c_str();
}

namespace internal {

void panic(std::string_view msg)
{
  throw panicked(std::string(msg));
}

} // namespace internal

} // namespace results
