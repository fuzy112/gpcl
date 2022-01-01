#include <gpcl/osyncstream.hpp>
#include <gpcl/thread.hpp>
#include <gpcl/vector.hpp>

#include <iostream>

int main()
{
  gpcl::vector<gpcl::thread> threads;

  for (int i = 0; i != 10; ++i)
    threads.push_back(gpcl::thread([] {
      gpcl::osyncstream synced_out(
          std::cout); // synchronized wrapper for std::cout
      synced_out << "Hello, ";
      synced_out << "World!";
      synced_out << std::endl; // flush is noted, but not yet performed
      synced_out << "and more!\n";
    } // characters are transferred and std::cout is flushed
                                   ));

  for (auto &&t : threads)
    t.join();
}
