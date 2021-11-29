#ifndef GPCL_ENABLE_SHARED_FROM_HPP
#define GPCL_ENABLE_SHARED_FROM_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/enable_shared_from_this.hpp>

namespace gpcl {

class enable_shared_from : public enable_shared_from_this<enable_shared_from>
{
protected:
  ~enable_shared_from() = default;
};

template <class T>
shared_ptr<T> shared_from(T *p)
{
  return static_pointer_cast<T>(p->enable_shared_from::shared_from_this());
}

template <class T>
weak_ptr<T> weak_from(T *p) noexcept
{
  return static_pointer_cast<T>(p->enable_shared_from::weak_from_this());
}

} // namespace gpcl

#endif // GPCL_ENABLE_SHARED_FROM_HPP
