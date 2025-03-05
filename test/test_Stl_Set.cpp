#include <complex>

#include "unittest.hpp"
#include "embed_ruby.hpp"
#include <rice/rice.hpp>
#include <rice/stl.hpp>

using namespace Rice;

TESTSUITE(Set);

SETUP(Set)
{
  embed_ruby();
}

TEARDOWN(Set)
{
  rb_gc_start();
}

namespace
{
  class MySetClass
  {
  public:
    std::set<std::string> stringSet()
    {
      std::set<std::string> result{ "One", "Two", "Three" };
      return result;
    }
  };
}

TESTCASE(Size)
{
  Module m = define_module("Testing");
  define_set<std::set<std::string>>("StringSet");

  std::string code = R"(s = Std::StringSet.new
                        s.size)";

  Object result = m.instance_eval(code);
  ASSERT_EQUAL(0, detail::From_Ruby<int32_t>().convert(result));

  code = R"(s = Std::StringSet.new
            s << "one" << "two"
            s.size)";
  
  result = m.instance_eval(code);
  ASSERT_EQUAL(2, detail::From_Ruby<int32_t>().convert(result));

  code = R"(s = Std::StringSet.new
            s << "one" << "two"
            s.clear
            s.size)";

  result = m.instance_eval(code);
  ASSERT_EQUAL(0, detail::From_Ruby<int32_t>().convert(result));
}

TESTCASE(Add)
{
  Module m = define_module("Testing");
  define_set<std::set<std::string>>("StringSet");

  std::string code = R"(set = Std::StringSet.new
                        set << "one" << "two" << "two" << "three"
                        set)";

  Object set = m.instance_eval(code);
  Object size = set.call("size");
  ASSERT_EQUAL(3, detail::From_Ruby<int32_t>().convert(size));

  String string = set.call("to_s");
  ASSERT_EQUAL("<Set≺string≺char≻≻:{one, three, two}>", string.c_str());

  set.call("insert", "four");

  size = set.call("size");
  ASSERT_EQUAL(4, detail::From_Ruby<int32_t>().convert(size));
}

TESTCASE(WrongType)
{
  Module m = define_module("Testing");
  Class c = define_set<std::set<std::string>>("StringSet");

  std::string code = R"(set = Std::StringSet.new
                        set << 1
                        set)";

  ASSERT_EXCEPTION_CHECK(
    Exception,
    m.module_eval(code),
    ASSERT_EQUAL("wrong argument type Integer (expected String)", ex.what()));
}

TESTCASE(Empty)
{
  Module m = define_module("Testing");

  Class c = define_set<std::set<std::int32_t>>("IntSet");

  std::string code = R"(set = Std::StringSet.new
                        set.size)";

  Object size = m.instance_eval(code);
  ASSERT_EQUAL(0, detail::From_Ruby<int32_t>().convert(size));

  code = R"(set = Std::StringSet.new
            set.empty?)";
  Object result = m.instance_eval(code);
  ASSERT_EQUAL(Qtrue, result.value());
}

TESTCASE(ToString)
{
  Module m = define_module("Testing");
  define_set<std::set<std::string>>("StringSet");

  std::string code = R"(set = Std::StringSet.new
                        set.insert("one")
                        set << "two" << "two" << "three"
                        set.to_s)";

  String string = m.instance_eval(code);
  ASSERT_EQUAL("<Set≺string≺char≻≻:{one, three, two}>", string.c_str());
}

TESTCASE(Include)
{
  Module m = define_module("Testing");
  define_set<std::set<std::string>>("StringSet");

  std::string code = R"(set = Std::StringSet.new
                        set << "one" << "two" << "two" << "three"
                        set.include?("two"))";

  Object result = m.instance_eval(code);
  ASSERT_EQUAL(Qtrue, result.value());
}

TESTCASE(Modify)
{
  Module m = define_module("Testing");

  Class c = define_set<std::set<int>>("Int64Set");
  Object set = c.call("new");

  Object result = set.call("insert", 11);
  ASSERT(result.is_equal(set));

  result = set.call("insert", 22);
  ASSERT(result.is_equal(set));

  result = set.call("size");
  ASSERT_EQUAL(2, detail::From_Ruby<int>().convert(result));

  result = set.call("insert", 33);
  ASSERT(result.is_equal(set));

  String string = set.call("to_s");
  ASSERT_EQUAL("<Set≺int≻:{11, 22, 33}>", string.c_str());

  set.call("delete", 11);
  string = set.call("to_s");
  ASSERT_EQUAL("<Set≺int≻:{22, 33}>", string.c_str());
}

TESTCASE(Clone)
{
  Module m = define_module("Testing");
  Class c = define_set<std::set<double>>("DoubleSet");

  std::string code = R"(set = Std::DoubleSet.new
                        set << 11.1 << 22.2)";

  Object set = m.instance_eval(code);
  Object copy = set.call("clone");

  Object size = set.call("size");
  Object copySize = copy.call("size");
  ASSERT_EQUAL(detail::From_Ruby<int32_t>().convert(size), detail::From_Ruby<int32_t>().convert(copySize));

  String string = set.call("to_s");
  String copyString = copy.call("to_s");
  ASSERT_EQUAL(string.c_str(), copyString.c_str());
}

namespace
{
  class NotPrintable
  {
  public:
    NotPrintable(uint32_t value) : value_(value)
    {
    };

    uint32_t value_;
  };

  bool operator< (const NotPrintable& lhs, const NotPrintable& rhs)
  {
    return lhs.value_ < rhs.value_;
  }
}

TESTCASE(NotPrintable)
{
  define_class<NotPrintable>("NotPrintable").
    define_constructor(Constructor<NotPrintable, uint32_t>());

  Class c = define_set<std::set<NotPrintable>>("NotPrintableSet");

  Object set = c.call("new");
  set.call("insert", NotPrintable(1));
  set.call("insert", NotPrintable(2));
  set.call("insert", NotPrintable(3));

  Object result = set.call("to_s");
  ASSERT_EQUAL("[Not printable]", detail::From_Ruby<std::string>().convert(result));
}

namespace
{
  std::set<float> returnFloatSet()
  {
    std::set<float> result;
    result.insert(1.1);
    result.insert(2.2);
    result.insert(3.3);
    return result;
  }

  std::set<float> passFloatSet(std::set<float>& set)
  {
    return set;
  }
}

TESTCASE(AutoRegisterReturn)
{
  define_global_function("return_float_set", &returnFloatSet);

  Module m = define_module("Testing");
  Object set = m.module_eval("return_float_set");
  ASSERT_EQUAL(u8"Std::Set≺float≻", set.class_name().str());

  std::string code = R"(set = return_float_set
                        set.size == 3)";

  Object result = m.module_eval(code);
  ASSERT_EQUAL(Qtrue, result.value());

  // Now register this same set
  define_set<std::set<float>>("FloatSet");
  code = R"(set = Std::FloatSet.new)";
  result = m.module_eval(code);
  ASSERT(result.is_instance_of(set.class_of()));

  // Now register it again in the module
  define_set<std::set<float>>("FloatSet2");
  code = R"(set = Std::FloatSet2.new)";
  result = m.module_eval(code);
  ASSERT(result.is_instance_of(set.class_of()));
}

TESTCASE(AutoRegisterParameter)
{
  define_global_function("pass_float_set", &passFloatSet);

  std::string code = u8R"(set = Std::Set≺float≻.new
                          set << 4.0
                          set << 5.0
                          pass_float_set(set))";

  Module m = define_module("Testing");
  Object set = m.module_eval(code);

  Object result = set.call("size");
  ASSERT_EQUAL(u8"Std::Set≺float≻", set.class_name().str());
  ASSERT_EQUAL(2, detail::From_Ruby<int32_t>().convert(result));
}

namespace
{
  std::set<std::string> defaultSet(std::set<std::string> strings = {"one", "two", "three"})
  {
    return strings;
  }
}

TESTCASE(DefaultValue)
{
  define_set<std::set<std::string>>("StringSet");
  define_global_function("default_set", &defaultSet, Arg("strings") = std::set<std::string> { "one", "two", "three" });

  Module m = define_module("Testing");
  Object result = m.module_eval("default_set");
  std::set<std::string> actual = detail::From_Ruby<std::set<std::string>>().convert(result);

  std::set<std::string> expected{ "one", "two", "three" };

  ASSERT_EQUAL(expected.size(), actual.size());
  ASSERT_EQUAL(expected, actual);
}

TESTCASE(Equal)
{
  Module m = define_module("Testing");
  Class c = define_set<std::set<int>>("IntSet");

  std::string code = R"(set1 = Std::IntSet.new
                        set1 << 4 << 5 << 6

                        set2 = Std::IntSet.new
                        set2 << 4 << 5 << 6

                        set1 == set2)";

  Object result = m.instance_eval(code);
  ASSERT_EQUAL(Qtrue, result.value());

  define_class<NotPrintable>("NotPrintable").
    define_constructor(Constructor<NotPrintable, uint32_t>());

  define_set<std::set<NotPrintable>>("NotPrintableSet");

  code = R"(set1 = Std::NotPrintableSet.new
            set2 = Std::NotPrintableSet.new
            set1 == set2)";

  result = m.instance_eval(code);
  ASSERT_EQUAL(Qfalse, result.value());
}

TESTCASE(ToArray)
{
  Module m = define_module("Testing");
  
  Class c = define_set<std::set<std::string>>("StringSet").
    define_constructor(Constructor<std::set<std::string>>());

  std::string code = R"(set = Std::StringSet.new
                        set << "abc"
                        set << "def"
                        set << "ghi"
                        set.to_a)";

  Array array = m.module_eval(code);
  ASSERT_EQUAL(3u, array.size());

  ASSERT_EQUAL("abc", detail::From_Ruby<std::string>().convert(array[0].value()));
  ASSERT_EQUAL("def", detail::From_Ruby<std::string>().convert(array[1].value()));
  ASSERT_EQUAL("ghi", detail::From_Ruby<std::string>().convert(array[2].value()));
}

namespace
{
  std::set<int> ints;
  std::set<float> floats;
  std::set<std::string> strings;

  void setToSet(std::set<int> aInts, std::set<float> aFloats, std::set<std::string> aStrings)
  {
    ints = aInts;
    floats = aFloats;
    strings = aStrings;
  }

  void setToSetRefs(std::set<int>& aInts, std::set<float>& aFloats, std::set<std::string>& aStrings)
  {
    ints = aInts;
    floats = aFloats;
    strings = aStrings;
  }

  void setToSetPointers(std::set<int>* aInts, std::set<float>* aFloats, std::set<std::string>* aStrings)
  {
    ints = *aInts;
    floats = *aFloats;
    strings = *aStrings;
  }
}

TESTCASE(ToSet)
{
  define_global_function("set_to_set", &setToSet);

  Module m = define_module("Testing");

  std::string code = R"(require 'set'
                        set_to_set(Set[7, 9, 1_000_000],
                                   Set[49.0, 78.0, 999.0],
                                   Set["one", "two", "three"]))";
  m.module_eval(code);

  std::set<int> expectedInts{ 7, 9, 1000000 };
  ASSERT_EQUAL(expectedInts, ints);

  std::set<float> expectedFloats{ 49.0, 78.0, 999.0 };
  ASSERT_EQUAL(expectedFloats, floats);

  std::set<std::string> expectedStrings{ "one", "two", "three" };
  ASSERT_EQUAL(expectedStrings, strings);
}

TESTCASE(ToSetRefs)
{
  define_global_function("set_to_set_refs", &setToSetRefs);

  Module m = define_module("Testing");

  std::string code = R"(require 'set'
                        set_to_set_refs(Set[8, 10, 1_000_001],
                                        Set[50.0, 79.0, 1_000.0],
                                        Set["eleven", "twelve", "thirteen"]))";
  m.module_eval(code);

  std::set<int> expectedInts{ 8, 10, 1000001 };
  ASSERT_EQUAL(expectedInts, ints);

  std::set<float> expectedFloats{ 50.0, 79.0, 1000.0 };
  ASSERT_EQUAL(expectedFloats, floats);

  std::set<std::string> expectedStrings{ "eleven", "twelve", "thirteen" };
  ASSERT_EQUAL(expectedStrings, strings);
}

TESTCASE(ToSetPointers)
{
  define_global_function("set_to_set_pointers", &setToSetPointers);

  Module m = define_module("Testing");

  std::string code = R"(require 'set'
                        set_to_set_pointers(Set[9, 11, 1_000_002],
                                            Set[51.0, 80.0, 1_001.0],
                                            Set["fourteen", "fifteen", "sixteen"]))";
  m.module_eval(code);

  std::set<int> expectedInts{ 9, 11, 1000002 };
  ASSERT_EQUAL(expectedInts, ints);

  std::set<float> expectedFloats{ 51.0, 80.0, 1001.0 };
  ASSERT_EQUAL(expectedFloats, floats);

  std::set<std::string> expectedStrings{ "fourteen", "fifteen", "sixteen" };
  ASSERT_EQUAL(expectedStrings, strings);
}

TESTCASE(ToSetWrongTypes)
{
  define_global_function("set_to_set", &setToSet);

  Module m = define_module("Testing");

  std::string code = R"(require 'set'
                        set_to_set(Set[7, 9, 1_000_000],
                                   Set[49.0, 78.0, 999.0],
                                   Set[50.0, 79.0, 1000.0]))";

  ASSERT_EXCEPTION_CHECK(
    Exception,
    m.module_eval(code),
    ASSERT_EQUAL("wrong argument type Float (expected String)", ex.what())
  );
}

TESTCASE(ToSetMixedTypes)
{
  define_global_function("set_to_set", &setToSet);

  Module m = define_module("Testing");

  std::string code = R"(set_to_set(Set[7, 'nine', true], Set[49.0, 78.0, 999.0], Set["one", "two", "three"]))";

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
    std::set<std::string>* returnPointer()
    {
      return &this->instance_;
    }

    std::set<std::string>& returnReference()
    {
      return this->instance_;
    }

    std::set<std::string> returnValue()
    {
      return this->instance_;
    }

  public:
    static inline std::set<std::string> instance_{ "one", "two", "three" };
  };

  std::ostream& operator<<(std::ostream& stream, const std::set<std::string>& set)
  {
    stream << "Set";
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

  std::set<std::string> expected{ "one", "two", "three" };

  Data_Object<std::set<std::string>> set1 = factory.call("pointer");
  ASSERT_EQUAL(expected, *set1);

  Data_Object<std::set<std::string>> set2 = factory.call("reference");
  ASSERT_EQUAL(expected, *set2);

  Data_Object<std::set<std::string>> set3 = factory.call("value");
  ASSERT_EQUAL(expected, *set3);
}

TESTCASE(Iterate)
{
  Module m = define_module("Testing");
  Class c = define_set<std::set<double>>("DoubleSet");

  std::string code = R"(set = Std::DoubleSet.new
                        set << 5.0 << 6.0 << 7.0
                        set.map do |value|
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
                        set = factory.pointer
                        updated = set.each.map do |value|
                                    value + "_updated"
                                  end)";

  Array result = m.module_eval(code);

  ASSERT_EQUAL(3, result.size());
  ASSERT_EQUAL("one_updated", detail::From_Ruby<std::string>().convert(result[0].value()));
  ASSERT_EQUAL("three_updated", detail::From_Ruby<std::string>().convert(result[1].value()));
  ASSERT_EQUAL("two_updated", detail::From_Ruby<std::string>().convert(result[2].value()));
}

TESTCASE(ToEnumReference)
{
  createFactoryClass();
  Module m = define_module("TestingModule");

  std::string code = R"(factory = Factory.new
                        set = factory.reference
                        updated = set.each.map do |value|
                                    value + "_updated"
                                  end)";

  Array result = m.module_eval(code);

  ASSERT_EQUAL(3, result.size());
  ASSERT_EQUAL("one_updated", detail::From_Ruby<std::string>().convert(result[0].value()));
  ASSERT_EQUAL("three_updated", detail::From_Ruby<std::string>().convert(result[1].value()));
  ASSERT_EQUAL("two_updated", detail::From_Ruby<std::string>().convert(result[2].value()));
}

TESTCASE(ToEnumValue)
{
  createFactoryClass();
  Module m = define_module("TestingModule");

  std::string code = R"(factory = Factory.new
                        set = factory.value
                        updated = set.each.map do |value|
                                    value + "_updated"
                                  end)";

  Array result = m.module_eval(code);

  ASSERT_EQUAL(3, result.size());
  ASSERT_EQUAL("one_updated", detail::From_Ruby<std::string>().convert(result[0].value()));
  ASSERT_EQUAL("three_updated", detail::From_Ruby<std::string>().convert(result[1].value()));
  ASSERT_EQUAL("two_updated", detail::From_Ruby<std::string>().convert(result[2].value()));
}

TESTCASE(ToEnumSize)
{
  createFactoryClass();
  Module m = define_module("TestingModule");
  Object factory = m.module_eval("Factory.new");
  Object set = factory.call("pointer");
  Object enumerable = set.call("each");
  Object result = enumerable.call("size");

  ASSERT_EQUAL(3, detail::From_Ruby<int>().convert(result));
}

namespace
{
  std::set<std::string*> setOfStringPointers()
  {
    std::set<std::string*> result;
    std::string* pString = new std::string("Hello");
    result.insert(pString);
    return result;
  }
}

TESTCASE(StringPointerSet)
{
  define_global_function("set_of_string_pointers", &setOfStringPointers);

  Module m(rb_mKernel);
  Data_Object<std::set<std::string*>> set = m.call("set_of_string_pointers");
  ASSERT_EQUAL(1, set->size());

  std::string expected = "Hello";
  const std::string* actual = *set->begin();
  ASSERT_EQUAL(expected, *actual);
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

  std::set<MyClass2*> setOfMyClass2Pointers()
  {
    std::set<MyClass2*> result;
    MyClass2* pMyClass = new MyClass2("Hello MyClass2");
    result.insert(pMyClass);
    return result;
  }
}

TESTCASE(MyClass2PointerSet)
{
  Class c = define_class<MyClass2>("MyClass2").
    define_constructor(Constructor<MyClass2, std::string>()).
    define_attr("name", &MyClass2::name, AttrAccess::Read);

  define_global_function("set_of_myclass2_pointers", &setOfMyClass2Pointers);

  Module m(rb_mKernel);
  Data_Object<std::set<MyClass2*>> result = m.call("set_of_myclass2_pointers");
  ASSERT_EQUAL(1, result->size());

 MyClass2* pMyClass = *result->begin();
 ASSERT_EQUAL("Hello MyClass2", pMyClass->name);
}

TESTCASE(Intersect)
{
  Module m = define_module("Testing");
  define_set<std::set<std::string>>("StringSet");

  std::string code = R"(set1 = Std::StringSet.new
                        set1 << "one" << "two" << "two" << "three"

                        set2 = Std::StringSet.new
                        set2 << "three" << "four"

                        set1 & set2)";

  Object set = m.instance_eval(code);
  Object size = set.call("size");
  ASSERT_EQUAL(1, detail::From_Ruby<int32_t>().convert(size));

  String string = set.call("to_s");
  ASSERT_EQUAL("<Set≺string≺char≻≻:{three}>", string.c_str());
}

TESTCASE(Union)
{
  Module m = define_module("Testing");
  define_set<std::set<std::string>>("StringSet");

  std::string code = R"(set1 = Std::StringSet.new
                        set1 << "one" << "two" << "two" << "three"

                        set2 = Std::StringSet.new
                        set2 << "three" << "four"

                        set1 | set2)";

  Object set = m.instance_eval(code);
  Object size = set.call("size");
  ASSERT_EQUAL(4, detail::From_Ruby<int32_t>().convert(size));

  String string = set.call("to_s");
  ASSERT_EQUAL("<Set≺string≺char≻≻:{four, one, three, two}>", string.c_str());
}

TESTCASE(Difference)
{
  Module m = define_module("Testing");
  define_set<std::set<std::string>>("StringSet");

  std::string code = R"(set1 = Std::StringSet.new
                        set1 << "one" << "two" << "two" << "three"

                        set2 = Std::StringSet.new
                        set2 << "three" << "four"

                        set1 - set2)";

  Object set = m.instance_eval(code);
  Object size = set.call("size");
  ASSERT_EQUAL(2, detail::From_Ruby<int32_t>().convert(size));

  String string = set.call("to_s");
  ASSERT_EQUAL("<Set≺string≺char≻≻:{one, two}>", string.c_str());
}

TESTCASE(Exclusive)
{
  Module m = define_module("Testing");
  define_set<std::set<std::string>>("StringSet");

  std::string code = R"(set1 = Std::StringSet.new
                        set1 << "one" << "two" << "three"

                        set2 = Std::StringSet.new
                        set2 << "three" << "four"

                        set1 ^ set2)";

  Object set = m.instance_eval(code);
  Object size = set.call("size");
  ASSERT_EQUAL(3, detail::From_Ruby<int32_t>().convert(size));

  String string = set.call("to_s");
  ASSERT_EQUAL("<Set≺string≺char≻≻:{four, one, two}>", string.c_str());
}

TESTCASE(Superset)
{
  Module m = define_module("Testing");
  define_set<std::set<std::string>>("StringSet");

  std::string code = R"(set1 = Std::StringSet.new
                        set1 << "one" << "two" << "three"

                        set2 = Std::StringSet.new
                        set2 << "three"

                        set1 > set2)";

  Object result = m.instance_eval(code);
  ASSERT_EQUAL(Qtrue, result.value());
}
