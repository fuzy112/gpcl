//
// const_buffer.hpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (c) 2022 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_CONST_BUFFER_HPP
#define GPCL_CONST_BUFFER_HPP

#include <gpcl/detail/config.hpp>

namespace gpcl {

/// Constant buffer.
class const_buffer
{
public:
  constexpr const_buffer() = default;

  constexpr const_buffer(const void *p, std::size_t size) noexcept
      : p_{static_cast<const char *>(p)},
        s_{size}
  {
  }

  const void *data() const noexcept { return p_; }

  std::size_t size() const noexcept { return s_; }

  [[deprecated]] std::size_t size_bytes() const noexcept { return size(); }

  const_buffer &operator+=(std::size_t bytes) noexcept
  {
    if (s_ > bytes)
    {
      s_ -= bytes;
      p_ += bytes;
    }
    else
    {
      p_ = nullptr;
      s_ = 0;
    }
    return *this;
  }

private:
  const char *p_{};
  std::size_t s_{};
};

/// Mutable buffer.
class mutable_buffer
{
public:
  constexpr mutable_buffer() = default;

  constexpr mutable_buffer(void *p, std::size_t size) noexcept
      : p_{static_cast<char *>(p)},
        s_{size}
  {
  }

  void *data() const noexcept { return p_; }

  std::size_t size() const noexcept { return s_; }

  [[deprecated]] std::size_t size_bytes() const noexcept { return size(); }

  mutable_buffer &operator+=(std::size_t bytes) noexcept
  {
    if (s_ > bytes)
    {
      s_ -= bytes;
      p_ += bytes;
    }
    else
    {
      p_ = nullptr;
      s_ = 0;
    }
    return *this;
  }

  operator const_buffer() const noexcept
  {
    return const_buffer(data(), size());
  }

private:
  char *p_{};
  std::size_t s_{};
};

} // namespace gpcl

#endif // GPCL_CONST_BUFFER_HPP
