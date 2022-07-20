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
    red = 0,
    black = 1,
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

  const T& value() const noexcept { return static_cast<const T &>(*this); }

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

  int pos_;

public:
  constexpr rbtree() noexcept = default;

  constexpr explicit rbtree(compare cmp) : cmp_(cmp) {}

  void insert(node_type& node)
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
    dump();

    fixup_tree(&node);

    dump();
  }

  template <typename K>
  node_type *find(const K &key) const
  {
  	node_type *parent = root_;
  	while (parent) {
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
  	while (node) {
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
  	dump_node (root_, ofile);
   	ofile << "}\n";
  	ofile.close();
  	std::string cmd = "dotty " + name + "&";
  	std::system(cmd.c_str());
  }

private:
  int node_id = 0;

  void dump_node (node_type *node, std::ostream &out)
  {
  	if (node)
  	{
  	  out << "node" << node << "[label=" << std::quoted(gpcl::lexical_cast<std::string>(node->value())) << " color=" << (node->color() == node_type::red ? "red" : "black") << "];\n";

  	  // if (node->rb_parent) {
  	  // 	out << node->rb_parent->value() << " -> " << node->value() << '\n';
  	  // }

  	  if (node->rb_left)
  	  {
  	  	GPCL_ASSERT(node->rb_left->rb_parent == node);
  	  	out << "node" << node << " -> " << "node" << node->rb_left  << "\n";
  	  }
  	  else
  	  {
  	  	out << "node" << ++node_id << "[label=nil]\n";
  	  	out << "node" << node  << " -> node" << node_id << "\n";
  	  }
  	  if (node->rb_right)
  	  {
  	  	GPCL_ASSERT(node->rb_right->rb_parent == node);
  	  	out <<  "node" << node << " -> " << "node" << node->rb_right << "\n";
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

  void link_nodes(node_type *node, node_type *parent, node_type *&link)
  {
    node->rb_parent = parent;
    node->color(node_type::red);
    link = node;
    node->rb_left = nullptr;
    node->rb_right = nullptr;
  }

  void fixup_tree(node_type *node)
  {
    node_type *parent = node->rb_parent;
    node_type *gparent = nullptr;

    while (node != root_ && parent && parent->color() == node_type::red)
    {
      gparent = parent->rb_parent;
      GPCL_ASSERT(gparent && "gparent must not be null because parent is red");
      // GPCL_ASSERT(gparent->color() == node_type::black);
      if (gparent->rb_right != parent)
      {
        // parent is left child of gparent

        node_type *uncle = gparent->rb_right;

        if (uncle && uncle->color() == node_type::red)
        {
          // uncle is red
          // case 1
          parent->color(node_type::black);
          uncle->color(node_type::black);
          gparent->color(node_type::red);

          node = gparent;
          parent = node->rb_parent;
          continue;
        }

        // uncle is black
        if (node == parent->rb_right)
        {
          // node is right child of parent
          // case 2

          // left rotate parent
          node = parent;
          left_rotate(node);
          parent = node->rb_parent;
          gparent = parent->rb_parent;
          // continue;
        }

        // node is left child of parent
        // case 3

        parent->color(node_type::black);
        gparent->color(node_type::red);
        // right rotate gparent
        right_rotate(gparent);
        break;
      }
      else
      {
        // parent is right child of gparent
        node_type *uncle = gparent->rb_left;

        if (uncle && uncle->color() == node_type::red)
        {
          // uncle is red

          // case 1
          parent->color(node_type::black);
          uncle->color(node_type::black);
          gparent->color(node_type::red);

          node = gparent;
          parent = node->rb_parent;
          continue;
        }

        // uncle is black
        if (node == parent->rb_left)
        {
          // node is left child of parent

          node = parent;
          right_rotate(node);
          parent = node->rb_parent;
          gparent = parent->rb_parent;
          // continue;
        }

        // node is right child of parent

        parent->color(node_type::black);
        gparent->color(node_type::red);
        // rotate
        left_rotate(gparent);
        break;
      }
    }

    root_->color(node_type::black);
    root_->rb_parent = nullptr;
  }

  void left_rotate(node_type *node)
  {
    node_type *sibling = node->rb_right;
    node->rb_right = sibling->rb_left;
    if (sibling->rb_left != nullptr)
    {
      sibling->rb_left->rb_parent = node;
    }
    sibling->rb_parent = node->rb_parent.get();
    if (node->rb_parent == nullptr)
    {
      root_ = sibling;
    }
    else
    {
      if (node == node->rb_parent->rb_left)
      {
        node->rb_parent->rb_left = sibling;
      }
      else
      {
        node->rb_parent->rb_right = sibling;
      }
    }
    sibling->rb_left = node;
    node->rb_parent = sibling;
  }

   void right_rotate(node_type *node)
  {
    node_type *sibling = node->rb_left;
    node->rb_left = sibling->rb_right;
    if (sibling->rb_right != nullptr)
    {
      sibling->rb_right->rb_parent = node;
    }
    sibling->rb_parent = node->rb_parent.get();
    if (node->rb_parent == nullptr)
    {
      root_ = sibling;
    }
    else
    {
      if (node == node->rb_parent->rb_right)
      {
        node->rb_parent->rb_right = sibling;
      }
      else
      {
        node->rb_parent->rb_left = sibling;
      }
    }
    sibling->rb_right = node;
    node->rb_parent = sibling;
  }

};

} // namespace gpcl

#endif // GPCL_RBTREE_HPP
