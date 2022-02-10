#include <gpcl/sqlite.hpp>
#include <gpcl/exception.hpp>
#include <gpcl/error.hpp>
#include <gpcl/debugstream.hpp>

int main()
{
#ifdef GPCL_SQLITE

  using namespace gpcl::sqlite;

  GPCL_TRY
  {
    database db("test.db");

    db.execute(R"sql(
      CREATE TABLE t1(n integer, s text);
    )sql");

    statement stmt(db, "insert into t1 (n, s) values (?, ?)");

    stmt.binder() << 1 << "1";
    stmt.step();
  }
  GPCL_CATCH(std::exception const &exc)
  {
    gpcl::cdebug() << gpcl::diagnostic_information(exc) << std::endl;
  }
  GPCL_CATCH_END
#endif
}
