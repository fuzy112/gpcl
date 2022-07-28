#include <gpcl/offset_ptr.hpp>
#include <gpcl/rbtree.hpp>
#include "doctest.h"

#include <random>

class my_tag;

class MyClass
    : public gpcl::rbtree_node<MyClass>,
      public gpcl::rbtree_node<MyClass, my_tag, gpcl::offset_ptr<void>>
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

  gpcl::rbtree<MyClass> tree;

  tree.insert(o1);
  tree.insert(o2);

  // dump_tree(tree);
  tree.insert(o3);
  tree.insert(o4);
  tree.insert(o5);
  tree.insert(o6);

  // dump_tree(tree);
  // // tree.dump();

  dump_tree(tree);

  tree.remove(o3);

  dump_tree(tree);
  // tree.dump();
}

TEST_CASE("rbtree with fancy pointer")
{
  using my_tree =
      gpcl::rbtree<MyClass, my_tag, std::less<MyClass>, gpcl::offset_ptr<void>>;
  my_tree tree;

  MyClass o1(1);
  MyClass o2(2);
  MyClass o3(3);
  MyClass o4(4);
  MyClass o5(5);
  MyClass o6(6);

  tree.insert(o1);
  tree.insert(o2);

  // dump_tree(tree);
  tree.insert(o3);
  tree.insert(o4);
  tree.insert(o5);
  tree.insert(o6);

  // dump_tree(tree);
  // // tree.dump();

  dump_tree(tree);

  tree.remove(o3);

  dump_tree(tree);

  // for (auto &x : std::as_const(tree))
  // {
  //   std::cout << x.value << '\n';
  // }

  // std::for_each(
  //   tree.crbegin(),
  //   tree.crend(),
  //   [](const MyClass &x) {
  //     std::cout << x.value << '\n';
  //   }
  // );
}
