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

template <typename T, typename Tag, typename VoidPtr>
class rbtree_node;

template <typename T, typename Tag, typename Compare, typename VoidPtr>
class rbtree_iterator;

template <typename T, typename Tag, typename Compare, typename VoidPtr>
class rbtree_const_iterator;

template <typename T, typename Tag, typename Compare, typename VoidPtr>
class rbtree;

template <typename Pointer>
class rbtree_end_node
{
public:
  using pointer = Pointer;

  pointer left;

  constexpr rbtree_end_node() noexcept : left() {}

  rbtree_end_node(const rbtree_end_node &) = delete;
  rbtree_end_node &operator=(const rbtree_end_node &) = delete;
};

template <typename T, typename Tag = class default_tag,
          typename VoidPtr = void *>
class rbtree_node : public rbtree_end_node<typename std::pointer_traits<
                        VoidPtr>::template rebind<rbtree_node<T, Tag, VoidPtr>>>
{
  template <typename T1, typename Tag1, typename Compare, typename VoidPtr1>
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

  pointer right;
  parent_pointer parent;
  bool is_black;

protected:
  constexpr rbtree_node() noexcept : right(), parent(), is_black() {}

  constexpr rbtree_node(const rbtree_node &) noexcept : rbtree_node() {}

  rbtree_node &operator=(const rbtree_node &) noexcept { return *this; }

private:
  const_pointer parent_unsafe() const noexcept
  {
    return static_cast<const_pointer>(parent);
  }

  pointer parent_unsafe() noexcept { return static_cast<pointer>(parent); }

  void set_parent(pointer p) noexcept
  {
    parent = static_cast<parent_pointer>(p);
  }

  bool is_left_child_of_parent() const noexcept
  {
    return (parent->left == this);
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
           " color=" + (is_black ? "black" : "red") + "]";
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

    auto left = dump_node(x->left, out);
    out << x->id() << " -> " << left << '\n';
    auto right = dump_node(x->right, out);
    out << x->id() << " -> " << right << '\n';
    return x->id();
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

  using reference = T &;
  using const_reference = const T &;
  using pointer =
      typename std::pointer_traits<void_pointer>::template rebind<T>;
  using const_pointer =
      typename std::pointer_traits<void_pointer>::template rebind<const T>;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

private:
  detail::compressed_pair<end_node_type, compare> p_;

  end_node_ptr get_base() noexcept
  {
    return std::pointer_traits<end_node_ptr>::pointer_to(p_.first());
  }

  const_end_node_ptr get_base() const noexcept
  {
    return std::pointer_traits<const_end_node_ptr>::pointer_to(p_.first());
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
    return static_cast<const_pointer>(p_.first().left);
  }

  pointer root() noexcept { return static_cast<pointer>(p_.first().left); }

  compare comp() const noexcept { return p_.second(); }

  template <typename K>
  const_pointer find(const K &k) const noexcept
  {
    const_node_pointer p = root();
    auto cmp = comp();

    while (p != nullptr)
    {
      if (cmp(k, p->value()))
        p = p->left;
      else if (cmp(p->value(), k))
        p = p->right;
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
        p = p->left;
      else if (cmp(p->value(), k))
        p = p->right;
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
    node_pointer y = (x->right && x->left) ? x->right : x;
    if (y != x)
    {
      // find x's successor
      while (y->left)
        y = y->left;
    }

    // z is y's (possible null) single child
    node_pointer z = y->left ? y->left : y->right;

    // w is z's (possible null) uncle and will be z's sibling
    node_pointer w = nullptr;
    if (y->is_left_child_of_parent())
      w = y == root() ? nullptr : y->parent_unsafe()->right;
    else
      w = y->parent->left;

    bool remove_black = y->is_black;

    // remove y
    if (y->is_left_child_of_parent())
      y->parent->left = z;
    else
      y->parent_unsafe()->right = z;
    if (z != nullptr)
      z->parent = y->parent;

    if (y != x)
    {
      // if x has not already been removed, replace x by y

      y->is_black = x->is_black;
      y->parent = x->parent;
      y->left = x->left;
      y->right = x->right;

      if (x->is_left_child_of_parent())
        x->parent->left = y;
      else
        x->parent_unsafe()->right = y;

      if (x->left != nullptr)
        x->left->set_parent(y);
      if (x->right != nullptr)
        x->right->set_parent(y);
    }

    if (remove_black)
    {
      for (;;)
      {
        if (z == root())
          break;
        if (z != nullptr && !z->is_black)
        {
          // Case 1.
          // z is red
          // we recolor it black
          z->is_black = true;
          break;
        }
        else
        {
          GPCL_ASSERT(w != nullptr);
          if (w != nullptr && !w->is_black)
          {
            w->parent_unsafe()->is_black = false;
            w->is_black = true;
            if (w->is_left_child_of_parent())
            {
              rotate_right(w->parent_unsafe());
              w = w->right->left;
            }
            else
            {
              rotate_left(w->parent_unsafe());
              w = w->left->right;
            }
          }
          else
          {
            if ((w->left == nullptr || w->left->is_black) &&
                (w->right == nullptr || w->right->is_black))
            {
              w->is_black = false;
              z = w->parent_unsafe();
              if (z == root())
                break;

              if (z->is_left_child_of_parent())
                w = z->parent_unsafe()->right;
              else
                w = z->parent->left;
            }
            else
            {
              // w has at least one red child
              if (w->is_left_child_of_parent())
              {
                if (w->right != nullptr && !w->right->is_black)
                {
                  w->is_black = false;
                  w->right->is_black = true;
                  w = w->right;
                  rotate_left(w->parent_unsafe());
                }
                w->is_black = w->parent_unsafe()->is_black;
                w->parent_unsafe()->is_black = true;
                w->left->is_black = true;
                rotate_right(w->parent_unsafe());
                break;
              }
              else
              {
                if (w->left != nullptr && !w->left->is_black)
                {
                  w->is_black = false;
                  w->left->is_black = true;
                  w = w->left;
                  rotate_right(w->parent_unsafe());
                }
                // !w->right->is_black

                w->is_black = w->parent_unsafe()->is_black;
                w->parent_unsafe()->is_black = true;
                w->right->is_black = true;
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
    node_pointer *link = std::addressof(get_base()->left);
    node_pointer p = static_cast<node_pointer>(get_base());

    node_pointer x = std::pointer_traits<node_pointer>::pointer_to(v);

    auto cmp = comp();

    while (*link != nullptr)
    {
      p = *link;
      if (cmp(v, p->value()))
        link = &p->left;
      else
        link = &p->right;
    }

    x->set_parent(p);
    *link = x;

    x->left = nullptr;
    x->right = nullptr;

    rebalance_after_insert(x);
  }

  template <typename K>
  pointer find_or_insert(const K &k, reference v)
  {
    node_pointer *link = std::addressof(get_base()->left);
    node_pointer p = static_cast<node_pointer>(get_base());
    node_pointer x = std::pointer_traits<node_pointer>::pointer_to(v);

    auto cmp = comp();

    while (*link != nullptr)
    {
      p = *link;
      if (cmp(k, p->value()))
        link = &p->left;
      else if (cmp(p->value(), k))
        link = &p->right;
      else
        return static_cast<pointer>(p);
    }

    x->set_parent(p);
    *link = x;

    x->left = nullptr;
    x->right = nullptr;

    rebalance_after_insert(x);

    return nullptr;
  }

  template <typename K>
  const_pointer upper_bound(const K &k) const noexcept
  {
    const_node_pointer p = 0;
    const_node_pointer const *link = std::addressof(get_base()->left);
    auto cmp = comp();

    while (*link)
    {
      p = *link;

      if (cmp(k, p->value()))
        link = &p->left;
      else
        link = &p->right;
    }

    return static_cast<const_pointer>(p);
  }

private:
  void rebalance_after_insert(node_pointer x) noexcept
  {
    x->is_black = x == root();
    while (x != root() && !x->parent_unsafe()->is_black)
    {
      if (x->parent_unsafe()->is_left_child_of_parent())
      {
        node_pointer y = x->parent_unsafe()->parent_unsafe()->right;
        if (y && !y->is_black)
        {
          x = x->parent_unsafe();
          x->is_black = true;
          y->is_black = true;
          x = x->parent_unsafe();
          x->is_black = x == root();
        }
        else
        {
          if (!x->is_left_child_of_parent())
          {
            x = x->parent_unsafe();
            rotate_left(x);
          }

          x = x->parent_unsafe();
          x->is_black = true;
          x = x->parent_unsafe();
          x->is_black = false;
          rotate_right(x);
          break;
        }
      }
      else
      {
        node_pointer y = x->parent_unsafe()->parent_unsafe()->left;
        if (y && !y->is_black)
        {
          x = x->parent_unsafe();
          x->is_black = true;
          y->is_black = true;
          x = x->parent_unsafe();
          x->is_black = x == root();
        }
        else
        {
          if (x->is_left_child_of_parent())
          {
            x = x->parent_unsafe();
            rotate_right(x);
          }
          x = x->parent_unsafe();
          x->is_black = true;
          x = x->parent_unsafe();
          x->is_black = false;
          rotate_left(x);
          break;
        }
      }
    }
  }

  void rotate_right(node_pointer x) noexcept
  {
    GPCL_ASSERT(x);

    node_pointer y = x->left;
    y->parent = x->parent;

    if (x->is_left_child_of_parent())
      x->parent->left = y;
    else
      x->parent_unsafe()->right = y;

    x->left = y->right;
    if (y->right != nullptr)
      y->right->set_parent(x);

    x->set_parent(y);
    y->right = x;
  }

  void rotate_left(node_pointer x) noexcept
  {
    GPCL_ASSERT(x);

    node_pointer y = x->right;
    y->parent = x->parent;

    if (x->is_left_child_of_parent())
      x->parent->left = y;
    else
      x->parent_unsafe()->right = y;

    x->right = y->left;
    if (y->left != nullptr)
      y->left->set_parent(x);
    x->set_parent(y);
    y->left = x;
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
