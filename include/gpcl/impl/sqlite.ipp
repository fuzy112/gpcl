//
// sqlite.ipp
// ~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_IMPL_SQLITE_IPP
#define GPCL_IMPL_SQLITE_IPP

#include <gpcl/assert.hpp>
#include <gpcl/detail/throw_system_error.hpp>
#include <gpcl/exception.hpp>
#include <gpcl/sqlite.hpp>

#include <cassert>
#include <cstring>

namespace gpcl {
namespace sqlite {

#ifdef GPCL_SQLITE
class sqlite_error_category_impl : public error_category
{
public:
  GPCL_NODISCARD const char *name() const noexcept override { return "sqlite"; }

  GPCL_NODISCARD std::string message(int ev) const override
  {
    return sqlite3_errstr(ev);
  }
};

auto sqlite_error_category() -> const error_category &
{
  const static sqlite_error_category_impl i;
  return i;
}

GPCL_NORETURN auto throw_sqlite_error(int err, const char *msg) -> void
{
  GPCL_THROW(system_error(err, sqlite_error_category(), msg));
}

void throw_sqlite_error_if_failed(int result, const char *message, source_location location)
{
  if (result != SQLITE_OK)
  {
    ::gpcl::detail::throw_system_error(result, sqlite_error_category(), message, location);
  }
}

#define GPCL_THROW_SQLITE_ERROR_IF_FAILED(...)  \
  ::gpcl::sqlite::throw_sqlite_error_if_failed((__VA_ARGS__), GPCL_TO_STR(__VA_ARGS__), GPCL_SOURCE_LOCATION_CURRENT_LINE())


database::database(const char *name)
{
  GPCL_THROW_SQLITE_ERROR_IF_FAILED( ::sqlite3_open_v2(
      name, &db_, SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE, nullptr) );
}

database::~database() noexcept
{
  GPCL_VERIFY_0(sqlite3_close(db_));
}

void database::execute(const char *sql)
{
  (void)0;
  GPCL_TRY
  {
    sqlite::statement stmt(*this, sql);
    stmt.step();
  }
  GPCL_CATCH(const std::system_error &err) 
  { 
    GPCL_THROW_EXCEPTION(err); 
  }
  GPCL_CATCH_END
}

statement::statement(database &db, czstring<> sql)
{
  GPCL_THROW_SQLITE_ERROR_IF_FAILED(sqlite3_prepare_v2(db.native_handle(), sql, std::strlen(sql),
                                                       &stmt_, nullptr));
}

statement::~statement()
{
  sqlite3_finalize(stmt_);
}

auto statement::step() -> step_result
{
  int err = sqlite3_step(stmt_);

  if (err == SQLITE_DONE)
    return step_result{nullptr, 0};
  if (err == SQLITE_ROW)
    return step_result{stmt_, 1};

  throw_sqlite_error(err, __func__);
}

auto statement::reset() -> void
{
  GPCL_THROW_SQLITE_ERROR_IF_FAILED( sqlite3_reset(stmt_) );
}

namespace detail {
auto tag_invoke(bind_fn, const bind_proxy &proxy, int col, int number) -> void
{
  GPCL_THROW_SQLITE_ERROR_IF_FAILED(sqlite3_bind_int(proxy.statement_handle(), col, number));
}

auto tag_invoke(bind_fn, const bind_proxy &proxy, int col, sqlite3_int64 number)
    -> void
{
  GPCL_THROW_SQLITE_ERROR_IF_FAILED(sqlite3_bind_int64(proxy.statement_handle(), col, number));
}

auto tag_invoke(bind_fn, const bind_proxy &proxy, int col, double number)
    -> void
{
  GPCL_THROW_SQLITE_ERROR_IF_FAILED(
        sqlite3_bind_double(proxy.statement_handle(), col, number));
}

auto tag_invoke(bind_fn, const bind_proxy &proxy, int col, const char *text)
    -> void
{
  GPCL_THROW_SQLITE_ERROR_IF_FAILED(sqlite3_bind_text(proxy.statement_handle(), col, text,
                              std::strlen(text), SQLITE_TRANSIENT));
}

void bind_static_string(const bind_proxy &proxy, int col, const char *str)
{
  GPCL_THROW_SQLITE_ERROR_IF_FAILED( sqlite3_bind_text(proxy.statement_handle(), col, str,
                                                       std::strlen(str), SQLITE_STATIC));
}

auto tag_invoke(get_fn, const step_result &result, int col, int &number) -> void
{
  number = sqlite3_column_int(result.statement_handle(), col);
}

auto tag_invoke(get_fn, const step_result &result, int col, double &number)
    -> void
{
  number = sqlite3_column_double(result.statement_handle(), col);
}

auto tag_invoke(get_fn, const step_result &result, int col, std::string &text)
    -> void
{
  const auto *str = sqlite3_column_text(result.statement_handle(), col);
  text.assign(reinterpret_cast<const char *>(str));
}

} // namespace detail

void transaction::rollback() noexcept
{
  if (db_)
    db_->execute("ROLLBACK");
  db_ = nullptr;
}

transaction &transaction::operator=(transaction &&other) noexcept
{
  if (db_)
    rollback();
  db_ = other.db_;
  other.db_ = nullptr;
  return *this;
}

void transaction::begin(database &db)
{
  GPCL_ASSERT(db_ == nullptr);
  db.execute("BEGIN TRANSACTION");
  db_ = &db;
}

void transaction::commit()
{
  GPCL_ASSERT(db_ != nullptr);
  auto db = db_;
  db_ = nullptr;
  db->execute("COMMIT");
}

transaction::transaction(database &db) : transaction{}
{
  begin(db);
}

transaction::transaction(transaction &&other) noexcept : db_(other.db_)
{
  other.db_ = nullptr;
}

transaction::~transaction() noexcept
{
  rollback();
}

#endif
} // namespace sqlite
} // namespace gpcl

#endif // GPCL_IMPL_SQLITE_IPP
