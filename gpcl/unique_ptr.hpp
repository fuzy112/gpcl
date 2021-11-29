//
// unique_ptr.hpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_UNIQUE_HPP
#define GPCL_UNIQUE_HPP

#include <gpcl/assert.hpp>
#include <gpcl/detail/compressed_pair.hpp>
#include <gpcl/detail/config.hpp>

#include <new>

namespace gpcl {

template <typename T>
struct default_delete
{
  constexpr default_delete() noexcept = default;

  template <class U, typename = typename std::enable_if<
                         std::is_convertible<U *, T *>::value>::type>
  default_delete(const default_delete<U> &) noexcept
  {
  }

  void operator()(T *x) const noexcept { delete x; }
};

template <typename T>
struct default_delete<T[]>
{
  constexpr default_delete() noexcept = default;

  default_delete(const default_delete &) noexcept = default;

  void operator()(T *x) const noexcept { delete[] x; }
};

template <typename T, typename Deleter, typename = void>
struct pointer_type
{
  using type = T *;
};

template <typename T, typename Deleter>
struct pointer_type<T, Deleter, std::void_t<typename Deleter::pointer>>
{
  using type = typename Deleter::pointer;
};

/// @ingroup SmartPtr
template <class T, class Deleter = default_delete<T>>
class unique_ptr
{
public:
  using pointer = typename pointer_type<T, Deleter>::type;
  using element_type = T;
  using deleter_type = Deleter;

  detail::compressed_pair<pointer, deleter_type> _p;

  template <typename D = Deleter,
            typename = std::enable_if_t<std::is_default_constructible<D>{} &&
                                        !std::is_pointer<D>{}>>
  constexpr unique_ptr() noexcept
  {
  }

  template <typename D = Deleter,
            typename = std::enable_if_t<std::is_default_constructible<D>{} &&
                                        !std::is_pointer<D>{}>>
  constexpr unique_ptr(std::nullptr_t) noexcept
  {
  }

  template <typename D = Deleter,
            typename = std::enable_if_t<std::is_default_constructible<D>{} &&
                                        !std::is_pointer<D>{}>>
  explicit unique_ptr(pointer p) noexcept : _p(p, D())
  {
    static_assert(!std::is_pointer<deleter_type>(), "");
  }

  unique_ptr(pointer p, Deleter &d) noexcept
      : _p(p, std::forward<decltype(d)>(d))
  {
  }

  unique_ptr(pointer p, std::remove_reference_t<Deleter> &&d) noexcept
      : _p(p, std::forward<decltype(d)>(d))
  {
  }

  unique_ptr(unique_ptr &&u) noexcept : _p(std::move(u._p)) { u.release(); }

  template <class U, class E>
  unique_ptr(unique_ptr<U, E> &&u) noexcept : _p(u.release(), u.get_deleter())
  {
  }

  // template <class U>
  // unique_ptr(std::auto_ptr<U> &&u) noexcept;

  ~unique_ptr()
  {
    if (get())
    {
      get_deleter()(get());
    }
  }

  pointer get() const noexcept { return _p.first(); }

  deleter_type &get_deleter() noexcept { return _p.second(); }

  const deleter_type &get_deleter() const noexcept { return _p.second(); }

  unique_ptr &operator=(unique_ptr &&r) noexcept
  {
    reset(r.release());
    get_deleter() = std::forward<Deleter>(r.get_deleter());
    return *this;
  }

  template <class U, class E>
  typename std::enable_if<
      !std::is_array<U>::value &&
          std::is_convertible<typename unique_ptr<U, E>::pointer,
                              pointer>::value &&
          std::is_assignable<Deleter &, E &&>::value,
      unique_ptr &>::type
  operator=(unique_ptr<U, E> &&r) noexcept
  {
    reset(r.release());
    get_deleter() = std::forward<E>(r.get_deleter());

    return *this;
  }

  unique_ptr &operator=(std::nullptr_t) noexcept { reset(); }

  pointer release() noexcept
  {
    pointer r = get();
    _p.first() = nullptr;
    return r;
  }

  void reset(pointer ptr = pointer()) noexcept
  {
    pointer old_ptr = get();
    _p.first() = ptr;
    if (old_ptr)
      get_deleter()(old_ptr);
  }

  void swap(unique_ptr &other) noexcept
  {
    using std::swap;
    swap(_p, other._p);
  }

  explicit operator bool() const noexcept { return get(); }

  typename std::add_lvalue_reference<T>::type operator*() const
  {
    GPCL_ASSERT(get());
    return *get();
  }

  pointer operator->() const noexcept
  {
    GPCL_ASSERT(get());
    return get();
  }
};

template <class T, class Deleter>
class unique_ptr<T[], Deleter>
{
};

template <class T1, class D1, class T2, class D2>
bool operator==(const unique_ptr<T1, D1> &x, const unique_ptr<T2, D2> &y)
{
  return x.get() == y.get();
}

template <class T1, class D1, class T2, class D2>
bool operator!=(const unique_ptr<T1, D1> &x, const unique_ptr<T2, D2> &y)
{
  return x.get() != y.get();
}

template <class T1, class D1, class T2, class D2>
bool operator<(const unique_ptr<T1, D1> &x, const unique_ptr<T2, D2> &y)
{
  using CT =
      typename std::common_type<decltype(x.get()), decltype(y.get())>::type;

  return std::less<CT>()(x.get(), y.get());
}

template <class T1, class D1, class T2, class D2>
bool operator<=(const unique_ptr<T1, D1> &x, const unique_ptr<T2, D2> &y)
{
  return !(y < x);
}

template <class T1, class D1, class T2, class D2>
bool operator>(const unique_ptr<T1, D1> &x, const unique_ptr<T2, D2> &y)
{
  return y < x;
}

template <class T1, class D1, class T2, class D2>
bool operator>=(const unique_ptr<T1, D1> &x, const unique_ptr<T2, D2> &y)
{
  return !(x < y);
}

template <class T, class D>
bool operator==(const unique_ptr<T, D> &x, std::nullptr_t) noexcept
{
  return !x;
}

template <class T, class D>
bool operator==(std::nullptr_t, const unique_ptr<T, D> &x) noexcept
{
  return !x;
}

template <class T, class D>
bool operator!=(const unique_ptr<T, D> &x, std::nullptr_t) noexcept
{
  return x;
}

template <class T, class D>
bool operator!=(std::nullptr_t, const unique_ptr<T, D> &x) noexcept
{
  return x;
}

template <class T, class D>
bool operator<(const unique_ptr<T, D> &x, std::nullptr_t)
{
  return std::less<typename unique_ptr<T, D>::pointer>()(x.get(), nullptr);
}

template <class T, class D>
bool operator<(std::nullptr_t, const unique_ptr<T, D> &y)
{
  return std::less<typename unique_ptr<T, D>::pointer>()(nullptr, y.get());
}

template <class T, class D>
bool operator<=(const unique_ptr<T, D> &x, std::nullptr_t)
{
  return !(nullptr < x);
}

template <class T, class D>
bool operator<=(std::nullptr_t, const unique_ptr<T, D> &y)
{
  return !(y < nullptr);
}

template <class T, class D>
bool operator>(const unique_ptr<T, D> &x, std::nullptr_t)
{
  return nullptr < x;
}

template <class T, class D>
bool operator>(std::nullptr_t, const unique_ptr<T, D> &y)
{
  return y < nullptr;
}

template <class T, class D>
bool operator>=(const unique_ptr<T, D> &x, std::nullptr_t)
{
  return !(x < nullptr);
}

template <class T, class D>
bool operator>=(std::nullptr_t, const unique_ptr<T, D> &y)
{
  return !(nullptr < y);
}

template <typename T, typename D>
void swap(unique_ptr<T, D> &x, unique_ptr<T, D> &y) noexcept
{
  x.swap(y);
}

template <typename T, typename U>
unique_ptr<T> static_pointer_cast(unique_ptr<U> &&p) noexcept
{
  return unique_ptr<T>(static_cast<T *>(p.release()));
}

template <typename T, typename U>
unique_ptr<T> dynamic_pointer_cast(unique_ptr<U> &&p) noexcept
{
  if (auto tp = dynamic_cast<T*>(p.get()))
  {
    p.release();
    return unique_ptr<T>(tp);
  }
  return unique_ptr<T>();
}

template <typename T, typename U>
unique_ptr<T> const_pointer_cast(unique_ptr<U>&& p) noexcept
{
  return unique_ptr<T>(const_cast<T *>(p.release()));
}

template <typename T, typename U>
unique_ptr<T> reinterpret_pointer_cast(unique_ptr<U> &&p) noexcept
{
  return unique_ptr<T>(reinterpret_cast<T *>(p.release()));
}


template <typename T, typename... Args>
unique_ptr<T> make_unique(Args &&... args)
{
  return unique_ptr<T>(new T(std::forward<Args>(args)...));
}

template <typename T, typename Deleter = gpcl::default_delete<T>>
unique_ptr<T, Deleter> wrap_unique(T *ptr, Deleter d = Deleter())
{
  return unique_ptr<T, Deleter>(ptr, std::move(d));
}

} // namespace gpcl

#endif // GPCL_UNIQUE_HPP
