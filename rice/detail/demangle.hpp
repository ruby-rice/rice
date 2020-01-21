#ifndef Rice__detail__demangle__hpp_
#define Rice__detail__demangle__hpp_

#include <string>

#ifdef __GNUC__
#include <cxxabi.h>
#include <cstdlib>
#include <cstring>
#endif

namespace Rice
{

namespace detail
{

std::string
demangle(char const * mangled_name)
{
#ifdef __GNUC__
    struct Helper
  {
    Helper(
        char const * mangled_name)
      : name_(0)
    {
      int status = 0;
      name_ = abi::__cxa_demangle(mangled_name, 0, 0, &status);
    }

    ~Helper()
    {
      std::free(name_);
    }

    char * name_;

  private:
    Helper(Helper const &);
    void operator=(Helper const &);
  };

  Helper helper(mangled_name);
  if(helper.name_)
  {
    return helper.name_;
  }
  else
  {
    return mangled_name;
  }
#else
  return mangled_name;
#endif
}

std::string
demangle(std::string const & mangled_name)
{
  return demangle(mangled_name.c_str());
}

} // detail

} // Rice

#endif // Rice__detail__demangle__hpp_

