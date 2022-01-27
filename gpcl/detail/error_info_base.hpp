#ifndef GPCL_DETAIL_ERROR_INFO_BASE_HPP
#define GPCL_DETAIL_ERROR_INFO_BASE_HPP

#include <gpcl/detail/config.hpp>

#include <gpcl/excfwd.hpp>
#include <gpcl/intrusive_list.hpp>
#include <gpcl/typeid.hpp>

namespace gpcl::detail {

class error_info_base : public intrusive_list_node<error_info_base>
{
  mutable std::atomic_long ref_count_{1};

protected:
  virtual ~error_info_base()
  {
    if (in_list())
      gpcl::delete_entry(this);
  }

public:
  virtual const type_info &type() const noexcept = 0;

  virtual std::ostream &format_to(std::ostream &os) const = 0;

  friend inline void
  intrusive_ref_count_inc(const error_info_base &errinfo) noexcept
  {
    errinfo.ref_count_ += 1;
  }

  friend inline void
  intrusive_ref_count_dec(const error_info_base &errinfo) noexcept
  {
    if (errinfo.ref_count_.fetch_sub(1) == 1)
    {
      delete std::addressof(errinfo);
    }
  }
};

} // namespace gpcl::detail

#endif // GPCL_DETAIL_ERROR_INFO_BASE_HPP
