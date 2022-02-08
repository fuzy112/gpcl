//
// posix_thread.ipp
// ~~~~~~~~~~~~~~~~
//
// Copyright (c) 2020-2022 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_IMPL_POSIX_THREAD_IPP
#define GPCL_DETAIL_IMPL_POSIX_THREAD_IPP

#include <gpcl/detail/assert.hpp>
#include <gpcl/detail/chrono.hpp>
#include <gpcl/detail/posix_thread.hpp>
#include <gpcl/detail/throw_system_error.hpp>
#include <gpcl/scope_success.hpp>

#include <signal.h>

#include <sys/select.h>

#ifdef __linux__
#  include <sys/syscall.h>
#endif

#ifdef __FreeBSD__
#  include <sys/sysctl.h>
#endif

#include <sys/types.h>
#include <unistd.h>

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

void posix_thread::start_thread(thread_attributes const &attr,
                                unique_ptr<function<void()>> fn)
{
  posix_thread_attributes attr1(attr);
  scope_success release_fn{[&] { (void)fn.release(); }};
  int err =
      pthread_create(&thread_, attr1.get(), posix_thread_function, fn.get());
  if (err)
  {
    throw_system_error(err, "pthread_create");
  }
}

void *posix_thread_function(void *arg) noexcept
{
  GPCL_ASSERT(arg != nullptr);
  auto fn =
      unique_ptr<function<void()>>(reinterpret_cast<function<void()> *>(arg));

  sigset_t set;
  sigemptyset(&set);
  sigaddset(&set, SIGINT);
  sigaddset(&set, SIGTERM);
  pthread_sigmask(SIG_BLOCK, &set, nullptr);

  int oldstate{};
  pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &oldstate);
  (*fn)();
  pthread_setcancelstate(oldstate, nullptr);

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
#if defined(GPCL_LINUX)
  int err = pthread_setname_np(thread_, s);
  if (err)
    throw_system_error(err, "pthread_setname_np");
#endif
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
  int err = sched_yield();
  if (err)
    throw_system_error("sched_yield");
}

#if GPCL_CONFIG_POSIX_THREAD_ID_IS_TID

posix_thread_id posix_thread::get_id() const
{
  pthread_id_np_t tid;
  int err = pthread_getthreadid_np(&thread_, &tid);
  if (err)
    throw_system_error(err, "pthread_getthreadid_np");
  return posix_thread_id{tid};
}

posix_thread_id posix_thread::this_thread_id()
{
  pid_t tid;
  tid = syscall(SYS_gettid);
  return posix_thread_id{tid};
}

#else

posix_thread_id posix_thread::get_id() const
{
  return posix_thread_id{thread_};
}

posix_thread_id posix_thread::this_thread_id()
{
  return posix_thread_id{pthread_self()};
}

#endif

#if defined(__FreeBSD__)
unsigned int posix_thread::hardware_concurrency()
{
  int s;
  int mib[2] = {CTL_HW, HW_NCPU};
  int ncpu{};
  size_t len = sizeof(ncpu);

  s = sysctl(mib, 2, &ncpu, &len, nullptr, 0);
  if (s == -1)
    throw_system_error(__func__);
  return ncpu;
}
#elif defined(__linux__) || defined(__CYGWIN__)
unsigned int posix_thread::hardware_concurrency()
{
  return sysconf(_SC_NPROCESSORS_CONF);
}
#else
unsigned int posix_thread::hardware_concurrency()
{
  return 0;
}
#endif

} // namespace detail
} // namespace gpcl

#endif // GPCL_DETAIL_IMPL_POSIX_THREAD_IPP
