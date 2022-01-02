#ifndef TESTS_SYNCBUF_DLL_H
#define TESTS_SYNCBUF_DLL_H

#include "syncbuf_export.h"

#include <gpcl/mutex.hpp>
#include <gpcl/shared_ptr.hpp>
#include <gpcl/syncbuf.hpp>

#include <streambuf>

namespace gpcl_syncbuf_test {

SYNCBUF_EXPORT gpcl::shared_ptr<gpcl::mutex>
get_mutex_for_address(std::streambuf const *s);

}

#endif // TESTS_SYNCBUF_DLL_H
