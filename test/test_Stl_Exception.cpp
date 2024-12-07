#include <complex>
#include <memory>

#include "unittest.hpp"
#include "embed_ruby.hpp"
#include <rice/rice.hpp>
#include <rice/stl.hpp>

using namespace Rice;

TESTSUITE(Exception);

SETUP(Exception)
{
  embed_ruby();
}

TEARDOWN(Exception)
{
  rb_gc_start();
}

namespace
{
  class MyException: public std::exception
  {
  public:
    std::string hello()
    {
      return "Hello";
    }
  };
}

TESTCASE(Create)
{
  Module m = define_module("Testing");
  Class c = define_class<MyException, std::exception>("MyException").
            define_constructor(Constructor<MyException>()).
            define_method("hello", &MyException::hello);

  Object exception = c.create();
  Object result = exception.call("hello");
  ASSERT_EQUAL("Hello", detail::From_Ruby<std::string>().convert(result.value()));

  result = exception.call("what");
  ASSERT_EQUAL("Unknown exception", detail::From_Ruby<std::string>().convert(result.value()));
}