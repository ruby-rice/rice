#include "unittest.hpp"
#include "embed_ruby.hpp"
#include <rice/rice.hpp>
#include <rice/stl.hpp>

#include <complex>
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

Class makeClass()
{
  Class c= define_class<MyClass>("MyClass").
    define_constructor(Constructor<MyClass>()).
    define_method("stringVector", &MyClass::stringVector);

  return c;
}

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
}

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

  result = vec.call("delete", 11);
  ASSERT_EQUAL(11, detail::From_Ruby<int64_t>::convert(result));

  result = vec.call("size");
  ASSERT_EQUAL(2, detail::From_Ruby<int32_t>::convert(result));

  result = vec.call("delete_at", 0);
  ASSERT_EQUAL(33, detail::From_Ruby<int64_t>::convert(result));

  result = vec.call("size");
  ASSERT_EQUAL(1, detail::From_Ruby<int32_t>::convert(result));

  result = vec.call("pop");
  ASSERT_EQUAL(22, detail::From_Ruby<int64_t>::convert(result));

  result = vec.call("size");
  ASSERT_EQUAL(0, detail::From_Ruby<int32_t>::convert(result));

  result = vec.call("pop");
  ASSERT_EQUAL(Qnil, result.value());
}

TESTCASE(Iterate)
{
  Module m = define_module("Testing");
  Class c = define_vector<std::vector<double>>("DoubleVector");

  std::string code = R"(vector = DoubleVector.new
                        vector << 5.0 << 6.0 << 7.0
                        updated = vector.map do |value|
                                    value * 2.0
                                  end)";

  Array result = m.instance_eval(code);
  ASSERT_EQUAL(3, result.size());
  ASSERT_EQUAL(10.0, detail::From_Ruby<double>::convert(result[0].value()));
  ASSERT_EQUAL(12.0, detail::From_Ruby<double>::convert(result[1].value()));
  ASSERT_EQUAL(14.0, detail::From_Ruby<double>::convert(result[2].value()));
}

namespace
{
  class NotComparable
  {
  public:
    NotComparable(uint32_t value) : value_(value)
    {
    };

    uint32_t value_;
  };
}

TESTCASE(NotComparable)
{
  define_class<NotComparable>("NotComparable").
    define_constructor(Constructor<NotComparable, uint32_t>());

  Class c = define_vector<std::vector<NotComparable>>("NotComparableVector");

  Object vec = c.call("new");
  vec.call("push", NotComparable(1));
  vec.call("push", NotComparable(2));
  vec.call("push", NotComparable(3));

  Object result = vec.call("delete", NotComparable(1));
  ASSERT_EQUAL(Qnil, result.value());

  result = vec.call("length");
  ASSERT_EQUAL(3, detail::From_Ruby<size_t>::convert(result));

  result = vec.call("include?", NotComparable(2));
  ASSERT_EQUAL(Qfalse, result.value());

  result = vec.call("index", NotComparable(3));
  ASSERT_EQUAL(Qnil, result.value());
}

TESTCASE(NotDefaultConstructable)
{
  define_class<NotComparable>("NotComparable").
    define_constructor(Constructor<NotComparable, uint32_t>());
    
  Class c = define_vector<std::vector<NotComparable>>("NotComparableVector");
  Object vec = c.call("new");

  Object result = vec.call("resize", 10);
  ASSERT_EQUAL(Qnil, result.value());

  result = vec.call("length");
  ASSERT_EQUAL(0, detail::From_Ruby<size_t>::convert(result));
}

TESTCASE(NotPrintable)
{
  define_class<NotComparable>("NotComparable").
    define_constructor(Constructor<NotComparable, uint32_t>());

  Class c = define_vector<std::vector<NotComparable>>("NotComparableVector");

  Object vec = c.call("new");
  vec.call("push", NotComparable(1));
  vec.call("push", NotComparable(2));
  vec.call("push", NotComparable(3));

  Object result = vec.call("to_s");
  ASSERT_EQUAL("[Not printable]", detail::From_Ruby<std::string>::convert(result));
}

c
TESTCASE(Comparable)
{
  define_class<Comparable>("IsComparable").
    define_constructor(Constructor<Comparable, uint32_t>());

  Class c = define_vector<std::vector<Comparable>>("ComparableVector");

  Object vec = c.call("new");
  
  Comparable comparable1(1);
  vec.call("push", comparable1);

  Comparable comparable2(2);
  vec.call("push", comparable2);

  Comparable comparable3(3);
  vec.call("push", comparable3);

  Object result = vec.call("delete", Comparable(1));
  Comparable comparable = detail::From_Ruby<Comparable>::convert(result);
  ASSERT_EQUAL(1, comparable.value_);

  result = vec.call("length");
  ASSERT_EQUAL(2, detail::From_Ruby<size_t>::convert(result));

  result = vec.call("include?", Comparable(2));
  ASSERT_EQUAL(Qtrue, result.value());

  result = vec.call("index", Comparable(3));
  ASSERT_EQUAL(1, detail::From_Ruby<size_t>::convert(result.value()));
}

TESTCASE(DefaultConstructable)
{
  define_class<Comparable>("IsComparable").
    define_constructor(Constructor<Comparable, uint32_t>());

  Class c = define_vector<std::vector<Comparable>>("ComparableVector");
  Object vec = c.call("new");

  Object result = vec.call("resize", 10);
  ASSERT_EQUAL(Qnil, result.value());

  result = vec.call("length");
  ASSERT_EQUAL(0, detail::From_Ruby<size_t>::convert(result));
}

TESTCASE(Printable)
{
  define_class<Comparable>("IsComparable").
    define_constructor(Constructor<Comparable, uint32_t>());

  Class c = define_vector<std::vector<Comparable>>("ComparableVector");

  Object vec = c.call("new");
  vec.call("push", Comparable(1));
  vec.call("push", Comparable(2));
  vec.call("push", Comparable(3));

  Object result = vec.call("to_s");
  ASSERT_EQUAL("[Comparable(1), Comparable(2), Comparable(3)]", detail::From_Ruby<std::string>::convert(result));
}*/

namespace
{
  std::vector<std::complex<uint32_t>> returnComplexVector()
  {
    std::complex<uint32_t> complex1(1, 1);
    std::complex<uint32_t> complex2(2, 2);
    std::complex<uint32_t> complex3(3, 3);

    std::vector<std::complex<uint32_t>> result;
    result.push_back(complex1);
    result.push_back(complex2);
    result.push_back(complex3);
    return result;
  }

  std::vector<std::complex<double>> passComplexVector(std::vector<std::complex<double>>& complexes)
  {
    return complexes;
  }
}
/*
TESTCASE(AutoRegisterReturn)
{
  define_global_function("return_complex_vector", &returnComplexVector);

  Module m = define_module("Testing");
  Object vec = m.instance_eval("return_complex_vector");
  ASSERT_EQUAL("Rice::Vector::ComplexUnsignedInt", vec.class_name().str());

  std::string code = R"(vector = return_complex_vector
                        complex = vector.last
                        complex == Complex(3, 3))";

  Object result = m.instance_eval(code);
  ASSERT_EQUAL(Qtrue, result.value());
}
*/
TESTCASE(AutoRegisterParameter)
{
  define_global_function("pass_complex_vector", &passComplexVector);

  std::string code = R"(vector = Rice::Vector::ComplexDouble.new
                        vector << Complex(4.0, 4.0)
                        vector << Complex(5.0, 5.0)
                        pass_complex_vector(vector))";

  Module m = define_module("Testing");
  Object vec = m.instance_eval(code);

  Object result = vec.call("size");
  ASSERT_EQUAL("Rice::Vector::ComplexDouble", vec.class_name().str());
  ASSERT_EQUAL(2, detail::From_Ruby<int32_t>::convert(result));

  std::vector<std::complex<double>> complexes = detail::From_Ruby<std::vector<std::complex<double>>>::convert(vec);
  ASSERT_EQUAL(complexes[0], std::complex<double>(4, 4));
  ASSERT_EQUAL(complexes[1], std::complex<double>(5, 5));
}