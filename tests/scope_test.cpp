#include <gpcl/scope_exit.hpp>
#include <gpcl/scope_fail.hpp>
#include <gpcl/scope_success.hpp>

#include "doctest.h"

namespace {

int exit_function_called = 0;

struct exit_function_object
{
  void operator()() { exit_function_called++; }
};

void exit_function()
{
  ++exit_function_called;
}

using exit_function_type = decltype(exit_function);

} // namespace

TEST_CASE("scope_exit")
{
  SUBCASE("function pointer/exit success")
  {
    exit_function_called = 0;
    {
      gpcl::scope_exit<exit_function_type *> se(exit_function);
    }
    CHECK(exit_function_called == 1);
  }

#ifdef __cpp_exceptions
  SUBCASE("function pointer/exit fail")
  {
    exit_function_called = 0;
    try
    {
      gpcl::scope_exit<exit_function_type *> se(exit_function);
      throw std::exception();
    }
    catch (...)
    {
      CHECK(exit_function_called == 1);
    }
    CHECK(exit_function_called == 1);
  }
#endif

  SUBCASE("function ref/exit success")
  {
    exit_function_called = 0;
    {
      gpcl::scope_exit<exit_function_type &> se(exit_function);
    }
    CHECK(exit_function_called == 1);
  }

#ifdef __cpp_exceptions
  SUBCASE("function ref/exit fail")
  {
    exit_function_called = 0;
    try
    {
      gpcl::scope_exit<exit_function_type &> se(exit_function);
      throw std::exception();
    }
    catch (...)
    {
      CHECK(exit_function_called == 1);
    }
    CHECK(exit_function_called == 1);
  }
#endif

  SUBCASE("function object/exit success")
  {
    exit_function_called = 0;
    {
      gpcl::scope_exit<exit_function_object> se(exit_function_object{});
    }
    CHECK(exit_function_called == 1);
  }

#ifdef __cpp_exceptions
  SUBCASE("function object/exit fail")
  {
    exit_function_called = 0;
    try
    {
      gpcl::scope_exit<exit_function_object> se(exit_function_object{});
      throw std::exception();
    }
    catch (...)
    {
      CHECK(exit_function_called == 1);
    }
    CHECK(exit_function_called == 1);
  }
#endif

  SUBCASE("function object ref/exit success")
  {
    exit_function_called = 0;
    exit_function_object exit_function_object_;
    {
      gpcl::scope_exit<exit_function_object &> se(exit_function_object_);
    }
    CHECK(exit_function_called == 1);
  }

#ifdef __cpp_exceptions
  SUBCASE("function object ref/exit fail")
  {
    exit_function_object exit_function_object_;
    exit_function_called = 0;
    try
    {
      gpcl::scope_exit<exit_function_object> se(exit_function_object_);
      throw std::exception();
    }
    catch (...)
    {
      CHECK(exit_function_called == 1);
    }
    CHECK(exit_function_called == 1);
  }
#endif
}

TEST_CASE("scope_success")
{
  SUBCASE("function pointer/exit success")
  {
    exit_function_called = 0;
    {
      gpcl::scope_success<exit_function_type *> se(exit_function);
    }
    CHECK(exit_function_called == 1);
  }

#ifdef __cpp_exceptions
  SUBCASE("function pointer/exit fail")
  {
    exit_function_called = 0;
    try
    {
      gpcl::scope_success<exit_function_type *> se(exit_function);
      throw std::exception();
    }
    catch (...)
    {
      CHECK(exit_function_called == 0);
    }
    CHECK(exit_function_called == 0);
  }
#endif

  SUBCASE("function ref/exit success")
  {
    exit_function_called = 0;
    {
      gpcl::scope_success<exit_function_type &> se(exit_function);
    }
    CHECK(exit_function_called == 1);
  }

#ifdef __cpp_exceptions
  SUBCASE("function ref/exit fail")
  {
    exit_function_called = 0;
    try
    {
      gpcl::scope_success<exit_function_type &> se(exit_function);
      throw std::exception();
    }
    catch (...)
    {
      CHECK(exit_function_called == 0);
    }
    CHECK(exit_function_called == 0);
  }
#endif

  SUBCASE("function object/exit success")
  {
    exit_function_called = 0;
    {
      gpcl::scope_success<exit_function_object> se(exit_function_object{});
    }
    CHECK(exit_function_called == 1);
  }

#ifdef __cpp_exceptions
  SUBCASE("function object/exit fail")
  {
    exit_function_called = 0;
    try
    {
      gpcl::scope_success<exit_function_object> se(exit_function_object{});
      throw std::exception();
    }
    catch (...)
    {
      CHECK(exit_function_called == 0);
    }
    CHECK(exit_function_called == 0);
  }
#endif

  SUBCASE("function object ref/exit success")
  {
    exit_function_called = 0;
    exit_function_object exit_function_object_;
    {
      gpcl::scope_success<exit_function_object &> se(exit_function_object_);
    }
    CHECK(exit_function_called == 1);
  }

#ifdef __cpp_exceptions
  SUBCASE("function object ref/exit fail")
  {
    exit_function_object exit_function_object_;
    exit_function_called = 0;
    try
    {
      gpcl::scope_success<exit_function_object> se(exit_function_object_);
      throw std::exception();
    }
    catch (...)
    {
      CHECK(exit_function_called == 0);
    }
    CHECK(exit_function_called == 0);
  }
#endif
}

TEST_CASE("scope_fail")
{
  SUBCASE("function pointer/exit success")
  {
    exit_function_called = 0;
    {
      gpcl::scope_fail<exit_function_type *> se(exit_function);
    }
    CHECK(exit_function_called == 0);
  }

#ifdef __cpp_exceptions
  SUBCASE("function pointer/exit fail")
  {
    exit_function_called = 0;
    try
    {
      gpcl::scope_fail<exit_function_type *> se(exit_function);
      throw std::exception();
    }
    catch (...)
    {
      CHECK(exit_function_called == 1);
    }
    CHECK(exit_function_called == 1);
  }
#endif

  SUBCASE("function ref/exit success")
  {
    exit_function_called = 0;
    {
      gpcl::scope_fail<exit_function_type &> se(exit_function);
    }
    CHECK(exit_function_called == 0);
  }

#ifdef __cpp_exceptions
  SUBCASE("function ref/exit fail")
  {
    exit_function_called = 0;
    try
    {
      gpcl::scope_fail<exit_function_type &> se(exit_function);
      throw std::exception();
    }
    catch (...)
    {
      CHECK(exit_function_called == 1);
    }
    CHECK(exit_function_called == 1);
  }
#endif

  SUBCASE("function object/exit success")
  {
    exit_function_called = 0;
    {
      gpcl::scope_fail<exit_function_object> se(exit_function_object{});
    }
    CHECK(exit_function_called == 0);
  }

#ifdef __cpp_exceptions
  SUBCASE("function object/exit fail")
  {
    exit_function_called = 0;
    try
    {
      gpcl::scope_fail<exit_function_object> se(exit_function_object{});
      throw std::exception();
    }
    catch (...)
    {
      CHECK(exit_function_called == 1);
    }
    CHECK(exit_function_called == 1);
  }
#endif

  SUBCASE("function object ref/exit success")
  {
    exit_function_called = 0;
    exit_function_object exit_function_object_;
    {
      gpcl::scope_fail<exit_function_object &> se(exit_function_object_);
    }
    CHECK(exit_function_called == 0);
  }

#ifdef __cpp_exceptions
  SUBCASE("function object ref/exit fail")
  {
    exit_function_object exit_function_object_;
    exit_function_called = 0;
    try
    {
      gpcl::scope_fail<exit_function_object> se(exit_function_object_);
      throw std::exception();
    }
    catch (...)
    {
      CHECK(exit_function_called == 1);
    }
    CHECK(exit_function_called == 1);
  }
#endif
}