//
// flat_buffer.hpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2022 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_FLAT_BUFFER_HPP
#define GPCL_FLAT_BUFFER_HPP

#include <gpcl/buffer.hpp>
#include <gpcl/default_allocator.hpp>
#include <gpcl/detail/config.hpp>
#include <gpcl/detail/throw_system_error.hpp>

#include <climits>

namespace gpcl {

template <typename Allocator>
class basic_flat_buffer
{
public:
  static_assert(
      std::is_same<typename std::allocator_traits<Allocator>::value_type,
                   char>::value,
      "");

  basic_flat_buffer(const basic_flat_buffer &) = delete;

  basic_flat_buffer &operator=(const basic_flat_buffer &) = delete;

  explicit basic_flat_buffer(const Allocator &a = Allocator()) noexcept
      : alloc_(a),
        start_(),
        end_(),
        input_(),
        output_()
  {
  }

  ~basic_flat_buffer()
  {
    typedef std::allocator_traits<Allocator> alloc_traits;
    alloc_traits::deallocate(alloc_, start_, end_ - start_);
  }

  Allocator get_allocator() const noexcept { return alloc_; }

  std::size_t size() const noexcept
  {
    std::ptrdiff_t r = output_ - input_;
    GPCL_ASSERT(r >= 0);
    return r;
  }

  std::size_t max_size() const noexcept { return INT_MAX; }

  std::size_t capacity() const noexcept
  {
    std::ptrdiff_t r = end_ - input_;
    GPCL_ASSERT(r >= 0);
    return r;
  }

  const_buffer data() const noexcept { return buffer(input_, size()); }

  mutable_buffer prepare(std::size_t n)
  {
    if (size() + n > max_size())
    {
      GPCL_THROW(std::length_error("basic_flat_buffer::prepare"));
    }

    if (end_ - output_ < n)
    {
      if (end_ - output_ + (input_ - start_) >= n)
      {
        std::memmove(start_, input_, size());
        std::size_t isize = size();
        input_ = start_;
        output_ = input_ + isize;
      }
      else
      {
        typedef std::allocator_traits<Allocator> alloc_traits;
        char *newbuf =
            static_cast<char *>(alloc_traits::allocate(alloc_, size() + n));
        if (size() != 0)
          std::memcpy(newbuf, input_, size());
        alloc_traits::deallocate(alloc_, start_, end_ - start_);
        std::size_t isize = size();
        start_ = newbuf;
        input_ = start_;
        output_ = input_ + isize;
        end_ = start_ + isize + n;
      }
    }

    return buffer(output_, n);
  }

  void commit(std::size_t n) noexcept
  {
    output_ += n;
    if (output_ > end_)
      output_ = end_;
  }

  void consume(std::size_t n) noexcept
  {
    input_ += n;
    if (input_ > output_)
      input_ = output_;
  }

private:
  Allocator alloc_;

  char *start_;
  char *end_;
  char *input_;
  char *output_;
};

using flat_buffer = basic_flat_buffer<default_allocator<char>>;

} // namespace gpcl

#endif // GPCL_FLAT_BUFFER_HPP
