#include <gpcl/offset_ptr.hpp>
#include <gpcl/rbtree.hpp>
#include "doctest.h"

#include <iostream>
#include <random>

class my_tag;

class MyClass
    : public gpcl::rbtree_base_hook<MyClass>,
      public gpcl::rbtree_base_hook<MyClass, gpcl::tag<my_tag>,
                                    gpcl::void_pointer<gpcl::offset_ptr<void>>>
{
public:
  int value;

  explicit MyClass(int v) : value(v) {}

  bool operator<(const MyClass &other) const { return value < other.value; }
};

std::ostream &operator<<(std::ostream &os, const MyClass &o)
{
  return os << o.value;
}

TEST_CASE("rbtree")
{
  MyClass o1(1);
  MyClass o2(2);
  MyClass o3(3);
  MyClass o4(4);
  MyClass o5(5);
  MyClass o6(6);

  gpcl::rbtree<MyClass, gpcl::rbtree_hook_traits<gpcl::rbtree_base_hook<MyClass>>>
      tree;

  tree.insert_equal(o1);
  tree.insert_equal(o2);

  // dump_tree(tree);
  tree.insert_equal(o3);
  tree.insert_equal(o4);
  tree.insert_equal(o5);
  tree.insert_equal(o6);

  // dump_tree(tree);
  // // tree.dump();

  // dump_tree(tree);

  tree.erase(o3);

  // dump_tree(tree);
  // tree.dump();
}

TEST_CASE("rbtree with fancy pointer")
{
  using my_tree =
      gpcl::rbtree<MyClass, gpcl::rbtree_hook_traits<gpcl::rbtree_base_hook<
                                MyClass, gpcl::tag<my_tag>,
                                gpcl::void_pointer<gpcl::offset_ptr<void>>>>>;
  my_tree tree;

  MyClass o1(1);
  MyClass o2(2);
  MyClass o3(3);
  MyClass o4(4);
  MyClass o5(5);
  MyClass o6(6);

  tree.insert_equal(o1);
  tree.insert_equal(o2);

  // dump_tree(tree);
  tree.insert_equal(o3);
  tree.insert_equal(o4);
  tree.insert_equal(o5);
  tree.insert_equal(o6);

  CHECK(tree.size() == 6);

  // dump_tree(tree);
  // // tree.dump();

  // dump_tree(tree);

  tree.erase(o3);

  CHECK(tree.size() == 5);

  // dump_tree(tree);

  for (auto &x : tree)
  {
    std::cout << x.value << '\n';
  }

  for (auto &x : std::as_const(tree))
  {
    std::cout << x.value << '\n';
  }

  // std::for_each(
  //   tree.crbegin(),
  //   tree.crend(),
  //   [](const MyClass &x) {
  //     std::cout << x.value << '\n';
  //   }
  // );
}
