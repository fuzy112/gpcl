//
// sqlite.hpp
// ~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_EXT_SQLITE_HPP
#define GPCL_EXT_SQLITE_HPP

#include <gpcl/detail/utility.hpp>
#include <gpcl/ext/detail/config.hpp>
#include <gpcl/tag_invoke.hpp>
#include <gpcl/zstring.hpp>

#include <system_error>

#ifdef GPCL_EXT_ENABLE_SQLITE
#  include <sqlite3.h>
#endif

namespace gpcl::ext::sqlite {

#if defined GPCL_EXT_ENABLE_SQLITE || defined GPCL_DOXYGEN

class database;
class query;
class statement;
class bind_proxy;
class step_result;

GPCL_DECL auto sqlite_error_category() -> std::error_category const &;

/// 數據庫連接
class database final
{
public:
  /// 創建數據庫連接
  GPCL_DECL explicit database(czstring<> dbname = ":memory:");

  database(database &&other) noexcept
      : db_(detail::exchange(other.db_, nullptr))
  {
  }

  /// 銷毀數據庫連接
  GPCL_DECL ~database() noexcept;

  /// 運行 SQL 語句
  GPCL_DECL auto execute(czstring<> sql) -> void;

  using native_handle_type = sqlite3 *;
  auto native_handle() -> native_handle_type
  {
    GPCL_ASSERT(db_);
    return db_;
  }

private:
  sqlite3 *db_;
};

namespace detail {

struct bind_fn
{
  template <typename T,
            std::enable_if_t<std::is_void_v<tag_invoke_result_t<
                                 bind_fn, const bind_proxy &, int, T &&>>,
                             int> = 0>
  void operator()(const bind_proxy &proxy, int col, T &&value) const
  {
    return gpcl::cpo::tag_invoke(*this, proxy, col, static_cast<T &&>(value));
  }
};

GPCL_DECL auto tag_invoke(bind_fn, const bind_proxy &proxy, int col,
                          sqlite3_int64 number) -> void;
GPCL_DECL auto tag_invoke(bind_fn, const bind_proxy &proxy, int col, int number)
    -> void;
GPCL_DECL auto tag_invoke(bind_fn, const bind_proxy &proxy, int col,
                          const char *str) -> void;
GPCL_DECL auto tag_invoke(bind_fn, const bind_proxy &proxy, int col,
                          double number) -> void;

inline auto tag_invoke(bind_fn, const bind_proxy &proxy, int col,
                       unsigned long long number) -> void
{
  tag_invoke(bind_fn, proxy, col, static_cast<sqlite3_int64>(number));
}

inline auto tag_invoke(bind_fn, const bind_proxy &proxy, int col,
                       unsigned int number) -> void
{
  tag_invoke(bind_fn, proxy, col, static_cast<sqlite3_int64>(number));
}

GPCL_DECL void bind_static_string(const bind_proxy &proxy, int col,
                                  const char *str);

template <std::size_t N>
inline auto tag_invoke(bind_fn, const bind_proxy &proxy, int col,
                       const char(&&str)[N]) -> void
{
  bind_static_string(proxy, col, str);
}

} // namespace detail

using bind_fn = detail::bind_fn;

/// Binds a value to a column.
///
/// @ingroup customisation_point
/// @remark This is a customisation point.
///
inline constexpr bind_fn bind{};

namespace detail {

struct get_fn
{
  template <typename T,
            std::enable_if_t<std::is_void_v<tag_invoke_result_t<
                                 get_fn, const step_result &, int, T &>>,
                             int> = 0>
  void operator()(const step_result &result, int column, T &value) const
  {
    return gpcl::cpo::tag_invoke(*this, column, value);
  }
};

auto tag_invoke(get_fn, const step_result &result, int col, int &number)
    -> void;
auto tag_invoke(get_fn, const step_result &result, int col, double &number)
    -> void;
auto tag_invoke(get_fn, const step_result &result, int col, std::string &number)
    -> void;

} // namespace detail

using get_fn = detail::get_fn;

inline constexpr get_fn get{};

class bind_proxy final
{
  sqlite3_stmt *stmt_;
  int col_;

public:
  explicit bind_proxy(sqlite3_stmt *stmt, int column)
      : stmt_(stmt),
        col_(column)
  {
  }

  bind_proxy(bind_proxy &&) = default;

  /// Bind the next parameter
  template <typename T>
  inline auto operator<<(T &&x) && -> bind_proxy
  {
    bind(*this, col_++, gpcl::detail::forward<T>(x));
    return gpcl::detail::move(*this);
  }

  [[nodiscard]] auto column() const -> int { return col_; }
  [[nodiscard]] auto statement_handle() const noexcept -> sqlite3_stmt *
  {
    return stmt_;
  }
};

class step_result final
{
  sqlite3_stmt *stmt_;
  int col_;

public:
  explicit step_result(sqlite3_stmt *stmt, int column)
      : stmt_(stmt),
        col_(column)
  {
  }

  step_result(step_result &&) = default;

  /// Fetch the next result
  template <typename T>
  inline auto operator>>(T &x) && -> step_result
  {
    get(*this, col_, x);
    col_++;
    return gpcl::detail::move(*this);
  }

  explicit operator bool() const { return stmt_; }

  [[nodiscard]] auto column() const noexcept -> int { return col_; }

  [[nodiscard]] auto statement_handle() const noexcept -> sqlite3_stmt *
  {
    return stmt_;
  }
};

/// A prepared statement
class statement final
{
  sqlite3_stmt *stmt_ = nullptr;

public:
  statement() = default;

  /// Create a prepared statement
  GPCL_DECL statement(database &db, const char *sql);

  statement(statement &&other) noexcept : stmt_(other.stmt_)
  {
    other.stmt_ = nullptr;
  }

  /// Destroy a statement
  GPCL_DECL ~statement();

  /// Assigment
  statement &operator=(statement &&other) noexcept
  {
    this->~statement();
    ::new (this) statement(gpcl::detail::move(other));
    return *this;
  }

  /// Execute the statement
  GPCL_DECL auto step() -> step_result;

  /// Reset the statement to prepare for the next execution
  GPCL_DECL auto reset() -> void;

  /// Get the parameter binder
  auto binder(int start_col = 1) -> bind_proxy
  {
    return bind_proxy{stmt_, start_col};
  }

  using native_handle_type = sqlite3_stmt *;
  auto native_handle() -> native_handle_type { return stmt_; }
};

/// RAII guard for transaction
class transaction final
{
  database *db_{};

public:
  /// Create an inactive transaction
  transaction() = default;

  /// Begin a transaction for db
  GPCL_DECL explicit transaction(database &db);

  /// Move constructor
  GPCL_DECL transaction(transaction &&other) noexcept;

  /// Disable copy
  transaction(const transaction &) = delete;
  transaction &operator=(const transaction &) = delete;

  /// Destructor
  ///
  /// If the transaction is active, it will be rolled back.
  GPCL_DECL ~transaction() noexcept;

  /// Move asignment operator
  GPCL_DECL transaction &operator=(transaction &&other) noexcept;

  /// Begin a transaction for db
  ///
  /// \pre The transaction must be inactive.
  /// \post The transaction shall be active
  GPCL_DECL void begin(database &db);

  /// Commit the transaction.
  ///
  /// \pre The transaction shall be active.
  /// \post The transaction shall be inactive.
  GPCL_DECL void commit();

  /// Rollback the transaction
  ///
  /// \pre The transaction shall be active.
  /// \post The transaction shall be inactive.
  GPCL_DECL void rollback() noexcept;
};
#endif

} // namespace gpcl::ext::sqlite

#endif // GPCL_EXT_SQLITE_HPP
