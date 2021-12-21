//
// posix_thread.ipp
// ~~~~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <gpcl/assert.hpp>
#include <gpcl/detail/chrono.hpp>
#include <gpcl/detail/error.hpp>
#include <gpcl/detail/posix_thread.hpp>
#include <gpcl/expected.hpp>
#include <signal.h>

#ifdef GPCL_POSIX
#  include <sys/select.h>

#  ifdef GPCL_USE_BOOST_SYSTEM_ERROR
#    include <boost/core/no_exceptions_support.hpp>
#    include <boost/exception/diagnostic_information.hpp>
#  endif

namespace gpcl {
namespace detail {

struct posix_thread_attributes : noncopyable
{
  pthread_attr_t attr_{};

  explicit posix_thread_attributes(const thread_attributes &attr)
  {
    int err;
    const sched_param param{attr.priority()};
    if ((err = pthread_attr_init(&attr_)) != 0)
      throw_system_error(err, "pthread_attr_init");

    if ((err = pthread_attr_setstacksize(&attr_, attr.stack_size())) != 0)
      throw_system_error(err, "pthread_attr_setstacksize");

    if (param.sched_priority != 0)
    {
      err = pthread_attr_setinheritsched(&attr_, PTHREAD_EXPLICIT_SCHED);
      if (err != 0)
        throw_system_error(err, "pthread_attr_setinheritsched");

      int schedpolicy = SCHED_OTHER;
      switch (attr.schedule_policy())
      {
      case schedule_policy::other:
        schedpolicy = SCHED_OTHER;
        break;

      case schedule_policy::fifo:
        schedpolicy = SCHED_FIFO;
        break;

      case schedule_policy::round_robin:
        schedpolicy = SCHED_RR;
        break;

      default:
        GPCL_UNREACHABLE("invalid schedule policy");
      }

      err = pthread_attr_setschedpolicy(&attr_, schedpolicy);
      if (err != 0)
        throw_system_error(err, "pthread_attr_setschedpolicy");

      err = pthread_attr_setschedparam(&attr_, &param);
      if (err != 0)
        throw_system_error(err, "pthread_attr_setschedparam");
    }
  }

  ~posix_thread_attributes() noexcept
  {
    GPCL_VERIFY_0(pthread_attr_destroy(&attr_));
  }

  [[nodiscard]] pthread_attr_t const *get() const { return &attr_; }
};

#  ifdef GPCL_POSIX
void posix_thread::start_thread(thread_attributes const &attr,
                                unique_ptr<func_base> fn)
{
  posix_thread_attributes attr1(attr);

  int err =
      pthread_create(&thread_, attr1.get(), posix_thread_function, fn.get());
  if (err)
  {
    throw_system_error(err, "pthread_create");
  }
  (void)fn.release();
}

void *posix_thread_function(void *arg) noexcept
{
  GPCL_ASSERT(arg != nullptr);
  auto fn = unique_ptr<posix_thread::func_base>(
      reinterpret_cast<posix_thread::func_base *>(arg));

  sigset_t set;
  sigemptyset(&set);
  sigaddset(&set, SIGINT);
  sigaddset(&set, SIGTERM);
  pthread_sigmask(SIG_BLOCK, &set, nullptr);

#    ifdef GPCL_USE_BOOST_SYSTEM_ERROR
  BOOST_TRY { fn->run(); }
  BOOST_CATCH(...)
  {
    std::clog << boost::current_exception_diagnostic_information() << std::endl;
    std::terminate();
  }
  BOOST_CATCH_END
#    else
  GPCL_TRY { fn->run(); }
  GPCL_CATCH(...) { std::terminate(); }
  GPCL_CATCH_END
#    endif

  return nullptr;
}

posix_thread::~posix_thread()
{
  if (joinable())
  {
    join();
  }
}

void posix_thread::name(czstring<> s)
{
  int err = pthread_setname_np(thread_, s);
  if (err)
    throw_system_error(err, "pthread_setname_np");
}

void posix_thread::join()
{
  void *ret;
  int err = pthread_join(thread_, &ret);
  if (err)
    throw_system_error(err, "pthread_join");

  thread_ = pthread_t();
}

void posix_thread::detach()
{
  int err = pthread_detach(thread_);
  if (err)
    throw_system_error(err, "pthread_detach");
  thread_ = pthread_t();
}

auto posix_thread::yield() -> void
{
  int err = pthread_yield();
  if (err)
    throw_system_error(err, "pthread_yield");
}

#  endif

} // namespace detail
} // namespace gpcl

#endif
