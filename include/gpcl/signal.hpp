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

#ifdef GPCL_CONFIG_POSIX_SIGNALS
#  include <signal.h>
#endif

namespace gpcl {

inline std::string_view signal_name(int signum)
{
  GPCL_ASSERT(signum > 0);

#define GPCL_MATCH_SIGNAL(Signal)                                              \
  if (signum == Signal)                                                        \
  return #Signal
  GPCL_MATCH_SIGNAL(SIGHUP);
  GPCL_MATCH_SIGNAL(SIGINT);
  GPCL_MATCH_SIGNAL(SIGQUIT);
  GPCL_MATCH_SIGNAL(SIGILL);
  GPCL_MATCH_SIGNAL(SIGTRAP);
  GPCL_MATCH_SIGNAL(SIGABRT);
  GPCL_MATCH_SIGNAL(SIGIOT);
  GPCL_MATCH_SIGNAL(SIGBUS);
#ifdef SIGEMT
  GPCL_MATCH_SIGNAL(SIGEMT);
#endif
  GPCL_MATCH_SIGNAL(SIGFPE);
  GPCL_MATCH_SIGNAL(SIGKILL);
  GPCL_MATCH_SIGNAL(SIGUSR1);
  GPCL_MATCH_SIGNAL(SIGSEGV);
  GPCL_MATCH_SIGNAL(SIGUSR2);
  GPCL_MATCH_SIGNAL(SIGPIPE);
  GPCL_MATCH_SIGNAL(SIGALRM);
  GPCL_MATCH_SIGNAL(SIGTERM);
#ifdef SIGSTKFLT
  GPCL_MATCH_SIGNAL(SIGSTKFLT);
#endif
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
#undef GPCL_MATCH_SIGNAL

  GPCL_THROW(system_error{EINTR, generic_category()});
}
} // namespace gpcl

#endif // GPCL_SIGNAL_HPP
