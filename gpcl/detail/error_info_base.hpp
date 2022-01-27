#ifndef GPCL_DETAIL_ERROR_INFO_BASE_HPP
#define GPCL_DETAIL_ERROR_INFO_BASE_HPP

#include <gpcl/detail/config.hpp>

#include <gpcl/excfwd.hpp>
#include <gpcl/typeid.hpp>

namespace gpcl::detail {

class error_info_base;

template <typename E1, typename... Es>
void link_error_info_helper(const error_info_base **pp, E1 &e1,
                            Es &...es) noexcept;

class error_info_base
{
  friend ::gpcl::exception;

  template <typename E1, typename... Es>
  friend void link_error_info_helper(const error_info_base **pp, E1 &e1,
                                     Es &...es) noexcept;

protected:
  mutable const error_info_base *next_ = nullptr;

  constexpr error_info_base() = default;
  error_info_base(const error_info_base &) {}

  error_info_base &operator=(error_info_base const &) { return *this; }

  virtual ~error_info_base() = default;

public:
  virtual const type_info &type() const noexcept = 0;

  virtual std::ostream &format_to(std::ostream &os) const = 0;
};

inline void link_error_info_helper(const error_info_base **) noexcept {}

template <typename E1, typename... Es>
void link_error_info_helper(const error_info_base **pp, E1 &e1,
                            Es &...es) noexcept
{
  *pp = std::addressof(e1);
  pp = &(*pp)->next_;
  link_error_info_helper(pp, es...);
}

template <typename... ErrorInfos>
const error_info_base *
link_error_infos(std::tuple<ErrorInfos...> &error_infos) noexcept
{
  const error_info_base *r = nullptr;
  const error_info_base **pp = &r;

  std::apply([&](auto &...errors) { link_error_info_helper(pp, errors...); },
             error_infos);

  return r;
}

} // namespace gpcl::detail

#endif // GPCL_DETAIL_ERROR_INFO_BASE_HPP
