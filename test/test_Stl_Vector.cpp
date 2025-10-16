#include <complex>

#include "unittest.hpp"
#include "embed_ruby.hpp"
#include <rice/rice.hpp>
#include <rice/stl.hpp>

using namespace Rice;

TESTSUITE(Vector);

SETUP(Vector)
{
  embed_ruby();
}

TEARDOWN(Vector)
{
  rb_gc_start();
}

namespace
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

Class makeVectorClass()
{
  Class c = define_class<MyClass>("MyClass").
    define_constructor(Constructor<MyClass>()).
    define_method("stringVector", &MyClass::stringVector);

  return c;
}

TESTCASE(StringVector)
{
  Module m = define_module("Testing");

  Class c = define_vector<std::string>("StringVector");

  Object vec = m.module_eval("$vector = Std::StringVector.new");
  Object result = vec.call("size");
  ASSERT_EQUAL(0, detail::From_Ruby<int32_t>().convert(result));

  m.module_eval("$vector << 'one' << 'two' << 'two' << 'three'");
  result = vec.call("size");
  ASSERT_EQUAL(4, detail::From_Ruby<int32_t>().convert(result));

  m.module_eval("$vector.append('four')");
  result = vec.call("size");
  ASSERT_EQUAL(5, detail::From_Ruby<int32_t>().convert(result));

  result = vec.call("first");
  ASSERT_EQUAL("one", detail::From_Ruby<std::string>().convert(result));

  result = vec.call("last");
  ASSERT_EQUAL("four", detail::From_Ruby<std::string>().convert(result));
}

TESTCASE(Constructors)
{
  Module m = define_module("Testing");

  Class c = define_vector<int>("IntVector");

  // Default constructor
  std::string code = R"(Std::IntVector.new)";
  Object vec = m.module_eval(code);
  Object result = vec.call("size");
  ASSERT_EQUAL(0, detail::From_Ruby<int32_t>().convert(result));

  detail::From_Ruby<int> fromRuby;

  // Constructor with specific size and default construtible values
  code = R"(Std::IntVector.new(3))";
  vec = m.module_eval(code);
  result = vec.call("size");
  ASSERT_EQUAL(3, fromRuby.convert(result));
  Object element = vec.call("[]", 0);
  ASSERT_EQUAL(0, fromRuby.convert(element));
  element = vec.call("[]", 1);
  ASSERT_EQUAL(0, fromRuby.convert(element));
  element = vec.call("[]", 2);
  ASSERT_EQUAL(0, fromRuby.convert(element));

  // Constructor with specific size and value
  code = R"(Std::IntVector.new(3, 5))";
  vec = m.module_eval(code);
  result = vec.call("size");
  ASSERT_EQUAL(3, fromRuby.convert(result));
  element = vec.call("[]", 0);
  ASSERT_EQUAL(5, fromRuby.convert(element));
  element = vec.call("[]", 1);
  ASSERT_EQUAL(5, fromRuby.convert(element));
  element = vec.call("[]", 2);
  ASSERT_EQUAL(5, fromRuby.convert(element));

  // Custom constructor
  code = R"(Std::IntVector.new([1, 2, 3]))";
  vec = m.module_eval(code);
  result = vec.call("size");
  ASSERT_EQUAL(3, fromRuby.convert(result));
  element = vec.call("[]", 0);
  ASSERT_EQUAL(1, fromRuby.convert(element));
  element = vec.call("[]", 1);
  ASSERT_EQUAL(2, fromRuby.convert(element));
  element = vec.call("[]", 2);
  ASSERT_EQUAL(3, fromRuby.convert(element));
}

TESTCASE(WrongElementType)
{
  Module m = define_module("Testing");

  Class c = define_vector<std::string>("StringVector");

  Object vec = m.module_eval("$vector = Std::StringVector.new");
  ASSERT_EXCEPTION_CHECK(
    Exception,
    m.module_eval("$vector << 1"),
    ASSERT_EQUAL("wrong argument type Integer (expected String)", ex.what()));
}

TESTCASE(Empty)
{
  Module m = define_module("Testing");

  Class c = define_vector<std::int32_t>("IntVector");
  Object vec = c.call("new");

  Object result = vec.call("size");
  ASSERT_EQUAL(0, detail::From_Ruby<int32_t>().convert(result));

  result = vec.call("empty?");
  ASSERT_EQUAL(Qtrue, result.value());

  result = vec.call("first");
  ASSERT_EQUAL(Qnil, result.value());

  result = vec.call("last");
  ASSERT_EQUAL(Qnil, result.value());

  result = vec.call("[]", -1);
  ASSERT_EQUAL(Qnil, result.value());
}

TESTCASE(BoolVector)
{
  Class boolVecClass = define_vector<bool>("BoolVector");

  Object vec = boolVecClass.call("new");
  vec.call("resize", 3, true);

  Object result = vec.call("size");
  ASSERT_EQUAL(3, detail::From_Ruby<int32_t>().convert(result));
  result = vec.call("first");
  ASSERT_EQUAL(Qtrue, result.value());

  Object enumerable = vec.call("each");
  Object arr = enumerable.call("to_a");
  Object size = arr.call("size");
  ASSERT_EQUAL(3, detail::From_Ruby<int32_t>().convert(size));

  result = vec.call("[]", 0);
  ASSERT_EQUAL(Qtrue, result.value());

  result = vec.call("[]=", 1, false);
  result = vec.call("[]", 1);
  ASSERT_EQUAL(Qfalse, result.value());

  std::string code = R"(array = self.each.to_a
                        array == [true, false, true])";

  result = vec.instance_eval(code);
  ASSERT_EQUAL(Qtrue, result.value());

  result = vec.call("delete", 1);
  ASSERT_EQUAL(Qtrue, result.value());
  result = vec.call("size");
  ASSERT_EQUAL(2, detail::From_Ruby<int32_t>().convert(result));

  vec.call("clear");
  result = vec.call("size");
  ASSERT_EQUAL(0, detail::From_Ruby<int32_t>().convert(result));
}

TESTCASE(Indexing)
{
  Module m = define_module("Testing");

  Class c = define_vector<std::int32_t>("IntVector");
  Object vec = c.call("new");
  vec.call("push", 0);
  vec.call("push", 1);
  vec.call("push", 2);
  
  Object result = vec.call("size");
  ASSERT_EQUAL(3, detail::From_Ruby<int32_t>().convert(result));

  result = vec.call("[]", 0);
  ASSERT_EQUAL(0, detail::From_Ruby<int32_t>().convert(result));

  result = vec.call("[]", 1);
  ASSERT_EQUAL(1, detail::From_Ruby<int32_t>().convert(result));

  result = vec.call("[]", 2);
  ASSERT_EQUAL(2, detail::From_Ruby<int32_t>().convert(result));

  result = vec.call("[]", 3);
  ASSERT_EQUAL(Qnil, result.value());

  result = vec.call("[]", -1);
  ASSERT_EQUAL(2, detail::From_Ruby<int32_t>().convert(result));

  result = vec.call("[]", -2);
  ASSERT_EQUAL(1, detail::From_Ruby<int32_t>().convert(result));

  result = vec.call("[]", -3);
  ASSERT_EQUAL(0, detail::From_Ruby<int32_t>().convert(result));

  result = vec.call("[]", -4);
  ASSERT_EQUAL(2, detail::From_Ruby<int32_t>().convert(result));

  result = vec.call("[]", -7);
  ASSERT_EQUAL(2, detail::From_Ruby<int32_t>().convert(result));
}

TESTCASE(Slice)
{
  Module m = define_module("Testing");

  Class c = define_vector<std::int32_t>("IntVector");
  Object vec = c.call("new");
  vec.call("push", 7);
  vec.call("push", 8);
  vec.call("push", 9);
  vec.call("push", 10);
  vec.call("push", 11);
  vec.call("push", 12);

  Array result = vec.call("[]", 3, 1);
  std::vector<std::int32_t> slice = result.to_vector<int32_t>();
  ASSERT_EQUAL(1, slice.size());
  ASSERT_EQUAL(10, slice[0]);

  result = vec.call("[]", 3, 2);
  slice = result.to_vector<int32_t>();
  ASSERT_EQUAL(2, slice.size());
  ASSERT_EQUAL(10, slice[0]);
  ASSERT_EQUAL(11, slice[1]);

  result = vec.call("[]", 4, 10);
  slice = result.to_vector<int32_t>();
  ASSERT_EQUAL(2, slice.size());
  ASSERT_EQUAL(11, slice[0]);
  ASSERT_EQUAL(12, slice[1]);

  result = vec.call("[]", -1, 2);
  slice = result.to_vector<int32_t>();
  ASSERT_EQUAL(1, slice.size());
  ASSERT_EQUAL(12, slice[0]);
}

TESTCASE(Sizing)
{
  Module m = define_module("Testing");

  Class c = define_vector<std::int32_t>("IntVector");
  Object vec = c.call("new");
  vec.call("resize", 10);

  Object result = vec.call("size");
  ASSERT_EQUAL(10, detail::From_Ruby<int32_t>().convert(result));

  vec.call("clear");

  result = vec.call("size");
  ASSERT_EQUAL(0, detail::From_Ruby<int32_t>().convert(result));
}

TESTCASE(ToString)
{
  Module m = define_module("Testing");

  Class c = define_vector<std::int32_t>("IntVector");
  Object vec = c.call("new");
  vec.call("resize", 3);

  Object result = vec.call("to_s");
  ASSERT_EQUAL("[0, 0, 0]", detail::From_Ruby<std::string>().convert(result));

  vec.call("clear");

  result = vec.call("to_s");
  ASSERT_EQUAL("[]", detail::From_Ruby<std::string>().convert(result));
}

TESTCASE(Update)
{
  Module m = define_module("Testing");

  Class c = define_vector<std::string>("StringVector");
  Object vec = c.call("new");
  vec.call("push", "original 1");
  vec.call("push", "original 2");

  Object result = vec.call("size");
  ASSERT_EQUAL(2, detail::From_Ruby<int32_t>().convert(result));

  vec.call("[]=", 1, "new 2");

  result = vec.call("[]", 1);
  ASSERT_EQUAL("new 2", detail::From_Ruby<std::string>().convert(result));

  ASSERT_EXCEPTION_CHECK(
    Exception,
    result = vec.call("[]=", 99999, "new 2"),
    ASSERT_EQUAL("Invalid index: 99999", ex.what()));
}

TESTCASE(Modify)
{
  Module m = define_module("Testing");

  Class c = define_vector<int64_t>("Int64Vector");
  Object vec = c.call("new");

  Object result = vec.call("push", 11);
  ASSERT(result.is_equal(vec));

  result = vec.call("push", 22);
  ASSERT(result.is_equal(vec));

  result = vec.call("size");
  ASSERT_EQUAL(2, detail::From_Ruby<int32_t>().convert(result));

  result = vec.call("insert", 1, 33);
  ASSERT(result.is_equal(vec));

  result = vec.call("to_s");
  ASSERT_EQUAL("[11, 33, 22]", detail::From_Ruby<std::string>().convert(result));

  result = vec.call("delete", 11);
  ASSERT_EQUAL(11, detail::From_Ruby<int64_t>().convert(result));

  result = vec.call("size");
  ASSERT_EQUAL(2, detail::From_Ruby<int32_t>().convert(result));

  result = vec.call("delete_at", 0);
  ASSERT_EQUAL(33, detail::From_Ruby<int64_t>().convert(result));

  result = vec.call("size");
  ASSERT_EQUAL(1, detail::From_Ruby<int32_t>().convert(result));

  result = vec.call("pop");
  ASSERT_EQUAL(22, detail::From_Ruby<int64_t>().convert(result));

  result = vec.call("size");
  ASSERT_EQUAL(0, detail::From_Ruby<int32_t>().convert(result));

  result = vec.call("pop");
  ASSERT_EQUAL(Qnil, result.value());
}

TESTCASE(Clone)
{
  Module m = define_module("Testing");

  Class c = define_vector<double>("DoubleVector");
  Object object = c.call("new");

  object.call("push", 11.1);
  object.call("push", 22.2);
  std::vector<double>& vec = detail::From_Ruby<std::vector<double>&>().convert(object);

  Object result = object.call("clone");
  std::vector<double>& vecClone = detail::From_Ruby<std::vector<double>&>().convert(result);

  ASSERT_EQUAL(vec.size(), vecClone.size());
  ASSERT_EQUAL(vec[0], vecClone[0]);
  ASSERT_EQUAL(vec[1], vecClone[1]);

  vecClone.push_back(33.3);
  ASSERT_NOT_EQUAL(vec.size(), vecClone.size());
}


namespace
{
  class SomeClass
  {
  public:
    SomeClass(int value) : value(value)
    {
    }
    int value;
  };

  std::vector<std::unique_ptr<SomeClass>> uniqueVector()
  {
    std::vector<std::unique_ptr<SomeClass>> result;
    result.push_back(std::make_unique<SomeClass>(1));
    result.push_back(std::make_unique<SomeClass>(2));
    return result;
  }
}

TESTCASE(UniqueVector)
{
  Module m(anonymous_module());

  Class c = define_class<SomeClass>("SomeClass").
    define_constructor(Constructor<SomeClass, int>()).
    define_attr("value", &SomeClass::value);

  m.define_module_function("unique_vector", &uniqueVector);

  std::string code = R"(vector = unique_vector
                        vector.size)";

  Object result = m.module_eval(code);
  ASSERT_EQUAL(2, detail::From_Ruby<int32_t>().convert(result));
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

  Class c = define_vector<NotComparable>("NotComparableVector");

  Object vec = c.call("new");
  vec.call("push", NotComparable(1));
  vec.call("push", NotComparable(2));
  vec.call("push", NotComparable(3));

  Object result = vec.call("delete", NotComparable(1));
  ASSERT_EQUAL(Qnil, result.value());

  result = vec.call("length");
  ASSERT_EQUAL(3u, detail::From_Ruby<size_t>().convert(result));

  result = vec.call("include?", NotComparable(2));
  ASSERT_EQUAL(Qfalse, result.value());

  result = vec.call("index", NotComparable(3));
  ASSERT_EQUAL(Qnil, result.value());
}

TESTCASE(NotDefaultConstructable)
{
  define_class<NotComparable>("NotComparable").
    define_constructor(Constructor<NotComparable, uint32_t>());
    
  Class c = define_vector<NotComparable>("NotComparableVector");
  Object vec = c.call("new");

  Object result = vec.call("resize", 10);
  ASSERT_EQUAL(Qnil, result.value());

  result = vec.call("length");
  ASSERT_EQUAL(0, detail::From_Ruby<size_t>().convert(result));
}

TESTCASE(NotPrintable)
{
  define_class<NotComparable>("NotComparable").
    define_constructor(Constructor<NotComparable, uint32_t>());

  Class c = define_vector<NotComparable>("NotComparableVector");

  Object vec = c.call("new");
  vec.call("push", NotComparable(1));
  vec.call("push", NotComparable(2));
  vec.call("push", NotComparable(3));

  Object result = vec.call("to_s");
  ASSERT_EQUAL("[Not printable]", detail::From_Ruby<std::string>().convert(result));
}

namespace
{
  class Comparable
  {
  public:
    Comparable(uint32_t value) : value_(value)
    {
    };

    bool operator==(const Comparable& other) const
    {
      return this->value_ == other.value_;
    }

    uint32_t value_;
  };

  inline std::ostream& operator<<(std::ostream& stream, Comparable const& comparable)
  {
    stream << "Comparable(" << std::to_string(comparable.value_) << ")";
    return stream;
  }
}

TESTCASE(Comparable)
{
  define_class<Comparable>("IsComparable").
    define_constructor(Constructor<Comparable, uint32_t>());

  Class c = define_vector<Comparable>("ComparableVector");

  Object vec = c.call("new");
  
  Comparable comparable1(1);
  vec.call("push", comparable1);

  Comparable comparable2(2);
  vec.call("push", comparable2);

  Comparable comparable3(3);
  vec.call("push", comparable3);

  Object result = vec.call("delete", Comparable(1));
  Comparable comparable = detail::From_Ruby<Comparable>().convert(result);
  ASSERT_EQUAL(1, comparable.value_);

  result = vec.call("length");
  ASSERT_EQUAL(2, detail::From_Ruby<size_t>().convert(result));

  result = vec.call("include?", Comparable(2));
  ASSERT_EQUAL(Qtrue, result.value());

  result = vec.call("index", Comparable(3));
  ASSERT_EQUAL(1, detail::From_Ruby<size_t>().convert(result.value()));
}

namespace
{
  class ComparableButNotBool
  {
  public:
    ComparableButNotBool(uint32_t value) : value_(value)
    {
    };

    std::string operator==(const ComparableButNotBool& other)
    {
      return "not a boolean";
    }

    uint32_t value_;
  };
}

TESTCASE(ComparableButNotBool)
{
  define_class<ComparableButNotBool>("IsComparableButNotBool").
    define_constructor(Constructor<ComparableButNotBool, uint32_t>());

  Class c = define_vector<ComparableButNotBool>("ComparableButNotBoolVector");

  Object vec = c.call("new");
  vec.call("push", ComparableButNotBool(1));
  vec.call("push", ComparableButNotBool(2));
  vec.call("push", ComparableButNotBool(3));

  Object result = vec.call("delete", ComparableButNotBool(1));
  ASSERT_EQUAL(Qnil, result.value());

  result = vec.call("length");
  ASSERT_EQUAL(3u, detail::From_Ruby<size_t>().convert(result));

  result = vec.call("include?", ComparableButNotBool(2));
  ASSERT_EQUAL(Qfalse, result.value());

  result = vec.call("index", ComparableButNotBool(3));
  ASSERT_EQUAL(Qnil, result.value());
}

TESTCASE(DefaultConstructable)
{
  define_class<Comparable>("IsComparable").
    define_constructor(Constructor<Comparable, uint32_t>());

  Class c = define_vector<Comparable>("ComparableVector");
  Object vec = c.call("new");

  Object result = vec.call("resize", 10);
  ASSERT_EQUAL(Qnil, result.value());

  result = vec.call("length");
  ASSERT_EQUAL(0, detail::From_Ruby<size_t>().convert(result));
}

TESTCASE(Printable)
{
  define_class<Comparable>("IsComparable").
    define_constructor(Constructor<Comparable, uint32_t>());

  Class c = define_vector<Comparable>("ComparableVector");

  Object vec = c.call("new");
  vec.call("push", Comparable(1));
  vec.call("push", Comparable(2));
  vec.call("push", Comparable(3));

  Object result = vec.call("to_s");
  ASSERT_EQUAL("[Comparable(1), Comparable(2), Comparable(3)]", detail::From_Ruby<std::string>().convert(result));
}

namespace
{
  std::vector<std::complex<double>> returnComplexVector()
  {
    std::complex<double> complex1(1, 1);
    std::complex<double> complex2(2, 2);
    std::complex<double> complex3(3, 3);

    std::vector<std::complex<double>> result;
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

TESTCASE(AutoRegisterReturn)
{
  define_global_function("return_complex_vector", &returnComplexVector);

  Module m = define_module("Testing");
  Object vec = m.module_eval("return_complex_vector");
  ASSERT_EQUAL("Std::Vector≺complex≺double≻≻", vec.class_name().str());

  std::string code = R"(vector = return_complex_vector
                        complex = vector.last
                        complex == Complex(3, 3))";

  Object result = m.module_eval(code);
  ASSERT_EQUAL(Qtrue, result.value());

  // Now register this same vector
  define_vector<std::complex<double>>("ComplexVector");
  code = R"(vector = Std::ComplexVector.new)";
  result = m.module_eval(code);
  ASSERT(result.is_instance_of(vec.class_of()));
}

TESTCASE(AutoRegisterParameter)
{
  define_global_function("pass_complex_vector", &passComplexVector);

  std::string code = R"(vector = Std::Vector≺complex≺double≻≻.new
                        vector << Complex(4.0, 4.0)
                        vector << Complex(5.0, 5.0)
                        pass_complex_vector(vector))";

  Module m = define_module("Testing");
  Object vec = m.module_eval(code);

  Object result = vec.call("size");
  ASSERT_EQUAL("Std::Vector≺complex≺double≻≻", vec.class_name().str());
  ASSERT_EQUAL(2, detail::From_Ruby<int32_t>().convert(result));

  std::vector<std::complex<double>> complexes = detail::From_Ruby<std::vector<std::complex<double>>>().convert(vec);
  ASSERT_EQUAL(complexes[0], std::complex<double>(4, 4));
  ASSERT_EQUAL(complexes[1], std::complex<double>(5, 5));
}

TESTCASE(AutoRegisterException)
{
  std::string code = R"(
    begin
      Std::Vector≺complex≺double≻≻.new('!!!!')
    rescue => e
      name = e.class.name
      "#{e.class.name} #{e.message} #{(e.backtrace || []).join("\n")}"  # shoud not raise 'incompatible character encodings: BINARY (ASCII-8BIT) and UTF-8' ?
    end
    "reachable! - #{name}"
  )";

  Module m = define_module("Testing");
  Object result = m.module_eval(code);
  std::string actual = detail::From_Ruby<std::string>().convert(result);
  ASSERT_EQUAL("reachable! - ArgumentError", actual);
}

namespace
{
  std::vector<std::string> defaultVector(std::vector<std::string> strings = {"one", "two", "three"})
  {
    return strings;
  }
}

TESTCASE(DefaultValue)
{
  define_vector<std::string>("StringVector");
  define_global_function("default_vector", &defaultVector, Arg("strings") = std::vector<std::string> { "one", "two", "three" });

  Module m = define_module("Testing");
  Object result = m.module_eval("default_vector");
  std::vector<std::string> actual = detail::From_Ruby<std::vector<std::string>>().convert(result);

  std::vector<std::string> expected{ "one", "two", "three" };

  ASSERT_EQUAL(expected.size(), actual.size());
  ASSERT_EQUAL(expected[0], actual[0]);
  ASSERT_EQUAL(expected[1], actual[1]);
  ASSERT_EQUAL(expected[2], actual[2]);
}

TESTCASE(ToArray)
{
  Module m = define_module("Testing");
  
  Class c = define_vector<std::string>("StringVector").
    define_constructor(Constructor<std::vector<std::string>>());

  std::string code = R"(vector = Std::StringVector.new
                        vector << "abc"
                        vector << "def"
                        vector << "ghi"
                        vector.to_a)";

  Array array = m.module_eval(code);
  ASSERT_EQUAL(3u, array.size());

  ASSERT_EQUAL("abc", detail::From_Ruby<std::string>().convert(array[0].value()));
  ASSERT_EQUAL("def", detail::From_Ruby<std::string>().convert(array[1].value()));
  ASSERT_EQUAL("ghi", detail::From_Ruby<std::string>().convert(array[2].value()));
}

namespace
{
  std::vector<int> ints;
  std::vector<float> floats;
  std::vector<std::string> strings;

  void arrayToVector(std::vector<int> aInts, std::vector<float> aFloats, std::vector<std::string> aStrings)
  {
    ints = aInts;
    floats = aFloats;
    strings = aStrings;
  }

  void arrayToVectorRefs(std::vector<int>& aInts, std::vector<float>& aFloats, std::vector<std::string>& aStrings)
  {
    ints = aInts;
    floats = aFloats;
    strings = aStrings;
  }

  void arrayToVectorPointers(std::vector<int>* aInts, std::vector<float>* aFloats, std::vector<std::string>* aStrings)
  {
    ints = *aInts;
    floats = *aFloats;
    strings = *aStrings;
  }
}

TESTCASE(ArrayToVector)
{
  define_global_function("array_to_vector", &arrayToVector);

  Module m = define_module("Testing");

  std::string code = "array_to_vector([7, 9, 1_000_000], [49.0, 78.0, 999.0], %w[one two three])";
  m.module_eval(code);

  ASSERT_EQUAL(3, ints.size());
  ASSERT_EQUAL(7, ints[0]);
  ASSERT_EQUAL(9, ints[1]);
  ASSERT_EQUAL(1'000'000, ints[2]);

  ASSERT_EQUAL(3, floats.size());
  ASSERT_EQUAL(49.0, floats[0]);
  ASSERT_EQUAL(78.0, floats[1]);
  ASSERT_EQUAL(999.0, floats[2]);

  ASSERT_EQUAL(3, strings.size());
  ASSERT_EQUAL("one", strings[0]);
  ASSERT_EQUAL("two", strings[1]);
  ASSERT_EQUAL("three", strings[2]);
}

TESTCASE(ArrayToVectorRefs)
{
  define_global_function("array_to_vector_refs", &arrayToVectorRefs);

  Module m = define_module("Testing");

  std::string code = "array_to_vector_refs([8, 10, 1_000_001], [50.0, 79.0, 1_000.0], %w[eleven twelve thirteen])";
  m.module_eval(code);

  ASSERT_EQUAL(3, ints.size());
  ASSERT_EQUAL(8, ints[0]);
  ASSERT_EQUAL(10, ints[1]);
  ASSERT_EQUAL(1'000'001, ints[2]);

  ASSERT_EQUAL(3, floats.size());
  ASSERT_EQUAL(50.0, floats[0]);
  ASSERT_EQUAL(79.0, floats[1]);
  ASSERT_EQUAL(1'000.0, floats[2]);

  ASSERT_EQUAL(3, strings.size());
  ASSERT_EQUAL("eleven", strings[0]);
  ASSERT_EQUAL("twelve", strings[1]);
  ASSERT_EQUAL("thirteen", strings[2]);
}

TESTCASE(ArrayToVectorPointers)
{
  define_global_function("array_to_vector_pointers", &arrayToVectorPointers);

  Module m = define_module("Testing");

  std::string code = "array_to_vector_pointers([9, 11, 1_000_002], [51.0, 80.0, 1_001.0], %w[fourteen fifteen sixteen])";
  m.module_eval(code);

  ASSERT_EQUAL(3, ints.size());
  ASSERT_EQUAL(9, ints[0]);
  ASSERT_EQUAL(11, ints[1]);
  ASSERT_EQUAL(1'000'002, ints[2]);

  ASSERT_EQUAL(3, floats.size());
  ASSERT_EQUAL(51.0, floats[0]);
  ASSERT_EQUAL(80.0, floats[1]);
  ASSERT_EQUAL(1'001.0, floats[2]);

  ASSERT_EQUAL(3, strings.size());
  ASSERT_EQUAL("fourteen", strings[0]);
  ASSERT_EQUAL("fifteen", strings[1]);
  ASSERT_EQUAL("sixteen", strings[2]);
}

TESTCASE(ArrayToVectorWrongTypes)
{
  define_global_function("array_to_vector", &arrayToVector);

  Module m = define_module("Testing");

  std::string code = "array_to_vector([7, 9, 1_000_000], [49.0, 78.0, 999.0], [50.0, 79.0, 1000.0])";

  ASSERT_EXCEPTION_CHECK(
    Exception,
    m.module_eval(code),
    ASSERT_EQUAL("wrong argument type Float (expected String)", ex.what())
  );
}

TESTCASE(ArrayToVectorMixedTypes)
{
  define_global_function("array_to_vector", &arrayToVector);

  Module m = define_module("Testing");

  std::string code = "array_to_vector([7, 'nine', true], [49.0, 78.0, 999.0], %w[one two three])";

  ASSERT_EXCEPTION_CHECK(
    Exception,
    m.module_eval(code),
    ASSERT_EQUAL("no implicit conversion of String into Integer", ex.what())
  );
}

namespace
{
  class Factory
  {
  public:
    std::vector<std::string>* returnPointer()
    {
      return &this->instance_;
    }

    std::vector<std::string>& returnReference()
    {
      return this->instance_;
    }

    std::vector<std::string> returnValue()
    {
      return this->instance_;
    }

  public:
    static inline std::vector<std::string> instance_{ "one", "two", "three" };
  };

  std::ostream& operator<<(std::ostream& stream, const std::vector<std::string>& vector)
  {
    stream << "Vector";
    return stream;
  }

  void createFactoryClass()
  {
    define_class<Factory>("Factory").
      define_constructor(Constructor<Factory>()).
      define_method("pointer", &Factory::returnPointer).
      define_method("reference", &Factory::returnReference).
      define_method("value", &Factory::returnValue);
  }
}

TESTCASE(Returns)
{
  createFactoryClass();
  Module m = define_module("TestingModule");
  Object factory = m.module_eval("Factory.new");

  std::vector<std::string> expected{ "one", "two", "three" };

  Data_Object<std::vector<std::string>> vec1 = factory.call("pointer");
  ASSERT_EQUAL(expected, *vec1);

  Data_Object<std::vector<std::string>> vec2 = factory.call("reference");
  ASSERT_EQUAL(expected, *vec2);

  Data_Object<std::vector<std::string>> vec3 = factory.call("value");
  ASSERT_EQUAL(expected, *vec3);
}

TESTCASE(Iterate)
{
  Module m = define_module("Testing");
  Class c = define_vector<double>("DoubleVector");

  std::string code = R"(vector = Std::DoubleVector.new
                        vector << 5.0 << 6.0 << 7.0
                        updated = vector.map do |value|
                                    value * 2.0
                                  end)";

  Array result = m.module_eval(code);
  ASSERT_EQUAL(3, result.size());
  ASSERT_EQUAL(10.0, detail::From_Ruby<double>().convert(result[0].value()));
  ASSERT_EQUAL(12.0, detail::From_Ruby<double>().convert(result[1].value()));
  ASSERT_EQUAL(14.0, detail::From_Ruby<double>().convert(result[2].value()));
}

TESTCASE(ToEnumPointer)
{
  createFactoryClass();
  Module m = define_module("TestingModule");

  std::string code = R"(factory = Factory.new
                        vector = factory.pointer
                        updated = vector.each.map do |value|
                                    value + "_updated"
                                  end)";

  Array result = m.module_eval(code);

  ASSERT_EQUAL(3, result.size());
  ASSERT_EQUAL("one_updated", detail::From_Ruby<std::string>().convert(result[0].value()));
  ASSERT_EQUAL("two_updated", detail::From_Ruby<std::string>().convert(result[1].value()));
  ASSERT_EQUAL("three_updated", detail::From_Ruby<std::string>().convert(result[2].value()));
}

TESTCASE(ToEnumReference)
{
  createFactoryClass();
  Module m = define_module("TestingModule");

  std::string code = R"(factory = Factory.new
                        vector = factory.reference
                        updated = vector.each.map do |value|
                                    value + "_updated"
                                  end)";

  Array result = m.module_eval(code);

  ASSERT_EQUAL(3, result.size());
  ASSERT_EQUAL("one_updated", detail::From_Ruby<std::string>().convert(result[0].value()));
  ASSERT_EQUAL("two_updated", detail::From_Ruby<std::string>().convert(result[1].value()));
  ASSERT_EQUAL("three_updated", detail::From_Ruby<std::string>().convert(result[2].value()));
}

TESTCASE(ToEnumValue)
{
  createFactoryClass();
  Module m = define_module("TestingModule");

  std::string code = R"(factory = Factory.new
                        vector = factory.value
                        updated = vector.each.map do |value|
                                    value + "_updated"
                                  end)";

  Array result = m.module_eval(code);

  ASSERT_EQUAL(3, result.size());
  ASSERT_EQUAL("one_updated", detail::From_Ruby<std::string>().convert(result[0].value()));
  ASSERT_EQUAL("two_updated", detail::From_Ruby<std::string>().convert(result[1].value()));
  ASSERT_EQUAL("three_updated", detail::From_Ruby<std::string>().convert(result[2].value()));
}

TESTCASE(ToEnumSize)
{
  createFactoryClass();
  Module m = define_module("TestingModule");
  Object factory = m.module_eval("Factory.new");
  Object vector = factory.call("pointer");
  Object enumerable = vector.call("each");
  Object result = enumerable.call("size");

  ASSERT_EQUAL(3, detail::From_Ruby<int>().convert(result));
}

namespace
{
  std::vector<std::string*> vectorOfStringPointers()
  {
    std::vector<std::string*> result;
    std::string* pString1 = new std::string("Hello");
    result.push_back(pString1);

    std::string* pString2 = new std::string("World");
    result.push_back(pString2);

    return result;
  }
}

TESTCASE(StringPointerVector)
{
  define_buffer<std::string>();
  define_global_function("vector_of_string_pointers", &vectorOfStringPointers);

  Module m(rb_mKernel);
  Data_Object<std::vector<std::string*>> vec = m.call("vector_of_string_pointers");
  ASSERT_EQUAL(2, vec->size());

  std::string expected("Hello");
  std::string* actual = (*vec)[0];
  ASSERT_EQUAL(expected, *actual);

  std::string code = R"(vec = vector_of_string_pointers
                        outer_buffer = vec.data.buffer
                        inner_buffer = Rice::Buffer≺string≻.new(outer_buffer[1])
                        inner_buffer.to_ary(1))";
  Array array = m.module_eval(code);
  ASSERT_EQUAL(1, array.size());
  ASSERT_EQUAL("World", detail::From_Ruby<std::string>().convert(array[0].value()).c_str());
}

namespace
{
  class MyClass2
  {
  public:
    MyClass2(std::string name): name(name)
    {
    }
    std::string name;
  };

  std::vector<MyClass2*> vectorOfMyClass2Pointers()
  {
    std::vector<MyClass2*> result;
    MyClass2* pMyClass = new MyClass2("Hello MyClass2");
    result.push_back(pMyClass);
    return result;
  }
}

TESTCASE(MyClass2PointerVector)
{
  Class c = define_class<MyClass2>("MyClass2").
    define_constructor(Constructor<MyClass2, std::string>()).
    define_attr("name", &MyClass2::name, AttrAccess::Read);

  define_global_function("vector_of_myclass2_pointers", &vectorOfMyClass2Pointers);

  Module m(rb_mKernel);
  Data_Object<std::vector<MyClass2*>> result = m.call("vector_of_myclass2_pointers");
  ASSERT_EQUAL(1, result->size());

 MyClass2* pMyClass = (*result)[0];
 ASSERT_EQUAL("Hello MyClass2", pMyClass->name);
}

namespace
{
  bool typeCheckValue(std::vector<std::string> vec)
  {
    return true;
  }

  bool typeCheckRef(std::vector<std::string>& vec)
  {
    return true;
  }

  bool typeCheckPtr(std::vector<std::string>* vec)
  {
    return true;
  }
}

TESTCASE(TypeCheck)
{
  define_vector<int>("Vector≺int≻");
  define_vector<std::string>("Vector≺string≻");

  Module m(anonymous_module());
  m.define_module_function("check_value", &typeCheckValue).
    define_module_function("check_ref", &typeCheckRef).
    define_module_function("check_ptr", &typeCheckPtr);

  std::string code = R"(vec = Std::Vector≺string≻.new
                        check_value(vec))";
  Object result = m.module_eval(code);
  ASSERT_EQUAL(Qtrue, result.value());

  code = R"(vec = Std::Vector≺string≻.new
            check_ref(vec))";
  result = m.module_eval(code);
  ASSERT_EQUAL(Qtrue, result.value());

  code = R"(vec = Std::Vector≺string≻.new
            check_ptr(vec))";
  result = m.module_eval(code);
  ASSERT_EQUAL(Qtrue, result.value());

  code = R"(vec = Std::Vector≺int≻.new
            check_value(vec))";

  ASSERT_EXCEPTION_CHECK(
    Exception,
    result = m.module_eval(code),
    ASSERT_EQUAL("wrong argument type Std::Vector≺int≻ (expected Std::Vector≺string≻)", ex.what()));

  code = R"(vec = Std::Vector≺int≻.new
            check_ref(vec))";

  ASSERT_EXCEPTION_CHECK(
    Exception,
    result = m.module_eval(code),
    ASSERT_EQUAL("wrong argument type Std::Vector≺int≻ (expected Std::Vector≺string≻)", ex.what()));

  code = R"(vec = Std::Vector≺int≻.new
            check_ptr(vec))";

  ASSERT_EXCEPTION_CHECK(
    Exception,
    result = m.module_eval(code),
    ASSERT_EQUAL("wrong argument type Std::Vector≺int≻ (expected Std::Vector≺string≻)", ex.what()));
}
