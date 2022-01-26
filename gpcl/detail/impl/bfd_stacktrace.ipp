#ifndef GPCL_DETAIL_IMPL_BFD_STACKTRACE_IPP
#define GPCL_DETAIL_IMPL_BFD_STACKTRACE_IPP

#include <gpcl/detail/bfd_stacktrace.hpp>

#include <gpcl/detail/config.hpp>
#include <gpcl/noncopyable.hpp>
#include <gpcl/scoped_lock.hpp>
#include <gpcl/unique_ptr.hpp>
#include <gpcl/mutex.hpp>

#include <sstream>
#include <string_view>
#include <unordered_map>
#include <vector>

/* Hack for BFD */
#ifndef PACKAGE
#  define PACKAGE "gpcl"
#endif

#include <bfd.h>

#if defined(GPCL_POSIX)
#  include <dlfcn.h>
#endif

#if defined(__CYGWIN__) || defined(GPCL_WINDOWS)
#  include <Windows.h>
#  include <psapi.h>
#endif

namespace gpcl::detail {

struct bfd_deleter
{
  void operator()(bfd *abfd) const noexcept { bfd_close(abfd); }
};

using unique_bfd = unique_ptr<bfd, bfd_deleter>;

struct bfd_cache : noncopyable
{
  unique_bfd abfd;
  std::vector<asymbol *> symtab;
  std::intptr_t fbase;
};

using fbase_address = const void *;

struct bfd_context
{
private:
  bfd_context() { ::bfd_init(); }

  ~bfd_context() {}

public:
  static bfd_context &instance()
  {
    static bfd_context s;
    return s;
  }

  recursive_mutex mtx;
  std::unordered_map<fbase_address, bfd_cache> cached_bfds;
};

struct line_info
{
  const char *source_file;
  unsigned source_line;

  const char *binary_file;
  std::intptr_t symbol_address;

  std::string func;
};

inline bfd_context &g_bfd_context = bfd_context::instance();

inline bfd_cache *cached_bfd_from_address(const void *address,
                                          const void **symbol_address,
                                          const char **symbol_name) noexcept
{
  GPCL_TRY
  {
#if !defined(__CYGWIN__) && !defined(GPCL_WINDOWS)
    Dl_info info{};
    if (0 == dladdr(address, &info)) // zero indicates failure
      return nullptr;

    symbol_address && (*symbol_address = info.dli_saddr);
    symbol_name && (*symbol_name = info.dli_sname);

    const void *fbase = info.dli_fbase;
    const char *fname = info.dli_fname;

#else
    HMODULE hModule;
    if (!GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
                            (LPCSTR)address, &hModule))
      return nullptr;

    MODULEINFO info;
    if (!GetModuleInformation(GetCurrentProcess(), hModule, &info,
                              sizeof(info)))
      return nullptr;

    const void *fbase = info.lpBaseOfDll;
    char fname[255];
    if (!GetModuleFileNameA(hModule, fname, sizeof(fname)))
      return nullptr;

#endif
    scoped_lock lock(g_bfd_context.mtx);
    const auto [iter, new_inserted] =
        g_bfd_context.cached_bfds.try_emplace(fbase);
    if (new_inserted)
    {
      auto &[abfd, symtab, rfbase] = iter->second;
      auto *abfd_ = bfd_openr(fname, nullptr);
      if (!abfd_)
      {
        g_bfd_context.cached_bfds.erase(iter);
        return nullptr;
      }
      abfd = unique_bfd(abfd_);

      bfd_check_format(abfd_, bfd_object);
      auto needed_storage = bfd_get_symtab_upper_bound(abfd_);
      if (needed_storage > 0)
      {
        symtab.resize(needed_storage / sizeof(void *));
        auto n = bfd_canonicalize_symtab(abfd.get(), symtab.data());
        symtab.resize(n);
      }
      rfbase = std::intptr_t(fbase);
    }
    return &iter->second;
  }
  GPCL_CATCH(...) { return nullptr; }
  GPCL_CATCH_END
}

inline std::string demangle_if_needed(bfd *abfd, const char *name) noexcept
{
  if (!name)
    return "";

  if (name[0] == '_' && name[1] == 'Z')
  {
    unique_ptr<char, void (*)(void *)> demangled_name(
        bfd_demangle(abfd, name, 0), free);
    return std::string(demangled_name.get());
  }

  return name;
}

inline line_info bfd_get_line_from_address(const void *address)
{
  const void *symbol_address{};
  const char *symbol_name{};
  auto *module_bfd =
      cached_bfd_from_address(address, &symbol_address, &symbol_name);
  if (!module_bfd)
    return {};

  auto &[abfd, symtab, fbase] = *module_bfd;

  for (auto *section = abfd->sections; section; section = section->next)
  {
    if (!(section->flags & SEC_CODE))
      continue;

    const bool relative = section->vma < (uintptr_t)fbase;

    std::intptr_t offset =
        std::intptr_t(address) - (relative ? fbase : 0) - section->vma;
    if (offset < 0 || std::size_t(offset) > section->size)
      continue;

    const char *file;
    const char *func;
    unsigned line;

    if (bfd_find_nearest_line(abfd.get(), section, symtab.data(), offset, &file,
                              &func, &line))
    {
      return line_info{
          file,
          line,

          abfd->filename,
          std::intptr_t(symbol_address),

          demangle_if_needed(abfd.get(), func),
      };
    }
    else
    {
      return line_info{
          "",
          0,
          abfd->filename,
          std::intptr_t(symbol_address),
          demangle_if_needed(abfd.get(), symbol_name),
      };
    }
  }

  return line_info{};
}

std::string bfd_stacktrace_entry_description(const void *address) noexcept
{
  line_info info = bfd_get_line_from_address(address);
  std::stringstream ss;

  if (!info.func.empty())
  {
    ss << info.func << "()";
  }
  else if (info.symbol_address)
  {
    ss << std::hex << std::showbase << info.symbol_address;
  }
  else
  {
    ss << address;
  }

  if (info.source_line != 0 && !!info.source_file)
  {
    ss << " at " << info.source_file << ":" << std::dec << info.source_line;
  }
  else if (!!info.binary_file)
  {
    ss << " in " << info.binary_file;
  }

  return ss.str();
}

std::string bfd_stacktrace_entry_source_file(const void *address) noexcept
{
  line_info info = bfd_get_line_from_address(address);
  return std::string(info.source_file);
}
std::uint_least32_t
bfd_stacktrace_entry_source_line(const void *address) noexcept
{
  line_info info = bfd_get_line_from_address(address);
  return info.source_line;
}

} // namespace gpcl::detail

#endif // GPCL_DETAIL_IMPL_BFD_STACKTRACE_IPP
