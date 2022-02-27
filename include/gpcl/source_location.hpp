#ifndef GPCL_SOURCE_LOCATION_HPP
#define GPCL_SOURCE_LOCATION_HPP

#include <gpcl/detail/config.hpp>

#include <string_view>

#ifndef GPCL_CONFIG_NO_IOSTREAMS
#  include <ostream>
#endif

namespace gpcl {

class source_location
{
public:
  constexpr source_location() = default;

  constexpr source_location(std::string_view file, std::uint_least32_t line,
                            std::string_view function) noexcept
      : file_{file},
        line_{line},
        function_{function}
  {
  }

  constexpr std::string_view file() const noexcept { return file_; }

  constexpr std::uint_least32_t line() const noexcept { return line_; }

  constexpr std::string_view function() const noexcept { return function_; }

#ifndef GPCL_CONFIG_NO_IOSTREAMS
  friend std::ostream &operator<<(std::ostream &out, source_location location)
  {
    std::ostream::sentry valid(out);
    if (!valid)
      return out;

    return out << location.file() << ':' << location.line() << ' ' << '('
               << location.function() << ')';
  }
#endif

private:
  std::string_view file_;
  std::uint_least32_t line_{};
  std::string_view function_;
};

} // namespace gpcl

#define GPCL_SOURCE_LOCATION_CURRENT_LINE()                                    \
  ::gpcl::source_location(__FILE__, __LINE__, __func__)

#endif // GPCL_SOURCE_LOCATION_HPP
