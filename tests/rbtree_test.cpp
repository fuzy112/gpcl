#include "doctest.h"
#include <gpcl/rbtree.hpp>

#include <random>

class MyClass : public gpcl::rbtree_node<MyClass>
{
public:
	int value;

	explicit MyClass (int v) : value(v) {}

	bool operator<(const MyClass &other) const
	{
		return value < other.value;
	}
};

std::ostream& operator<<(std::ostream& os, const MyClass &o)
{
  return os << o.value;
}

TEST_CASE ("rbtree") {
	MyClass o1(1);
	MyClass o2(2);
	MyClass o3(3);
	MyClass o4(4);
	MyClass o5(5);
	MyClass o6(6);

	gpcl::rbtree<MyClass> tree;

	tree.insert(&o1);
	tree.insert(&o2);

	// dump_tree(tree);
	tree.insert(&o3);
	tree.insert(&o4);
	tree.insert(&o5);
	tree.insert(&o6);

	// dump_tree(tree);
	// // tree.dump();

	dump_tree(tree);

	tree.remove(&o3);
	
	dump_tree(tree);
	// tree.dump();
}
