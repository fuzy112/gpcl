#ifndef GPCL_DETAIL_WIN_STACKTRACE_HPP
#define GPCL_DETAIL_WIN_STACKTRACE_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/error.hpp>

#include <atomic>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <type_traits>
#include <vector>

#include <windows.h>

#include <dbghelp.h>

namespace gpcl::detail {

template <typename Allocator>
class basic_win_stacktrace;

class win_stacktrace_entry;

class win_stacktrace_entry
{
  mutable STACKFRAME64 data_ = {};

public:
  using native_handle_type = LPSTACKFRAME;

  constexpr win_stacktrace_entry() noexcept = default;

  constexpr win_stacktrace_entry(const win_stacktrace_entry &) = default;

  constexpr win_stacktrace_entry &
  operator=(const win_stacktrace_entry &) = default;

  constexpr native_handle_type native_handle() const noexcept { return &data_; }

  constexpr explicit operator bool() const noexcept
  {
    STACKFRAME empty{};
    return data_.AddrPC.Offset == 0;
  }

  std::string description() const;

  std::string source_file() const;

  std::uint_least32_t source_line() const;

  friend inline bool operator<(const win_stacktrace_entry &x,
                               const win_stacktrace_entry &y);

  friend inline bool operator!=(const win_stacktrace_entry &x,
                                const win_stacktrace_entry &y);

  friend inline bool operator==(const win_stacktrace_entry &x,
                                const win_stacktrace_entry &y);

  template <typename CharT, typename Traits>
  friend std::basic_ostream<CharT, Traits> &
  operator<<(std::basic_ostream<CharT, Traits> &os,
             const win_stacktrace_entry &f)
  {
    auto desc = f.description();
    if (desc.empty())
      return os << "???";
    return os << desc;
  }
};

template <typename Allocator>
class basic_win_stacktrace
{
  static_assert(
      std::is_same<typename std::allocator_traits<Allocator>::value_type,
                   win_stacktrace_entry>::value);

  std::vector<win_stacktrace_entry, Allocator> data_;

public:
  using value_type = win_stacktrace_entry;
  using const_reference = const value_type &;
  using reference = value_type &;

  using allocator_type = Allocator;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  using const_iterator =
      typename std::vector<win_stacktrace_entry, Allocator>::const_iterator;
  using iterator = const_iterator;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using reverse_const_iterator = std::reverse_iterator<const_iterator>;

  basic_win_stacktrace() noexcept(
      std::is_nothrow_default_constructible_v<Allocator>) = default;

  explicit basic_win_stacktrace(const allocator_type &alloc) noexcept
      : data_(alloc)
  {
  }

  basic_win_stacktrace(const basic_win_stacktrace &other) = default;

  basic_win_stacktrace(basic_win_stacktrace &&other) noexcept = default;

  basic_win_stacktrace(const basic_win_stacktrace &other,
                       const allocator_type &alloc)
      : data_(other.data_, alloc)
  {
  }

  basic_win_stacktrace(basic_win_stacktrace &&other,
                       const allocator_type &alloc)
      : data_(std::move(other).data_, alloc)
  {
  }

  ~basic_win_stacktrace() = default;

  basic_win_stacktrace &operator=(const basic_win_stacktrace &other);

  basic_win_stacktrace &operator=(basic_win_stacktrace &&other) noexcept(
      std::allocator_traits<
          Allocator>::propagate_on_container_move_assignment::value ||
      std::allocator_traits<Allocator>::is_always_equal::value);

  static __forceinline basic_win_stacktrace
  current(const allocator_type &alloc = allocator_type()) noexcept;

  static __forceinline basic_win_stacktrace
  current(size_type skip,
          const allocator_type &alloc = allocator_type()) noexcept;

  static __forceinline basic_win_stacktrace
  current(size_type skip, size_type max_depth,
          const allocator_type &alloc = allocator_type()) noexcept;

  allocator_type get_allocator() const noexcept
  {
    return data_.get_allocator();
  }

  const_iterator begin() const noexcept { return data_.begin(); }
  const_iterator cbegin() const noexcept { return begin(); }

  const_iterator end() const noexcept { return data_.end(); }
  const_iterator cend() const noexcept { return end(); }

  reverse_const_iterator rbegin() const noexcept
  {
    return reverse_const_iterator(end());
  }
  reverse_const_iterator crbegin() const noexcept { return rbegin(); }

  reverse_const_iterator rend() const noexcept
  {
    return reverse_const_iterator(begin());
  }
  reverse_const_iterator crend() const noexcept { return rend(); }

  bool empty() const noexcept { return data_.empty(); }

  size_type size() const noexcept { return data_.size(); }

  size_type max_size() const noexcept { return data_.max_size(); }

  const_reference operator[](size_type pos) const { return data_[pos]; }

  const_reference at(size_type pos) const { return data_.at(pos); }

  void swap(basic_win_stacktrace &other) noexcept(
      std::allocator_traits<Allocator>::propagate_on_container_swap::value ||
      std::allocator_traits<Allocator>::is_always_equal::value)
  {
    data_.swap(other.data_);
  }

  bool operator==(const basic_win_stacktrace &other)
  {
    return data_ == other.data_;
  }

  bool operator!=(const basic_win_stacktrace &other)
  {
    return data_ != other.data_;
  }

  bool operator<(const basic_win_stacktrace &other)
  {
    return data_ < other.data_;
  }
};

template <typename CharT, typename Traits, typename Allocator>
std::basic_ostream<CharT, Traits> &
operator<<(std::basic_ostream<CharT, Traits> &os,
           const basic_win_stacktrace<Allocator> &st)
{
  typename std::basic_ostream<CharT, Traits>::sentry sentry(os);
  if (!sentry)
    return os;

  int width = 1;
  if (st.size() > 9)
    width = 2;
  if (st.size() > 99)
    width = 3;
  if (st.size() > 999)
    width = 4;

  int i(0);
  for (auto &e : st)
  {
    os << '[' << std::setw(width) << ++i << "] " << e << '\n';
  }

  return os;
}

template <typename Allocator>
void swap(basic_win_stacktrace<Allocator> &x,
          basic_win_stacktrace<Allocator> &y) noexcept(noexcept(x.swap(y)))
{
  x.swap(y);
}

struct win_sym_init
{
  inline static std::atomic<long> init_count{0};
  inline static HANDLE process;

  win_sym_init()
  {
    if (init_count.fetch_add(1) == 0)
    {
      process = GetCurrentProcess();
      SymInitialize(process, NULL, TRUE);
    }
  }

  ~win_sym_init()
  {
    if (init_count.fetch_sub(1) == 1)
    {
      SymCleanup(process);
    }
  }
};

inline win_sym_init g_win_sym_init_;

template <typename Allocator>
__forceinline void win_stacktrace_impl(
    std::size_t skip, std::size_t max_depth,
    std::vector<win_stacktrace_entry, Allocator> &container) noexcept
{
  HANDLE process = GetCurrentProcess();
  HANDLE thread = GetCurrentThread();

  CONTEXT context;
  memset(&context, 0, sizeof(context));
  context.ContextFlags = CONTEXT_FULL;
  RtlCaptureContext(&context);

  DWORD image;
  STACKFRAME64 stackframe;
  ZeroMemory(&stackframe, sizeof(stackframe));

#ifdef _M_IX86
  image = IMAGE_FILE_MACHINE_I386;
  stackframe.AddrPC.Offset = context.Eip;
  stackframe.AddrPC.Mode = AddrModeFlat;
  stackframe.AddrFrame.Offset = context.Ebp;
  stackframe.AddrFrame.Mode = AddrModeFlat;
  stackframe.AddrStack.Offset = context.Esp;
  stackframe.AddrStack.Mode = AddrModeFlat;
#elif _M_X64
  image = IMAGE_FILE_MACHINE_AMD64;
  stackframe.AddrPC.Offset = context.Rip;
  stackframe.AddrPC.Mode = AddrModeFlat;
  stackframe.AddrFrame.Offset = context.Rsp;
  stackframe.AddrFrame.Mode = AddrModeFlat;
  stackframe.AddrStack.Offset = context.Rsp;
  stackframe.AddrStack.Mode = AddrModeFlat;
#elif _M_IA64
  image = IMAGE_FILE_MACHINE_IA64;
  stackframe.AddrPC.Offset = context.StIIP;
  stackframe.AddrPC.Mode = AddrModeFlat;
  stackframe.AddrFrame.Offset = context.IntSp;
  stackframe.AddrFrame.Mode = AddrModeFlat;
  stackframe.AddrBStore.Offset = context.RsBSP;
  stackframe.AddrBStore.Mode = AddrModeFlat;
  stackframe.AddrStack.Offset = context.IntSp;
  stackframe.AddrStack.Mode = AddrModeFlat;
#endif

  GPCL_TRY
  {
    for (std::size_t i = 0; i < max_depth; ++i)
    {
      BOOL result =
          StackWalk64(image, process, thread, &stackframe, &context, nullptr,
                      SymFunctionTableAccess64, SymGetModuleBase64, nullptr);

      if (!result)
        break;

      if (i > skip)
      {
        win_stacktrace_entry entry;
        std::memcpy(entry.native_handle(), &stackframe, sizeof(stackframe));
        container.push_back(entry);
      }
    }
  }
  GPCL_CATCH(...) { container.clear(); }
  GPCL_CATCH_END
}

template <typename Allocator>
basic_win_stacktrace<Allocator>
basic_win_stacktrace<Allocator>::current(size_type skip, size_type max_depth,
                                         const Allocator &alloc) noexcept
{
  basic_win_stacktrace result(alloc);
  win_stacktrace_impl(skip, max_depth, result.data_);
  return result;
}

template <typename Allocator>
basic_win_stacktrace<Allocator>
basic_win_stacktrace<Allocator>::current(size_type skip,
                                         const Allocator &alloc) noexcept
{
  basic_win_stacktrace result(alloc);
  win_stacktrace_impl(skip, size_type(-1), result.data_);
  return result;
}

template <typename Allocator>
basic_win_stacktrace<Allocator>
basic_win_stacktrace<Allocator>::current(const Allocator &alloc) noexcept
{
  basic_win_stacktrace result(alloc);
  win_stacktrace_impl(0, size_type(-1), result.data_);
  return result;
}

inline std::string win_stacktrace_entry::description() const
{
  char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME + sizeof(char)];
  PSYMBOL_INFO symbol = (PSYMBOL_INFO)buffer;
  symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
  symbol->MaxNameLen = MAX_SYM_NAME;

  DWORD64 displacement = 0;
  if (SymFromAddr(g_win_sym_init_.process, data_.AddrPC.Offset, &displacement,
                  symbol))
  {
    return std::string(symbol->Name, symbol->NameLen);
  }

  return "";
}

} // namespace gpcl::detail

#endif // GPCL_DETAIL_WIN_STACKTRACE_HPP
