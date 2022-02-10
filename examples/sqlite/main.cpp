#include <gpcl/sqlite.hpp>

int main()
{
#ifdef GPCL_SQLITE

  using namespace gpcl::sqlite;

  database db("test.db");

  db.execute(R"sql(
    CREATE TABLE t1(n integer, s text);
  )sql");

  statement stmt(db, "insert into t1 (n, s) values (?, ?)");

  stmt.binder() << 1 << "1";
  stmt.step();

#endif
}
