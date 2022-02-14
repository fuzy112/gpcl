//
// list.hpp
// ~~~~~~~~
//
// Copyright (c) 2022 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_LIST_HPP
#define GPCL_LIST_HPP

#include <gpcl/detail/config.hpp>

#include <gpcl/assert.hpp>
#include <gpcl/default_allocator.hpp>
#include <gpcl/detail/compressed_pair.hpp>
#include <gpcl/error.hpp>
#include <gpcl/noncopyable.hpp>
#include <gpcl/propagate_const.hpp>
#include <gpcl/swap.hpp>
#include <gpcl/type_identity.hpp>

#include <initializer_list>
#ifndef GPCL_CONFIG_NO_IOSTEAMS
# include <iostream>
#endif

namespace gpcl {

template <typename T, typename Allocator = default_allocator<T>>
class list;

template <typename T>
struct list_node : noncopyable
{
  propagate_const<list_node *> prev_ = this;

  propagate_const<list_node *> next_ = this;

  union
  {
    char dummy_;
    T val_;
  };

  constexpr list_node() noexcept : dummy_() {}

  template <typename... Args>
  explicit list_node(list_node *prev, list_node *next, Args &&...args)
      : prev_(prev),
        next_(next),
        val_(std::forward<Args>(args)...)
  {
  }

  ~list_node() noexcept {}

  void destroy() noexcept
  {
    val_.~T();
    dummy_ = '\0';
  }

  void swap(list_node &other) noexcept
  {
    if (this == &other)
      return;

    list_node *prev = prev_;
    list_node *next = next_;

    list_node *other_prev = other.prev_;
    list_node *other_next = other.next_;

    this->prev_ = other_prev;
    this->next_ = other_next;
    prev->next_ = &other;
    next->prev_ = &other;

    other_prev->next_ = this;
    other_next->prev_ = this;
    other.prev_ = prev;
    other.next_ = next;
  }
};

template <typename T>
void swap(list_node<T> &x, list_node<T> &y) noexcept
{
  x.swap(y);
}

template <typename T, typename E>
class list_iterator
{
public:
  using iterator_category = std::bidirectional_iterator_tag;
  using size_type = std::size_t;
  using value_type = T;
  using reference = E &;
  using const_reference = const E &;
  using difference_type = std::ptrdiff_t;
  using pointer = E *;
  using const_pointer = const E *;

private:
  using _node_type = list_node<T>;
  using list_node_type =
      typename std::conditional<std::is_const<E>::value, _node_type const,
                                _node_type>::type;

  list_node_type *node_{};

public:
  constexpr list_iterator() = default;

  constexpr explicit list_iterator(list_node_type *node) noexcept : node_(node)
  {
  }

  template <typename U,
            typename std::enable_if<std::is_convertible<U *, E *>::value,
                                    int>::type = 0>
  constexpr list_iterator(list_iterator<T, U> other) noexcept
      : node_(other.get_node())
  {
  }

  list_node_type *get_node() const noexcept { return node_; }

  reference operator*() const noexcept { return node_->val_; }

  pointer operator->() const noexcept { return std::addressof(node_->val_); }

  list_iterator &operator++() noexcept
  {
    node_ = node_->next_;
    return *this;
  }

  list_iterator operator++(int) noexcept
  {
    auto r = *this;
    ++*this;
    return r;
  }

  list_iterator &operator--() noexcept
  {
    node_ = node_->prev_;
    return *this;
  }

  list_iterator &operator--(int) noexcept
  {
    auto r = *this;
    --*this;
    return *this;
  }

  template <typename E1>
  bool operator==(const list_iterator<T, E1> &other) const noexcept
  {
    return other.node_ == node_;
  }

  template <typename E1>
  bool operator!=(const list_iterator<T, E1> &other) const noexcept
  {
    return other.node_ != node_;
  }
};

template <typename T, typename Allocator>
class list
{
public:
  using allocator_type = Allocator;

  using size_type = std::size_t;
  using value_type = T;
  using reference = T &;
  using const_reference = const T &;
  using difference_type = std::ptrdiff_t;
  using pointer = T *;
  using const_pointer = const T *;
  using iterator = list_iterator<T, T>;
  using const_iterator = list_iterator<T, const T>;

private:
  using node_type = list_node<T>;

  using node_allocator = typename std::allocator_traits<
      Allocator>::template rebind_alloc<node_type>;

  detail::compressed_pair<node_allocator, node_type> p_;
  size_type size_ = 0;

public:
  constexpr list() = default;

  constexpr explicit list(const type_identity_t<Allocator> &a) noexcept : p_(a)
  {
  }

  list(size_type count, const T &value, const Allocator &a = Allocator())
      : list(a)
  {
    resize(count, value);
  }

  explicit list(size_type count, const Allocator &a = Allocator()) : list(a)
  {
    resize(count);
  }

  template <typename InputIt,
            typename std::enable_if<
                std::is_convertible<
                    typename std::iterator_traits<InputIt>::iterator_category *,
                    std::input_iterator_tag *>::value,
                int>::type = 0>
  list(InputIt first, InputIt last,
       const type_identity_t<Allocator> &a = Allocator())
      : list(a)
  {
    insert(end(), first, last);
  }

  constexpr list(std::initializer_list<T> il,
                 const type_identity_t<Allocator> &a = Allocator())
      : list(il.begin(), il.end(), a)
  {
  }

  list(list &&other) noexcept : list(other.get_allocator())
  {
    splice(end(), std::move(other));
  }

  list(list &&other, const type_identity_t<Allocator> &a) : list(a)
  {
    if (a != other.get_allocator())
    {
      for (auto &e : other)
      {
        push_back(std::move(e));
      }
    }
    else
    {
      splice(end(), std::move(other));
    }
  }

  list(const list &other)
      : list(other,
             std::allocator_traits<allocator_type>::
                 select_on_container_copy_construction(other.get_allocator()))
  {
  }

  list(const list &other, const type_identity_t<Allocator> &a) : list(a)
  {
    insert(end(), other.begin(), other.end());
  }

  ~list() { clear(); }

  list &operator=(const list &other)
  {
    if (std::allocator_traits<
            Allocator>::propagate_on_container_copy_assignment::value)
    {
      if (p_.first() != other.p_.first())
      {
        auto first = begin();
        auto last = end();
        splice(first, list(other, other.get_allocator()));
        erase(first, last);
        p_.first() = other.p_.first();
        return *this;
      }
      p_.first() = other.p_.first();
    }

    if (size() < other.size())
    {
      auto first = begin();
      auto last = end();
      auto other_first = other.begin();
      while (first != last)
      {
        *first = *other_first;
        ++first;
        ++other_first;
      }

      insert(last, other_first, other.end());
    }
    else
    {
      auto first = begin();
      auto other_first = other.begin();
      auto other_last = other.end();
      while (other_first != other_last)
      {
        *first = *other_first;
        ++first;
        ++other_first;
      }
      erase(first, end());
    }

    return *this;
  }

  list &operator=(list &&other) noexcept(
      std::allocator_traits<Allocator>::is_always_equal::value)
  {
    if (std::allocator_traits<
            Allocator>::propagate_on_container_move_assignment::value)
    {
      clear();
      p_.first() = other.p_.first();
    }

    if (get_allocator() == other.get_allocator())
    {
      clear();
      splice(end(), std::move(other));
    }
    else
    {
      if (size() < other.size())
      {
        auto first = begin();
        auto last = end();
        auto other_first = other.begin();
        while (first != last)
        {
          *first = std::move(*other_first);
          ++first;
          ++other_first;
        }

        auto other_last = other.end();
        while (other_first != other_last)
        {
          push_back(std::move(*other_first));
          ++other_first;
        }
      }
      else
      {
        auto first = begin();
        auto other_first = other.begin();
        auto other_last = other.end();
        while (other_first != other_last)
        {
          *first = std::move(*other_first);
          ++first;
          ++other_first;
        }
        erase(first, end());
      }
    }

    return *this;
  }

  allocator_type get_allocator() const noexcept { return p_.first(); }

  iterator begin() noexcept { return iterator(p_.second().next_); }
  const_iterator begin() const noexcept
  {
    return const_iterator(p_.second().next_);
  }
  const_iterator cbegin() const noexcept { return begin(); }

  iterator end() noexcept { return iterator(&p_.second()); }
  const_iterator end() const noexcept { return const_iterator(&p_.second()); }
  const_iterator cend() const noexcept { return end(); }

  reference front() noexcept { return *begin(); }
  const_reference front() const noexcept { return *begin(); }

  reference back() noexcept { return *std::prev(end()); }
  const_reference back() const noexcept { return *std::prev(end()); }

  bool empty() const noexcept { return begin() == end(); }

  size_type size() const noexcept
  {
    return size_;
  }

private:
  node_allocator &get_alloc() noexcept { return p_.first(); }

  template <typename... Args>
  node_type *create_node(node_type *prev, node_type *next, Args &&...args)
  {
    node_type *p =
        std::allocator_traits<node_allocator>::allocate(get_alloc(), 1);
    GPCL_TRY
    {
      std::allocator_traits<node_allocator>::construct(
          get_alloc(), p, prev, next, std::forward<Args>(args)...);
      return p;
    }
    GPCL_CATCH(...)
    {
      std::allocator_traits<node_allocator>::deallocate(get_alloc(), p, 1);
      GPCL_RETHROW;
    }
    GPCL_CATCH_END
  }

  void destroy_node(node_type *node) noexcept
  {
    node->destroy();
    std::allocator_traits<node_allocator>::destroy(get_alloc(), node);
    std::allocator_traits<node_allocator>::deallocate(get_alloc(), node, 1);
  }

public:
  template <typename... Args>
  reference emplace_front(Args &&...args)
  {
    return *emplace(begin(), std::forward<Args>(args)...);
  }

  void push_front(const T &value) { emplace_front(value); }

  void push_front(T &&value) { emplace_front(value); }

  void pop_front() noexcept { erase(begin()); }

  template <typename... Args>
  reference emplace_back(Args &&...args)
  {
    return *emplace(end(), std::forward<Args>(args)...);
  }

  void push_back(const T &value) { emplace_back(value); }

  void push_back(T &&value) { emplace_back(value); }

  void pop_back() noexcept { erase(std::prev(end())); }

  template <typename... Args>
  iterator emplace(const_iterator pos, Args &&...args)
  {
    auto next = const_cast<node_type *>(pos.get_node());
    auto node = create_node(next->prev_, next, std::forward<Args>(args)...);
    next->prev_->next_ = node;
    next->prev_ = node;
    ++size_;
    return iterator(node);
  }

  iterator insert(const_iterator pos, const T &value)
  {
    return emplace(pos, value);
  }

  iterator insert(const_iterator pos, T &&value)
  {
    return emplace(pos, std::move(value));
  }

  iterator insert(const_iterator pos, size_type count, const T &value)
  {
    size_type n = 0;
    GPCL_TRY
    {
      while (n < count)
      {
        pos = insert(pos, value);
        ++n;
      }
      return pos;
    }
    GPCL_CATCH(...)
    {
      while (n > 0)
      {
        pos = erase(pos);
        --n;
      }
      GPCL_RETHROW;
    }
    GPCL_CATCH_END
  }

  template <typename InputIt,
            typename std::enable_if<
                std::is_convertible<
                    typename std::iterator_traits<InputIt>::iterator_category,
                    std::input_iterator_tag>::value,
                int>::type = 0>
  iterator insert(const_iterator pos, InputIt first, InputIt last)
  {
    size_type n = 0;
    GPCL_TRY
    {
      while (first != last)
      {
        pos = insert(pos, *std::prev(last));
        --last;
        ++n;
      }
      return iterator(const_cast<node_type *>(pos.get_node()));
    }
    GPCL_CATCH(...)
    {
      while (n > 0)
      {
        pos = erase(pos);
        --n;
      }
      GPCL_RETHROW;
    }
    GPCL_CATCH_END
  }

  iterator insert(const_iterator pos, std::initializer_list<T> il)
  {
    return insert(pos, il.begin(), il.end());
  }

  void resize(size_type count)
  {
    if (count > size_)
    {
      while (count > size_)
        emplace_back();
    }
    else
    {
      while (size_ > count)
        pop_back();
    }

    GPCL_ASSERT(count == size_);
  }

  void resize(size_type count, const T &value)
  {
    if (count > size_)
    {
      insert(end(), count - size_, value);
    }
    else
    {
      while (size_ > count)
        pop_back();
    }
    GPCL_ASSERT(count == size_);
  }

  iterator erase(const_iterator pos) noexcept
  {
    auto node = const_cast<node_type *>(pos.get_node());
    node_type *prev = node->prev_;
    node_type *next = node->next_;
    destroy_node(node);
    --size_;
    prev->next_ = next;
    next->prev_ = prev;
    return iterator(next);
  }

  iterator erase(const_iterator first, const_iterator last) noexcept
  {
    auto node = const_cast<node_type *>(first.get_node());
    auto next = const_cast<node_type *>(last.get_node());
    node_type *prev = node->prev_;

    while (node != next)
    {
      node_type *tmp = node->next_;
      destroy_node(node);
      node = tmp;
      --size_;
    }
    prev->next_ = next;
    next->prev_ = prev;
    return iterator(next);
  }

  void clear() noexcept { erase(begin(), end()); }

  /// @todo implement swap.
  void swap(list &other) noexcept(
      std::allocator_traits<Allocator>::is_always_equal::value)
  {
    if (std::allocator_traits<Allocator>::propagate_on_container_swap::value)
    {
      using gpcl::swap;
      swap(p_, other.p_);
      swap(size_, other.size_);
    }
    else
    {
      GPCL_ASSERT(get_allocator() == other.get_allocator());
      using gpcl::swap;
      swap(p_.second(), other.p_.second());
      swap(size_, other.size_);
    }
  }

  void splice(const_iterator pos, list &&other) noexcept
  {
    splice(pos, std::move(other), other.begin(), other.end());
  }

  void splice(const_iterator pos, list &&other, const_iterator it) noexcept
  {
    splice(pos, std::move(other), it, std::next(it));
  }

  void splice(const_iterator pos, list &&other, const_iterator first,
              const_iterator last) noexcept
  {
    GPCL_ASSERT(get_allocator() == other.get_allocator());

    if (this != &other)
    {
      size_type count = std::distance(first, last);
      other.size_ -= count;
      size_ += count;
    }

    auto node = const_cast<node_type *>(first.get_node());
    auto next = const_cast<node_type *>(last.get_node());
    node_type *prev = node->prev_;
    node_type *node_last = next->prev_;

    prev->next_ = next;
    next->prev_ = prev;

    auto pos_node = const_cast<node_type *>(pos.get_node());
    node_type *pos_prev_node = pos_node->prev_;
    node->prev_ = pos_prev_node;
    node_last->next_ = pos_node;

    pos_prev_node->next_ = node;
    pos_node->prev_ = node_last;
  }

  void sort()
  {
    auto i = cbegin();
    auto last = cend();

    while (i != last)
    {
      auto j = std::next(i);
      while (j != last)
      {
        if (*j < *i)
        {
          const auto tmp = j;
          splice(i, std::move(*this), j++);
          i = tmp;
          continue;
        }
        ++j;
      }

      ++i;
    }
  }
};

template <typename T, typename Allocator>
void swap(list<T, Allocator> &x,
          list<T, Allocator> &y) noexcept(noexcept(x.swap(y)))
{
  x.swap(y);
}

template <typename T, typename Allocator>
bool operator==(const list<T, Allocator> &x,
                const list<T, Allocator> &y) noexcept
{
  if (x.size() != y.size())
    return false;
  return std::equal(x.begin(), x.end(), y.begin());
}

template <typename T, typename Allocator>
bool operator!=(const list<T, Allocator> &x,
                const list<T, Allocator> &y) noexcept
{
  return !(x == y);
}

template <typename T, typename Allocator>
bool operator<(const list<T, Allocator> &x,
               const list<T, Allocator> &y) noexcept
{
  return std::lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
}

#ifndef GPCL_CONFIG_NO_IOSTEAMS
template <typename CharT, typename Traits, typename T, typename Allocator>
std::basic_ostream<CharT, Traits>&
operator<<(std::basic_ostream<CharT, Traits> &os, const list<T, Allocator> &rhs)
{
  typename std::basic_ostream<CharT, Traits>::sentry valid(os);
  if (!valid)
    return os;
  os << CharT('[');
  bool need_comma = false;
  for (auto &&value : rhs)
  {
    if (need_comma)
      os << CharT(',') << CharT(' ');
    else
      need_comma = true;
    os << value;
  }
  os << CharT(']');
  return os;
}
#endif

} // namespace gpcl

#endif // GPCL_LIST_HPP
