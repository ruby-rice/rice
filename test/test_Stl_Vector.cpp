#include "unittest.hpp"
#include "embed_ruby.hpp"
#include <rice/rice.hpp>
#include <rice/stl.hpp>

#include <memory>

using namespace Rice;

TESTSUITE(Vector);

SETUP(Vector)
{
  embed_ruby();
}

/*namespace
{

  class MyClass
  {
  public:
    std::vector<std::string> stringVector()
    {
      std::vector<std::string> result{ "One", "Two", "Three" };
      return result;
    }
  };
}
/*
Class makeClass()
{
  Class c= define_class<MyClass>("MyClass").
    define_constructor(Constructor<MyClass>()).
    define_method("stringVector", &MyClass::stringVector);

  return c;
}*/

TESTCASE(StringVector)
{
  Class c = define_vector<std::vector<std::string>>("StringVector");
  Object vec = c.call("new");
  
  Object result = vec.call("size");
  ASSERT_EQUAL(0, detail::From_Ruby<int32_t>::convert(result));

  result = vec.call("first");
  ASSERT_EQUAL(Qnil, result.value());
}
