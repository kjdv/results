#include "result.hh"

namespace results {

error::error(std::string_view m) noexcept
  : msg(m)
{
}

} // namespace results
