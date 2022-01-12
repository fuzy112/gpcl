//
// vector.hpp
// ~~~~~~~~~~
//
// Copyright (c) 2021-2022 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_VECTOR_HPP
#define GPCL_VECTOR_HPP

#include <gpcl/default_allocator.hpp>
#include <gpcl/detail/compressed_pair.hpp>
#include <gpcl/detail/config.hpp>
#include <gpcl/detail/iterator.hpp>
#include <gpcl/error.hpp>
#include <gpcl/swap.hpp>
#include <gpcl/type_identity.hpp>

namespace gpcl {

template <typename Tp>
class vector_const_iterator;

template <typename Tp>
class vector_iterator
    : public detail::iterator_wrapper<vector_iterator<Tp>, Tp, Tp &, Tp *,
                                      std::random_access_iterator_tag>
{
public:
  constexpr vector_iterator() noexcept = default;

  explicit vector_iterator(Tp *data) noexcept : m_data(data) {}

  constexpr operator vector_const_iterator<Tp>() const noexcept;

  Tp *m_data;
};

template <typename Tp>
class vector_const_iterator
    : public detail::iterator_wrapper<vector_const_iterator<Tp>, Tp, const Tp &,
                                      const Tp *,
                                      std::random_access_iterator_tag>
{
public:
  constexpr vector_const_iterator() noexcept = default;

  explicit vector_const_iterator(const Tp *data) noexcept
      : m_data(const_cast<Tp *>(data))
  {
  }

  constexpr explicit vector_const_iterator(vector_iterator<Tp> it) noexcept
      : vector_const_iterator(it.m_data)
  {
  }

  Tp *m_data;
};

template <typename Tp>
constexpr vector_iterator<Tp>::operator vector_const_iterator<Tp>()
    const noexcept
{
  return vector_const_iterator<Tp>{*this};
}

template <typename Tp, typename Allocator = gpcl::default_allocator<Tp>>
class vector
{
public:
  // types:
  using value_type = Tp;
  using allocator_type = Allocator;
  using alloc_traits = std::allocator_traits<allocator_type>;
  using pointer = typename alloc_traits::pointer;
  using const_pointer = typename alloc_traits::const_pointer;
  using reference = value_type &;
  using rvalue_reference = value_type &&;
  using const_reference = const value_type &;
  using size_type = size_t;                                 // see 26.2
  using difference_type = std::ptrdiff_t;                   // see 26.2
  using iterator = vector_iterator<value_type>;             // see 26.2
  using const_iterator = vector_const_iterator<value_type>; // see 26.2
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

private:
  static void destroy(pointer data, size_type count,
                      allocator_type &allocator) noexcept
  {
    [](auto &&...) {}(data, count, allocator);
    if constexpr (!std::is_trivially_destructible_v<value_type> &&
                  !std::is_fundamental_v<value_type>)

      for (size_type i = 0; i != count; ++i)
        alloc_traits::destroy(allocator, &data[i]);
  }

  static void destroy_and_dealloc(pointer data, size_type count, size_type cap,
                                  allocator_type &allocator) noexcept
  {
    destroy(data, count, allocator);
    alloc_traits::deallocate(allocator, data, cap);
  }

  static void construct(pointer data, size_type count, std::tuple<>,
                        allocator_type &allocator)
  {
    if constexpr (std::is_trivially_default_constructible_v<value_type>)
      return;
    if constexpr (std::is_fundamental_v<value_type>)
    {
      std::memset(data, 0, count * sizeof(value_type));
      return;
    }
    size_type i = 0;
    GPCL_TRY
    {
      while (i < count)
      {
        alloc_traits::construct(allocator, data + i);
        ++i;
      }
    }
    GPCL_CATCH(...)
    {
      destroy(data, i, allocator);
      GPCL_RETHROW;
    }
    GPCL_CATCH_END
  }

  template <typename Arg>
  static auto construct(pointer data, size_type count, Arg &&arg,
                        allocator_type &allocator)
      -> std::void_t<decltype(std::tuple_size<std::decay_t<Arg>>::value)>
  {
    size_type i = 0;
    GPCL_TRY
    {
      while (i < count)
      {
        std::apply(
            [&allocator, ptr = data + i](auto &&...args) {
              alloc_traits::construct(allocator, ptr,
                                      std::forward<decltype(args)>(args)...);
            },
            std::forward<Arg>(arg));
        ++i;
      }
    }
    GPCL_CATCH(...)
    {
      destroy(data, i, allocator);
      GPCL_RETHROW;
    }
    GPCL_CATCH_END
  }

  template <typename InputIterator,
            typename = std::enable_if_t<
                std::is_same_v<std::remove_cv_t<typename std::iterator_traits<
                                   InputIterator>::value_type>,
                               value_type>>>
  static auto construct(pointer data, size_type count, InputIterator it,
                        allocator_type &allocator)
  {
    size_type i = 0;
    GPCL_TRY
    {
      while (i < count)
      {
        alloc_traits::construct(allocator, data + i, *it++);
        ++i;
      }
    }
    GPCL_CATCH(...)
    {
      destroy(data, i, allocator);
      GPCL_RETHROW;
    }
    GPCL_CATCH_END
  }

  template <typename ValTp = value_type>
  static auto construct(pointer data, size_type count,
                        detail::move_if_noexcept_iterator<pointer> it,
                        allocator_type &) noexcept
      -> std::enable_if_t<std::is_trivially_copyable_v<ValTp>>
  {
    std::memmove(data, it.data(), count * sizeof(value_type));
  }

  template <typename Arg>
  static pointer alloc_and_construct(size_type count, size_type cap, Arg &&arg,
                                     allocator_type &allocator,
                                     pointer hint = nullptr)
  {
    pointer const data = alloc_traits::allocate(allocator, cap, hint);
    GPCL_TRY
    {
      construct(data, count, arg, allocator);
      return data;
    }
    GPCL_CATCH(...)
    {
      alloc_traits::deallocate(allocator, data, cap);
      GPCL_RETHROW;
    }
    GPCL_CATCH_END
  }

  template <typename Arg>
  static pointer alloc_and_construct(size_type count, Arg &&arg,
                                     allocator_type &allocator,
                                     pointer hint = nullptr)
  {
    return alloc_and_construct(count, count, std::forward<Arg>(arg), allocator,
                               hint);
  }

  template <typename... Args>
  static decltype(auto) forward_args(Args &&...args) noexcept
  {
    return std::tuple<Args &&...>(std::forward<Args>(args)...);
  }

  static void realloc(pointer &data, size_type count, size_type &cap,
                      size_type new_cap, allocator_type &allocator)
  {
    GPCL_ASSERT_CONST((!data && !count) || (data && cap));

    if constexpr (std::is_trivially_copyable_v<value_type>)
    {
      const auto new_data = alloc_traits::allocate(allocator, new_cap);
      if (data)
        std::memcpy(new_data, data, count * sizeof(value_type));
      alloc_traits::deallocate(allocator, data, cap);
      data = new_data;
      cap = new_cap;
    }
    else
    {
      pointer const new_data = alloc_and_construct(
          count, new_cap, detail::make_move_if_noexcept_iterator(data),
          allocator, data);
      destroy_and_dealloc(data, count, cap, allocator);
      data = new_data;
      cap = new_cap;
    }
  }

public:
  // 26.3.11.2, construct/copy/destroy
  constexpr vector() noexcept(noexcept(Allocator())) : vector(Allocator()) {}

  constexpr explicit vector(const allocator_type &allocator) noexcept
      : p_(allocator, nullptr),
        cap_(0),
        size_(0)
  {
  }

  explicit vector(size_type num,
                  const allocator_type &allocator = allocator_type())
      : p_(allocator, nullptr),
        cap_(num),
        size_(num)
  {
    p_.second() = alloc_and_construct(num, forward_args(), stored_allocator());
  }

  vector(size_type num, const_reference val,
         const allocator_type &allocator = allocator_type())
      : p_(allocator, nullptr),
        cap_(num),
        size_(num)
  {
    p_.second() =
        alloc_and_construct(num, forward_args(val), stored_allocator());
  }

  template <typename InputIterator>
  vector(InputIterator first, InputIterator last,
         const allocator_type &allocator = allocator_type())
      : p_(allocator, nullptr)
  {
    if constexpr (std::is_same_v<typename std::iterator_traits<
                                     InputIterator>::iterator_category,
                                 std::input_iterator_tag>)
    {
      cap_ = 0;
      size_ = 0;
      storage() = nullptr;
      GPCL_TRY
      {
        while (first != last)
          emplace_back(*first++);
      }
      GPCL_CATCH(...)
      {
        destroy_and_dealloc(storage(), size_, cap_, stored_allocator());
      }
      GPCL_CATCH_END
    }
    else
    {
      const size_type num = std::distance(first, last);
      GPCL_ASSERT(num >= 0);
      storage() = alloc_and_construct(num, first, stored_allocator());
      cap_ = num;
      size_ = num;
    }
  }

  vector(const vector &other)
      : vector(other, alloc_traits::select_on_container_copy_construction(
                          other.stored_allocator()))
  {
  }

  vector(vector &&other) noexcept
      : vector(std::move(other), other.get_allocator())
  {
  }

  vector(const vector &other, const type_identity_t<Allocator> &allocator)
      : p_(allocator, nullptr),
        cap_(other.size_),
        size_(other.size_)
  {
    p_.second() =
        alloc_and_construct(other.size_, other.storage(), stored_allocator());
  }

// clang-format off
#if defined _MSC_VER
# pragma warning(push)
# pragma warning(disable : 4127)
#endif
  // clang-format on
  vector(vector &&other, const type_identity_t<Allocator> &allocator)
      : p_(allocator, nullptr)
  {
    if (alloc_traits::is_always_equal::value ||
        stored_allocator() == other.get_allocator())
    {
      storage() = other.storage();
      cap_ = other.cap_;
      size_ = other.size_;
    }
    else
    {
      storage() =
          alloc_and_construct(other.size_, other.storage(), stored_allocator());
      size_ = other.size_;
      cap_ = size_;
    }
    other.storage() = nullptr;
    other.cap_ = 0;
    other.size_ = 0;
  }
// clang-format off
#if defined _MSC_VER
# pragma warning(pop)
#endif
  // clang-format on

  vector(std::initializer_list<value_type> il,
         const allocator_type &allocator = Allocator())
      : vector(il.begin(), il.end(), allocator)
  {
  }

  ~vector() { destroy_and_dealloc(storage(), size_, cap_, stored_allocator()); }

  vector &operator=(const vector &other)
  {
    constexpr bool copy_allocator =
        alloc_traits::propagate_on_container_copy_assignment::value;
    const bool reallocate =
        cap_ < other.size_ ||
        (copy_allocator && stored_allocator() != other.get_allocator());

    if (reallocate)
    {
      destroy_and_dealloc(storage(), size_, cap_, stored_allocator());
      cap_ = 0;
      size_ = 0;
    }

    if constexpr (copy_allocator)
      stored_allocator() = other.get_allocator();

    if (reallocate)
    {
      storage() = alloc_traits::allocate(stored_allocator(), other.size_,
                                         other.storage());

      cap_ = other.size_;
      size_ = 0;
    }

    const size_type common = (std::min)(size_, other.size_);

    // CopyAssignable
    for (size_type i = 0; i != common; ++i)
      storage()[i] = other.storage()[i];

    // Erasable
    if (size_ > common)
    {
      destroy(storage() + common, size_ - common, stored_allocator());
      size_ = common;
    }

    // CopyInsertible
    if (size_ < other.size_)
    {
      construct(storage() + size_, other.size_ - size_, other.storage() + size_,
                stored_allocator());
      size_ = other.size_;
    }

    return *this;
  }

  vector &operator=(vector &&other) noexcept(
      alloc_traits::propagate_on_container_move_assignment::value ||
      alloc_traits::is_always_equal::value)
  {
    constexpr bool move_allocator =
        alloc_traits::propagate_on_container_move_assignment::value;
    const bool can_take_ownership =
        move_allocator || stored_allocator() == other.get_allocator();
    const bool deallocate = can_take_ownership || cap_ < other.size_;

    if (deallocate)
    {
      destroy_and_dealloc(storage(), size_, cap_, stored_allocator());
      storage() = nullptr;
      cap_ = 0;
      size_ = 0;
    }

    if constexpr (move_allocator)
    {
      stored_allocator() = std::move(other.stored_allocator());
    }

    if (can_take_ownership)
    {
      using gpcl::swap;
      swap(storage(), other.storage());
      swap(cap_, other.cap_);
      swap(size_, other.size_);
      return *this;
    }

    if (cap_ < other.size_)
    {
      storage() = alloc_traits::allocate(stored_allocator(), other.size_,
                                         other.storage());
      cap_ = other.size_;
    }

    const size_type common = (std::min)(size_, other.size_);

    // MoveAssignable
    for (size_type i = 0; i != common; ++i)
      storage()[i] = std::move_if_noexcept(other.storage()[i]);

    // Erasable
    if (size_ > common)
    {
      destroy(storage() + common, size_ - common, stored_allocator());
      size_ = common;
    }

    // MoveInsertible
    if (size_ < other.size_)
    {
      construct(storage() + size_, other.size_ - size_,
                detail::make_move_if_noexcept_iterator(other.storage() + size_),
                stored_allocator());
      size_ = other.size_;
    }

    return *this;
  }

  vector &operator=(std::initializer_list<value_type> il)
  {
    assign(il.begin(), il.end());
    return *this;
  }

  template <typename InputIterator>
  void assign(InputIterator first, InputIterator last)
  {
    if constexpr (std::is_same_v<typename std::iterator_traits<
                                     InputIterator>::iterator_category,
                                 std::input_iterator_tag>)
    {
      size_type i;
      for (i = 0; first != last && i < size_; ++i, ++first)
        storage()[i] = *first;

      if (i != size_)
        resize(i);

      while (first != last)
        emplace_back(*first++);
    }
    else
    {
      const size_type count = std::distance(first, last);

      if (cap_ < count)
      {
        destroy_and_dealloc(storage(), size_, cap_, stored_allocator());
        storage() = alloc_traits::allocate(stored_allocator(), count);
        cap_ = count;
        size_ = 0;
      }

      const size_type common = (std::min)(size_, count);

      // CopyAssignable
      for (size_type i = 0; i != common; ++i)
        storage()[i] = *first++;

      // Erasable
      if (size_ > common)
      {
        destroy(storage() + common, size_ - common, stored_allocator());
        size_ = common;
      }

      // CopyInsertible
      if (size_ < count)
      {
        construct(storage() + size_, count - size_, first + size_,
                  stored_allocator());
        size_ = count;
      }
    }
  }

  void assign(size_type num, const_reference val)
  {
    if (cap_ < num)
    {
      destroy_and_dealloc(storage(), size_, cap_, stored_allocator());
      storage() = alloc_traits::allocate(stored_allocator(), num, storage());
      cap_ = num;
      size_ = 0;
    }

    const size_type common = (std::min)(size_, num);

    // CopyAssignable
    for (size_type i = 0; i != common; ++i)
      storage()[i] = val;

    // Erasable
    if (size_ > common)
    {
      destroy(storage() + common, size_ - common, stored_allocator());
      size_ = common;
    }

    // CopyInsertible
    if (size_ < num)
    {
      construct(storage() + size_, num - size_, forward_args(val),
                stored_allocator());
      size_ = num;
    }
  }

  void assign(std::initializer_list<value_type> il)
  {
    assign(il.begin(), il.end());
  }

  allocator_type get_allocator() const noexcept { return stored_allocator(); }

  // iterators:
  iterator begin() noexcept { return iterator{storage()}; }
  const_iterator begin() const noexcept { return const_iterator{storage()}; }
  iterator end() noexcept { return iterator{storage() + size_}; }
  const_iterator end() const noexcept
  {
    return const_iterator{storage() + size_};
  }
  reverse_iterator rbegin() noexcept
  {
    return std::make_reverse_iterator(end());
  }
  const_reverse_iterator rbegin() const noexcept
  {
    return std::make_reverse_iterator(end());
  }
  reverse_iterator rend() noexcept
  {
    return std::make_reverse_iterator(begin());
  }
  const_reverse_iterator rend() const noexcept
  {
    return std::make_reverse_iterator(begin());
  }
  const_iterator cbegin() const noexcept { return begin(); }
  const_iterator cend() const noexcept { return end(); }
  const_reverse_iterator crbegin() const noexcept { return rbegin(); }
  const_reverse_iterator crend() const noexcept { return rend(); }

  // 26.3.11.3, capacity
  bool empty() const noexcept { return size_ == 0; }
  size_type size() const noexcept { return size_; }
  size_type max_size() const noexcept
  {
    return alloc_traits::max_size(stored_allocator());
  }

  size_type capacity() const noexcept { return cap_; }

  void resize(size_type size) { resize_impl(size, std::tuple<>()); }

  void resize(size_type size, const_reference val)
  {
    resize_impl(size, forward_args(val));
  }

  template <typename Arg>
  void resize_impl(size_type size, Arg &&arg)
  {
    reserve(size);
    if (size_ < size)
      construct(storage() + size_, size - size_, std::forward<Arg>(arg),
                stored_allocator());
    if (size_ > size)
      destroy(storage() + size, size_ - size, stored_allocator());
    size_ = size;
  }

  void reserve(size_type num)
  {
    if (num > cap_)
    {
      auto new_cap = (std::max)(cap_ * 2, num);
      realloc(storage(), size_, cap_, new_cap, stored_allocator());
    }
  }

  void shrink_to_fit()
  {
    if (cap_ != size_)
    {
      realloc(storage(), size_, cap_, size_, stored_allocator());
    }
  }

  // element access:
  reference operator[](size_type num) { return storage()[num]; }
  const_reference operator[](size_type num) const { return storage()[num]; }
  const_reference at(size_type num) const
  {
    if (num < size_)
      return storage()[num];
    GPCL_THROW(std::out_of_range("vector::at"));
  }
  reference at(size_type num)
  {
    if (num < size_)
      return storage()[num];
    GPCL_THROW(std::out_of_range("vector::at"));
  }
  reference front() { return storage()[0]; }
  const_reference front() const { return storage()[0]; }
  reference back() { return storage()[size_ - 1]; }
  const_reference back() const { return storage()[size_ - 1]; }

  // 26.3.11.4, data access
  pointer data() noexcept { return storage(); }
  const_pointer data() const noexcept { return storage(); }

  // 26.3.11.5, modifiers
  template <typename... Args>
  reference emplace_back(Args &&...args)
  {
    reserve(size_ + 1);
    alloc_traits::construct(stored_allocator(), &storage()[size_],
                            std::forward<Args>(args)...);
    ++size_;
    return back();
  }

  void push_back(const_reference elem)
  {
    reserve(size_ + 1);
    alloc_traits::construct(stored_allocator(), &storage()[size_], elem);
    ++size_;
  }

  void push_back(rvalue_reference elem)
  {
    reserve(size_ + 1);
    alloc_traits::construct(stored_allocator(), &storage()[size_],
                            std::move(elem));
    ++size_;
  }

  void pop_back() // noexcept
  {
    alloc_traits::destroy(stored_allocator(), &storage()[--size_]);
  }

  template <typename... Args>
  iterator emplace(const_iterator position, Args &&...args)
  {
    auto ptr = insert_impl(position - begin(), 1,
                           forward_args(std::forward<Args>(args)...));
    return iterator{ptr};
  }

  template <typename Arg>
  pointer insert_impl(size_type idx, size_type count, Arg &&arg)
  {
    if (size_ + count > cap_)
    {
      const auto new_cap = (std::max)(size_ + count, size_ * 2);
      const pointer new_data = alloc_and_construct(
          idx, new_cap, detail::make_move_if_noexcept_iterator(storage()),
          stored_allocator(), storage());

      GPCL_TRY
      {
        construct(new_data + idx, count, std::forward<Arg>(arg),
                  stored_allocator());
      }
      GPCL_CATCH(...)
      {
        destroy_and_dealloc(new_data, idx, new_cap, stored_allocator());
        GPCL_RETHROW;
      }
      GPCL_CATCH_END

      GPCL_TRY
      {
        construct(new_data + idx + count, size_ - idx,
                  detail::make_move_if_noexcept_iterator(storage() + idx),
                  stored_allocator());
      }
      GPCL_CATCH(...)
      {
        destroy_and_dealloc(new_data, idx + count, new_cap, stored_allocator());
        GPCL_RETHROW;
      }
      GPCL_CATCH_END

      destroy_and_dealloc(storage(), size_, cap_, stored_allocator());

      storage() = new_data;
      cap_ = new_cap;
      size_ = size_ + count;
      return storage() + idx;
    }
    else
    {
      const size_type num = (std::min)(size_ - idx, count);
      const auto size = size_;
      construct(storage() + size + count - num, num,
                detail::make_move_if_noexcept_iterator(storage() + size - num),
                stored_allocator());
      size_ += count;
      std::copy_backward(
          detail::make_move_if_noexcept_iterator(storage() + idx),
          detail::make_move_if_noexcept_iterator(storage() + size - num),
          storage() + size + count - num);
      destroy(storage() + idx, num,
              stored_allocator()); // destruction might be unneeded
      GPCL_TRY
      {
        construct(storage() + idx, count, std::forward<Arg>(arg),
                  stored_allocator());
      }
      GPCL_CATCH(...)
      {
        std::copy(
            detail::make_move_if_noexcept_iterator(storage() + size - num),
            detail::make_move_if_noexcept_iterator(storage() + idx),
            storage() + size + count - num);
        destroy(storage() + size + count - num, num, stored_allocator());
        size_ -= count;
        GPCL_RETHROW;
      }
      GPCL_CATCH_END
      return storage() + idx;
    }
  }

  iterator insert(const_iterator position, const_reference elem)
  {
    auto *ptr = insert_impl(position - begin(), 1, forward_args(elem));
    return iterator{ptr};
  }

  iterator insert(const_iterator position, rvalue_reference elem)
  {
    auto *ptr =
        insert_impl(position - begin(), 1, forward_args(std::move(elem)));
    return iterator{ptr};
  }

  iterator insert(const_iterator position, size_type num, const_reference elem)
  {
    auto *ptr = insert_impl(position - begin(), num, forward_args(elem));
    return iterator{ptr};
  }

  template <typename InputIterator>
  iterator insert(const_iterator position, InputIterator first,
                  InputIterator last)
  {
    if constexpr (std::is_same_v<std::input_iterator_tag,
                                 typename std::iterator_traits<
                                     InputIterator>::iterator_category>)
    {
      auto idx = position - begin();
      const auto idx_origin = idx;
      while (first != last)
        insert_impl(idx++, 1, forward_args(*first++));
      return iterator{storage() + idx_origin};
    }
    else
    {
      const size_type num = std::distance(first, last);
      auto *const ptr = insert_impl(position - begin(), num, first);
      return iterator{ptr};
    }
  }

  iterator insert(const_iterator position, std::initializer_list<value_type> il)
  {
    auto *const ptr = insert_impl(position - begin(), il.size(), il.begin());
    return iterator{ptr};
  }

  iterator erase(const_iterator position)
  {
    return erase(position, position + 1);
  }

  iterator erase(const_iterator first, const_iterator last)
  {
    const size_type num = last - first;
    std::copy(detail::make_move_if_noexcept_iterator(last.data()),
              detail::make_move_if_noexcept_iterator(cend().data()),
              first.data());
    destroy(storage() + size_ - num, num, stored_allocator());
    size_ -= num;
    return iterator(first.data());
  }

  void swap(vector &other) noexcept(
      alloc_traits::propagate_on_container_swap::value ||
      alloc_traits::is_always_equal::value)
  {
    using gpcl::swap;
    if constexpr (alloc_traits::propagate_on_container_swap::value ||
                  alloc_traits::is_always_equal::value)
    {
      swap(stored_allocator(), other.stored_allocator());
      swap(storage(), other.storage());
      swap(size_, other.size_);
      swap(cap_, other.cap_);
    }
    else if (stored_allocator() == other.get_allocator())
    {
      swap(storage(), other.storage());
      swap(size_, other.size_);
      swap(cap_, other.cap_);
    }
    else if (size_ != other.size_)
    {
      auto [t_more, t_less] =
          other.size_ > size_ ? std::tie(other, *this) : std::tie(*this, other);
      auto *const new_data = alloc_and_construct(
          t_more.size_, detail::move_if_noexcept_iterator(t_more.storage()),
          t_less.stored_allocator(), t_more.storage());

      GPCL_TRY
      {
        for (size_type i = 0; i != t_less.size_; ++i)
          t_more.storage()[i] = std::move_if_noexcept(t_less.storage()[i]);
      }
      GPCL_CATCH(...)
      {
        destroy_and_dealloc(new_data, t_more.size_, t_more.size_,
                            t_less.stored_allocator());
        GPCL_RETHROW;
      }
      GPCL_CATCH_END

      destroy_and_dealloc(t_less.storage(), t_less.size_, t_less.cap_,
                          t_less.stored_allocator());

      t_less.storage() = new_data;
      destroy(t_more.storage() + t_less.size_, t_more.size_ - t_less.size_,
              t_more.stored_allocator());
      swap(t_less.size_, t_more.size_);
      swap(t_less.cap_, t_more.cap_);
    }
    else
    {
      for (size_type i{0}; i != size_; ++i)
      {
        swap(storage()[i], other.storage()[i]);
      }
    }
  }

  void clear() noexcept
  {
    destroy(storage(), size_, stored_allocator());
    size_ = 0;
  }

private:
  allocator_type &stored_allocator() noexcept { return p_.first(); }
  const allocator_type &stored_allocator() const noexcept { return p_.first(); }

  pointer &storage() noexcept { return p_.second(); }
  const pointer &storage() const noexcept { return p_.second(); }

  detail::compressed_pair<allocator_type, pointer> p_;
  size_type cap_;
  size_type size_;
};

template <typename InputIterator,
          typename Allocator = gpcl::default_allocator<
              typename std::iterator_traits<InputIterator>::value_type>>
vector(InputIterator, InputIterator, Allocator = Allocator())
    -> vector<typename std::iterator_traits<InputIterator>::value_type,
              Allocator>;

template <typename Tp, typename Allocator>
inline bool operator==(const vector<Tp, Allocator> &x,
                       const vector<Tp, Allocator> &y)
{
  if (x.size() != y.size())
    return false;
  for (auto i = x.begin(), j = y.begin(), end = x.end(); i != end; ++i, ++j)
    if (*i != *j)
      return false;
  return true;
}

template <typename Tp, typename Allocator>
bool operator<(const vector<Tp, Allocator> &x, const vector<Tp, Allocator> &y)
{
  typename vector<Tp, Allocator>::size_type i;
  for (i = 0; i < x.size() && i < y.size(); ++i)
  {
    if (x[i] < y[i])
      return true;
    if (x[i] > y[i])
      return false;
  }
  return x.size() < y.size();
}

template <typename Tp, typename Allocator>
bool operator!=(const vector<Tp, Allocator> &x, const vector<Tp, Allocator> &y)
{
  return !(x == y);
}

template <typename Tp, typename Allocator>
bool operator>(const vector<Tp, Allocator> &x, const vector<Tp, Allocator> &y)
{
  return !(x < y) && !(x == y);
}

template <typename Tp, typename Allocator>
bool operator>=(const vector<Tp, Allocator> &x, const vector<Tp, Allocator> &y)
{
  return !(x < y);
}

template <typename Tp, typename Allocator>
bool operator<=(const vector<Tp, Allocator> &x, const vector<Tp, Allocator> &y)
{
  return !(y < x);
}

/// @name specialized algorithms
/// @relates vector
template <typename Tp, typename Allocator>
void swap(vector<Tp, Allocator> &x,
          vector<Tp, Allocator> &y) noexcept(noexcept(x.swap(y)))
{
  x.swap(y);
}

} // namespace gpcl

#endif // GPCL_VECTOR_HPP
