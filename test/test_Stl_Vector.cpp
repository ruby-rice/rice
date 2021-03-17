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
  Module m = define_module("Testing");

  Class c = define_vector<std::vector<std::string>>("StringVector");

  Object vec = m.instance_eval("$vector = StringVector.new");
  Object result = vec.call("size");
  ASSERT_EQUAL(0, detail::From_Ruby<int32_t>::convert(result));

  m.instance_eval("$vector << 'one' << 'two' << 'two' << 'three'");
  result = vec.call("size");
  ASSERT_EQUAL(4, detail::From_Ruby<int32_t>::convert(result));

  m.instance_eval("$vector.append('four')");
  result = vec.call("size");
  ASSERT_EQUAL(5, detail::From_Ruby<int32_t>::convert(result));

  result = vec.call("first");
  ASSERT_EQUAL("one", detail::From_Ruby<std::string>::convert(result));

  result = vec.call("last");
  ASSERT_EQUAL("four", detail::From_Ruby<std::string>::convert(result));
}

TESTCASE(StringVectorWrongType)
{
  Module m = define_module("Testing");

  Class c = define_vector<std::vector<std::string>>("StringVector");

  Object vec = m.instance_eval("$vector = StringVector.new");
  ASSERT_EXCEPTION_CHECK(Exception,
    m.instance_eval("$vector << 1"),
    ASSERT_EQUAL("wrong argument type Integer (expected String)", ex.what()));
}

TESTCASE(EmptyVector)
{
  Module m = define_module("Testing");

  Class c = define_vector<std::vector<std::int32_t>>("IntVector");

  Object vec = m.instance_eval("$vector = IntVector.new");
  ASSERT_EXCEPTION_CHECK(Exception,
    m.instance_eval("$vector.first"),
    ASSERT_EQUAL("wrong argument type Integer (expected String)", ex.what()));
}
