#ifndef GPCL_LEXICAL_CAST_HPP
#define GPCL_LEXICAL_CAST_HPP

#include <gpcl/error.hpp>

#include <cstring>
#include <exception>
#include <sstream>
#include <type_traits>

namespace gpcl {

template <typename T, typename S, typename = void>
struct is_output_streamable : std::false_type
{
};

template <typename T, typename S>
struct is_output_streamable<
    T, S,
    std::void_t<decltype(std::declval<S &>() << std::declval<const T &>())>>
    : std::true_type
{
};

template <typename T, typename S, typename = void>
struct is_input_streamable : std::false_type
{
};

template <typename T, typename S>
struct is_input_streamable<
    T, S, std::void_t<decltype(std::declval<S &>() >> std::declval<T &>())>>
    : std::true_type
{
};

class bad_lexical_cast : public std::exception
{
public:
  using std::exception::exception;

  const char *what() const noexcept final { return "bad lexical cast"; }
};

namespace detail {

template <typename CharType, typename Source>
class lexical_cast_istream : public std::basic_stringstream<CharType>
{
public:
  explicit lexical_cast_istream(const Source &arg) { (*this) << arg; }
};

template <typename CharType>
class lexical_cast_istream<CharType, std::basic_string<CharType>>
    : public std::basic_istringstream<CharType>
{
public:
  explicit lexical_cast_istream(const std::basic_string<CharType> &s)
      : std::basic_istringstream<CharType>(s)
  {
  }
};

template <typename CharType>
class lexical_cast_streambuf : public std::basic_streambuf<CharType>
{
public:
  lexical_cast_streambuf(const CharType *s, std::size_t n)
  {
    auto p = const_cast<CharType *>(s);
    this->setg(p, p, p + n);
  }

  std::basic_string<CharType> str() const
  {
    return std::basic_string<CharType>(this->gptr(), this->egptr());
  }
};

template <typename CharType>
class lexical_cast_istringstream : private lexical_cast_streambuf<CharType>,
                                   public std::basic_istream<CharType>
{
public:
  lexical_cast_istringstream(const CharType *s, std::size_t n)
      : lexical_cast_streambuf<CharType>(s, n),
        std::basic_istream<CharType>(
            static_cast<std::basic_streambuf<CharType> *>(this))
  {
  }
};

template <typename StrStream, typename CharType>
void lexical_cast_extract_result(StrStream &is,
                                 std::basic_string<CharType> &result)
{
  if (is)
  {
    result = is.str();
    return;
  }

  GPCL_THROW(bad_lexical_cast());
}

template <typename Stream, typename Target>
void lexical_cast_extract_result(Stream &is, Target &result)
{
  if (is)
  {
    is >> result;

    if (is && is.eof())
      return;
  }

  GPCL_THROW(bad_lexical_cast());
}

template <typename Target>
struct lexical_cast_impl
{
  static_assert(std::is_default_constructible_v<Target>);
  static_assert(std::is_copy_constructible_v<Target>);

  template <typename Source, bool False = false>
  Target operator()(const Source &arg) const
  {
    Target result{};

    if constexpr (is_output_streamable<Source, std::ostream>() &&
                  is_input_streamable<Target, std::istream>())
    {
      lexical_cast_istream<char, Source> is(arg);
      lexical_cast_extract_result(is, result);
    }

    else if constexpr (is_output_streamable<Source, std::wostream>() &&
                       is_input_streamable<Target, std::wistream>())
    {
      lexical_cast_istream<wchar_t, Source> is(arg);
      lexical_cast_extract_result(is, result);
    }

    else
    {
      GPCL_UNREACHABLE("cannot cast such type");
    }

    return result;
  }

  template <typename CharType>
  Target operator()(const CharType *s, std::size_t n) const
  {
    Target result{};
    lexical_cast_istringstream<CharType> is(s, n);
    lexical_cast_extract_result(is, result);
    return result;
  }
};

} // namespace detail

template <typename Target>
constexpr detail::lexical_cast_impl<Target> lexical_cast{};

} // namespace gpcl

#endif // GPCL_LEXICAL_CAST_HPP
