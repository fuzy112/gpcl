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
  std::string str;
  str.resize(100);

#if defined(__STDC_LIB_EXT1__)
  strerror_s(&str[0], str.size(), ei.value());
#elif defined(GPCL_POSIX)
  int err;
  do
  {
    err = strerror_r(ei.value(), &str[0], str.size());
    if (err == EINVAL)
      str = "Unknown error";
    if (err == 0)
      break;
    str.resize(str.size() * 2);
  } while (err == ERANGE);
#else
  str = std::strerror(ei.value());
#endif
  str.resize(strlen(str.c_str()));

  ss << "["
     << "cerrno_errinfo"
     << " ] = { " << ei.value() << ", " << std::quoted(str) << " }";
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
  GPCL_CATCH(gpcl::exception const &exc)
  {
    gpcl::cdebug() << "Exception caught: " << exc << std::endl;
  }
  GPCL_CATCH_END
}
