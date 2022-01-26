#pragma once

#include <gpcl/detail/config.hpp>
#include <gpcl/swap.hpp>

#include <type_traits>

namespace gpcl {

template <typename T, typename = void>
struct is_reference_countable : std::false_type
{
};

template <typename T>
struct is_reference_countable<
    T, decltype(intrusive_ref_count_inc(std::declval<const T *>()),
                intrusive_ref_count_dec(std::declval<const T *>()), void())>
    : std::true_type
{
};

template <typename T>
class intrusive_ptr
{
  static_assert(is_reference_countable<T>::value, "");

  T *p_ = nullptr;

public:
  explicit constexpr intrusive_ptr() noexcept = default;

  explicit constexpr intrusive_ptr(std::nullptr_t) noexcept {}

  explicit constexpr intrusive_ptr(T *p) noexcept : p_(p) {}

  intrusive_ptr(T *p, bool inc) : p_(p)
  {
    if (inc && p_)
    {
      intrusive_ref_count_inc(p_);
    }
  }

  intrusive_ptr(const intrusive_ptr &other) : p_(other.p_)
  {
    if (p_)
      intrusive_ref_count_inc(p_);
  }

  template <typename U,
            typename std::enable_if<std::is_convertible<U *, T *>::value,
                                    int>::type = 0>
  intrusive_ptr(const intrusive_ptr<U> &other) : p_(other.p_)
  {
    if (p_)
      intrusive_ref_count_inc(p_);
  }

  intrusive_ptr(intrusive_ptr &&other) noexcept : p_(other.p_)
  {
    other.p_ = nullptr;
  }

  template <typename U,
            typename std::enable_if<std::is_convertible<U *, T *>::value,
                                    int>::type = 0>
  intrusive_ptr(intrusive_ptr<U> &&other) noexcept : p_(other.p_)
  {
    other.p_ = nullptr;
  }

  ~intrusive_ptr() noexcept
  {
    if (p_ == nullptr)
      return;
    intrusive_ref_count_dec(p_);
    p_ = nullptr;
  }

  intrusive_ptr &operator=(const intrusive_ptr &other)
  {
    intrusive_ptr(other).swap(*this);
    return *this;
  }

  template <typename U,
            typename std::enable_if<std::is_convertible<U *, T *>::value,
                                    int>::type = 0>
  intrusive_ptr &operator=(const intrusive_ptr<U> &other)
  {
    intrusive_ptr(other).swap(*this);
    return *this;
  }

  intrusive_ptr &operator=(intrusive_ptr &&other) noexcept
  {
    intrusive_ptr(std::move(other)).swap(*this);
    return *this;
  }

  template <typename U,
            typename std::enable_if<std::is_convertible<U *, T *>::value,
                                    int>::type = 0>
  intrusive_ptr &operator=(intrusive_ptr<U> &&other) noexcept
  {
    intrusive_ptr(std::move(other)).swap(*this);
    return *this;
  }

  explicit operator bool() const noexcept { return p_ != nullptr; }

  T *operator->() const noexcept
  {
    (void)sizeof(T);
    return p_;
  }

  T &operator*() const noexcept
  {
    (void)sizeof(T);
    return *p_;
  }

  void swap(intrusive_ptr &other) noexcept
  {
    using gpcl::swap;

    swap(p_, other.p_);
  }

  T *get() const noexcept { return p_; }

  void reset() noexcept
  {
    if (p_)
    {
      intrusive_ref_count_dec(p_);
      p_ = nullptr;
    }
  }

  template <typename U,
            typename std::enable_if<std::is_convertible<U *, T *>::value,
                                    int>::type = 0>
  void reset(U *p) noexcept
  {
    intrusive_ptr(p).swap(*this);
  }

  template <typename U,
            typename std::enable_if<std::is_convertible<U *, T *>::value,
                                    int>::type = 0>
  void reset(U *p, bool inc)
  {
    intrusive_ptr(p, inc).swap(*this);
  }

  void release() noexcept { p_ = nullptr; }
};

template <class T1, class T2>
bool operator==(const intrusive_ptr<T1> &x, const intrusive_ptr<T2> &y)
{
  return x.get() == y.get();
}

template <class T1, class T2>
bool operator!=(const intrusive_ptr<T1> &x, const intrusive_ptr<T2> &y)
{
  return x.get() != y.get();
}

template <class T1, class T2>
bool operator<(const intrusive_ptr<T1> &x, const intrusive_ptr<T2> &y)
{
  using CT = typename std::common_type<T1 *, T2 *>::type;

  return std::less<CT>()(x.get(), y.get());
}

template <class T1, class T2>
bool operator<=(const intrusive_ptr<T1> &x, const intrusive_ptr<T2> &y)
{
  return !(y < x);
}

template <class T1, class T2>
bool operator>(const intrusive_ptr<T1> &x, const intrusive_ptr<T2> &y)
{
  return y < x;
}

template <class T1, class T2>
bool operator>=(const intrusive_ptr<T1> &x, const intrusive_ptr<T2> &y)
{
  return !(x < y);
}

template <class T>
bool operator==(const intrusive_ptr<T> &x, std::nullptr_t) noexcept
{
  return !x;
}

template <class T>
bool operator==(std::nullptr_t, const intrusive_ptr<T> &x) noexcept
{
  return !x;
}

template <class T>
bool operator!=(const intrusive_ptr<T> &x, std::nullptr_t) noexcept
{
  return x;
}

template <class T>
bool operator!=(std::nullptr_t, const intrusive_ptr<T> &x) noexcept
{
  return x;
}

template <class T>
bool operator<(const intrusive_ptr<T> &x, std::nullptr_t)
{
  return std::less<typename intrusive_ptr<T>::pointer>()(x.get(), nullptr);
}

template <class T>
bool operator<(std::nullptr_t, const intrusive_ptr<T> &y)
{
  return std::less<typename intrusive_ptr<T>::pointer>()(nullptr, y.get());
}

template <class T>
bool operator<=(const intrusive_ptr<T> &x, std::nullptr_t)
{
  return !(nullptr < x);
}

template <class T>
bool operator<=(std::nullptr_t, const intrusive_ptr<T> &y)
{
  return !(y < nullptr);
}

template <class T>
bool operator>(const intrusive_ptr<T> &x, std::nullptr_t)
{
  return nullptr < x;
}

template <class T>
bool operator>(std::nullptr_t, const intrusive_ptr<T> &y)
{
  return y < nullptr;
}

template <class T>
bool operator>=(const intrusive_ptr<T> &x, std::nullptr_t)
{
  return !(x < nullptr);
}

template <class T>
bool operator>=(std::nullptr_t, const intrusive_ptr<T> &y)
{
  return !(nullptr < y);
}

/// @}

/// @relates intrusive_ptr
template <typename T>
void swap(intrusive_ptr<T> &x, intrusive_ptr<T> &y) noexcept
{
  x.swap(y);
}

/// @name Generic Pointer Casts
/// @relates gpcl::intrusive_ptr
/// @{

template <typename T, typename Y>
intrusive_ptr<T> static_pointer_cast(const intrusive_ptr<Y> &p) noexcept
{
  return intrusive_ptr<T>(static_cast<T *>(p.get()), true);
}

#if !defined GPCL_NO_RTTI
template <typename T, typename U>
intrusive_ptr<T> dynamic_pointer_cast(const intrusive_ptr<U> &p) noexcept
{
  return intrusive_ptr<T>(dynamic_cast<T *>(p.get()), true);
}
#endif

template <typename T, typename U>
intrusive_ptr<T> const_pointer_cast(const intrusive_ptr<U> &p) noexcept
{
  return intrusive_ptr<T>(const_cast<T *>(p.get()), true);
}

template <typename T, typename U>
intrusive_ptr<T> reinterpret_pointer_cast(const intrusive_ptr<U> &p) noexcept
{
  return intrusive_ptr<T>(reinterpret_cast<T *>(p.get()), true);
}

} // namespace gpcl
