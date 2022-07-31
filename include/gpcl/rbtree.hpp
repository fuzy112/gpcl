//
// rbtree.hpp
// ~~~~~~~~~~
//
// Copyright (c) 2022 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_RBTREE_HPP
#define GPCL_RBTREE_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/invoke.hpp>
#include <gpcl/meta.hpp>
#include <gpcl/options.hpp>
#include <gpcl/rbtree_algorithms.hpp>

namespace gpcl {

struct default_tag;

template <typename Hook>
struct rbtree_hook_traits;

template <typename T, typename... Options>
class rbtree_base_hook;

class default_tag;

template <typename T, typename... Options>
class rbtree_base_hook
{
  friend rbtree_hook_traits<rbtree_base_hook>;

private:
  using option_list = meta::list<Options...>;
  using value_type = T;
  using reference = T &;
  using const_reference = const T &;

  using void_pointer =
      typename options::find_option<option_list, options::void_pointer,
                                    options::void_pointer<void *>>::type;
  using tag = typename options::find_option<option_list, options::tag,
                                            options::tag<default_tag>>::type;

  using compare =
      typename options::find_option<option_list, options::compare,
                                    options::compare<std::less<T>>>::type;

  using node_type = rbtree_base_hook<T, Options...>;
  using node_pointer =
      typename std::pointer_traits<void_pointer>::template rebind<node_type>;
  using const_node_pointer = typename std::pointer_traits<
      void_pointer>::template rebind<const node_type>;
  using color_type = bool;

  using pointer =
      typename std::pointer_traits<void_pointer>::template rebind<value_type>;
  using const_pointer = typename std::pointer_traits<
      void_pointer>::template rebind<const value_type>;

  struct projection
  {
    reference operator()(node_pointer n) const noexcept
    {
      return static_cast<reference>(*n);
    }

    const_reference operator()(const_node_pointer n) const noexcept
    {
      return static_cast<const_reference>(*n);
    }
  };

private:
  node_pointer parent_;
  node_pointer left_;
  node_pointer right_;
  color_type color_;

protected:
  constexpr rbtree_base_hook() noexcept : parent_(), left_(), right_(), color_()
  {
  }

  rbtree_base_hook(const rbtree_base_hook &) = delete;
  rbtree_base_hook &operator=(const rbtree_base_hook &) = delete;

  ~rbtree_base_hook() = default;
};

template <typename T, typename... Options>
struct rbtree_hook_traits<rbtree_base_hook<T, Options...>>
{
  using node_type = rbtree_base_hook<T, Options...>;
  using node_pointer = typename node_type::node_pointer;
  using const_node_pointer = typename node_type::const_node_pointer;
  using color_type = typename node_type::color_type;

  using value_type = typename node_type::value_type;
  using reference = typename node_type::reference;
  using const_reference = typename node_type::const_reference;
  using pointer = typename node_type::pointer;
  using const_pointer = typename node_type::const_pointer;

  using compare = typename node_type::compare;
  using projection = typename node_type::projection;

  static node_pointer get_parent(const_node_pointer n) noexcept
  {
    return n->parent_;
  }

  static void set_parent(node_pointer n, node_pointer p) noexcept
  {
    n->parent_ = p;
  }

  static node_pointer get_left(const_node_pointer p) noexcept
  {
    return p->left_;
  }

  static void set_left(node_pointer p, node_pointer n) noexcept
  {
    p->left_ = n;
  }

  static node_pointer get_right(const_node_pointer p) noexcept
  {
    return p->right_;
  }

  static void set_right(node_pointer p, node_pointer n) noexcept
  {
    p->right_ = n;
  }

  static color_type get_color(const_node_pointer n) noexcept
  {
    return n->color_;
  }

  static void set_color(node_pointer n, color_type c) noexcept
  {
    n->color_ = c;
  }

  static constexpr color_type black() noexcept { return 1; }

  static constexpr color_type red() noexcept { return 0; }

  static constexpr reference to_value(node_pointer n) noexcept
  {
    return static_cast<reference>(*n);
  }

  static constexpr const_reference to_value(const_node_pointer n) noexcept
  {
    return static_cast<const_reference>(*n);
  }
};

template <typename T, typename Hook>
class tree_iterator
{
public:
  using pointer = typename Hook::pointer;
  using value_type = typename Hook::value_type;
  using reference = typename Hook::reference;
  using size_type = std::size_t;
  using difference_type = std::size_t;
  using iterator_category = std::bidirectional_iterator_tag;

  using node_type = typename Hook::node_type;
  using node_pointer = typename Hook::node_pointer;

  using projection = typename Hook::projection;

  using algo = rbtree_algorithms<Hook>;

private:
  detail::compressed_pair<node_pointer, projection> pair_;

  node_pointer &ptr() noexcept { return pair_.first(); }
  node_pointer const &ptr() const noexcept { return pair_.first(); }

  projection &project() noexcept { return pair_.second(); }
  projection const &project() const noexcept { return pair_.second(); }

public:
  tree_iterator() = default;

  explicit tree_iterator(node_pointer p,
                         projection proj = projection()) noexcept
      : pair_(p, proj)
  {
  }

  reference operator*() const noexcept
  {
    return gpcl::invoke(project(), ptr());
  }

  pointer operator->() const noexcept
  {
    return std::pointer_traits<pointer>::pointer_to(**this);
  }

  tree_iterator &operator++() noexcept
  {
    ptr() = algo::next_node(ptr());
    return *this;
  }

  tree_iterator operator++(int) noexcept
  {
    auto ret = *this;
    ++*this;
    return ret;
  }

  tree_iterator &operator--() noexcept
  {
    ptr() = algo::prev_node(ptr());
    return *this;
  }

  tree_iterator operator--(int) noexcept
  {
    auto ret = *this;
    --*this;
    return ret;
  }

  bool operator==(const tree_iterator &rhs) const noexcept
  {
    return ptr() == rhs.ptr();
  }

  bool operator!=(const tree_iterator &rhs) const noexcept
  {
    return !(*this == rhs);
  }
};

template <typename T, typename Hook>
class const_tree_iterator
{
public:
  using pointer = typename Hook::const_pointer;
  using value_type = typename Hook::value_type;
  using reference = typename Hook::const_reference;
  using size_type = std::size_t;
  using difference_type = std::size_t;
  using iterator_category = std::bidirectional_iterator_tag;

  using node_type = typename Hook::node_type;
  using node_pointer = typename Hook::node_pointer;

  using projection = typename Hook::projection;

  using algo = rbtree_algorithms<Hook>;

private:
  detail::compressed_pair<node_pointer, projection> pair_;

  node_pointer &ptr() noexcept { return pair_.first(); }
  node_pointer const &ptr() const noexcept { return pair_.first(); }

  projection &project() noexcept { return pair_.second(); }
  projection const &project() const noexcept { return pair_.second(); }

public:
  const_tree_iterator() = default;

  const_tree_iterator(tree_iterator<T, Hook> iter) noexcept : pair_(iter.pair_)
  {
  }

  explicit const_tree_iterator(node_pointer p,
                               projection proj = projection()) noexcept
      : pair_(p, proj)
  {
  }

  reference operator*() const noexcept
  {
    return gpcl::invoke(project(), ptr());
  }

  pointer operator->() const noexcept
  {
    return std::pointer_traits<pointer>::pointer_to(**this);
  }

  const_tree_iterator &operator++() noexcept
  {
    ptr() = algo::next_node(ptr());
    return *this;
  }

  const_tree_iterator operator++(int) noexcept
  {
    auto ret = *this;
    ++*this;
    return ret;
  }

  const_tree_iterator &operator--() noexcept
  {
    ptr() = algo::prev_node(ptr());
    return *this;
  }

  const_tree_iterator operator--(int) noexcept
  {
    auto ret = *this;
    --*this;
    return ret;
  }

  bool operator==(const const_tree_iterator &rhs) const noexcept
  {
    return ptr() == rhs.ptr();
  }

  bool operator!=(const const_tree_iterator &rhs) const noexcept
  {
    return !(*this == rhs);
  }
};

template <typename T, typename... Options>
class rbtree
{
public:
  using hook_traits =
      options::find_option<meta::list<Options...>, rbtree_hook_traits, void>;
  using node_type = typename hook_traits::node_type;
  using node_pointer = typename hook_traits::node_pointer;
  using const_node_pointer = typename hook_traits::const_node_pointer;
  using color_type = typename hook_traits::color_type;

  using value_type = T;
  using reference = value_type &;
  using const_reference = const value_type &;
  using pointer = typename hook_traits::pointer;
  using const_pointer = typename hook_traits::const_pointer;

  using compare = typename hook_traits::compare;
  using projection = typename hook_traits::projection;

  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using constant_time_size =
      typename options::find_option<meta::list<Options...>,
                                    options::constant_time_size,
                                    options::constant_time_size_c<true>>::type;

  using algo = rbtree_algorithms<hook_traits>;

  using iterator = tree_iterator<T, hook_traits>;
  using const_iterator = const_tree_iterator<T, hook_traits>;

  explicit rbtree(compare comp = compare(), projection proj = projection())
      : pair_(comp, proj)
  {
  }

  size_type size() const noexcept
  {
    if constexpr (constant_time_size_)
    {
      return pair1_.second();
    }
    else
    {
      return -1;
    }
  }

  compare comp() const { return pair_.first(); }

  projection project() const { return pair_.second(); }

  node_pointer header() noexcept
  {
    return std::pointer_traits<node_pointer>::pointer_to(pair1_.first());
  }

  const_node_pointer header() const noexcept
  {
    return std::pointer_traits<const_node_pointer>::pointer_to(pair1_.first());
  }

  iterator begin() noexcept
  {
    return iterator(algo::begin_node(header()), project());
  }

  const_iterator begin() const noexcept
  {
    return const_iterator(algo::begin_node(header()), project());
  }

  iterator end() noexcept
  {
    return iterator(algo::end_node(header()), project());
  }

  const_iterator end() const noexcept
  {
    return const_iterator(algo::end_node(header()), project());
  }

  iterator root() noexcept
  {
    return iterator(algo::root_node(header()), project());
  }

  const_iterator root() const noexcept
  {
    return const_iterator(algo::root_node(header()), project());
  }

  const_iterator cbegin() const noexcept { return begin(); }

  const_iterator cend() const noexcept { return end(); }

  const_iterator croot() const noexcept { return root(); }

  iterator lower_bound(const_reference value)
  {
    return iterator(algo::lower_bound(header(), value, comp(), project()),
                    project());
  }

  const_iterator lower_bound(const_reference value) const
  {
    return const_iterator(algo::lower_bound(header(), value, comp(), project()),
                          project());
  }

  template <typename K, typename Compare = compare,
            std::void_t<typename Compare::is_transparent> * = 0>
  iterator lower_bound(const K &key)
  {
    return iterator(algo::lower_bound(header(), key, comp(), project()),
                    project());
  }

  template <typename K, typename Compare = compare,
            std::void_t<typename Compare::is_transparent> * = 0>
  const_iterator lower_bound(const K &key) const
  {
    return const_iterator(algo::lower_bound(header(), key, comp(), project()),
                          project());
  }

  iterator upper_bound(const_reference value)
  {
    return iterator(algo::upper_bound(header(), value, comp(), project()),
                    project());
  }

  const_iterator upper_bound(const_reference value) const
  {
    return const_iterator(algo::upper_bound(header(), value, comp(), project()),
                          project());
  }

  template <typename K, typename Compare = compare,
            std::void_t<typename Compare::is_transparent> * = 0>
  iterator upper_bound(const K &key)
  {
    return iterator(algo::upper_bound(header(), key, comp(), project()),
                    project());
  }

  template <typename K, typename Compare = compare,
            std::void_t<typename Compare::is_transparent> * = 0>
  const_iterator upper_bound(const K &key) const
  {
    return const_iterator(algo::upper_bound(header(), key, comp(), project()),
                          project());
  }

  void insert_equal(reference value)
  {
    algo::insert_equal_upper_bound(
        header(), std::pointer_traits<node_pointer>::pointer_to(value), comp(),
        project());

    update_size(+1);
  }

  void erase(reference value) noexcept
  {
    algo::erase(header(), std::pointer_traits<node_pointer>::pointer_to(value));

    update_size(-1);
  }

private:
  void update_size(difference_type diff) noexcept
  {
    if constexpr (constant_time_size_)
    {
      pair1_.second() += diff;
    }
  }

  class header_node : public node_type
  {
  public:
    header_node() noexcept : node_type()
    {
      algo::init_header(std::pointer_traits<node_pointer>::pointer_to(*this));
    }
  };

  static constexpr constant_time_size constant_time_size_{};

  struct empty_class
  {
  };

  gpcl::detail::compressed_pair<
      header_node, meta::if_<constant_time_size, size_type, empty_class>>
      pair1_;
  gpcl::detail::compressed_pair<compare, projection> pair_;
};

} // namespace gpcl

#endif // GPCL_RBTREE_HPP
