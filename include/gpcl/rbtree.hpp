#ifndef GPCL_RBTREE_HPP
#define GPCL_RBTREE_HPP

#include <gpcl/detail/assert.hpp>
#include <gpcl/detail/compressed_pointer.hpp>
#include <gpcl/detail/config.hpp>
#include <gpcl/lexical_cast.hpp>

#include <fstream>
#include <functional>

namespace gpcl {

template <typename T, typename Compare, typename Tag>
class rbtree;

template <typename T, typename Tag = class default_tag>
class rbtree_node
{
  template <typename, typename, typename>
  friend class rbtree;

public:
  enum color_type
  {
    red = false,
    black = true,
  };

protected:
  constexpr rbtree_node() = default;

  constexpr rbtree_node(const rbtree_node &other) noexcept {}

  rbtree_node &operator=(const rbtree_node &other) noexcept { return *this; }

  ~rbtree_node() = default;

private:
  color_type color() const noexcept
  {
    return static_cast<color_type>(rb_parent.flags() & 1);
  }

  void color(color_type c) noexcept { rb_parent.flags(c); }

  T &value() noexcept { return static_cast<T &>(*this); }

  const T &value() const noexcept { return static_cast<const T &>(*this); }

  detail::compressed_pointer<rbtree_node, alignof(void *)> rb_parent;
  rbtree_node *rb_left{};
  rbtree_node *rb_right{};
};

template <typename T, typename Compare = std::less<T>,
          typename Tag = class default_tag>
class rbtree
{
public:
  using tag = Tag;
  using node_type = rbtree_node<T, Tag>;
  using value_type = T;
  using compare = Compare;

private:
  node_type *root_{};
  compare cmp_;

public:
  constexpr rbtree() noexcept = default;

  constexpr explicit rbtree(compare cmp) : cmp_(cmp) {}

  void insert(node_type &node)
  {
    node_type **link = &root_;
    node_type *parent = nullptr;

    while (*link)
    {
      parent = *link;
      if (cmp_(node.value(), parent->value()))
        link = &parent->rb_left;
      else
        link = &parent->rb_right;
    }

    link_nodes(&node, parent, *link);
    rebalance_after_insert(&node);
  }

  template <typename K>
  node_type *find(const K &key) const
  {
    node_type *parent = root_;
    while (parent)
    {
      if (cmp_(key, parent->value()))
        parent = parent->rb_left;
      else if (cmp_(parent->value(), key))
        parent = parent->rb_right;
      else
        return parent;
    }

    return nullptr;
  }

  node_type *first()
  {
    node_type *node = root_;
    node_type *first = nullptr;
    while (node)
    {
      first = node;
      node = node->rb_left;
    }
    return first;
  }

  void dump()
  {
    std::string name = tmpnam(NULL);
    std::ofstream ofile(name);
    ofile << "digraph {\n";
    ofile << "ordering=\"out\"\n";
    ofile << "node" << root_ << "[root=true]\n";
    dump_node(root_, ofile);
    ofile << "}\n";
    ofile.close();
    std::string cmd = "dotty " + name + "&";
    std::system(cmd.c_str());
  }

private:
  int node_id = 0;

  void dump_node(node_type *node, std::ostream &out)
  {
    if (node)
    {
      out << "node" << node << "[label="
          << std::quoted(gpcl::lexical_cast<std::string>(node->value()))
          << " color=" << (node->color() == node_type::red ? "red" : "black")
          << "];\n";

      if (node->rb_left)
      {
        GPCL_ASSERT(node->rb_left->rb_parent == node);
        out << "node" << node << " -> "
            << "node" << node->rb_left << "\n";
      }
      else
      {
        out << "node" << ++node_id << "[label=nil]\n";
        out << "node" << node << " -> node" << node_id << "\n";
      }

      if (node->rb_right)
      {
        GPCL_ASSERT(node->rb_right->rb_parent == node);
        out << "node" << node << " -> "
            << "node" << node->rb_right << "\n";
      }
      else
      {
        out << "node" << ++node_id << "[label=nil]\n";
        out << "node" << node << " -> node" << node_id << "\n";
      }

      dump_node(node->rb_left, out);
      dump_node(node->rb_right, out);
    }
  }

  void link_nodes(node_type *node, node_type *parent, node_type *&link) noexcept
  {
    node->rb_parent = parent;
    node->color(node_type::red);
    link = node;
    node->rb_left = nullptr;
    node->rb_right = nullptr;
  }

  void rebalance_after_insert(node_type *x) noexcept
  {
    GPCL_ASSERT(x != nullptr);
    GPCL_ASSERT(x->rb_left == nullptr && x->rb_right == nullptr);

    x->color((x == root_) ? node_type::black : node_type::red);

    while (x != root_ && x->rb_parent->color() == node_type::red)
    {
      if (x->rb_parent->rb_parent->rb_right != x->rb_parent)
      {
        node_type *y = x->rb_parent->rb_parent->rb_right;
        if (y != nullptr && y->color() == node_type::red)
        {
          x = x->rb_parent;
          x->color(node_type::black);
          x = x->rb_parent;
          x->color(x == root_ ? node_type::black : node_type::red);
          y->color(node_type::black);
        }
        else
        {
          if (x == x->rb_parent->rb_right)
          {
            x = x->rb_parent;
            left_rotate(x);
          }
          x = x->rb_parent;
          x->color(node_type::black);
          x = x->rb_parent;
          x->color(node_type::red);
          right_rotate(x);
          break;
        }
      }
      else
      {
        node_type *y = x->rb_parent->rb_parent->rb_left;
        if (y != nullptr && y->color() == node_type::red)
        {
          x = x->rb_parent;
          x->color(node_type::black);
          x = x->rb_parent;
          x->color(x == root_ ? node_type::black : node_type::red);
          y->color(node_type::black);
        }
        else
        {
          if (x == x->rb_parent->rb_left)
          {
            x = x->rb_parent;
            right_rotate(x);
          }
          x = x->rb_parent;
          x->color(node_type::black);
          x = x->rb_parent;
          x->color(node_type::red);
          left_rotate(x);
          break;
        }
      }
    }
  }

  void left_rotate(node_type *x) noexcept
  {
    node_type *y = x->rb_right;
    x->rb_right = y->rb_left;
    if (y->rb_left != nullptr)
      y->rb_left->rb_parent = x;

    y->rb_parent = x->rb_parent.get();
    if (x->rb_parent == nullptr)
    {
      root_ = y;
    }
    else
    {
      if (x == x->rb_parent->rb_left)
        x->rb_parent->rb_left = y;
      else
        x->rb_parent->rb_right = y;
    }
    y->rb_left = x;
    x->rb_parent = y;
  }

  void right_rotate(node_type *x) noexcept
  {
    node_type *y = x->rb_left;
    x->rb_left = y->rb_right;
    if (y->rb_right != nullptr)
      y->rb_right->rb_parent = x;

    y->rb_parent = x->rb_parent.get();
    if (x->rb_parent == nullptr)
    {
      root_ = y;
    }
    else
    {
      if (x == x->rb_parent->rb_right)
        x->rb_parent->rb_right = y;
      else
        x->rb_parent->rb_left = y;
    }
    y->rb_right = x;
    x->rb_parent = y;
  }

public:
  void remove (node_type *z) noexcept
  {

  }
};

} // namespace gpcl

#endif // GPCL_RBTREE_HPP
