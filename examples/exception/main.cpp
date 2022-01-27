#include <gpcl.hpp>

#include <cerrno>
#include <cstdio>
#include <sstream>

using cerrno_errinfo = gpcl::error_info<struct cerrno_errinfo_, int>;
using filename_errinfo =
    gpcl::error_info<struct filename_errinfo_, std::string>;

using stacktrace_errinfo =
    gpcl::error_info<struct stacktrace_errinfo_, gpcl::stacktrace>;

std::string tag_invoke(cerrno_errinfo::format_fn, int err)
{
  std::ostringstream ss;

  ss << "["
     << "cerrno_errinfo"
     << "] = { " << err << ", " << std::quoted(gpcl::strerror(err)) << " }";
  return ss.str();
}

std::string tag_invoke(filename_errinfo::format_fn, std::string filename)
{
  std::ostringstream ss;
  ss << "["
     << "filename_errinfo"
     << "] = " << std::quoted(filename);
  return ss.str();
}

auto tag_invoke(stacktrace_errinfo::format_fn, const gpcl::stacktrace &st)
{
  return gpcl::make_iomanip(
      [&st](auto &s) { s << "[stacktrace] = { " << st << " }"; });
}

class my_error : virtual public std::exception, virtual public gpcl::exception
{
};

#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable : 4996)
#endif
FILE *openFile(const char *name)
{
  FILE *p = std::fopen(name, "r");
  if (!p)
    GPCL_THROW_EXCEPTION(my_error()
                         << cerrno_errinfo(errno) << filename_errinfo(name)
                         << stacktrace_errinfo(gpcl::stacktrace::current()));
  return p;
}
#ifdef _MSC_VER
#  pragma warning(pop)
#endif

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
    if (auto filename = gpcl::get_error_info<filename_errinfo>(exc))
    {
      gpcl::cdebug() << "filename: " << *filename << std::endl;
    }
  }
  GPCL_CATCH_END
}
