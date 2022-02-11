//
// array.hpp
// ~~~~~~~~~
//
// Copyright (c) 2022 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_ARRAY_HPP
#define GPCL_ARRAY_HPP

#include <gpcl/assert.hpp>
#include <gpcl/default_allocator.hpp>
#include <gpcl/detail/compressed_pair.hpp>
#include <gpcl/detail/config.hpp>
#include <gpcl/error.hpp>
#include <gpcl/propagate_const.hpp>
#include <gpcl/scope_fail.hpp>
#include <gpcl/swap.hpp>
#include <gpcl/type_identity.hpp>

#include <algorithm>
#include <exception>
#include <initializer_list>

namespace gpcl {

/// A vector-like container with strong exception guarantee.
template <typename T, typename Allocator = default_allocator<T>>
class array
{
public:
  using size_type = std::size_t;
  using value_type = T;
  using reference = T &;
  using const_reference = const T &;
  using difference_type = std::ptrdiff_t;
  using pointer = T *;
  using const_pointer = const T *;
  using iterator = T *;
  using const_iterator = const T *;

public:
  detail::compressed_pair<Allocator, propagate_const<T *>> p_ = {};
  std::size_t size_ = 0;
  std::size_t capacity_ = 0;

public:
  array() noexcept(noexcept(Allocator())) : array(Allocator()) {}

  explicit array(const type_identity_t<Allocator> &a) noexcept : p_(a) {}

  explicit array(std::size_t n,
                 const type_identity_t<Allocator> &a = Allocator())
      : p_(a),
        capacity_(n)
  {
    T *const s = p_.second() =
        std::allocator_traits<Allocator>::allocate(p_.first(), capacity_);
    GPCL_TRY
    {
      while (size_ != capacity_)
      {
        std::allocator_traits<Allocator>::construct(p_.first(), &s[size_]);
        ++size_;
      }
    }
    GPCL_CATCH(...)
    {
      release();
      GPCL_RETHROW;
    }
    GPCL_CATCH_END
  }

  array(const array &other)
      : array(other,
              std::allocator_traits<Allocator>::
                  select_on_container_copy_construction(other.get_allocator()))
  {
  }

  array(const array &other, const type_identity_t<Allocator> &a)
      : array(other.begin(), other.end(), a)
  {
  }

  array(array &&other) noexcept
      : p_(std::move(other.p_)),
        size_(other.size_),
        capacity_(other.capacity_)
  {
    other.p_.second() = nullptr;
    other.size_ = 0;
    other.capacity_ = 0;
  }

  array(array &&other, const type_identity_t<Allocator> &a)
      : array(std::move(other), a,
              typename std::allocator_traits<Allocator>::is_always_equal{})
  {
  }

  array(array &&other, const type_identity_t<Allocator> &,
        std::true_type /*is_always_equal*/) noexcept
      : array(std::move(other))
  {
  }

  array(array &&other, const type_identity_t<Allocator> &a,
        std::false_type /*is_always_equal*/)
      : p_(a),
        capacity_(other.size())
  {
    if (a == other.get_allocator())
    {
      using gpcl::swap;
      p_.second() = std::move(other.p_.second());
      size_ = other.size_;
      capacity_ = other.capacity_;

      other.p_.second() = nullptr;
      other.size_ = 0;
      other.capacity_ = 0;
      return;
    }

    T *const s = p_.second() =
        std::allocator_traits<Allocator>::allocate(p_.first(), capacity_);
    GPCL_TRY
    {
      while (size_ != capacity_)
      {
        std::allocator_traits<Allocator>::construct(p_.first(), &s[size_],
                                                    std::move(other[size_]));
        ++size_;
      }
    }
    GPCL_CATCH(...)
    {
      release();
      GPCL_RETHROW;
    }
    GPCL_CATCH_END
  }

  template <typename FwdIt>
  array(FwdIt first, FwdIt last, const type_identity_t<Allocator> &a,
        std::forward_iterator_tag =
            typename std::iterator_traits<FwdIt>::iterator_category())
      : p_(a),
        capacity_(std::distance(first, last))
  {
    T *const s = p_.second() =
        std::allocator_traits<Allocator>::allocate(p_.first(), capacity_);
    GPCL_TRY
    {
      while (capacity_ != size_)
      {
        std::allocator_traits<Allocator>::construct(p_.first(), &s[size_],
                                                    *first++);
        ++size_;
      }
    }
    GPCL_CATCH(...)
    {
      release();
      GPCL_RETHROW;
    }
    GPCL_CATCH_END
  }

  array(std::initializer_list<T> init,
        const type_identity_t<Allocator> &alloc = Allocator())
      : array(init.begin(), init.end(), alloc)
  {
  }

  ~array() { release(); }

  void clear() noexcept { keep_first_n(0); }

private:
  void keep_first_n(size_t num_to_keep) noexcept
  {
    GPCL_ASSERT(size_ >= num_to_keep);
    while (size_ > num_to_keep)
    {
      std::allocator_traits<Allocator>::destroy(p_.first(),
                                                &p_.second()[size_ - 1]);
      --size_;
    }
  }

private:
  void release() noexcept
  {
    clear();
    std::allocator_traits<Allocator>::deallocate(p_.first(), p_.second(),
                                                 capacity_);
    p_.second() = nullptr;
    capacity_ = 0;
  }

public:
  array &operator=(const array &other)
  {
    assign(other);
    return *this;
  }

  array &operator=(array &&other) noexcept(
      std::allocator_traits<
          Allocator>::propagate_on_container_move_assignment::value ||
      std::allocator_traits<Allocator>::is_always_equal::value)
  {
    assign(std::move(other));
    return *this;
  }

  array &operator=(std::initializer_list<T> il)
  {
    assign(il.begin(), il.end());
    return *this;
  }

  template <
      typename A = Allocator,
      typename std::enable_if<!std::allocator_traits<A>::
                                  propagate_on_container_copy_assignment::value,
                              int>::type = 0>
  void assign(const array &other)
  {
    assign(other.begin(), other.end(), get_allocator());
  }

  template <
      typename A = Allocator,
      typename std::enable_if<std::allocator_traits<A>::
                                  propagate_on_container_copy_assignment::value,
                              int>::type = 0>
  void assign(const array &other)
  {
    assign(other.begin(), other.end(), other.get_allocator());
  }

  template <typename A = Allocator,
            typename std::enable_if<
                !std::allocator_traits<
                    A>::propagate_on_container_move_assignment::value &&
                    std::allocator_traits<A>::is_always_equal::value,
                int>::type = 0>
  void assign(array &&other) noexcept
  {
    using gpcl::swap;
    swap(p_, other.p_);
    swap(size_, other.size_);
    swap(capacity_, other.capacity_);
  }

  template <typename A = Allocator,
            typename std::enable_if<
                !std::allocator_traits<
                    A>::propagate_on_container_move_assignment::value &&
                    !std::allocator_traits<A>::is_always_equal::value,
                int>::type = 0>
  void assign(array &&other)
  {
    if (get_allocator() == other.get_allocator())
    {
      using gpcl::swap;
      swap(p_, other.p_);
      swap(size_, other.size_);
      swap(capacity_, other.capacity_);
      return;
    }

    array(std::move(other), get_allocator()).swap(*this);
  }

  template <
      typename A = Allocator,
      typename std::enable_if<std::allocator_traits<A>::
                                  propagate_on_container_move_assignment::value,
                              int>::type = 0>
  void assign(array &&other) noexcept
  {
    using gpcl::swap;
    swap(p_, other.p_);
    swap(size_, other.size_);
    swap(capacity_, other.capacity_);
  }

private:
  template <
      typename FwdIt, typename A = Allocator,
      typename std::enable_if<!std::allocator_traits<A>::is_always_equal::value,
                              int>::type = 0>
  void assign(FwdIt first, FwdIt last, const Allocator &alloc)
  {
    array(first, last, alloc).swap(*this);
  }

  template <
      typename FwdIt, typename A = Allocator,
      typename std::enable_if<std::allocator_traits<A>::is_always_equal::value,
                              int>::type = 0>
  void assign(FwdIt first, FwdIt last, const Allocator &)
  {
    auto diff = std::distance(first, last);
    GPCL_ASSERT(diff >= 0);
    std::size_t count = diff;

    if (count > size() || !std::is_nothrow_copy_assignable<T>::value)
    {
      array(first, last, get_allocator()).swap(*this);
      return;
    }

    keep_first_n(count);

    for (std::size_t i = 0; i != count; ++i)
    {
      (*this)[i] = *first++;
    }
  }

public:
  void swap(array &other) noexcept(
      std::allocator_traits<Allocator>::propagate_on_container_swap::value ||
      std::allocator_traits<Allocator>::is_always_equal::value)
  {
    do_swap(other);
  }

private:
  template <typename A = Allocator,
            typename std::enable_if<
                std::allocator_traits<A>::propagate_on_container_swap::value,
                int>::type = 0>
  void do_swap(array &other) noexcept
  {
    using gpcl::swap;
    swap(p_, other.p_);
    swap(size_, other.size_);
    swap(capacity_, other.capacity_);
  }

  template <typename A = Allocator,
            typename std::enable_if<
                !std::allocator_traits<A>::propagate_on_container_swap::value &&
                    std::allocator_traits<A>::is_always_equal::value,
                int>::type = 0>
  void do_swap(array &other) noexcept
  {
    using gpcl::swap;
    swap(p_.second(), other.p_.second());
    swap(size_, other.size_);
    swap(capacity_, other.capacity_);
  }

  template <typename A = Allocator,
            typename std::enable_if<
                !std::allocator_traits<A>::propagate_on_container_swap::value &&
                    !std::allocator_traits<A>::is_always_equal::value,
                int>::type = 0>
  void do_swap(array &other)
  {
    auto tmp = *this;
    *this = std::move(other);
    other = std::move(tmp);
  }

public:
  // void resize(size_type n) { array() }

public:
  Allocator get_allocator() const noexcept { return p_.first(); }

  T *data() noexcept { return p_.second(); }

  const T *data() const noexcept { return p_.second(); }

  std::size_t size() const noexcept { return size_; }

  bool empty() const noexcept
  {
    return size_ == 0;
  }

  std::size_t capacity() const noexcept { return capacity_; }

  T &operator[](std::size_t i) noexcept
  {
    GPCL_ASSERT(i < size_);
    return data()[i];
  }

  const T &operator[](std::size_t i) const noexcept
  {
    GPCL_ASSERT(i < size_);
    return data()[i];
  }

  T &at(std::size_t i)
  {
    if (i < size_)
      return data()[i];
    GPCL_THROW(std::out_of_range(__func__));
  }

  const T &at(std::size_t i) const
  {
    if (i < size_)
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

template <typename T, typename Allocator>
void swap(array<T, Allocator> &x,
          array<T, Allocator> &y) noexcept(noexcept(x.swap(y)))
{
  x.swap(y);
}

template <typename T, typename Allocator>
bool operator==(const array<T, Allocator> &x,
                const array<T, Allocator> &y) noexcept
{
  return std::equal(x.begin(), x.end(), y.begin(), y.end());
}

template <typename T, typename Allocator>
bool operator!=(const array<T, Allocator> &x,
                const array<T, Allocator> &y) noexcept
{
  return !(x == y);
}

template <typename T, typename Allocator>
bool operator<(const array<T, Allocator> &x,
               const array<T, Allocator> &y) noexcept
{
  return std::lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
}

} // namespace gpcl

#endif // GPCL_ARRAY_HPP
