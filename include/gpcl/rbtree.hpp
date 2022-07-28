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

#include <gpcl/detail/assert.hpp>
#include <gpcl/detail/compressed_pointer.hpp>
#include <gpcl/detail/config.hpp>
#include <gpcl/lexical_cast.hpp>

#include <fstream>
#include <functional>

namespace gpcl {

template <typename>
class rbtree_end_node;

template <typename, typename, typename>
class rbtree_node;

template <typename, typename, typename>
class rbtree_iterator;

template <typename, typename, typename>
class rbtree_const_iterator;

template <typename, typename, typename, typename>
class rbtree;

template <typename Pointer>
class rbtree_end_node
{
public:
  using pointer = Pointer;

  pointer left_;

  constexpr rbtree_end_node() noexcept : left_() {}

  rbtree_end_node(const rbtree_end_node &) = delete;
  rbtree_end_node &operator=(const rbtree_end_node &) = delete;
};

template <typename T, typename Tag = class default_tag,
          typename VoidPtr = void *>
class rbtree_node : public rbtree_end_node<typename std::pointer_traits<
                        VoidPtr>::template rebind<rbtree_node<T, Tag, VoidPtr>>>
{
  template <typename, typename, typename, typename>
  friend class rbtree;

public:
  using void_pointer = VoidPtr;
  using pointer =
      typename std::pointer_traits<void_pointer>::template rebind<rbtree_node>;
  using const_pointer = typename std::pointer_traits<
      void_pointer>::template rebind<const rbtree_node>;
  using end_node_type = rbtree_end_node<pointer>;
  using end_node_ptr = typename std::pointer_traits<
      void_pointer>::template rebind<end_node_type>;
  using parent_pointer = end_node_ptr;
  using node_type = rbtree_node;

  using iter_pointer = end_node_ptr;

  pointer right_;
  parent_pointer parent_;
  bool is_black_;

protected:
  constexpr rbtree_node() noexcept : right_(), parent_(), is_black_() {}

  constexpr rbtree_node(const rbtree_node &) noexcept : rbtree_node() {}

  rbtree_node &operator=(const rbtree_node &) noexcept { return *this; }

public:
  const_pointer parent_unsafe() const noexcept
  {
    return static_cast<const_pointer>(parent_);
  }

  pointer parent_unsafe() noexcept { return static_cast<pointer>(parent_); }

  void set_parent(pointer p) noexcept
  {
    parent_ = static_cast<parent_pointer>(p);
  }

  bool is_left_child_of_parent() const noexcept
  {
    return (parent_->left_ == this);
  }

  T &value() noexcept { return static_cast<T &>(*this); }

  T const &value() const noexcept { return static_cast<T const &>(*this); }

  std::string id() const
  {
    return std::string("node") +
           std::to_string(reinterpret_cast<std::uintptr_t>(this));
  }

  std::string node_def() const
  {
    return id() + "[label=" + lexical_cast<std::string>(value()) +
           " color=" + (is_black_ ? "black" : "red") + "]";
  }

  pointer maximum_in_subtree() noexcept
  {
    pointer y = nullptr;
    pointer x = static_cast<pointer>(this);
    while (x != nullptr)
    {
      y = x;
      x = x->right_;
    }
    return y;
  }

  pointer minimum_in_subtree() noexcept
  {
    pointer y = nullptr;
    pointer x = static_cast<pointer>(this);
    while (x != nullptr)
    {
      y = x;
      x = x->left_;
    }
    return y;
  }

public:
  friend inline std::string dump_node(const_pointer x, std::ostream &out)
  {
    if (!x)
    {
      static int id = 0;
      out << "node" << ++id << "[label=nil]\n";
      return std::string("node") + std::to_string(id);
    }

    out << x->node_def() << '\n';

    auto left_ = dump_node(x->left_, out);
    out << x->id() << " -> " << left_ << '\n';
    auto right_ = dump_node(x->right_, out);
    out << x->id() << " -> " << right_ << '\n';
    return x->id();
  }
};

template <typename NodePtr,
          typename IterPtr = typename std::pointer_traits<
              NodePtr>::template rebind<rbtree_end_node<NodePtr>>>
IterPtr rbtree_next(NodePtr x) noexcept
{
  if (x->right_ != nullptr)
    return static_cast<IterPtr>(x->right_->minimum_in_subtree());

  while (!x->is_left_child_of_parent())
    x = x->parent_unsafe();

  return x->parent_;
}

// Calling prev on the first node is undefined behaviour
template <typename NodePtr,
          typename IterPtr = typename std::pointer_traits<
              NodePtr>::template rebind<rbtree_end_node<NodePtr>>>
IterPtr rbtree_prev(NodePtr x) noexcept
{
  if (x->left_ != nullptr)
    return static_cast<IterPtr>(x->left_->maximum_in_subtree());

  while (x->is_left_child_of_parent())
    x = x->parent_unsafe();

  return x->parent_;
}

template <typename T, typename Tag, typename VoidPtr>
class rbtree_iterator
{
public:
  using void_pointer = VoidPtr;
  using node_type = rbtree_node<T, Tag, VoidPtr>;
  using node_pointer = typename node_type::pointer;
  using end_node_type = typename node_type::end_node_type;
  using end_node_ptr = typename node_type::end_node_ptr;
  using iter_pointer = end_node_ptr;

  using value_type = T;
  using pointer =
      typename std::pointer_traits<void_pointer>::template rebind<T>;
  using reference = T &;
  using difference_type =
      typename std::pointer_traits<void_pointer>::difference_type;
  using size_type = std::make_unsigned_t<difference_type>;
  using iterator_category = std::bidirectional_iterator_tag;

  iter_pointer ptr_;

  explicit rbtree_iterator(iter_pointer p = nullptr) noexcept : ptr_(p) {}

  rbtree_iterator &operator++() noexcept
  {
    GPCL_ASSERT(ptr_ != nullptr);
    ptr_ = rbtree_next(static_cast<node_pointer>(ptr_));
    return *this;
  }

  rbtree_iterator operator++(int) noexcept
  {
    auto ret = *this;
    ++*this;
    return ret;
  }

  rbtree_iterator &operator--() noexcept
  {
    GPCL_ASSERT(ptr_ != nullptr);
    ptr_ = rbtree_prev(static_cast<node_pointer>(ptr_));
    return *this;
  }

  rbtree_iterator operator--(int) noexcept
  {
    auto ret = *this;
    --*this;
    return ret;
  }

  reference operator*() const noexcept
  {
    GPCL_ASSERT(ptr_ != nullptr);
    return *static_cast<pointer>(ptr_);
  }

  pointer operator->() const noexcept { return static_cast<pointer>(ptr_); }

  friend bool operator==(const rbtree_iterator &x,
                         const rbtree_iterator &y) noexcept
  {
    return x.ptr_ == y.ptr_;
  }

  friend bool operator!=(const rbtree_iterator &x,
                         const rbtree_iterator &y) noexcept
  {
    return !(x == y);
  }
};

template <typename T, typename Tag, typename VoidPtr>
class rbtree_const_iterator
{
public:
  using void_pointer = VoidPtr;
  using node_type = rbtree_node<T, Tag, VoidPtr>;
  using node_pointer = typename node_type::pointer;
  using end_node_type = typename node_type::end_node_type;
  using end_node_ptr = typename node_type::end_node_ptr;
  using iter_pointer = end_node_ptr;

  using value_type = T;
  using pointer =
      typename std::pointer_traits<void_pointer>::template rebind<const T>;
  using reference = const T &;
  using difference_type =
      typename std::pointer_traits<void_pointer>::difference_type;
  using size_type = std::make_unsigned_t<difference_type>;
  using iterator_category = std::bidirectional_iterator_tag;

  iter_pointer ptr_;

  explicit rbtree_const_iterator(iter_pointer p = nullptr) noexcept : ptr_(p) {}

  rbtree_const_iterator(rbtree_iterator<T, Tag, VoidPtr> iter) noexcept
      : ptr_(iter.ptr_)
  {
  }

  rbtree_const_iterator &operator++() noexcept
  {
    GPCL_ASSERT(ptr_ != nullptr);
    ptr_ = rbtree_next(static_cast<node_pointer>(ptr_));
    return *this;
  }

  rbtree_const_iterator operator++(int) noexcept
  {
    auto ret = *this;
    ++*this;
    return ret;
  }

  rbtree_const_iterator &operator--() noexcept
  {
    GPCL_ASSERT(ptr_ != nullptr);
    ptr_ = rbtree_prev(static_cast<node_pointer>(ptr_));
    return *this;
  }

  rbtree_const_iterator operator--(int) noexcept
  {
    auto ret = *this;
    --*this;
    return ret;
  }

  reference operator*() const noexcept
  {
    GPCL_ASSERT(ptr_ != nullptr);
    return *static_cast<pointer>(ptr_);
  }

  pointer operator->() const noexcept { return static_cast<pointer>(ptr_); }

  friend bool operator==(const rbtree_const_iterator &x,
                         const rbtree_const_iterator &y) noexcept
  {
    return x.ptr_ == y.ptr_;
  }

  friend bool operator!=(const rbtree_const_iterator &x,
                         const rbtree_const_iterator &y) noexcept
  {
    return !(x == y);
  }
};

template <typename T, typename Tag = class default_tag,
          typename Compare = std::less<T>, typename VoidPtr = void *>
class rbtree
{
public:
  using void_pointer = VoidPtr;
  using value_type = T;
  using node_type = rbtree_node<T, Tag, VoidPtr>;
  using node_pointer =
      typename std::pointer_traits<void_pointer>::template rebind<node_type>;
  using const_node_pointer = typename std::pointer_traits<
      void_pointer>::template rebind<const node_type>;
  using end_node_type = rbtree_end_node<node_pointer>;
  using end_node_ptr = typename std::pointer_traits<
      void_pointer>::template rebind<end_node_type>;
  using const_end_node_ptr = typename std::pointer_traits<
      void_pointer>::template rebind<const end_node_type>;
  using compare = Compare;
  using iter_pointer = end_node_ptr;

  using reference = T &;
  using const_reference = const T &;
  using pointer =
      typename std::pointer_traits<void_pointer>::template rebind<T>;
  using const_pointer =
      typename std::pointer_traits<void_pointer>::template rebind<const T>;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  using iterator = rbtree_iterator<T, Tag, VoidPtr>;
  using const_iterator = rbtree_const_iterator<T, Tag, VoidPtr>;

  using reverse_iterator = std::reverse_iterator<iterator>;
  using reverse_const_iterator = std::reverse_iterator<const_iterator>;

private:
  mutable detail::compressed_pair<end_node_type, compare> p_;

  end_node_ptr get_base() const noexcept
  {
    return std::pointer_traits<end_node_ptr>::pointer_to(p_.first());
  }

  node_pointer root_node() const noexcept
  {
    return static_cast<node_pointer>(p_.first().left_);
  }

public:
  constexpr rbtree(const compare &cmp = compare()) noexcept(
      std::is_nothrow_copy_constructible<compare>::value)
      : p_(detail::piecewise_construct, std::make_tuple(),
           std::forward_as_tuple(cmp))
  {
  }

  rbtree(const rbtree &) = delete;
  rbtree &operator=(const rbtree &) = delete;

  const_pointer root() const noexcept
  {
    return static_cast<const_pointer>(root_node());
  }

  pointer root() noexcept { return static_cast<pointer>(root_node()); }

  compare comp() const noexcept { return p_.second(); }

  iterator begin() noexcept
  {
    if (root_node())
      return iterator(
          static_cast<iter_pointer>(root_node()->minimum_in_subtree()));
    return end();
  }

  const_iterator begin() const noexcept
  {
    if (root_node())
      return const_iterator(
          static_cast<iter_pointer>(root_node()->minimum_in_subtree()));
    return end();
  }

  const_iterator cbegin() const noexcept { return begin(); }

  iterator end() noexcept { return iterator(get_base()); }

  const_iterator end() const noexcept
  {
    return const_iterator(static_cast<iter_pointer>(get_base()));
  }

  const_iterator cend() const noexcept { return end(); }

  reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }

  reverse_const_iterator rbegin() const noexcept
  {
    return reverse_const_iterator(end());
  }

  reverse_const_iterator crbegin() const noexcept { return rbegin(); }

  reverse_iterator rend() noexcept { return reverse_iterator(begin()); }

  reverse_const_iterator rend() const noexcept
  {
    return reverse_const_iterator(begin());
  }

  reverse_const_iterator crend() const noexcept { return rend(); }

  template <typename K>
  const_pointer find(const K &k) const noexcept
  {
    const_node_pointer p = root();
    auto cmp = comp();

    while (p != nullptr)
    {
      if (cmp(k, p->value()))
        p = p->left_;
      else if (cmp(p->value(), k))
        p = p->right_;
      else
        return static_cast<const_pointer>(p);
    }

    return nullptr;
  }

  template <typename K>
  pointer find(const K &k) noexcept
  {
    node_pointer p = root();
    auto cmp = comp();

    while (p != nullptr)
    {
      if (cmp(k, p->value()))
        p = p->left_;
      else if (cmp(p->value(), k))
        p = p->right_;
      else
        return static_cast<pointer>(p);
    }

    return nullptr;
  }

  void remove(reference v) noexcept
  {
    node_pointer x = std::pointer_traits<node_pointer>::pointer_to(v);

    // y will be node to delete
    // either x or x's successor
    node_pointer y = (x->right_ && x->left_) ? x->right_ : x;
    if (y != x)
    {
      // find x's successor
      while (y->left_)
        y = y->left_;
    }

    // z is y's (possible null) single child
    node_pointer z = y->left_ ? y->left_ : y->right_;

    // w is z's (possible null) uncle and will be z's sibling
    node_pointer w = nullptr;
    if (y->is_left_child_of_parent())
      w = y == root() ? nullptr : y->parent_unsafe()->right_;
    else
      w = y->parent_->left_;

    bool remove_black = y->is_black_;

    // remove y
    if (y->is_left_child_of_parent())
      y->parent_->left_ = z;
    else
      y->parent_unsafe()->right_ = z;
    if (z != nullptr)
      z->parent_ = y->parent_;

    if (y != x)
    {
      // if x has not already been removed, replace x by y

      y->is_black_ = x->is_black_;
      y->parent_ = x->parent_;
      y->left_ = x->left_;
      y->right_ = x->right_;

      if (x->is_left_child_of_parent())
        x->parent_->left_ = y;
      else
        x->parent_unsafe()->right_ = y;

      if (x->left_ != nullptr)
        x->left_->set_parent(y);
      if (x->right_ != nullptr)
        x->right_->set_parent(y);
    }

    if (remove_black)
    {
      for (;;)
      {
        if (z == root())
          break;
        if (z != nullptr && !z->is_black_)
        {
          // Case 1.
          // z is red
          // we recolor it black
          z->is_black_ = true;
          break;
        }
        else
        {
          GPCL_ASSERT(w != nullptr);
          if (w != nullptr && !w->is_black_)
          {
            w->parent_unsafe()->is_black_ = false;
            w->is_black_ = true;
            if (w->is_left_child_of_parent())
            {
              rotate_right_(w->parent_unsafe());
              w = w->right_->left_;
            }
            else
            {
              rotate_left(w->parent_unsafe());
              w = w->left_->right_;
            }
          }
          else
          {
            if ((w->left_ == nullptr || w->left_->is_black_) &&
                (w->right_ == nullptr || w->right_->is_black_))
            {
              w->is_black_ = false;
              z = w->parent_unsafe();
              if (z == root())
                break;

              if (z->is_left_child_of_parent())
                w = z->parent_unsafe()->right_;
              else
                w = z->parent_->left_;
            }
            else
            {
              // w has at least one red child
              if (w->is_left_child_of_parent())
              {
                if (w->right_ != nullptr && !w->right_->is_black_)
                {
                  w->is_black_ = false;
                  w->right_->is_black_ = true;
                  w = w->right_;
                  rotate_left(w->parent_unsafe());
                }
                w->is_black_ = w->parent_unsafe()->is_black_;
                w->parent_unsafe()->is_black_ = true;
                w->left_->is_black_ = true;
                rotate_right_(w->parent_unsafe());
                break;
              }
              else
              {
                if (w->left_ != nullptr && !w->left_->is_black_)
                {
                  w->is_black_ = false;
                  w->left_->is_black_ = true;
                  w = w->left_;
                  rotate_right_(w->parent_unsafe());
                }
                // !w->right_->is_black_

                w->is_black_ = w->parent_unsafe()->is_black_;
                w->parent_unsafe()->is_black_ = true;
                w->right_->is_black_ = true;
                rotate_left(w->parent_unsafe());
                break;
              }
            }
          }
        }
      }
    }
  }

  void insert(reference v) noexcept
  {
    node_pointer *link = std::addressof(get_base()->left_);
    node_pointer p = static_cast<node_pointer>(get_base());

    node_pointer x = std::pointer_traits<node_pointer>::pointer_to(v);

    auto cmp = comp();

    while (*link != nullptr)
    {
      p = *link;
      if (cmp(v, p->value()))
        link = &p->left_;
      else
        link = &p->right_;
    }

    x->set_parent(p);
    *link = x;

    x->left_ = nullptr;
    x->right_ = nullptr;

    rebalance_after_insert(x);
  }

  template <typename K>
  pointer find_or_insert(const K &k, reference v)
  {
    node_pointer *link = std::addressof(get_base()->left_);
    node_pointer p = static_cast<node_pointer>(get_base());
    node_pointer x = std::pointer_traits<node_pointer>::pointer_to(v);

    auto cmp = comp();

    while (*link != nullptr)
    {
      p = *link;
      if (cmp(k, p->value()))
        link = &p->left_;
      else if (cmp(p->value(), k))
        link = &p->right_;
      else
        return static_cast<pointer>(p);
    }

    x->set_parent(p);
    *link = x;

    x->left_ = nullptr;
    x->right_ = nullptr;

    rebalance_after_insert(x);

    return nullptr;
  }

  template <typename K>
  const_pointer upper_bound(const K &k) const noexcept
  {
    const_node_pointer p = 0;
    const_node_pointer const *link = std::addressof(get_base()->left_);
    auto cmp = comp();

    while (*link)
    {
      p = *link;

      if (cmp(k, p->value()))
        link = &p->left_;
      else
        link = &p->right_;
    }

    return static_cast<const_pointer>(p);
  }

private:
  void rebalance_after_insert(node_pointer x) noexcept
  {
    x->is_black_ = x == root();
    while (x != root() && !x->parent_unsafe()->is_black_)
    {
      if (x->parent_unsafe()->is_left_child_of_parent())
      {
        node_pointer y = x->parent_unsafe()->parent_unsafe()->right_;
        if (y && !y->is_black_)
        {
          x = x->parent_unsafe();
          x->is_black_ = true;
          y->is_black_ = true;
          x = x->parent_unsafe();
          x->is_black_ = x == root();
        }
        else
        {
          if (!x->is_left_child_of_parent())
          {
            x = x->parent_unsafe();
            rotate_left(x);
          }

          x = x->parent_unsafe();
          x->is_black_ = true;
          x = x->parent_unsafe();
          x->is_black_ = false;
          rotate_right_(x);
          break;
        }
      }
      else
      {
        node_pointer y = x->parent_unsafe()->parent_unsafe()->left_;
        if (y && !y->is_black_)
        {
          x = x->parent_unsafe();
          x->is_black_ = true;
          y->is_black_ = true;
          x = x->parent_unsafe();
          x->is_black_ = x == root();
        }
        else
        {
          if (x->is_left_child_of_parent())
          {
            x = x->parent_unsafe();
            rotate_right_(x);
          }
          x = x->parent_unsafe();
          x->is_black_ = true;
          x = x->parent_unsafe();
          x->is_black_ = false;
          rotate_left(x);
          break;
        }
      }
    }
  }

  void rotate_right_(node_pointer x) noexcept
  {
    GPCL_ASSERT(x);

    node_pointer y = x->left_;
    y->parent_ = x->parent_;

    if (x->is_left_child_of_parent())
      x->parent_->left_ = y;
    else
      x->parent_unsafe()->right_ = y;

    x->left_ = y->right_;
    if (y->right_ != nullptr)
      y->right_->set_parent(x);

    x->set_parent(y);
    y->right_ = x;
  }

  void rotate_left(node_pointer x) noexcept
  {
    GPCL_ASSERT(x);

    node_pointer y = x->right_;
    y->parent_ = x->parent_;

    if (x->is_left_child_of_parent())
      x->parent_->left_ = y;
    else
      x->parent_unsafe()->right_ = y;

    x->right_ = y->left_;
    if (y->left_ != nullptr)
      y->left_->set_parent(x);
    x->set_parent(y);
    y->left_ = x;
  }
};

template <typename Tree>
void dump_tree(const Tree &tree)
{
  std::string tmpfile = tmpnam(NULL);
  std::ofstream file(tmpfile);
  file << "digraph {\n";
  file << "ordering=out\n";
  dump_node(static_cast<typename Tree::const_node_pointer>(tree.root()), file);
  file << "}\n";

  std::clog << "Output to " << tmpfile << '\n';
  file.close();
  std::string cmd = "dotty " + tmpfile + " &";
  std::system(cmd.c_str());
}

} // namespace gpcl

#endif // GPCL_RBTREE_HPP
