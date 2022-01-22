#include <gpcl/debugstream.hpp>
#include <gpcl/error.hpp>
#include <gpcl/exception.hpp>
#include <gpcl/stacktrace.hpp>

#include <cerrno>
#include <cstdio>
#include <sstream>

using cerrno_errinfo = gpcl::error_info<struct cerrno_errinfo_, int>;
using filename_errinfo =
    gpcl::error_info<struct filename_errinfo_, std::string>;
using stacktrace_errinfo =
    gpcl::error_info<struct stacktrace_errinfo_, gpcl::stacktrace>;

std::string to_string(cerrno_errinfo const &ei)
{
  std::ostringstream ss;
  ss << "[" << typeid(filename_errinfo::tag *).name() << " ] = { " << ei.value()
     << ", " << std::quoted(std::strerror(ei.value())) << " }";
  return ss.str();
}

std::string to_string(filename_errinfo const &ei)
{
  std::ostringstream ss;
  ss << "[" << typeid(filename_errinfo::tag *).name()
     << "] = " << std::quoted(ei.value());
  return ss.str();
}

class my_error : virtual public std::exception, virtual public gpcl::exception
{
};

FILE *openFile(const char *name)
{
  FILE *p = std::fopen(name, "r");
  if (!p)
    GPCL_THROW(my_error() << cerrno_errinfo(errno) << filename_errinfo(name)
                          << stacktrace_errinfo(gpcl::stacktrace::current()));
  return p;
}

int main()
{
  GPCL_TRY
  {
    FILE *fp = openFile("non-existing-file.txt");
    std::fclose(fp);
  }
  GPCL_CATCH(gpcl::exception const &exc)
  {
    gpcl::cdebug() << "Exception caught: " << exc << std::endl;
  }
  GPCL_CATCH_END
}