#ifndef GPCL_GC_HPP
#define GPCL_GC_HPP

#include <gpcl/detail/config.hpp>

#include <gpcl/mutex.hpp>
#include <gpcl/pool.hpp>
#include <gpcl/thread.hpp>
#include <gpcl/unique_lock.hpp>
#include <gpcl/unique_ptr.hpp>

#include <atomic>
#include <fstream>
#include <map>
#include <string>

#include <setjmp.h>
#include <signal.h>
#include <stdlib.h>
#include <ucontext.h>

namespace gpcl {

struct gc_tag_t
{
};

inline constexpr gc_tag_t gc_tag{};

namespace detail {

struct gc_header
{
  alignas(16) unsigned flag;
};

inline void gc_mark_obj(void *obj);

class gc_pool : public pool<>
{
public:
  using pool<>::pool;

  void sweep()
  {
    storage_.reset();
    block_info block = block_list_;
    while (block.ptr)
    {
      size_t data_size = block_info_offset(block.size);
      char *chunk = block.ptr;
      while (chunk < block.ptr + data_size)
      {
        if (reinterpret_cast<gc_header *>(chunk)->flag == 0)
        {
          storage_.free(chunk);
        }
        else
        {
          reinterpret_cast<gc_header *>(chunk)->flag = 0;
        }

        chunk += chunk_size_;
      }
      block = *reinterpret_cast<block_info *>(block.ptr + data_size);
    }
  }

  friend inline void gc_mark_obj(void *obj);
};

inline std::map<std::size_t, unique_ptr<gc_pool>> gc_pool_map;

inline void gc_mark_region(void *p, std::size_t size);

inline void gc_mark_obj(void *obj)
{
  if (!obj)
    return;
  if (reinterpret_cast<uintptr_t>(obj) % 8 != 0)
    return;

  auto header = reinterpret_cast<gc_header *>(obj) - 1;

  for (auto &[size, pool] : gc_pool_map)
  {
    if (pool->is_from(header))
    {
      if (header->flag == 1)
        return;
      if (header->flag != 0)
      {
        printf("invalid pointer: %p\n", header);
      }

      header->flag = 1;
      // printf("marking %p\n", header + 1);
      gc_mark_region(obj, pool->chunk_size_ - sizeof(gc_header));

      return;
    }
  }
}

inline thread_local sigjmp_buf *volatile segv_jmpbuf;

inline void sigsegv_handler(int sig)
{
  if (segv_jmpbuf)
  {
    auto p = segv_jmpbuf;
    segv_jmpbuf = nullptr;
    siglongjmp(*p, sig);
  }
}

inline void install_signal_handlers()
{
  struct sigaction act;
  act.sa_handler = &sigsegv_handler;
  act.sa_flags = SA_NODEFER;
  sigemptyset(&act.sa_mask);

  sigaction(SIGSEGV, &act, nullptr);
}

template <typename Callable>
inline bool try_catch_segv(Callable &&callable)
{
  sigjmp_buf jmpbuf;
  segv_jmpbuf = &jmpbuf;

  if (sigsetjmp(jmpbuf, 0) == 0)
  {
    callable();

    segv_jmpbuf = nullptr;
    return true;
  }

  return false;
}

inline void gc_mark_region(void *p, std::size_t size)
{
  char **ptr = (char **)p;
  while ((char *)ptr < (char *)p + size)
  {
    gc_mark_obj(*ptr);
    ++ptr;
  }
}

inline void gc_do_mark_context(ucontext_t *ctx)
{
  gc_mark_region(ctx->uc_mcontext.gregs, __NGREG * sizeof(void *));

  gc_mark_region(ctx->uc_stack.ss_sp, ctx->uc_stack.ss_size);

  if (auto link = ctx->uc_link)
  {
    gc_do_mark_context(link);
  }
}

inline void gc_mark_context()
{
  ucontext_t context;
  getcontext(&context);

  try_catch_segv([&] { gc_do_mark_context(&context); });
}

inline void gc_mark_stack()
{
  std::ifstream map("/proc/self/maps");
  std::string line;

  alignas(16) std::uintptr_t start;
  std::uintptr_t end;

  while (std::getline(map, line))
  {
    if (line.find("rw-p") != line.npos && line.find("[heap]") == line.npos)
    {
      sscanf(line.c_str(), "%lx-%lx", &start, &end);

      try_catch_segv([&] { gc_mark_region((void *)start, end - start); });
    }
  }
}

inline void gc_mark_all()
{
  gc_mark_context();
  gc_mark_stack();
}

inline void gc_collect()
{
  gc_mark_all();

  for (auto &[size, pool] : gc_pool_map)
  {
    pool->sweep();
  }
}

inline void *gc_alloc(std::size_t sz)
{
  if (sz < 240)
  {
    auto &pool = gc_pool_map[sz];
    if (!pool)
    {
      pool = gpcl::make_unique<gc_pool>(sz + sizeof(gc_header));
    }
    auto *p = reinterpret_cast<gc_header *>(pool->malloc());
    p->flag = 0;
    return p + 1;
  }

  return nullptr;
}

inline std::shared_ptr<gpcl::mutex> gc_mutex = std::make_shared<gpcl::mutex>();

inline std::atomic_bool gc_exit{};

inline void gc_exit_handler()
{
  gc_exit = true;
}

} // namespace detail

inline void gc()
{
  auto mtx = gpcl::detail::gc_mutex;
  gpcl::unique_lock<gpcl::mutex> lock(*mtx);
  detail::gc_collect();
}

inline void auto_gc()
{
  detail::install_signal_handlers();

  atexit(detail::gc_exit_handler);

  gpcl::thread([] {
    while (!detail::gc_exit)
    {
      if (::sleep(1))
        break;
      gc();
    }
  }).detach();
}

inline void stop_auto_gc()
{
  detail::gc_exit = 1;
}

} // namespace gpcl

inline void *operator new(std::size_t count, gpcl::gc_tag_t const &)
{
  auto mtx = gpcl::detail::gc_mutex;
  gpcl::unique_lock<gpcl::mutex> lock(*mtx);
  return gpcl::detail::gc_alloc(count);
}

#endif // GPCL_GC_HPP
