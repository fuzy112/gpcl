#ifndef GPCL_ATOMIC_HPP
#define GPCL_ATOMIC_HPP

#include <gpcl/detail/config.hpp>

#ifdef GPCL_WINDOWS
#  include <gpcl/detail/win_atomic.hpp>
#elif GPCL_GCC || GPCL_CLANG
#  include <gpcl/detail/gcc_atomic.hpp>
#endif

#include <type_traits>

namespace gpcl {

namespace detail {

} // namespace detail

template <typename T>
struct atomic;

#ifdef GPCL_WINDOWS
template <>
struct atomic<char> : detail::win_atomic<char>
{
};

template <>
struct atomic<unsigned char> : detail::win_atomic<unsigned char>
{
};

template <>
struct atomic<signed char> : detail::win_atomic<signed char>
{
};

template <>
struct atomic<short> : detail::win_atomic<short>
{
};

template <>
struct atomic<unsigned short> : detail::win_atomic<unsigned short>
{
};

template <>
struct atomic<int> : detail::win_atomic<int>
{
};

template <>
struct atomic<unsigned int> : detail::win_atomic<unsigned int>
{
};

template <>
struct atomic<long> : detail::win_atomic<long>
{
};

template <>
struct atomic<unsigned long> : detail::win_atomic<unsigned long>
{
};

#  if defined(_M_ARM) || defined(_M_ARM64) || defined(_M_X64)
template <>
struct atomic<long long> : detail::win_atomic<long long>
{
};

template <>
struct atomic<unsigned long long> : detail::win_atomic<unsigned long long>
{
};
#  endif

template <>
struct atomic<char16_t> : detail::win_atomic<char16_t>
{
};

template <>
struct atomic<char32_t> : detail::win_atomic<char32_t>
{
};

template <>
struct atomic<wchar_t> : detail::win_atomic<wchar_t>
{
};

// primary template
template <typename T>
struct atomic : detail::win_atomic_base<T>
{
};

#elif GPCL_GCC || GPCL_CLANG

template <>
struct atomic<char> : detail::gcc_atomic<char>
{
};

template <>
struct atomic<signed char> : detail::gcc_atomic<signed char>
{
};

template <>
struct atomic<unsigned char> : detail ::gcc_atomic<unsigned char>
{
};

template <>
struct atomic<short> : detail::gcc_atomic<short>
{
};

template <>
struct atomic<unsigned short> : detail::gcc_atomic<unsigned short>
{
};

template <>
struct atomic<int> : detail::gcc_atomic<int>
{
};

template <>
struct atomic<unsigned int> : detail::gcc_atomic<unsigned int>
{
};

template <>
struct atomic<long> : detail ::gcc_atomic<long>
{
};

template <>
struct atomic<unsigned long> : detail::gcc_atomic<unsigned long>
{
};

template <>
struct atomic<long long> : detail ::gcc_atomic<long long>
{
};

template <>
struct atomic<unsigned long long> : detail::gcc_atomic<unsigned long long>
{
};

template <>
struct atomic<wchar_t> : detail ::gcc_atomic<wchar_t>
{
};

template <>
struct atomic<char16_t> : detail::gcc_atomic<char16_t>
{
};

template <>
struct atomic<char32_t> : detail ::gcc_atomic<char32_t>
{
};

// primary template
template <typename T>
struct atomic : detail::gcc_atomic_base<T>
{
};

#endif

////extern template struct atomic<bool>;
// extern template struct atomic<char>;
// extern template struct atomic<signed char>;
// extern template struct atomic<unsigned char>;
// extern template struct atomic<short>;
// extern template struct atomic<unsigned short>;
// extern template struct atomic<int>;
// extern template struct atomic<unsigned int>;
// extern template struct atomic<long>;
// extern template struct atomic<unsigned long>;
// extern template struct atomic<long long>;
// extern template struct atomic<unsigned long long>;
//// extern template struct atomic<char8_t> ;
// extern template struct atomic<char16_t>;
// extern template struct atomic<char32_t>;
// extern template struct atomic<wchar_t>;

typedef atomic<bool> atomic_bool;
typedef atomic<char> atomic_char;
typedef atomic<signed char> atomic_schar;
typedef atomic<unsigned char> atomic_uchar;
typedef atomic<short> atomic_short;
typedef atomic<unsigned short> atomic_ushort;
typedef atomic<int> atomic_int;
typedef atomic<unsigned int> atomic_uint;
typedef atomic<long> atomic_long;
typedef atomic<unsigned long> atomic_ulong;
typedef atomic<long long> atomic_llong;
typedef atomic<unsigned long long> atomic_ullong;
// typedef atomic<char8_t> atomic_char8_t;
typedef atomic<char16_t> atomic_char16_t;
typedef atomic<char32_t> atomic_char32_t;
typedef atomic<wchar_t> atomic_wchar_t;

typedef atomic<int8_t> atomic_int8_t;
typedef atomic<uint8_t> atomic_uint8_t;
typedef atomic<int16_t> atomic_int16_t;
typedef atomic<uint16_t> atomic_uint16_t;
typedef atomic<int32_t> atomic_int32_t;
typedef atomic<uint32_t> atomic_uint32_t;
typedef atomic<int64_t> atomic_int64_t;
typedef atomic<uint64_t> atomic_uint64_t;

typedef atomic<int_least8_t> atomic_int_least8_t;
typedef atomic<uint_least8_t> atomic_uint_least8_t;
typedef atomic<int_least16_t> atomic_int_least16_t;
typedef atomic<uint_least16_t> atomic_uint_least16_t;
typedef atomic<int_least32_t> atomic_int_least32_t;
typedef atomic<uint_least32_t> atomic_uint_least32_t;
typedef atomic<int_least64_t> atomic_int_least64_t;
typedef atomic<uint_least64_t> atomic_uint_least64_t;

typedef atomic<int_fast8_t> atomic_int_fast8_t;
typedef atomic<uint_fast8_t> atomic_uint_fast8_t;
typedef atomic<int_fast16_t> atomic_int_fast16_t;
typedef atomic<uint_fast16_t> atomic_uint_fast16_t;
typedef atomic<int_fast32_t> atomic_int_fast32_t;
typedef atomic<uint_fast32_t> atomic_uint_fast32_t;
typedef atomic<int_fast64_t> atomic_int_fast64_t;
typedef atomic<uint_fast64_t> atomic_uint_fast64_t;

typedef atomic<intptr_t> atomic_intptr_t;
typedef atomic<uintptr_t> atomic_uintptr_t;
typedef atomic<size_t> atomic_size_t;
typedef atomic<ptrdiff_t> atomic_ptrdiff_t;
typedef atomic<intmax_t> atomic_intmax_t;
typedef atomic<uintmax_t> atomic_uintmax_t;

} // namespace gpcl

#endif //
