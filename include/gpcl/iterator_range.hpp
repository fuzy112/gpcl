//
// iterator_range.hpp
// ~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2022-2026 Zhengyi Fu <i@fuzy.me>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_ITERATOR_RANGE_HPP
#define GPCL_ITERATOR_RANGE_HPP

#include <gpcl/detail/config.hpp>

namespace gpcl {

template <typename Iterator>
class iterator_range
{
  Iterator first_{};
  Iterator last_{};

public:
  iterator_range(Iterator first, Iterator last) : first_(first), last_(last) {}

  Iterator begin() const { return first_; }

  Iterator end() const { return last_; }
};

} // namespace gpcl

#endif // GPCL_ITERATOR_RANGE_HPP
