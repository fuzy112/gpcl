#include <gpcl/osyncstream.hpp>

#include <iostream>

int main()
{
  {
    gpcl::osyncstream synced_out(
        std::cout); // synchronized wrapper for std::cout
    synced_out << "Hello, ";
    synced_out << "World!";
    synced_out << std::endl; // flush is noted, but not yet performed
    synced_out << "and more!\n";
  } // characters are transferred and std::cout is flushed
}
