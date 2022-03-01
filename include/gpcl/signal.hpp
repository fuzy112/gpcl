//
// signal.hpp
// ~~~~~~~~~~
//
// Copyright (c) 2022 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_SIGNAL_HPP
#define GPCL_SIGNAL_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/detail/error.hpp>

#include <signal.h>

namespace gpcl {

inline std::string_view signal_name(int signum)
{
  GPCL_ASSERT(signum > 0);

#define GPCL_MATCH_SIGNAL(Signal)                                              \
  if (signum == Signal)                                                        \
  return #Signal

#ifdef GPCL_CONFIG_POSIX_SIGNALS
  GPCL_MATCH_SIGNAL(SIGHUP);
#endif
  GPCL_MATCH_SIGNAL(SIGINT);
#ifdef GPCL_CONFIG_POSIX_SIGNALS
  GPCL_MATCH_SIGNAL(SIGQUIT);
#endif
  GPCL_MATCH_SIGNAL(SIGILL);
#ifdef GPCL_CONFIG_POSIX_SIGNALS
  GPCL_MATCH_SIGNAL(SIGTRAP);
#endif
  GPCL_MATCH_SIGNAL(SIGABRT);
#ifdef GPCL_CONFIG_POSIX_SIGNALS
  GPCL_MATCH_SIGNAL(SIGIOT);
  GPCL_MATCH_SIGNAL(SIGBUS);
#endif
#ifdef SIGEMT
  GPCL_MATCH_SIGNAL(SIGEMT);
#endif
  GPCL_MATCH_SIGNAL(SIGFPE);
#ifdef GPCL_CONFIG_POSIX_SIGNALS
  GPCL_MATCH_SIGNAL(SIGKILL);
  GPCL_MATCH_SIGNAL(SIGUSR1);
#endif
  GPCL_MATCH_SIGNAL(SIGSEGV);
#ifdef GPCL_CONFIG_POSIX_SIGNALS
  GPCL_MATCH_SIGNAL(SIGUSR2);
  GPCL_MATCH_SIGNAL(SIGPIPE);
  GPCL_MATCH_SIGNAL(SIGALRM);
#endif
  GPCL_MATCH_SIGNAL(SIGTERM);
#ifdef SIGSTKFLT
  GPCL_MATCH_SIGNAL(SIGSTKFLT);
#endif
#ifdef GPCL_CONFIG_POSIX_SIGNALS
  GPCL_MATCH_SIGNAL(SIGCHLD);
#ifdef SIGCLD
  GPCL_MATCH_SIGNAL(SIGCLD);
#endif
  GPCL_MATCH_SIGNAL(SIGCONT);
  GPCL_MATCH_SIGNAL(SIGSTOP);
  GPCL_MATCH_SIGNAL(SIGTSTP);
  GPCL_MATCH_SIGNAL(SIGTTIN);
  GPCL_MATCH_SIGNAL(SIGTTOU);
  GPCL_MATCH_SIGNAL(SIGURG);
  GPCL_MATCH_SIGNAL(SIGXCPU);
  GPCL_MATCH_SIGNAL(SIGXFSZ);
  GPCL_MATCH_SIGNAL(SIGVTALRM);
  GPCL_MATCH_SIGNAL(SIGPROF);
  GPCL_MATCH_SIGNAL(SIGWINCH);
  GPCL_MATCH_SIGNAL(SIGIO);
// GPCL_MATCH_SIGNAL(SIGPOLL);
#ifdef SIGPWR
  GPCL_MATCH_SIGNAL(SIGPWR);
#endif
#ifdef SIGINFO
  GPCL_MATCH_SIGNAL(SIGINFO);
#endif
#ifdef SIGLOST
  GPCL_MATCH_SIGNAL(SIGLOST);
#endif
  GPCL_MATCH_SIGNAL(SIGSYS);
#endif // GPCL_CONFIG_POSIX_SIGNALS

#undef GPCL_MATCH_SIGNAL

  GPCL_THROW(system_error{EINTR, generic_category()});
}
} // namespace gpcl

#endif // GPCL_SIGNAL_HPP
