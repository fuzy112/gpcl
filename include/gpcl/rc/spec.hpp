//
// spec.hpp
// ~~~~~~~~
//
// Copyright (c) 2022 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_RC_SPEC_HPP
#define GPCL_RC_SPEC_HPP

#include <gpcl/span.hpp>
#include <gpcl/variant.hpp>
#include <gpcl/array.hpp>

#include <string>

namespace gpcl {
namespace rc {
namespace spec {
class item;
class dir;

class node;

class item
{
  std::string name_;
  std::string file_path_;

public:
  item() = default;

  item(std::string name, std::string file_path)
      : name_(std::move(name)),
        file_path_(std::move(file_path))
  {
  }

  item(const item &) = default;
  item(item &&) noexcept = default;

  item &operator=(const item &) = default;
  item &operator=(item &&) noexcept = default;

  std::string name() const { return name_; }

  std::string file_path() const { return file_path_; }
};

class dir
{
  std::string name_;
  gpcl::array<node> nodes_;

public:
  dir() = default;

  explicit dir(std::string name, gpcl::array<node> nodes)
      : name_(std::move(name)),
        nodes_(std::move(nodes))
  {
  }

  dir(const dir &) = default;
  dir(dir &&) noexcept = default;

  dir &operator=(const dir &) = default;
  dir &operator=(dir &&) noexcept = default;

  std::string name() const { return name_; }

  span<const node> nodes() const { return nodes_; }
};

class node
{
  using data_t = variant<item, dir>;
  data_t data_;

public:
  enum class node_category
  {
    item = meta::find_index<data_t, rc::item>::type::value,
    dir = meta::find_index<data_t, rc::dir>::type::value,
  };

  node(item it) : data_(std::move(it)) {}

  node(dir d) : data_(std::move(d)) {}

  node(const node &) = default;
  node(node &&) noexcept = default;

  node &operator=(const node &) = default;
  node &operator=(node &&) noexcept = default;

  node_category category() const
  {
    return static_cast<node_category>(data_.index());
  }

  std::string name() const
  {
    return visit([](auto &data) { return data.name(); }, data_);
  }

  item const &as_item() const { return get<item>(data_); }

  dir const &as_dir() const { return get<dir>(data_); }
};
} // namespace spec

} // namespace rc
} // namespace gpcl

#endif // GPCL_RC_SPEC_HPP
