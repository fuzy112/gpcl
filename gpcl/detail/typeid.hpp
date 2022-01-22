#pragma once

#include <gpcl/detail/config.hpp>
#include <gpcl/noncopyable.hpp>

#include <functional>

namespace gpcl::detail {

template <typename T>
struct typeid_impl;

enum class type_info_init
{
  init,
};

struct type_info : noncopyable
{
  template <typename T>
  friend struct typeid_impl;


  const char *name() const noexcept { return "unknown"; }

  constexpr bool before(const type_info &other) const noexcept
  {
    return this < &other;
  }

  constexpr bool operator==(const type_info &other) const noexcept
  {
    return this == &other;
  }

  constexpr bool operator!=(const type_info &other) const noexcept
  {
    return this != &other;
  }

  std::size_t hash_code() const noexcept
  {
    return std::hash<const type_info *>()(this);
  }

private:
  constexpr explicit type_info(type_info_init) noexcept {}
};

template <typename T>
struct typeid_impl
{
  static type_info typeid_;
};

template <typename T>
type_info typeid_impl<T>::typeid_{type_info_init::init};

} // namespace gpcl::detail
