#ifndef GPCL_RBTREE_HPP
#define GPCL_RBTREE_HPP

#include <gpcl/detail/assert.hpp>
#include <gpcl/detail/compressed_pointer.hpp>
#include <gpcl/detail/config.hpp>
#include <gpcl/lexical_cast.hpp>

#include <fstream>
#include <functional>

namespace gpcl {

template <typename T, typename Tag, typename Compare>
class rbtree_base;

template <typename T, typename Tag, typename Compare>
class rbtree_node;

template <typename T, typename Tag, typename Compare>
class rbtree_iterator;

template <typename T, typename Tag, typename Compare>
class rbtree_const_iterator;

template <typename T, typename Tag, typename Compare>
class rbtree;

template <typename T, typename Tag, typename Compare>
class rbtree_base
{
  friend rbtree<T, Tag, Compare>;
  friend rbtree_iterator<T, Tag, Compare>;
  friend rbtree_const_iterator<T, Tag, Compare>;

public:
  using node_type = rbtree_node<T, Tag, Compare>;

  node_type *left;

  constexpr rbtree_base() noexcept : left() {}

  rbtree_base(const rbtree_base &) = delete;
  rbtree_base &operator=(const rbtree_base &) = delete;
};

template <typename T, typename Tag = class default_tag,
          typename Compare = std::less<T>>
class rbtree_node : public rbtree_base<T, Tag, Compare>
{
  friend rbtree<T, Tag, Compare>;

private:
  using base_type = rbtree_base<T, Tag, Compare>;

  using pointer = rbtree_node *;
  using const_pointer = const rbtree_node *;

  rbtree_node *right;
  base_type *parent;
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



  void set_parent(rbtree_node *p) noexcept { parent = p; }

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

  template <typename NodePtr>
  friend std::string dump_node(NodePtr x, std::ostream &out)
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

template <typename T, typename Tag, typename Compare>
class rbtree_iterator
{
public:
  using value_type = T;
  using node_type = rbtree_node<T, Tag, Compare>;
  using tag = Tag;
  using compare = Compare;
  using reference = value_type &;
  using pointer = value_type *;

  using iterator_category = std::bidirectional_iterator_tag;

private:
  node_type *node_;

public:
  constexpr rbtree_iterator() noexcept : node_() {}

  constexpr rbtree_iterator(node_type *node) noexcept : node_(node) {}

  rbtree_iterator &operator++() noexcept
  {
 
 } 
};

template <typename T, typename Tag = class default_tag,
          typename Compare = std::less<T>>
class rbtree
{
public:
  using value_type = T;
  using node_type = rbtree_node<T, Tag, Compare>;
  using base_type = rbtree_base<T, Tag, Compare>;
  using compare = Compare;

  using reference = T &;
  using const_reference = const T &;
  using pointer = T *;
  using const_pointer = const T *;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

private:
  detail::compressed_pair<base_type, compare> p_;

  base_type *get_base() noexcept { return &p_.first(); }

  const base_type *get_base() const noexcept { return &p_.first(); }

public:
  constexpr rbtree() noexcept = default;

  rbtree(const rbtree &) = delete;
  rbtree &operator=(const rbtree &) = delete;

  T *root() const noexcept { return static_cast<T *>(p_.first().left); }

  compare comp() const noexcept { return p_.second(); }

  template <typename K>
  T *find(const K &k) const noexcept
  {
    node_type *p = root();
    auto cmp = comp();

    while (p != nullptr)
    {
      if (cmp(k, p->value()))
        p = p->left;
      else if (cmp(p->value(), k))
        p = p->right;
      else
        return static_cast<T *>(p);
    }

    return nullptr;
  }

  void remove(node_type *const x) noexcept
  {
    // y will be node to delete
    // either x or x's successor
    node_type *y = (x->right && x->left) ? x->right : x;
    if (y != x)
    {
      // find x's successor
      while (y->left)
        y = y->left;
    }

    // z is y's (possible null) single child
    node_type *z = y->left ? y->left : y->right;

    // w is z's (possible null) uncle and will be z's sibling
    node_type *w = nullptr;
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
        x->left->parent = y;
      if (x->right != nullptr)
        x->right->parent = y;
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

  void insert(T *x) noexcept
  {
    GPCL_ASSERT(x != nullptr);
    node_type **link = &get_base()->left;
    ;
    node_type *p = static_cast<node_type *>(get_base());

    auto cmp = comp();

    while (*link != nullptr)
    {
      p = *link;
      if (cmp(*x, p->value()))
        link = &p->left;
      else
        link = &p->right;
    }

    x->parent = p;
    *link = x;

    x->left = nullptr;
    x->right = nullptr;

    rebalance_after_insert(x);
  }

  template <typename K>
  T *find_or_insert(const K &k, T *x)
  {
    GPCL_ASSERT(x != nullptr);
    node_type **link = &get_base()->left;
    node_type *p = static_cast<node_type *>(get_base());

    auto cmp = comp();

    while (*link != nullptr)
    {
      p = *link;
      if (cmp(k, p->value()))
        link = &p->left;
      else if (cmp(p->value(), k))
        link = &p->right;
      else
        return static_cast<T *>(p);
    }

    x->parent = p;
    *link = x;

    x->left = nullptr;
    x->right = nullptr;

    rebalance_after_insert(x);

    return nullptr;
  }

  template <typename K>
  const T *lower_bound(const K &k) const noexcept
  {
    const node_type *p = 0;
    const node_type *const *link = &get_base()->left;
    auto cmp = comp();

    while (*link)
    {
      p = *link;

      if (cmp(k, p->value()))
        link = &p->left;
      else
        link = &p->right;
    }

    return static_cast<const T *>(p);
  }

private:
  void rebalance_after_insert(node_type *x) noexcept
  {
    x->is_black = x == root();
    while (x != root() && !x->parent_unsafe()->is_black)
    {
      if (x->parent_unsafe()->is_left_child_of_parent())
      {
        node_type *y = x->parent_unsafe()->parent_unsafe()->right;
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
        node_type *y = x->parent_unsafe()->parent_unsafe()->left;
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

  void rotate_right(node_type *x) noexcept
  {
    GPCL_ASSERT(x);

    node_type *y = x->left;
    y->parent = x->parent;

    if (x->is_left_child_of_parent())
      x->parent->left = y;
    else
      x->parent_unsafe()->right = y;

    x->left = y->right;
    if (y->right != nullptr)
      y->right->parent = x;

    x->parent = y;
    y->right = x;
  }

  void rotate_left(node_type *x) noexcept
  {
    GPCL_ASSERT(x);

    node_type *y = x->right;
    y->parent = x->parent;

    if (x->is_left_child_of_parent())
      x->parent->left = y;
    else
      x->parent_unsafe()->right = y;

    x->right = y->left;
    if (y->left != nullptr)
      y->left->parent = x;
    x->parent = y;
    y->left = x;
  }
};

template <typename Tree>
void dump_tree(Tree &tree)
{
  std::string tmpfile = tmpnam(NULL);
  std::ofstream file(tmpfile);
  file << "digraph {\n";
  file << "ordering=out\n";
  dump_node(tree.root(), file);
  file << "}\n";

  std::clog << "Output to " << tmpfile << '\n';
  file.close();
  std::string cmd = "dotty " + tmpfile + " &";
  std::system(cmd.c_str());
}

} // namespace gpcl

#endif // GPCL_RBTREE_HPP
