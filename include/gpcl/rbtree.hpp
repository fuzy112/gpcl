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

#include <gpcl/detail/compressed_pair.hpp>
#include <gpcl/detail/config.hpp>
#include <gpcl/invoke.hpp>
#include <gpcl/meta.hpp>
#include <gpcl/options.hpp>
#include <gpcl/rbtree_algorithms.hpp>

namespace gpcl {

struct default_tag;

template <typename ValueTraits>
class const_tree_iterator;
template <typename ValueTraits>
class tree_iterator;

namespace detail {
template <typename NodeType>
struct rbtree_node_traits
{
  using node_type = NodeType;
  using node_pointer = typename node_type::node_pointer;
  using const_node_pointer = typename node_type::const_node_pointer;
  using color_type = typename node_type::color_type;

  static color_type red() noexcept { return node_type::red(); }

  static color_type black() noexcept { return node_type::black(); }

  static node_pointer get_parent(const_node_pointer n) noexcept
  {
    GPCL_ASSERT(n);

    return n->parent;
  }

  static void set_parent(node_pointer n, node_pointer p) noexcept
  {
    GPCL_ASSERT(n);
    n->parent = p;
  }

  static node_pointer get_left(const_node_pointer n) noexcept
  {
    GPCL_ASSERT(n);
    return n->left;
  }

  static void set_left(node_pointer n, node_pointer x) noexcept
  {
    GPCL_ASSERT(n);

    n->left = x;
  }

  static node_pointer get_right(const_node_pointer n) noexcept
  {
    return n->right;
  }

  static void set_right(node_pointer n, node_pointer x) noexcept
  {
    GPCL_ASSERT(n);
    n->right = x;
  }

  static color_type get_color(const_node_pointer n) noexcept
  {
    GPCL_ASSERT(n);
    return n->color;
  }

  static void set_color(node_pointer n, color_type c) noexcept
  {
    GPCL_ASSERT(n);
    n->color = c;
  }
};
} // namespace detail

template <typename... Options>
class rbtree_base_hook
{
private:
  friend detail::rbtree_node_traits<rbtree_base_hook>;

  using void_pointer =
      typename options::find_option<meta::list<Options...>,
                                    options::void_pointer,
                                    options::void_pointer<void *>>::type;

  using node_type = rbtree_base_hook;

  using node_traits = detail::rbtree_node_traits<rbtree_base_hook>;
  using algo = gpcl::rbtree_algorithms<node_traits>;

  using node_pointer =
      typename std::pointer_traits<void_pointer>::template rebind<node_type>;
  using const_node_pointer = typename std::pointer_traits<
      void_pointer>::template rebind<const node_type>;
  using color_type = bool;

  static constexpr color_type red() noexcept { return 0; }
  static constexpr color_type black() noexcept { return 1; }

  node_pointer parent;
  node_pointer left;
  node_pointer right;
  color_type color;

protected:
  constexpr rbtree_base_hook() = default;

  ~rbtree_base_hook() = default;

  rbtree_base_hook(const rbtree_base_hook &) = delete;
  rbtree_base_hook &operator=(const rbtree_base_hook &) = delete;

  rbtree_base_hook(rbtree_base_hook &&other) noexcept
  {
    algo::replace_node(std::pointer_traits<node_pointer>::pointer_to(other),
                       std::pointer_traits<node_pointer>::pointer_to(*this));
  }

  rbtree_base_hook &operator=(rbtree_base_hook &&other) noexcept
  {
    algo::replace_node(std::pointer_traits<node_pointer>::pointer_to(other),
                       std::pointer_traits<node_pointer>::pointer_to(*this));
    return *this;
  }
  /*
    void swap(rbtree_base_hook &other) noexcept
    {
      algo::swap_node(std::pointer_traits<node_pointer>::pointer_to(other),
                      std::pointer_traits<node_pointer>::pointer_to(*this));
    }*/
};

namespace detail {
template <typename T, typename Hook>
struct rbtree_value_traits;

template <typename T, typename... Options>
struct rbtree_value_traits<T, rbtree_base_hook<Options...>>
{
  using value_type = T;
  using node_type = rbtree_base_hook<Options...>;
  using node_traits = detail::rbtree_node_traits<node_type>;
  using node_pointer = typename node_traits::node_pointer;
  using const_node_pointer = typename node_traits::const_node_pointer;

  using pointer =
      typename std::pointer_traits<node_pointer>::template rebind<value_type>;
  using const_pointer = typename std::pointer_traits<
      node_pointer>::template rebind<const value_type>;

  using link_mode_type = typename options::find_option<
      meta::list<Options...>, options::link_mode,
      options::link_mode<options::normal_link>>::type;

  static constexpr link_mode_type link_mode{};

  static node_pointer to_node_pointer(value_type &value) noexcept
  {
    return std::pointer_traits<node_pointer>::pointer_to(value);
  }

  static const_node_pointer to_node_pointer(const value_type &value) noexcept
  {
    return std::pointer_traits<const_node_pointer>::pointer_to(value);
  }

  static pointer to_value_pointer(node_pointer n) noexcept
  {
    return static_cast<pointer>(n);
  }

  static const_pointer to_value_pointer(const_node_pointer n) noexcept
  {
    return static_cast<const_pointer>(n);
  }
};

}; // namespace detail

template <typename ValueTraits>
class tree_iterator
{
public:
  using value_traits = ValueTraits;
  using node_traits = typename value_traits::node_traits;
  using value_type = typename value_traits::value_type;
  using node_pointer = typename value_traits::node_pointer;
  using const_node_pointer = typename value_traits::const_node_pointer;

  using pointer = typename value_traits::pointer;
  using reference = value_type &;
  using size_type = std::size_t;
  using difference_type = std::size_t;
  using iterator_category = std::bidirectional_iterator_tag;

  using algo = rbtree_algorithms<node_traits>;

private:
  friend const_tree_iterator<value_traits>;

  node_pointer n_;

public:
  tree_iterator() = default;

  explicit tree_iterator(node_pointer p) noexcept : n_(p) {}

  reference operator*() const noexcept { return *this->operator->(); }

  pointer operator->() const noexcept
  {
    return value_traits::to_value_pointer(n_);
  }

  tree_iterator &operator++() noexcept
  {
    n_ = algo::next_node(n_);
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
    n_ = algo::prev_node(n_);
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
    return n_ == rhs.n_;
  }

  bool operator!=(const tree_iterator &rhs) const noexcept
  {
    return !(*this == rhs);
  }
};
template <typename ValueTraits>
class const_tree_iterator
{
public:
  using value_traits = ValueTraits;
  using node_traits = typename value_traits::node_traits;
  using value_type = typename value_traits::value_type;
  using node_pointer = typename value_traits::node_pointer;
  using const_node_pointer = typename value_traits::const_node_pointer;

  using pointer = typename value_traits::const_pointer;
  using reference = const value_type &;
  using size_type = std::size_t;
  using difference_type = std::size_t;
  using iterator_category = std::bidirectional_iterator_tag;

  using algo = rbtree_algorithms<node_traits>;

private:
  node_pointer n_;

public:
  const_tree_iterator() = default;

  explicit const_tree_iterator(node_pointer p) noexcept : n_(p) {}

  const_tree_iterator(tree_iterator<ValueTraits> iter) noexcept : n_(iter.n_) {}

  reference operator*() const noexcept { return *this->operator->(); }

  pointer operator->() const noexcept
  {
    return value_traits::to_value_pointer(n_);
  }

  const_tree_iterator &operator++() noexcept
  {
    n_ = algo::next_node(n_);
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
    n_ = algo::prev_node(n_);
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
    return n_ == rhs.n_;
  }

  bool operator!=(const const_tree_iterator &rhs) const noexcept
  {
    return !(*this == rhs);
  }
};

namespace detail {

template <typename T>
struct identity
{
  using type = T;

  constexpr T &operator()(T &value) const noexcept { return value; }

  constexpr const T &operator()(const T &value) const noexcept { return value; }

  constexpr T &&operator()(T &&value) const noexcept
  {
    return std::move(value);
  }

  constexpr const T &&operator()(const T &&value) const noexcept
  {
    return std::move(value);
  }
};

template <typename ValueType, typename KeyCompare, typename KeyOfValue>
struct value_compare
{
  using value_type = ValueType;
  using key_type = typename KeyOfValue::type;

  using key_compare = KeyCompare;
  using key_of_value = KeyOfValue;

  detail::compressed_pair<KeyCompare, KeyOfValue> pair_;

  constexpr value_compare(KeyCompare key_comp, KeyOfValue key_of_v)
      : pair_(key_comp, key_of_v)
  {
  }

  constexpr bool operator()(const value_type &x, const value_type &y) const
  {
    return pair_.first()(pair_.second()(x), pair_.second()(y));
  }

  key_compare key_comp() const { return pair_.first(); }

  key_of_value get_key_of_value() const { return pair_.second(); }

  void swap(value_compare &other) noexcept { pair_.swap(other.pair_); }
};

template <typename ValueType, typename KeyCompare, typename KeyOfValue>
void swap(value_compare<ValueType, KeyCompare, KeyOfValue> &x,
          value_compare<ValueType, KeyCompare, KeyOfValue>
              &y) noexcept(noexcept(x.swap(y)))
{
  x.swap(y);
}

template <typename ValueTraits, typename ValueCompare>
struct node_pointer_compare
{
  using value_traits = ValueTraits;
  using node_pointer = typename value_traits::node_pointer;
  using const_node_pointer = typename value_traits::const_node_pointer;
  using pointer = typename value_traits::pointer;
  using const_pointer = typename value_traits::const_pointer;

  ValueCompare value_comp_;

  constexpr explicit node_pointer_compare(ValueCompare comp) : value_comp_(comp)
  {
  }

  constexpr bool operator()(const_node_pointer x, const_node_pointer y) const
  {
    const_pointer vx = value_traits::to_value_pointer(x);
    const_pointer vy = value_traits::to_value_pointer(y);
    return value_comp_(*vx, *vy);
  }

  void swap(node_pointer_compare &other) noexcept
  {
    value_comp_.swap(other.value_comp_);
  }
};

template <typename ValueTraits, typename ValueCompare>
void swap(node_pointer_compare<ValueTraits, ValueCompare> &x,
          node_pointer_compare<ValueTraits, ValueCompare>
              &y) noexcept(noexcept(x.swap(y)))
{
  x.swap(y);
}

template <typename ValueTraits, typename KeyCompare, typename KeyOfValue>
struct key_node_pointer_compare
{
  using value_traits = ValueTraits;
  using node_pointer = typename value_traits::node_pointer;
  using const_node_pointer = typename value_traits::const_node_pointer;
  using pointer = typename value_traits::pointer;
  using const_pointer = typename value_traits::const_pointer;

  using key_type = typename KeyOfValue::type;

  struct is_transparent;

  detail::compressed_pair<KeyCompare, KeyOfValue> pair_;

  constexpr key_node_pointer_compare(KeyCompare key_comp = KeyCompare(),
                                     KeyOfValue key_of_value = KeyOfValue())
      : pair_(key_comp, key_of_value)
  {
  }

  constexpr bool operator()(const_node_pointer x, const key_type &y) const
  {
    const_pointer vx = value_traits::to_value_pointer(x);
    const key_type &kx = pair_.second()(*vx);
    return pair_.first()(kx, y);
  }

  constexpr bool operator()(const key_type &x, const_node_pointer y) const
  {
    const_pointer vy = value_traits::to_value_pointer(y);
    const key_type &ky = pair_.second()(*vy);
    return pair_.first()(x, ky);
  }

  void swap(key_node_pointer_compare &other) noexcept
  {
    pair_.swap(other.pair_);
  }
};

template <typename ValueTraits, typename KeyCompare, typename KeyOfValue>
void swap(key_node_pointer_compare<ValueTraits, KeyCompare, KeyOfValue> &x,
          key_node_pointer_compare<ValueTraits, KeyCompare, KeyOfValue>
              &y) noexcept(noexcept(x.swap(y)))
{
  x.swap(y);
}

template <typename T, typename... Options>
struct get_value_traits
{
  using base_hook = typename options::find_option<
      meta::list<Options...>, options::base_hook,
      options::base_hook<rbtree_base_hook<>>>::type;

  using type = rbtree_value_traits<T, base_hook>;
};

} // namespace detail

template <typename T, typename... Options>
class rbtree
{
public:
  using option_list = meta::list<Options...>;
  using value_traits = typename detail::get_value_traits<T, Options...>::type;
  using node_traits = typename value_traits::node_traits;

  using value_type = typename value_traits::value_type;
  using node_type = typename node_traits::node_type;
  using node_pointer = typename value_traits::node_pointer;
  using const_node_pointer = typename value_traits::const_node_pointer;
  using pointer = typename value_traits::pointer;
  using const_pointer = typename value_traits::const_pointer;

  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using constant_time_size =
      typename options::find_option<meta::list<Options...>,
                                    options::constant_time_size,
                                    options::constant_time_size_c<true>>::type;

  using key_of_value = typename options::find_option<
      option_list, options::key_of_value,
      options::key_of_value<detail::identity<value_type>>>::type;
  using key_type = typename key_of_value::type;

  using key_compare = typename options::find_option<
      option_list, options::compare,
      options::compare<std::less<key_type>>>::type;

  using value_compare =
      detail::value_compare<value_type, key_compare, key_of_value>;

  using node_pointer_compare =
      detail::node_pointer_compare<value_traits, value_compare>;

  using key_node_pointer_compare =
      detail::key_node_pointer_compare<value_traits, key_compare, key_of_value>;

  using algo = rbtree_algorithms<node_traits>;

  using iterator = tree_iterator<value_traits>;
  using const_iterator = const_tree_iterator<value_traits>;

  using reference = value_type &;
  using const_reference = value_type const &;

  explicit rbtree(key_compare key_comp = key_compare(),
                  key_of_value proj = key_of_value())
      : value_comp_(key_comp, proj)
  {
  }

  rbtree(const rbtree &other) = delete;

  rbtree &operator=(const rbtree &other) = delete;

  rbtree(rbtree &&other) noexcept : value_comp_(std::move(other).value_comp_)
  {
    algo::swap_trees(header(), other.header());
  }

  rbtree &operator=(rbtree &&other) noexcept
  {
    swap(other);
    return *this;
  }

  void swap(rbtree &other) noexcept
  {
    using std::swap;
    swap(value_comp_, other.value_comp_);
    algo::swap_trees(header(), other.header());
  }

  template <typename C = constant_time_size,
            typename std::enable_if<!C::value, int>::type = 0>
  size_type size() const noexcept
  {
    return algo::size(header());
  }

  template <typename C = constant_time_size,
            typename std::enable_if<C::value, int>::type = 0>
  size_type size() const noexcept
  {
    return pair1_.second();
  }

  key_compare key_comp() const { return value_comp_.key_comp(); }

  value_compare value_comp() const { return value_comp_; }

  node_pointer_compare node_pointer_comp() const
  {
    return node_pointer_compare(value_comp());
  }

  key_node_pointer_compare key_node_pointer_comp() const
  {
    return key_node_pointer_compare(key_comp(),
                                    value_comp().get_key_of_value());
  }

  node_pointer header() noexcept
  {
    return std::pointer_traits<node_pointer>::pointer_to(pair1_.first());
  }

  const_node_pointer header() const noexcept
  {
    return std::pointer_traits<const_node_pointer>::pointer_to(pair1_.first());
  }

  iterator begin() noexcept { return iterator(algo::begin_node(header())); }

  const_iterator begin() const noexcept
  {
    return const_iterator(algo::begin_node(header()));
  }

  iterator end() noexcept { return iterator(algo::end_node(header())); }

  const_iterator end() const noexcept
  {
    return const_iterator(algo::end_node(header()));
  }

  iterator root() noexcept { return iterator(algo::root_node(header())); }

  const_iterator root() const noexcept
  {
    return const_iterator(algo::root_node(header()));
  }

  const_iterator cbegin() const noexcept { return begin(); }

  const_iterator cend() const noexcept { return end(); }

  const_iterator croot() const noexcept { return root(); }

  iterator lower_bound(const key_type &key)
  {
    return iterator(algo::lower_bound(header(), key, key_node_pointer_comp()));
  }

  const_iterator lower_bound(const key_type &key) const
  {
    return const_iterator(
        algo::lower_bound(header(), key, key_node_pointer_comp()));
  }

  iterator upper_bound(const key_type &key)
  {
    return iterator(algo::upper_bound(header(), key, key_node_pointer_comp()));
  }

  const_iterator upper_bound(const key_type &key) const
  {
    return const_iterator(
        algo::upper_bound(header(), key, key_node_pointer_comp()));
  }

  std::pair<iterator, iterator> equal_range(const key_type &key)
  {
    const auto [first, last] =
        algo::equal_range(header(), key, key_node_pointer_comp());
    return std::make_pair(iterator(first), iterator(last));
  }

  std::pair<const_iterator, const_iterator>
  equal_range(const key_type &key) const
  {
    const auto [first, last] =
        algo::equal_range(header(), key, key_node_pointer_comp());
    return std::make_pair(const_iterator(first), const_iterator(last));
  }

  size_type count(const key_type &key) const
  {
    return algo::count(header(), key, key_node_pointer_comp());
  }

  void insert_equal(reference value)
  {
    algo::insert_equal_upper_bound(
        header(), value_traits::to_node_pointer(value), node_pointer_comp());

    update_size(+1);
  }

  void insert_equal_hint(const_iterator hint, reference value)
  {
    algo::insert_equal_upper_bound_hint(header(), hint.p_, value);

    update_size(+1);
  }

  void erase(reference value) noexcept
  {
    algo::erase(header(), value_traits::to_node_pointer(value));

    update_size(-1);
  }

private:
  void update_size(difference_type diff) noexcept
  {
    update_size_impl(diff, constant_time_size_);
  }

  void update_size_impl(difference_type diff, std::true_type) noexcept
  {
    pair1_.second() += diff;
  }

  void update_size_impl(difference_type, std::false_type) noexcept {}

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
  value_compare value_comp_;
};

template <typename T, typename... Options>
void swap(rbtree<T, Options...> &x,
          rbtree<T, Options...> &y) noexcept(noexcept(x.swap(y)))
{
  x.swap(y);
}

} // namespace gpcl

#endif // GPCL_RBTREE_HPP
