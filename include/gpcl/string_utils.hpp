//
// string_utils.hpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (c) 2022 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_STRING_UTILS_HPP
#define GPCL_STRING_UTILS_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/dynarray.hpp>
#include <gpcl/flags.hpp>

#include <algorithm>
#include <iterator>
#include <string>

namespace gpcl {
namespace detail {
template <typename BidIt, typename Alloc>
class split_results_builder;
}

template <typename BidIt>
class sub_string;

template <typename BidIt, typename Alloc>
class split_results;

template <typename BidIt>
class sub_string : public std::pair<BidIt, BidIt>
{
public:
  using difference_type = typename std::iterator_traits<BidIt>::difference_type;
  using iterator = BidIt;
  using value_type = typename std::iterator_traits<BidIt>::value_type;

  difference_type length() const
  {
    return std::distance(this->first, this->second);
  }

  std::basic_string<value_type> str() const
  {
    return std::basic_string<value_type>(this->first, this->second);
  }

  operator std::basic_string<value_type>() const { return str(); }

  bool empty() const { return this->first == this->second; }

  sub_string trimmed() const
  {
    auto i = std::find_if(this->first, this->second,
                          [](auto ch) { return !std::isspace(ch); });

    auto j =
        std::find_if(i, this->second, [](auto ch) { return std::isspace(ch); });
    return sub_string{{i, j}};
  }
};

template <typename BidIt,
          typename Alloc = gpcl::default_allocator<sub_string<BidIt>>>
class split_results
{
public:
  using value_type = sub_string<BidIt>;
  using allocator_type = Alloc;
  using char_type = typename std::iterator_traits<BidIt>::value_type;
  using const_iterator = typename dynarray<value_type, Alloc>::const_iterator;
  using const_reference = const value_type &;
  using difference_type = typename std::iterator_traits<BidIt>::difference_type;
  using iterator = const_iterator;
  using reference = const_reference;
  using size_type = typename Alloc::size_type;
  using string_type = std::basic_string<char_type>;

  static_assert(std::is_same<typename std::allocator_traits<Alloc>::value_type,
                             value_type>::value,
                "");

private:
  static const value_type empty_value_;

  BidIt start_;
  dynarray<value_type, Alloc> sub_strings_;

  friend class detail::split_results_builder<BidIt, Alloc>;

public:
  explicit split_results(const Alloc &alloc = Alloc()) : sub_strings_(alloc) {}

  split_results(const split_results &other) = default;

  split_results &operator=(const split_results &other) = default;

  const_iterator begin() const { return sub_strings_.begin(); }

  const_iterator end() const { return sub_strings_.end(); }

  bool empty() const noexcept { return sub_strings_.empty(); }

  allocator_type get_allocator() const { return sub_strings_.get_allocator(); }

  difference_type length(size_type sub) const { return (*this)[sub].length(); }

  size_type size() const { return sub_strings_.size(); }

  size_type max_size() const { return sub_strings_.max_size(); }

  const_reference operator[](size_type n) const
  {
    if (sub_strings_.size() > n)
    {
      return sub_strings_[n];
    }
    return empty_value_;
  }

  string_type str(size_type sub) const { return (*this)[sub].str(); }

  difference_type position(size_type sub = 0) const
  {
    return (*this)[sub].first - start_;
  }

  void swap(split_results &other) noexcept
  {
    using std::swap;
    swap(start_, other.start_);
    swap(sub_strings_, other.sub_strings_);
  }
};

template <typename BidIt, typename Alloc>
typename split_results<BidIt, Alloc>::value_type const
    split_results<BidIt, Alloc>::empty_value_;

using ssplit_results = split_results<typename std::string::const_iterator>;
using csplit_results = split_results<const char *>;

namespace detail {
template <typename BidIt, typename Alloc>
class split_results_builder
{
public:
  using result_type = split_results<BidIt, Alloc>;
  using value_type = sub_string<BidIt>;

private:
  result_type &result_;

public:
  explicit split_results_builder(result_type &result, BidIt start)
      : result_(result)
  {
    result_.start_ = start;
    result_.sub_strings_.clear();
  }

  void add(value_type val) { result_.sub_strings_.push_back(val); }
};
} // namespace detail

enum class split_flag
{
  skip_empty_string = (1 << 0),
  trim_results = (1 << 1),
};

GPCL_DEFINE_FLAGS(split_flags, split_flag)

template <typename BidIt, typename Alloc, typename Elem>
void split(BidIt first, BidIt last, split_results<BidIt, Alloc> &result,
           const Elem *delim, split_flags flag = split_flags())
{
  detail::split_results_builder<BidIt, Alloc> rb(result, first);
  const Elem *delim_end = delim;
  while (*delim_end != Elem())
    ++delim_end;

  for (;;)
  {
    auto pos = std::search(first, last, delim, delim_end);
    gpcl::sub_string<BidIt> substr{{first, pos}};
    if (!substr.empty() || !flag.is_set(split_flag::skip_empty_string))
    {
      if (flag & split_flag::trim_results)
        rb.add(substr.trimmed());
      else
        rb.add(substr);
    }
    if (pos == last)
      break;

    first = pos + (delim_end - delim);
  }
}

template <typename BidIt, typename Alloc, typename Elem>
void split_old(BidIt first, BidIt last, split_results<BidIt, Alloc> &result,
               const Elem *delim, split_flags flag = split_flags())
{
  detail::split_results_builder<BidIt, Alloc> rb(result, first);

  const std::size_t delim_length = std::strlen(delim);
  auto full_length = std::distance(first, last);

  const Elem *delim_end = delim + delim_length;

  BidIt substr_start = first;

  while (full_length >= delim_length)
  {
    if (std::equal(delim, delim_end, first))
    {
      const gpcl::sub_string<BidIt> substr{{substr_start, first}};
      if (!substr.empty() || !flag.is_set(split_flag::skip_empty_string))
      {
        rb.add(substr);
      }
      first += delim_length;
      substr_start = first;
      full_length -= delim_length;
      continue;
    }
    ++first;
    --full_length;
  }

  const gpcl::sub_string<BidIt> substr{{substr_start, last}};
  if (!substr.empty() || !flag.is_set(split_flag::skip_empty_string))
  {
    rb.add(substr);
  }
}

template <typename Elem, typename Alloc>
void split(const Elem *ptr, split_results<const Elem *, Alloc> &result,
           const Elem *delim, split_flags flag = split_flags())
{
  return gpcl::split(ptr, ptr + std::strlen(ptr), result, delim, flag);
}

template <typename Elem, typename IOTraits, typename IOAlloc, typename Alloc2>
void split(const std::basic_string<Elem, IOTraits, IOAlloc> &str,
           split_results<typename std::basic_string<Elem, IOTraits,
                                                    IOAlloc>::const_iterator,
                         Alloc2> &result,
           const Elem *delim, split_flags flag = split_flags())
{
  return gpcl::split(str.cbegin(), str.cend(), result, delim, flag);
}

} // namespace gpcl

#endif // GPCL_STRING_UTILS_HPP
