#ifndef GPCL_DETAIL_TAG_INVOKE_HPP
#define GPCL_DETAIL_TAG_INVOKE_HPP

#include <gpcl/detail/config.hpp>

namespace gpcl {
namespace detail {

void tag_invoke();

struct tag_invoke_impl
{
  template <typename Tag, typename... Args>
  constexpr decltype(auto) operator()(Tag &&tag, Args &&...args) const
      noexcept(noexcept(tag_invoke(static_cast<Tag &&>(tag),
                                   static_cast<Args &&>(args)...)))
  {
    return tag_invoke(static_cast<Tag &&>(tag), static_cast<Args &&>(args)...);
  }
};

} // namespace detail
} // namespace gpcl

#endif // GPCL_DETAIL_TAG_INVOKE_HPP
