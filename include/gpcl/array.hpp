#ifndef GPCL_ARRAY_HPP
#define GPCL_ARRAY_HPP

#include <gpcl/assert.hpp>
#include <gpcl/default_allocator.hpp>
#include <gpcl/detail/compressed_pair.hpp>
#include <gpcl/detail/config.hpp>
#include <gpcl/error.hpp>
#include <gpcl/noncopyable.hpp>
#include <gpcl/propagate_const.hpp>
#include <gpcl/scope_fail.hpp>
#include <gpcl/type_identity.hpp>

#include <exception>

namespace gpcl {

template <typename T, typename Allocator = default_allocator<T>>
class array : noncopyable
{
  detail::compressed_pair<Allocator, propagate_const<T *>> p_ = {};
  std::size_t n_ = 0;

public:
  explicit array(const type_identity_t<Allocator> &a = Allocator()) noexcept
      : p_(a)
  {
  }

  explicit array(std::size_t n,
                 const type_identity_t<Allocator> &a = Allocator())
      : p_(a),
        n_(n)
  {
    auto *const s = std::allocator_traits<Allocator>::allocate(p_.first(), n_);
    std::size_t i = 0;
    GPCL_TRY
    {
      while (i != n)
      {
        std::allocator_traits<Allocator>::destroy(p_.first(), &s[i]);
        ++i;
      }
      p_.second() = s;
    }
    GPCL_CATCH(...)
    {
      while (i > 0)
      {
        std::allocator_traits<Allocator>::destroy(p_.first(), &s[i - 1]);
        --i;
      }
      std::allocator_traits<Allocator>::deallocate(p_.first(), s, n_);
      GPCL_RETHROW;
    }
    GPCL_CATCH_END
  }

  ~array()
  {
    std::size_t i = n_;
    T *s = p_.second();
    while (i > 0)
    {
      std::allocator_traits<Allocator>::destroy(p_.first(), &s[i - 1]);
      --i;
    }
    std::allocator_traits<Allocator>::deallocate(p_.first(), s, n_);
  }

  Allocator get_allocator() const noexcept { return p_.first(); }

  T *data() noexcept { return p_.second(); }

  const T *data() const noexcept { return p_.second(); }

  std::size_t size() const noexcept { return n_; }

  T &operator[](std::size_t i) noexcept
  {
    GPCL_ASSERT(i < n_);
    return data()[i];
  }

  const T &operator[](std::size_t i) const noexcept
  {
    GPCL_ASSERT(i < n_);
    return data()[i];
  }

  T &at(std::size_t i)
  {
    if (i < n_)
      return data()[i];
    GPCL_THROW(std::out_of_range(__func__));
  }

  const T &at(std::size_t i) const
  {
    if (i < n_)
      return data()[i];
    GPCL_THROW(std::out_of_range(__func__));
  }

  T *begin() noexcept { return data(); }

  const T *begin() const noexcept { return data(); }

  const T *cbegin() const noexcept { return data(); }

  T *end() noexcept { return data() + size(); }

  const T *end() const noexcept { return data() + size(); }

  const T *cend() const noexcept { return data() + size(); }
};

} // namespace gpcl

#endif // GPCL_ARRAY_HPP
