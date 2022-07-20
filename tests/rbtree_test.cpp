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

	gpcl::rbtree<MyClass> tree;

	tree.insert(o1);
	tree.insert(o3);


	// tree.dump();

	CHECK(tree.find(o1) == &o1);
	CHECK(tree.find(o2) == nullptr);
	tree.insert(o2);

	// tree.dump();
	CHECK(tree.find(o2) == &o2);

	std::vector<MyClass> vec;
	std::random_device rd;
	for (int i=0; i < 10; ++i) {
		vec.emplace_back(rd() % 100);
	}
	for (auto &o : vec) {
		tree.insert(o);
	}
	tree.dump();
}
