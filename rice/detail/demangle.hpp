#ifndef Rice__detail__demangle__hpp_
#define Rice__detail__demangle__hpp_

#include <string>

namespace Rice
{

namespace detail
{

std::string demangle(char const * mangled_name);
std::string demangle(std::string const & mangled_name);

} // detail

} // Rice

#endif // Rice__detail__demangle__hpp_
