//
// intrusive_list.hpp
// ~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_INTRUSIVE_LIST_HPP
#define GPCL_INTRUSIVE_LIST_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/detail/type_traits.hpp>
#include <gpcl/swap.hpp>

namespace gpcl {

template <typename T, typename Tag = class default_tag>
class intrusive_list;

template <typename T, typename Tag>
class intrusive_list_iterator;

class intrusive_list_node_base
{
public:
  constexpr intrusive_list_node_base() noexcept : prev(this), next(this) {}

  intrusive_list_node_base(const intrusive_list_node_base &) = delete;

  intrusive_list_node_base &
  operator=(const intrusive_list_node_base &) = delete;

  void init() noexcept
  {
    prev = this;
    next = this;
  }

  bool in_list() const noexcept { return !(prev == this && next == this); }

  ~intrusive_list_node_base() noexcept { GPCL_ASSERT(!in_list()); }

  void swap(intrusive_list_node_base &other) noexcept
  {
    using gpcl::swap;
    swap(prev, other.prev);
    swap(next, other.next);
  }

  intrusive_list_node_base *prev;
  intrusive_list_node_base *next;
};

inline void swap(intrusive_list_node_base &x,
                 intrusive_list_node_base &y) noexcept
{
  x.swap(y);
}

inline void insert_between(intrusive_list_node_base *node,
                           intrusive_list_node_base *prev,
                           intrusive_list_node_base *next) noexcept
{
  next->prev = node;
  node->next = next;
  node->prev = prev;
  prev->next = node;
}

inline void delete_between(intrusive_list_node_base *prev,
                           intrusive_list_node_base *next) noexcept
{
  next->prev = prev;
  prev->next = next;
}

inline void delete_entry(intrusive_list_node_base *entry) noexcept
{
  delete_between(entry->prev, entry->next);
}

inline void replace_entry(intrusive_list_node_base *old,
                          intrusive_list_node_base *node) noexcept
{
  node->next = old->next;
  node->next->prev = node;
  node->prev = old->prev;
  node->prev->next = node;

  old->init();
}

template <typename T, typename Tag = class default_tag>
class intrusive_list_node : public intrusive_list_node_base
{
  friend class intrusive_list<T, Tag>;

  friend class intrusive_list_iterator<T, Tag>;
};

template <typename T, typename Tag>
class intrusive_list_iterator
{
public:
  using value_type = T;
  using reference = T &;
  using pointer = T *;
  using difference_type = std::ptrdiff_t;
  using iterator_category = std::bidirectional_iterator_tag;
  using node_type =
      intrusive_list_node<typename std::remove_const<T>::type, Tag>;

  constexpr intrusive_list_iterator() noexcept : data_() {}

  explicit constexpr intrusive_list_iterator(node_type *p) noexcept : data_(p)
  {
  }

  intrusive_list_iterator(const intrusive_list_iterator &) = default;

  intrusive_list_iterator &operator=(const intrusive_list_iterator &) = default;

  reference operator*() const { return static_cast<reference>(*data_); }

  pointer operator->() const { return static_cast<pointer>(data_); }

  intrusive_list_iterator &operator++()
  {
    data_ = static_cast<node_type *>(data_->next);
    return *this;
  }

  intrusive_list_iterator &operator--()
  {
    data_ = static_cast<node_type *>(data_->prev);
    return *this;
  }

  intrusive_list_iterator operator++(int)
  {
    auto ret = *this;
    ++*this;
    return ret;
  }

  intrusive_list_iterator operator--(int)
  {
    auto ret = *this;
    --*this;
    return ret;
  }

  friend bool operator==(const intrusive_list_iterator<T, Tag> &x,
                         const intrusive_list_iterator<T, Tag> &y) noexcept
  {
    return x.data_ == y.data_;
  }

  friend bool operator!=(const intrusive_list_iterator<T, Tag> &x,
                         const intrusive_list_iterator<T, Tag> &y) noexcept
  {
    return x.data_ != y.data_;
  }

private:
  node_type *data_;
};

template <typename T, typename Tag>
class intrusive_list
{
public:
  using value_type = std::decay_t<T>;
  using reference = T &;
  using node_type = intrusive_list_node<T, Tag>;
  using pointer = T *;
  using iterator = intrusive_list_iterator<T, Tag>;
  using const_iterator = intrusive_list_iterator<std::add_const_t<T>, Tag>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  constexpr intrusive_list() noexcept = default;

  intrusive_list(intrusive_list &&other) noexcept
  {
    if (!other.empty())
      replace_entry(&other.head_, &head_);
  }

  intrusive_list(const intrusive_list &&) = delete;

  ~intrusive_list() noexcept { clear(); }

  intrusive_list &operator=(intrusive_list &&other)
  {
    if (this == &other)
    {
      return *this;
    }
    this->clear();
    if (!other.empty())
    {
      replace_entry(&other.head_, &head_);
    }
    return *this;
  }

  iterator begin() noexcept
  {
    return iterator{static_cast<node_type *>(head_.next)};
  }

  iterator end() noexcept { return iterator{static_cast<node_type *>(&head_)}; }

  const_iterator begin() const noexcept
  {
    return const_iterator{static_cast<node_type *>(head_.next)};
  }

  const_iterator end() const noexcept
  {
    return const_iterator{static_cast<node_type *>(&head_)};
  }

  const_iterator cbegin() const noexcept { return begin(); }

  const_iterator cend() const noexcept { return end(); }

  reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }

  reverse_iterator rend() noexcept { return reverse_iterator(begin()); }

  const_reverse_iterator rbegin() const noexcept
  {
    return const_reverse_iterator(end());
  }

  const_reverse_iterator rend() const noexcept
  {
    return const_reverse_iterator(end());
  }

  const_reverse_iterator crbegin() const noexcept { return rbegin(); }

  const_reverse_iterator crend() const noexcept { return rend(); }

  /// Indicates whether the list is empty.
  /// Complexity: constant.
  bool empty() const noexcept { return head_.next == &head_; }

  void push_front(reference node) noexcept
  {
    delete_entry(static_cast<node_type *>(&node));
    insert_between(static_cast<node_type *>(&node), &head_, head_.next);
  }

  void push_back(reference node) noexcept
  {
    delete_entry(static_cast<node_type *>(&node));
    insert_between(static_cast<node_type *>(&node), head_.prev, &head_);
  }

  void pop_front() noexcept
  {
    GPCL_ASSERT(!empty());
    erase(front());
  }

  void pop_back() noexcept
  {
    GPCL_ASSERT(!empty());
    erase(back());
  }

  void replace(reference old, reference new_)
  {
    replace_entry(static_cast<node_type *>(&old),
                  static_cast<node_type *>(&new_));
  }

  void replace(iterator old, reference new_) { replace_entry(*old, new_); }

  void erase(reference node) noexcept
  {
    delete_entry(static_cast<node_type *>(&node));
    node.node_type::init();
  }

  void erase(iterator pos) { erase(*pos); }

  reference back() const noexcept
  {
    GPCL_ASSERT(!empty());
    return *pointer(static_cast<node_type *>(head_.prev));
  }

  reference front() const noexcept
  {
    GPCL_ASSERT(!empty());
    return *pointer(static_cast<node_type *>(head_.next));
  }

  /// Tests whether the list has exactly one elements.
  /// Complexity: constant.
  bool is_singular() const noexcept
  {
    return !empty() && (head_.next == head_.prev);
  }

  void clear() noexcept
  {
    while (!empty())
    {
      pop_back();
    }
  }

  void reverse() noexcept
  {
    // todo: simplify the code

    intrusive_list stack;
    while (!empty())
    {
      auto &entry = front();
      erase(entry);
      stack.push_back(entry);
    }

    while (!stack.empty())
    {
      auto &entry = stack.back();
      stack.erase(entry);
      push_back(entry);
    }
  }

  void exchange(reference x, reference y) noexcept
  {
    swap(static_cast<node_type &>(x), static_cast<node_type &>(y));
  }

  void exchange(iterator x, iterator y) noexcept { exchange(*x, *y); }

private:
  // Use next as the head and prev as the tail.
  mutable intrusive_list_node<T, Tag> head_;
};

// template <typename T, typename Tag>
// void swap(intrusive_list<T, Tag> &x, intrusive_list<T, Tag> &y) noexcept
// {
//   x.swap(y);
// }

} // namespace gpcl

#endif // GPCL_INTRUSIVE_LIST_HPP
