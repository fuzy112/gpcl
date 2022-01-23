#include <gpcl.hpp>

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


  ss << "["
     << "cerrno_errinfo"
     << " ] = { " << ei.value() << ", " << std::quoted(gpcl::strerror(ei.value())) << " }";
  return ss.str();
}

std::string to_string(filename_errinfo const &ei)
{
  std::ostringstream ss;
  ss << "["
     << "filename_errinfo"
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
  GPCL_CATCH(my_error const &exc)
  {
    gpcl::cdebug() << exc << std::endl;
  }
  GPCL_CATCH_END
}
