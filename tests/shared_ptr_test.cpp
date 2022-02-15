#include "doctest.h"

#include <gpcl/default_allocator.hpp>
#include <gpcl/enable_shared_from_this.hpp>
#include <gpcl/shared_ptr.hpp>

using gpcl::allocate_shared;
using gpcl::make_shared;
using gpcl::shared_ptr;

GPCL_MSVC_SUPPRESS_WARNING(26800)

TEST_CASE("shared_ptr constructors")
{
  auto not_default_construtible = []() {};

  SUBCASE("default construction")
  {
    shared_ptr<decltype(not_default_construtible)> p1;
    shared_ptr<decltype(not_default_construtible)> p2 = nullptr;
    shared_ptr<decltype(not_default_construtible)> p3(
        nullptr, [](decltype(not_default_construtible) *p) { delete p; });
    shared_ptr<decltype(not_default_construtible)> p4(
        nullptr, [](decltype(not_default_construtible) *p) { delete p; },
        gpcl::default_allocator<char>());

    REQUIRE(!p1);
    REQUIRE(!p2);
    REQUIRE(!p3);
    REQUIRE(!p4);
  }

  SUBCASE("copy constructor")
  {
    shared_ptr<decltype(not_default_construtible)> p1;

    shared_ptr<decltype(not_default_construtible)> p2 = p1;
    shared_ptr<decltype(not_default_construtible)> p3(p1);
    // REQUIRE(p1 == p2);
    // REQUIRE(p2 == p3);

    shared_ptr<int> p4(new int);
    shared_ptr<int> p5(p4);
    REQUIRE(p4.get() == p5.get());
    // REQUIRE(p4 == p5);

    shared_ptr<int> p6 = make_shared<int>();
    shared_ptr<int> p7(p6);

    REQUIRE(p6.get() == p7.get());
    // REQUIRE(p6 == p7);

    shared_ptr<int> p8(new int, [](auto *p) { delete p; });
    shared_ptr p9 = p8;
    REQUIRE(p9.get() == p8.get());
    REQUIRE(p9.use_count() == 2);

    shared_ptr<int> p10(
        new int, [](auto *p) { delete p; }, gpcl::default_allocator<char>());
    shared_ptr p11 = p10;
    REQUIRE(p11.get() == p10.get());
    REQUIRE(p11.use_count() == 2);
  }

  SUBCASE("move constructor")
  {
    shared_ptr<decltype(not_default_construtible)> p1;

    shared_ptr<decltype(not_default_construtible)> p2 = std::move(p1);
    shared_ptr<decltype(not_default_construtible)> p3(std::move(p1));

    shared_ptr<int> p4(new int);
    auto p4_a = p4.get();
    shared_ptr<int> p5(std::move(p4));
    REQUIRE(!p4);
    REQUIRE(p4_a == p5.get());
    REQUIRE(p5.use_count() == 1);

    shared_ptr<int> p6(make_shared<int>(42));
    auto p6_a = p6.get();
    shared_ptr<int> p7(std::move(p6));
    REQUIRE(!p6);
    REQUIRE(p6_a == p7.get());
    REQUIRE(*p6_a == 42);
    REQUIRE(p7.use_count() == 1);
  }

  SUBCASE("pointer to Y")
  {
    shared_ptr<int> p1 = make_shared<int>(42);
    int n = 43;

    shared_ptr<int> p2(p1, &n);
    REQUIRE(*p2 == 43);
    REQUIRE(p2.get() == &n);
    REQUIRE(p1.use_count() == 2);

    REQUIRE_FALSE(p1.owner_before(p2));
    REQUIRE_FALSE(p2.owner_before(p1));
  }

  SUBCASE("make_shared")
  {
    shared_ptr<int> p1 = make_shared<int>(129);
    REQUIRE(*p1 == 129);

    shared_ptr<int> p2 = make_shared<int>();
    REQUIRE(*p2 == 0);
  }

  SUBCASE("shared_ptr<void>")
  {
    shared_ptr<void> p1(new int);
    shared_ptr<void> p2 = make_shared<int>();
    shared_ptr<void> p3 =
        gpcl::allocate_shared<int>(gpcl::default_allocator<char>());
  }
}

TEST_CASE("enable_shared_from_this")
{
  class MyBase
  {
  public:
    virtual ~MyBase() {}
  };

  class MyClass : public MyBase, public gpcl::enable_shared_from_this<MyClass>
  {
  };

  shared_ptr<MyClass> p1(new MyClass);
  p1->shared_from_this();
  REQUIRE(p1.get() == p1->shared_from_this().get());

  shared_ptr<MyClass> p2 = make_shared<MyClass>();
  REQUIRE(p2.get() == p2->shared_from_this().get());
  p2->shared_from_this();

  shared_ptr<MyClass> p3 =
      gpcl::allocate_shared<MyClass>(gpcl::default_allocator<char>());
  auto p4 = p3->shared_from_this();
  REQUIRE(p3.get());

  auto *p5 = new MyClass;
  shared_ptr<MyBase> p6(p5);
  REQUIRE(p5 == p5->shared_from_this().get());
}

#include <gpcl/enable_shared_from.hpp>

TEST_CASE("enable_shared_from")
{
  class Y : public gpcl::enable_shared_from
  {
  public:
    shared_ptr<Y> f() { return gpcl::shared_from(this); }
  };

  shared_ptr<Y> p(new Y);
  shared_ptr<Y> q = p->f();

  REQUIRE(p.get() == q.get());
  // REQUIRE(!(p < q || q < p)); // p and q must share ownership
}

#include <thread>
#include <vector>

TEST_CASE("weak_ptr")
{

  SUBCASE("not-expired")
  {
    auto sp = gpcl::make_shared<int>(42);

    std::vector<std::thread> threads;

    for (int i = 0; i != 10; ++i)
    {
      threads.emplace_back([sp]() mutable {
        REQUIRE(*sp == 42);
        sp.reset();
      });
    }

    for (int i = 0; i != 10; ++i)
    {
      threads.emplace_back([wp = gpcl::weak_ptr<int>(sp)]() mutable {
        if (auto sp = wp.lock())
        {
          REQUIRE(*sp == 42);
        }
      });
    }

    sp.reset();

    for (auto &t : threads)
    {
      t.join();
    }
  }

  SUBCASE("expired")
  {
    auto sp = make_shared<int>(3);
    auto wp = gpcl::weak_ptr<int>(sp);
    sp.reset();

    REQUIRE(wp.expired());

    std::vector<std::thread> threads;

    for (int i = 0; i != 10; ++i)
    {
      threads.emplace_back([wp = wp]() mutable {
        for (int j = 0; j != 1000; ++j)
          REQUIRE(!wp.lock());
      });
    }

    for (auto &t : threads)
    {
      t.join();
    }
  }

#if !defined GPCL_CONFIG_NO_RTTI
  SUBCASE("dynamic_pointer_cast")
  {
    class A
    {
    public:
      virtual ~A() = default;
    };
    class B : public A
    {
    };

    auto sp = make_shared<B>();
    auto wp = gpcl::weak_ptr<A>(sp);

    SUBCASE("not expired")
    {
      auto p = gpcl::dynamic_pointer_cast<B>(wp);
      REQUIRE(p.lock());
    }

    SUBCASE("expired")
    {
      sp.reset();

      auto p = gpcl::dynamic_pointer_cast<B>(wp);
      REQUIRE(!p.lock());
    }
  }
#endif
}
