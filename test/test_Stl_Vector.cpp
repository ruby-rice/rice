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

/*TESTCASE(StringVector)
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

TESTCASE(WrongType)
{
  Module m = define_module("Testing");

  Class c = define_vector<std::vector<std::string>>("StringVector");

  Object vec = m.instance_eval("$vector = StringVector.new");
  ASSERT_EXCEPTION_CHECK(
    Exception,
    m.instance_eval("$vector << 1"),
    ASSERT_EQUAL("wrong argument type Integer (expected String)", ex.what()));
}

TESTCASE(Empty)
{
  Module m = define_module("Testing");

  Class c = define_vector<std::vector<std::int32_t>>("IntVector");
  Object vec = c.call("new");

  Object result = vec.call("size");
  ASSERT_EQUAL(0, detail::From_Ruby<int32_t>::convert(result));

  result = vec.call("empty?");
  ASSERT_EQUAL(Qtrue, result.value());

  result = vec.call("first");
  ASSERT_EQUAL(Qnil, result.value());

  result = vec.call("last");
  ASSERT_EQUAL(Qnil, result.value());
}

TESTCASE(Indexing)
{
  Module m = define_module("Testing");

  Class c = define_vector<std::vector<std::int32_t>>("IntVector");
  Object vec = c.call("new");
  vec.call("push", 0);
  vec.call("push", 1);
  vec.call("push", 2);
  
  Object result = vec.call("size");
  ASSERT_EQUAL(3, detail::From_Ruby<int32_t>::convert(result));

  result = vec.call("[]", 0);
  ASSERT_EQUAL(0, detail::From_Ruby<int32_t>::convert(result));

  result = vec.call("[]", 1);
  ASSERT_EQUAL(1, detail::From_Ruby<int32_t>::convert(result));

  result = vec.call("[]", 2);
  ASSERT_EQUAL(2, detail::From_Ruby<int32_t>::convert(result));

  result = vec.call("[]", 3);
  ASSERT_EQUAL(Qnil, result.value());

  result = vec.call("[]", -1);
  ASSERT_EQUAL(2, detail::From_Ruby<int32_t>::convert(result));

  result = vec.call("[]", -2);
  ASSERT_EQUAL(1, detail::From_Ruby<int32_t>::convert(result));

  result = vec.call("[]", -3);
  ASSERT_EQUAL(0, detail::From_Ruby<int32_t>::convert(result));

  result = vec.call("[]", -4);
  ASSERT_EQUAL(2, detail::From_Ruby<int32_t>::convert(result));

  result = vec.call("[]", -7);
  ASSERT_EQUAL(2, detail::From_Ruby<int32_t>::convert(result));
}

TESTCASE(Sizing)
{
  Module m = define_module("Testing");

  Class c = define_vector<std::vector<std::int32_t>>("IntVector");
  Object vec = c.call("new");
  vec.call("resize", 10);

  Object result = vec.call("size");
  ASSERT_EQUAL(10, detail::From_Ruby<int32_t>::convert(result));

  vec.call("clear");

  result = vec.call("size");
  ASSERT_EQUAL(0, detail::From_Ruby<int32_t>::convert(result));
}

TESTCASE(ToString)
{
  Module m = define_module("Testing");

  Class c = define_vector<std::vector<std::int32_t>>("IntVector");
  Object vec = c.call("new");
  vec.call("resize", 3);

  Object result = vec.call("to_s");
  ASSERT_EQUAL("[0, 0, 0]", detail::From_Ruby<std::string>::convert(result));

  vec.call("clear");

  result = vec.call("to_s");
  ASSERT_EQUAL("[]", detail::From_Ruby<std::string>::convert(result));
}

TESTCASE(Update)
{
  Module m = define_module("Testing");

  Class c = define_vector<std::vector<std::string>>("StringVector");
  Object vec = c.call("new");
  vec.call("push", "original 1");
  vec.call("push", "original 2");

  Object result = vec.call("size");
  ASSERT_EQUAL(2, detail::From_Ruby<int32_t>::convert(result));

  result = vec.call("[]=", 1, "new 2");
  ASSERT_EQUAL("new 2", detail::From_Ruby<std::string>::convert(result));

  result = vec.call("[]", 1);
  ASSERT_EQUAL("new 2", detail::From_Ruby<std::string>::convert(result));

  ASSERT_EXCEPTION_CHECK(
    Exception,
    result = vec.call("[]=", 99999, "new 2"),
    ASSERT_EQUAL("Invalid index: 99999", ex.what()));
}*/

TESTCASE(Modify)
{
  Module m = define_module("Testing");

  Class c = define_vector<std::vector<int64_t>>("Int64Vector");
  Object vec = c.call("new");

  Object result = vec.call("push", 11);
  ASSERT(result.is_equal(vec));

  result = vec.call("push", 22);
  ASSERT(result.is_equal(vec));

  result = vec.call("size");
  ASSERT_EQUAL(2, detail::From_Ruby<int32_t>::convert(result));

  result = vec.call("insert", 1, 33);
  ASSERT(result.is_equal(vec));

  result = vec.call("to_s");
  ASSERT_EQUAL("[11, 33, 22]", detail::From_Ruby<std::string>::convert(result));

  result = vec.call("pop");
  ASSERT_EQUAL(22, detail::From_Ruby<int64_t>::convert(result));

  result = vec.call("size");
  ASSERT_EQUAL(2, detail::From_Ruby<int32_t>::convert(result));

  result = vec.call("pop");
  ASSERT_EQUAL(33, detail::From_Ruby<int64_t>::convert(result));

  result = vec.call("size");
  ASSERT_EQUAL(1, detail::From_Ruby<int32_t>::convert(result));

  result = vec.call("pop");
  ASSERT_EQUAL(11, detail::From_Ruby<int64_t>::convert(result));

  result = vec.call("size");
  ASSERT_EQUAL(0, detail::From_Ruby<int32_t>::convert(result));

  result = vec.call("pop");
  ASSERT_EQUAL(Qnil, result.value());
}

