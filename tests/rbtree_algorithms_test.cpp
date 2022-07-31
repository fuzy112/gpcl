#include <gpcl/rbtree_algorithms.hpp>
#include "doctest.h"
#include <iostream>

struct my_node
{
  int value;
  int color;
  my_node *parent;
  my_node *left;
  my_node *right;

  explicit my_node(int n = -1) noexcept : value(n) {}
};

struct my_rbtree_node_traits
{
  using node_type = my_node;
  using node_pointer = node_type *;
  using const_node_pointer = const node_type *;
  using color_type = std::uintptr_t;

  static color_type red() noexcept { return 0; }

  static color_type black() noexcept { return 1; }

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

using my_rbtree_alg = gpcl::rbtree_algorithms<my_rbtree_node_traits>;

static bool my_node_compare(const my_node *x, const my_node *y) noexcept
{
  GPCL_ASSERT(x && y);
  return x->value < y->value;
}

TEST_CASE("rbtree_algorithms")
{
  my_node header;

  my_rbtree_alg::init_header(&header);

  CHECK(header.parent == nullptr);
  CHECK(header.left == &header);
  CHECK(header.right == &header);

  CHECK(my_rbtree_alg::begin_node(&header) == &header);
  CHECK(my_rbtree_alg::end_node(&header) == &header);

  CHECK(my_rbtree_alg::upper_bound(&header, 1, std::less<int>(),
                                   std::mem_fn(&my_node::value)) == &header);

  my_node n1(1);
  my_rbtree_alg::insert_equal_upper_bound(&header, &n1, my_node_compare);

  CHECK(header.parent == &n1);
  CHECK(header.left == &n1);
  CHECK(header.right == &n1);
  CHECK(n1.parent == &header);
  CHECK(n1.left == nullptr);
  CHECK(n1.right == nullptr);
  CHECK(n1.value == 1);
  CHECK(n1.color == my_rbtree_node_traits::black());

  CHECK(my_rbtree_alg::upper_bound(&header, 1, std::less<int>(),
                                   std::mem_fn(&my_node::value)) == &header);
  CHECK(my_rbtree_alg::upper_bound(&header, 0, std::less<int>(),
                                   std::mem_fn(&my_node::value)) == &n1);

  SUBCASE("insert 1")
  {
    my_node n2(1);

    my_rbtree_alg::insert_equal_upper_bound(&header, &n2, my_node_compare);
    CHECK(my_rbtree_alg::begin_node(&header) == &n1);
    CHECK(my_rbtree_alg::end_node(&header) == &header);
    CHECK(header.left == &n2);
    CHECK(n2.parent == &n1);
    CHECK(n1.right == &n2);
    CHECK(n1.color == 1);
    CHECK(n2.color == 0);
  }

  SUBCASE("insert 2")
  {
    my_node n2(2);

    my_rbtree_alg::insert_equal_upper_bound(&header, &n2, my_node_compare);
    CHECK(my_rbtree_alg::begin_node(&header) == &n1);
    CHECK(my_rbtree_alg::end_node(&header) == &header);
    CHECK(header.left == &n2);
    CHECK(n2.parent == &n1);
    CHECK(n1.right == &n2);
    CHECK(n1.color == 1);
    CHECK(n2.color == 0);
  }

  SUBCASE("insert 0")
  {
    my_node n2(0);

    my_rbtree_alg::insert_equal_upper_bound(&header, &n2, my_node_compare);
    CHECK(my_rbtree_alg::begin_node(&header) == &n2);
    CHECK(my_rbtree_alg::end_node(&header) == &header);
    CHECK(header.left == &n1);
    CHECK(n2.parent == &n1);
    CHECK(n1.right == nullptr);
    CHECK(n1.left == &n2);
    CHECK(n1.color == 1);
    CHECK(n2.color == 0);
  }

  SUBCASE("color")
  {
    my_node n3(3);
    my_node n2(2);

    my_rbtree_alg::insert_equal_upper_bound(&header, &n3, my_node_compare);
    my_rbtree_alg::insert_equal_upper_bound(&header, &n2, my_node_compare);

    CHECK(header.parent == &n2);
    CHECK(n2.color == 1);
    CHECK(n2.parent == &header);
    CHECK(n2.left == &n1);
    CHECK(n2.right == &n3);
    CHECK(header.left == &n3);
    CHECK(header.right == &n1);
    CHECK(n1.parent == &n2);
    CHECK(n1.color == 0);
    CHECK(n1.left == nullptr);
    CHECK(n1.right == nullptr);
    CHECK(n3.color == 0);
    CHECK(n3.parent == &n2);
    CHECK(n3.left == nullptr);
    CHECK(n3.right == nullptr);
  }

  SUBCASE("color 2")
  {
    my_node n2(2);
    my_node n3(3);
    my_rbtree_alg::insert_equal_upper_bound(&header, &n2, my_node_compare);
    my_rbtree_alg::insert_equal_upper_bound(&header, &n3, my_node_compare);

    CHECK(header.parent == &n2);
    CHECK(n2.color == 1);
    CHECK(n2.parent == &header);
    CHECK(n2.left == &n1);
    CHECK(n2.right == &n3);
    CHECK(header.left == &n3);
    CHECK(header.right == &n1);
    CHECK(n1.parent == &n2);
    CHECK(n1.color == 0);
    CHECK(n1.left == nullptr);
    CHECK(n1.right == nullptr);
    CHECK(n3.color == 0);
    CHECK(n3.parent == &n2);
    CHECK(n3.left == nullptr);
    CHECK(n3.right == nullptr);
  }
}

TEST_CASE("rbtree erase")
{
  my_node header;

  my_rbtree_alg::init_header(&header);

  my_node n1(1);
  my_rbtree_alg::insert_equal_upper_bound(&header, &n1, my_node_compare);
  my_node n2(2);
  my_node n3(3);
  my_rbtree_alg::insert_equal_upper_bound(&header, &n2, my_node_compare);
  my_rbtree_alg::insert_equal_upper_bound(&header, &n3, my_node_compare);

  SUBCASE("erase 1 2")
  {
    my_rbtree_alg::erase(&header, &n1);

    CHECK(header.right == &n2);
    CHECK(header.left == &n3);
    CHECK(header.parent == &n2);
    CHECK(n2.left == nullptr);

    my_rbtree_alg::erase(&header, &n2);
    CHECK(header.right == &n3);
    CHECK(header.left == &n3);
    CHECK(header.parent == &n3);
    CHECK(n3.color == 1);
    CHECK(n3.parent == &header);
  }

  SUBCASE("erase 2 3")
  {
    my_rbtree_alg::erase(&header, &n2);

    CHECK(header.parent == &n3);
    CHECK(header.right == &n1);
    CHECK(header.left == &n3);
    CHECK(n3.color == 1);
    CHECK(n3.left == &n1);
    CHECK(n1.parent == &n3);
  }
}

TEST_CASE("rbtree search")
{
  my_node header;

  my_rbtree_alg::init_header(&header);
  my_node n1(1), n2(2), n3(3), n4(4), n22(2);

  my_rbtree_alg::insert_equal_upper_bound(&header, &n1, my_node_compare);
  CHECK(header.parent == &n1);
  CHECK(header.left == &n1);
  CHECK(header.right == &n1);
  my_rbtree_alg::insert_equal_upper_bound(&header, &n2, my_node_compare);
  CHECK(header.parent == &n1);
  CHECK(header.left == &n2);
  CHECK(header.right == &n1);
  CHECK(n1.right == &n2);
  my_rbtree_alg::insert_equal_upper_bound(&header, &n3, my_node_compare);
  CHECK(header.parent == &n2);
  CHECK(header.left == &n3);
  CHECK(header.right == &n1);
  my_rbtree_alg::insert_equal_upper_bound(&header, &n4, my_node_compare);
  CHECK(header.parent == &n2);
  CHECK(header.left == &n4);
  CHECK(header.right == &n1);

  my_node *p = my_rbtree_alg::upper_bound(&header, 2, std::less<int>(),
                                          std::mem_fn(&my_node::value));
  CHECK(p->value == 3);
  CHECK(p == &n3);

  my_rbtree_alg::insert_equal_upper_bound(&header, &n22, my_node_compare);

  CHECK(header.parent == &n2);
  CHECK(header.right == &n1);
  CHECK(header.left == &n4);
  CHECK(n3.left == &n22);
  CHECK(n3.right == &n4);

  auto *n = my_rbtree_alg::upper_bound(&header, 1, std::less<int>(),
                                       std::mem_fn(&my_node::value));
  CHECK(n->value == 2);
  CHECK(my_rbtree_alg::next_node(n)->value == 2);
  CHECK(my_rbtree_alg::upper_bound(&header, 2, std::less<int>(),
                                   std::mem_fn(&my_node::value)) == &n3);

  n = my_rbtree_alg::lower_bound(&header, 1, std::less<int>(),
                                 std::mem_fn(&my_node::value));
  CHECK(n == &n1);

  my_node n222(2);
  my_rbtree_alg::insert_equal_upper_bound(&header, &n222, my_node_compare);

  auto *first = my_rbtree_alg::lower_bound(&header, 2, std::less<int>(),
                                           std::mem_fn(&my_node::value));
  auto *last = my_rbtree_alg::upper_bound(&header, 2, std::less<int>(),
                                          std::mem_fn(&my_node::value));

  CHECK(first == &n2);
  CHECK(last == &n3);
  CHECK(my_rbtree_alg::next_node(first) == &n22);
  CHECK(my_rbtree_alg::prev_node(last) == &n222);

  /*
        1         1          2          2            2                 2
                   \        / \        / \          / \               / \
                    2      1   3      1   3        1   3             1   3
                                           \          / \               / \
                                            4        2   4             2   4
                                                                        \
                                                                         2
  */

  CHECK(header.right == &n1);
  n = my_rbtree_alg::next_node(&n1);
  CHECK(n == &n2);
  n = my_rbtree_alg::next_node(&n2);
  CHECK(n == &n22);
  n = my_rbtree_alg::next_node(&n22);
  CHECK(n == &n222);
  n = my_rbtree_alg::next_node(&n222);
  CHECK(n == &n3);
  n = my_rbtree_alg::next_node(&n3);
  CHECK(n == &n4);
  n = my_rbtree_alg::next_node(&n4);
  CHECK(n == &header);
}
