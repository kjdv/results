#pragma once

#include <stdexcept>
#include <string>
#include <string_view>

namespace results {

class panicked : public std::exception
{
public:
  explicit panicked(std::string_view msg);

  const char* what() const noexcept;

private:
  std::string d_msg;
};

namespace internal {

[[noreturn]] void panic(std::string_view msg);

}

} // namespace results
