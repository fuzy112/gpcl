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

template <typename Target>
struct lexical_cast_str_impl;

template <typename CharType>
struct lexical_cast_str_impl<std::basic_string<CharType>>
{
  using target_type = std::basic_string<CharType>;

  template <typename Source>
  target_type operator()(const Source &arg) const
  {
    if constexpr (is_output_streamable<Source, std::basic_ostream<CharType>>())
    {
      std::basic_stringstream<CharType> ss;
      ss << arg;
      if (!ss)
        GPCL_THROW(bad_lexical_cast());
      return ss.str();
    }

    else
    {
      GPCL_UNREACHABLE("cannot cast such type");
    }
  }

  target_type operator()(const CharType *s, std::size_t n) const
  {
    return target_type(s, n);
  }
};

template <typename Target>
struct lexical_cast_impl
{
  static_assert(std::is_default_constructible_v<Target>);
  static_assert(std::is_copy_constructible_v<Target>);

  template <typename Source, bool False = false>
  Target operator()(const Source &arg) const
  {
    if constexpr (is_output_streamable<Source, std::ostream>() &&
                  is_input_streamable<Target, std::istream>())
    {
      std::stringstream ss;
      ss << arg;
      return extract_result(ss);
    }

    else if constexpr (is_output_streamable<Source, std::wostream>() &&
                       is_input_streamable<Target, std::wistream>())
    {
      std::wstringstream ss;
      ss << arg;
      return extract_result(ss);
    }

    else
    {
      GPCL_UNREACHABLE("cannot cast such type");
    }
  }

  template <typename CharType>
  Target operator()(const CharType *s, std::size_t n) const
  {
    class streambuf : public std::basic_streambuf<CharType>
    {
    public:
      streambuf(const CharType *s, std::size_t n)
      {
        auto p = const_cast<CharType *>(s);
        this->setg(p, p, p + n);
      }
    };

    class istream : private streambuf, public std::basic_istream<CharType>
    {
    public:
      istream(const CharType *s, std::size_t n)
          : streambuf(s, n),
            std::basic_istream<CharType>(this)
      {
      }
    };

    istream is(s, n);

    return extract_result(is);
  }

  template <typename CharType>
  static Target extract_result(std::basic_istream<CharType> &is)
  {
    if (is)
    {
      Target result{};
      is >> result;

      if (is && is.eof())
        return result;
    }
    GPCL_THROW(bad_lexical_cast());
  }
};

template <>
struct lexical_cast_impl<std::string> : lexical_cast_str_impl<std::string>
{
};
template <>
struct lexical_cast_impl<std::wstring> : lexical_cast_str_impl<std::wstring>
{
};

} // namespace detail

template <typename Target>
constexpr detail::lexical_cast_impl<Target> lexical_cast{};

} // namespace gpcl

#endif // GPCL_LEXICAL_CAST_HPP
