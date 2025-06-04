#include <complex>
#include <memory>

#include "unittest.hpp"
#include "embed_ruby.hpp"
#include <rice/rice.hpp>
#include <rice/stl.hpp>
#include <ruby/version.h>

using namespace Rice;

TESTSUITE(Multimap);

SETUP(Multimap)
{
  embed_ruby();
}

TEARDOWN(Multimap)
{
  rb_gc_start();
}

namespace
{
  class MyClass
  {
  public:
    std::multimap<std::string, std::string> stringMultimap()
    {
      std::multimap<std::string, std::string> result{ {"One", "1"}, {"Two", "2"}, {"Three", "3"} };
      return result;
    }
  };
}

Class makeMultimapClass()
{
  Class c = define_class<MyClass>("MyClass").
    define_constructor(Constructor<MyClass>()).
    define_method("stringMultimap", &MyClass::stringMultimap);

  return c;
}

TESTCASE(StringMultimap)
{
  Module m = define_module("Testing");

  Class c = define_multimap<std::string, std::string>("StringMultimap");

  Object multimap = m.module_eval("$multimap = Std::StringMultimap.new");
  Object result = multimap.call("size");
  ASSERT_EQUAL(0, detail::From_Ruby<int32_t>().convert(result));

  m.module_eval("$multimap.insert('a_key', 'a_value')");
  result = multimap.call("size");
  ASSERT_EQUAL(1, detail::From_Ruby<int32_t>().convert(result));

  m.module_eval("$multimap.clear");
  result = multimap.call("size");
  ASSERT_EQUAL(0, detail::From_Ruby<int32_t>().convert(result));
}

TESTCASE(WrongType)
{
  Module m = define_module("Testing");

  Class c = define_multimap<std::string, std::string>("StringMultimap");
  Object multimap = m.module_eval("$multimap = Std::StringMultimap.new");

  ASSERT_EXCEPTION_CHECK(
    Exception,
    m.module_eval("$multimap.insert(1, 'abc')"),
    ASSERT_EQUAL("wrong argument type Integer (expected String)", ex.what()));

  ASSERT_EXCEPTION_CHECK(
    Exception,
    m.module_eval("$multimap.insert('abc', true)"),
    ASSERT_EQUAL("wrong argument type true (expected String)", ex.what()));
}

TESTCASE(Empty)
{
  Module m = define_module("Testing");

  Class c = define_multimap<std::string, std::int32_t>("IntMultimap");
  Object multimap = c.call("new");

  Object result = multimap.call("size");
  ASSERT_EQUAL(0, detail::From_Ruby<int32_t>().convert(result));

  result = multimap.call("empty?");
  ASSERT_EQUAL(Qtrue, result.value());
}

TESTCASE(Include)
{
  Module m = define_module("Testing");

  Class c = define_multimap<std::string, std::int32_t>("IntMultimap");
  Object multimap = c.call("new");
  multimap.call("insert", "one", 1);
  multimap.call("insert", "two", 2);

  Object result = multimap.call("include?", "two");
  ASSERT_EQUAL(Qtrue, result.value());

  result = multimap.call("include?", "three");
  ASSERT_EQUAL(Qfalse, result.value());

  Array array = multimap.call("[]", "three");
  ASSERT_EQUAL(0, array.size());
}

TESTCASE(Value)
{
  Module m = define_module("Testing");

  Class c = define_multimap<std::string, std::int32_t>("IntMultimap");
  Object multimap = c.call("new");
  multimap.call("insert", "one", 1);
  multimap.call("insert", "two", 2);

  Object result = multimap.call("value?", 2);
  ASSERT_EQUAL(Qtrue, result.value());

  result = multimap.call("value?", 4);
  ASSERT_EQUAL(Qfalse, result.value());
}

TESTCASE(ToString)
{
  Module m = define_module("Testing");

  Class c = define_multimap<std::string, std::int32_t>("IntMultimap");
  Object multimap = c.call("new");
  multimap.call("insert", "one", 1);
  multimap.call("insert", "two", 2);

  Object result = multimap.call("to_s");
  ASSERT_EQUAL("<Multimap≺string‚ int≻:{one => 1, two => 2}>", detail::From_Ruby<std::string>().convert(result));

  multimap.call("clear");

  result = multimap.call("to_s");
  ASSERT_EQUAL("<Multimap≺string‚ int≻:{}>", detail::From_Ruby<std::string>().convert(result));
}

TESTCASE(Update)
{
  Module m = define_module("Testing");

  Class c = define_multimap<std::string, std::string>("StringMultimap");
  Object multimap = c.call("new");
  multimap.call("insert", "one", "original 1");
  multimap.call("insert", "two", "original 2");

  Object result = multimap.call("size");
  ASSERT_EQUAL(2, detail::From_Ruby<int32_t>().convert(result));

  result = multimap.call("insert", "two", "new 2");
  ASSERT_EQUAL("new 2", detail::From_Ruby<std::string>().convert(result));

  result = multimap.call("size");
  ASSERT_EQUAL(3, detail::From_Ruby<int32_t>().convert(result));

  Array array = multimap.call("[]", "two");
  ASSERT_EQUAL(2, array.size());
  ASSERT_EQUAL("original 2", String(array[0]).c_str());
  ASSERT_EQUAL("new 2", String(array[1]).c_str());
}

TESTCASE(Modify)
{
  Module m = define_module("Testing");

  Class c = define_multimap<std::string, int64_t>("Int64Multimap");
  Object multimap = c.call("new");

  Object result = multimap.call("insert", "one", 3232323232);

  result = multimap.call("size");
  ASSERT_EQUAL(1, detail::From_Ruby<int32_t>().convert(result));

  result = multimap.call("delete", "one");
  ASSERT_EQUAL(3232323232, detail::From_Ruby<int64_t>().convert(result));

  result = multimap.call("size");
  ASSERT_EQUAL(0, detail::From_Ruby<int32_t>().convert(result));
}

TESTCASE(keysAndValues)
{
  Module m = define_module("Testing");

  Class c = define_multimap<std::string, int32_t>("Int32Multimap");
  Object multimap = c.call("new");

  multimap.call("insert", "one", 1);
  multimap.call("insert", "two", 2);
  multimap.call("insert", "three", 3);

  // Keys returns a std::vector
  Data_Object<std::vector<std::string>> keys = multimap.call("keys");
  std::vector<std::string> expected_keys{ {"one", "three", "two"} };
  ASSERT_EQUAL(3u, keys->size());
  ASSERT_EQUAL(expected_keys[0], keys->operator[](0));
  ASSERT_EQUAL(expected_keys[1], keys->operator[](1));
  ASSERT_EQUAL(expected_keys[2], keys->operator[](2));

  // Keys returns a std::vector
  Data_Object<std::vector<std::int32_t>> values = multimap.call("values");
  std::vector<std::int32_t> expected_values{ {1, 3, 2} };
  ASSERT_EQUAL(3u, values->size());
  ASSERT_EQUAL(expected_values[0], values->operator[](0));
  ASSERT_EQUAL(expected_values[1], values->operator[](1));
  ASSERT_EQUAL(expected_values[2], values->operator[](2));
}

TESTCASE(Copy)
{
  Module m = define_module("Testing");

  Class c = define_multimap<std::string, double>("DoubleMultimap");
  Object object = c.call("new");

  object.call("insert", "one", 11.1);
  object.call("insert", "two", 22.2);
  std::multimap<std::string, double>& multimap = detail::From_Ruby<std::multimap<std::string, double>&>().convert(object);

  Object result = object.call("copy");
  std::multimap<std::string, double>& multimapCopy = detail::From_Ruby<std::multimap<std::string, double>&>().convert(result);

  ASSERT_EQUAL(multimap, multimapCopy);
}

TESTCASE(Iterate)
{
  Module m = define_module("Testing");
  Class c = define_multimap<std::string, int>("IntMultimap");

  std::string code = R"(multimap = Std::IntMultimap.new
                        multimap.insert("five", 5)
                        multimap.insert("six", 6)
                        multimap.insert("seven", 7)

                        result = Hash.new
                        multimap.map do |pair|
                          result[pair.first] = 2 * pair.second
                        end
                        result)";

  Hash result = m.module_eval(code);
  ASSERT_EQUAL(3u, result.size());

  std::string result_string = result.to_s().str();
#if RUBY_API_VERSION_MAJOR == 3 && RUBY_API_VERSION_MINOR >= 4
  ASSERT_EQUAL("{\"five\" => 10, \"seven\" => 14, \"six\" => 12}", result_string);
#else
  ASSERT_EQUAL("{\"five\"=>10, \"seven\"=>14, \"six\"=>12}", result_string);
#endif
}

TESTCASE(ToEnum)
{
  Module m = define_module("Testing");
  Class c = define_multimap<std::string, int>("IntMultimap");

  std::string code = R"(multimap = Std::IntMultimap.new
                        multimap.insert("five", 5)
                        multimap.insert("six", 6)
                        multimap.insert("seven", 7)

                        result = Hash.new
                        multimap.each do |pair|
                          result[pair.first] = 2 * pair.second
                        end
                        result)";

  Hash result = m.module_eval(code);
  ASSERT_EQUAL(3u, result.size());

  std::string result_string = result.to_s().str();

#if RUBY_API_VERSION_MAJOR == 3 && RUBY_API_VERSION_MINOR >= 4
  ASSERT_EQUAL("{\"five\" => 10, \"seven\" => 14, \"six\" => 12}", result_string);
#else
  ASSERT_EQUAL("{\"five\"=>10, \"seven\"=>14, \"six\"=>12}", result_string);
#endif
}

TESTCASE(ToEnumSize)
{
  Module m = define_module("TestingModule");
  Class c = define_multimap<std::string, int>("IntMultimap");

  std::string code = R"(multimap = Std::IntMultimap.new
                        multimap.insert("five", 5)
                        multimap.insert("six", 6)
                        multimap.insert("seven", 7)
                        multimap.insert("eight", 7)
                        multimap.insert("eight", 8)
                        multimap)";

  Object multimap = m.module_eval(code);
  Object enumerable = multimap.call("each");
  Object result = enumerable.call("size");

  ASSERT_EQUAL(5, detail::From_Ruby<int>().convert(result));
}

namespace
{
  class NotComparable
  {
  public:
    NotComparable(uint32_t value) : value_(value)
    {
    };

    NotComparable() = default;

    uint32_t value_;
  };
}

TESTCASE(NotComparable)
{
  define_class<NotComparable>("NotComparable").
    define_constructor(Constructor<NotComparable, uint32_t>());

  Class c = define_multimap<std::string, NotComparable>("NotComparableMultimap");

  Object multimap = c.call("new");
  multimap.call("insert", "one", NotComparable(1));
  multimap.call("insert", "two", NotComparable(2));
  multimap.call("insert", "three", NotComparable(3));

  Object result = multimap.call("include?", "two");
  ASSERT_EQUAL(Qtrue, result.value());

  result = multimap.call("value?", NotComparable(3));
  ASSERT_EQUAL(Qfalse, result.value());
}

TESTCASE(NotPrintable)
{
  define_class<NotComparable>("NotComparable").
    define_constructor(Constructor<NotComparable, uint32_t>());

  Class c = define_multimap<std::string, NotComparable>("NotComparableMultimap");

  Object multimap = c.call("new");
  multimap.call("insert", "one", NotComparable(1));
  multimap.call("insert", "two", NotComparable(2));
  multimap.call("insert", "three", NotComparable(3));

  Object result = multimap.call("to_s");
  ASSERT_EQUAL("[Not printable]", detail::From_Ruby<std::string>().convert(result));
}

namespace
{
  class Comparable
  {
  public:
    Comparable() = default;
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

  Class c = define_multimap<std::string, Comparable>("ComparableMultimap");

  Object multimap = c.call("new");
  
  multimap.call("insert", "one", Comparable(1));
  multimap.call("insert", "two", Comparable(2));
  multimap.call("insert", "three", Comparable(3));

  Object result = multimap.call("value?", Comparable(2));
  ASSERT_EQUAL(Qtrue, result.value());
}

TESTCASE(Printable)
{
  define_class<Comparable>("IsComparable").
    define_constructor(Constructor<Comparable, uint32_t>());

  Class c = define_multimap<std::string, Comparable>("ComparableMultimap");

  Object multimap = c.call("new");
  multimap.call("insert", "one", Comparable(1));
  multimap.call("insert", "two", Comparable(2));
  multimap.call("insert", "three", Comparable(3));

  Object result = multimap.call("to_s");
  ASSERT_EQUAL("<Multimap≺string‚ AnonymousNamespace꞉꞉Comparable≻:{one => Comparable(1), three => Comparable(3), two => Comparable(2)}>", 
                detail::From_Ruby<std::string>().convert(result));
}

namespace
{
  std::multimap<std::string, std::complex<double>> returnComplexMultimap()
  {
    std::complex<double> complex1(1, 1);
    std::complex<double> complex2(2, 2);
    std::complex<double> complex3(3, 3);

    std::multimap<std::string, std::complex<double>> result;
    using Value_T = std::multimap<std::string, std::complex<double>>::value_type;
    result.insert(Value_T{ "one", complex1 });
    result.insert(Value_T{ "two", complex2 });
    result.insert(Value_T{ "three", complex3 });
    return result;
  }

  std::multimap<std::string, std::complex<double>> passComplexMultimap(std::multimap<std::string, std::complex<double>>& complexes)
  {
    return complexes;
  }
}

TESTCASE(AutoRegisterReturn)
{
  define_global_function("return_complex_multimap", &returnComplexMultimap);

  Module m = define_module("Testing");
  Object multimap = m.module_eval("return_complex_multimap");
  ASSERT_EQUAL("Std::Multimap≺string‚ complex≺double≻≻",
               multimap.class_name().str());

  std::string code = R"(multimap = return_complex_multimap
                        complex = multimap['three']
                        complex == [Complex(3, 3)])";

  Object result = m.module_eval(code);
  ASSERT_EQUAL(Qtrue, result.value());

  // Now register the multimap again
  define_multimap<std::string, std::complex<double>>("ComplexMultimap");
  code = R"(multimap = Std::ComplexMultimap.new)";
  result = m.module_eval(code);
  ASSERT(result.is_instance_of(multimap.class_of()));

  // And again in the module
  define_multimap<std::string, std::complex<double>>("ComplexMultimap2");
  code = R"(multimap = Std::ComplexMultimap2.new)";
  result = m.module_eval(code);
  ASSERT(result.is_instance_of(multimap.class_of()));
}

TESTCASE(AutoRegisterParameter)
{
  define_global_function("pass_complex_multimap", &passComplexMultimap);

  std::string code = R"(multimap = Std::Multimap≺string‚ complex≺double≻≻.new
                          multimap.insert("four", Complex(4.0, 4.0))
                          multimap.insert("five", Complex(5.0, 5.0))
                          pass_complex_multimap(multimap))";

  Module m = define_module("Testing");
  Object multimap = m.module_eval(code);

  Object result = multimap.call("size");
  ASSERT_EQUAL("Std::Multimap≺string‚ complex≺double≻≻",
               multimap.class_name().str());
  ASSERT_EQUAL(2, detail::From_Ruby<int32_t>().convert(result));

  std::multimap<std::string, std::complex<double>> expected{ {"four", std::complex<double>(4, 4)},
                                                             {"five", std::complex<double>(5, 5)} };

  std::multimap<std::string, std::complex<double>> complexes = detail::From_Ruby<std::multimap<std::string, std::complex<double>>>().convert(multimap);
  
  ASSERT_EQUAL(expected, complexes);
}

namespace
{
  std::multimap<std::string, std::string> defaultMultimap(std::multimap<std::string, std::string> strings = {{"one", "value 1"}, {"two", "value 2"}, {"three", "value 3"}})
  {
    return strings;
  }
}

TESTCASE(DefaultValue)
{
  define_multimap<std::string, std::string>("StringMultimap");
  define_global_function("default_multimap", &defaultMultimap, Arg("strings") = std::multimap<std::string, std::string>{ {"one", "value 1"}, {"two", "value 2"}, {"three", "value 3"} });

  Module m = define_module("Testing");
  Object result = m.module_eval("default_multimap");
  std::multimap<std::string, std::string> actual = detail::From_Ruby<std::multimap<std::string, std::string>>().convert(result);

  std::multimap<std::string, std::string> expected{ {"one", "value 1"}, {"two", "value 2"}, {"three", "value 3"} };

  ASSERT_EQUAL(expected, actual);
}

namespace
{
  std::multimap<std::string, int> ints;
  std::multimap<std::string, float> floats;
  std::multimap<std::string, std::string> strings;

  void hashToMultimap(std::multimap<std::string, int> aInts, std::multimap<std::string, float> aFloats, std::multimap<std::string, std::string> aStrings)
  {
    ints = aInts;
    floats = aFloats;
    strings = aStrings;
  }

  void hashToMultimapRefs(std::multimap<std::string, int>& aInts, std::multimap<std::string, float>& aFloats, std::multimap<std::string, std::string>& aStrings)
  {
    ints = aInts;
    floats = aFloats;
    strings = aStrings;
  }

  void hashToMultimapPointers(std::multimap<std::string, int>* aInts, std::multimap<std::string, float>* aFloats, std::multimap<std::string, std::string>* aStrings)
  {
    ints = *aInts;
    floats = *aFloats;
    strings = *aStrings;
  }
}

TESTCASE(HashToMultimap)
{
  define_global_function("hash_to_multimap", &hashToMultimap);

  Module m = define_module("Testing");

  std::string code = R"(hash_to_multimap({"seven" => 7, 
                                     "nine" => 9,
                                     "million" => 1_000_000},
                                    {"forty nine" => 49.0, 
                                     "seventy eight" => 78.0,
                                     "nine hundred ninety nine" => 999.0},
                                    {"one" => "one", 
                                     "two" => "two",
                                     "three" => "three"}))";

  m.module_eval(code);

  std::multimap<std::string, int> expectedInts{{"seven", 7},
                                                {"nine", 9},
                                                {"million", 1000000 } };
  ASSERT_EQUAL(expectedInts, ints);

  std::multimap<std::string, float> expectedFloats{ {"forty nine", 49.0f},
                                                    {"seventy eight", 78.0f},
                                                    {"nine hundred ninety nine", 999.0f} };
  ASSERT_EQUAL(expectedFloats, floats);

  std::multimap<std::string, std::string> expectedStrings{ {"one", "one"},
                                                           {"two", "two"},
                                                           {"three", "three"} };
  ASSERT_EQUAL(expectedStrings, strings);
}

TESTCASE(HashToMultimapRefs)
{
  define_global_function("hash_to_multimap_refs", &hashToMultimapRefs);

  Module m = define_module("Testing");

  std::string code = R"(hash_to_multimap_refs({"eight" => 8, 
                                               "ten" => 10,
                                               "million one" => 1_000_001},
                                              {"fifty" => 50.0, 
                                               "seventy nine" => 79.0,
                                               "one thousand" => 1_000.0},
                                              {"eleven" => "eleven", 
                                               "twelve" => "twelve",
                                               "thirteen" => "thirteen"}))";
  m.module_eval(code);

  std::multimap<std::string, int> expectedInts{ {"eight", 8},
                                                {"ten", 10},
                                                {"million one", 1000001 } };
  ASSERT_EQUAL(expectedInts, ints);

  std::multimap<std::string, float> expectedFloats{ {"fifty", 50.0f},
                                                    {"seventy nine", 79.0f},
                                                    {"one thousand", 1000.0f} };
  ASSERT_EQUAL(expectedFloats, floats);

  std::multimap<std::string, std::string> expectedStrings{ {"eleven", "eleven"},
                                                           {"twelve", "twelve"},
                                                           {"thirteen", "thirteen"} };
  ASSERT_EQUAL(expectedStrings, strings);
}

TESTCASE(HashToMultimapPointers)
{
  define_global_function("hash_to_multimap_pointers", &hashToMultimapPointers);

  Module m = define_module("Testing");

  std::string code = R"(hash_to_multimap_pointers({"nine" => 9, 
                                                   "eleven" => 11,
                                                   "million two" => 1_000_002},
                                                  {"fifty one" => 51.0, 
                                                   "eighty" => 80.0,
                                                   "one thousand one" => 1_001.0},
                                                  {"fourteen" => "fourteen", 
                                                   "fifteen" => "fifteen",
                                                   "sixteen" => "sixteen"}))";

  m.module_eval(code);

  std::multimap<std::string, int> expectedInts{ {"nine", 9},
                                                {"eleven", 11},
                                                {"million two", 1000002 } };
  ASSERT_EQUAL(expectedInts, ints);

  std::multimap<std::string, float> expectedFloats{ {"fifty one", 51.0f},
                                                    {"eighty", 80.0f},
                                                    {"one thousand one", 1001.0f} };
  ASSERT_EQUAL(expectedFloats, floats);

  std::multimap<std::string, std::string> expectedStrings{ {"fourteen", "fourteen"},
                                                           {"fifteen", "fifteen"},
                                                           {"sixteen", "sixteen"} };
  ASSERT_EQUAL(expectedStrings, strings);
}

TESTCASE(HashToMultimapWrongTypes)
{
  define_global_function("hash_to_multimap", &hashToMultimap);

  Module m = define_module("Testing");

  std::string code = R"(hash_to_multimap({"seven" => 7, 
                                          "nine" => 9,
                                          "million" => 1_000_000},
                                         {"forty nine" => 49.0, 
                                          "seventy eight" => 78.0,
                                          "nine hundred ninety nine" => 999.0},
                                         {"one" => 50.0, 
                                          "two" => 79.0,
                                          "three" => 1000.0}))";

  ASSERT_EXCEPTION_CHECK(
    Exception,
    m.module_eval(code),
    ASSERT_EQUAL("wrong argument type Float (expected String)", ex.what())
  );
}

TESTCASE(HashToMultimapMixedTypes)
{
  define_global_function("hash_to_multimap", &hashToMultimap);

  Module m = define_module("Testing");

  std::string code = R"(hash_to_multimap({"seven" => 7, 
                                          "nine" => "nine",
                                          "million" => true},
                                         {"forty nine" => 49.0, 
                                          "seventy eight" => 78.0,
                                          "nine hundred ninety nine" => 999.0},
                                         {"one" => "one", 
                                          "two" => "two",
                                          "three" => "three"}))";

  ASSERT_EXCEPTION_CHECK(
    Exception,
    m.module_eval(code),
    ASSERT_EQUAL("no implicit conversion of String into Integer", ex.what())
  );
}