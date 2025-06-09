#include "unittest.hpp"
#include "embed_ruby.hpp"
#include <rice/rice.hpp>
#include <rice/stl.hpp>

#include <variant>
#include <complex>

using namespace Rice;

TESTSUITE(Variant);

namespace
{
  using Intrinsic_Variant_T = std::variant<
    std::string,
    std::complex<double>,
    std::vector<int>,
    double,
    bool,
    int
  >;

  inline std::ostream& operator<<(std::ostream& stream, Intrinsic_Variant_T const& variant)
  {
    stream << "Variant(" << "index: " << variant.index() << ")";
    return stream;
  }

  class MyClass
  {
  public:
    Intrinsic_Variant_T variantString()
    {
      // Need to tell compiler this is std::string and not a const char[8]. Because that
      // becomes const char* which sets the boolean field to true. Oops. https://stackoverflow.com/a/44086312
      Intrinsic_Variant_T result { std::string("a string") };
      return result;
    }

    Intrinsic_Variant_T variantComplex()
    {
      using namespace std::complex_literals;
      Intrinsic_Variant_T result { 1i };
      return result;
    }

    Intrinsic_Variant_T variantVector()
    {
      Intrinsic_Variant_T result { std::vector<int>{1, 2, 3} };
      return result;
    }

    Intrinsic_Variant_T variantDouble()
    {
      Intrinsic_Variant_T result { 3.3 };
      return result;
    }

    Intrinsic_Variant_T variantBoolTrue()
    {
      Intrinsic_Variant_T result { true };
      return result;
    }

    Intrinsic_Variant_T variantBoolFalse()
    {
      Intrinsic_Variant_T result{ false };
      return result;
    }

    Intrinsic_Variant_T variantInt()
    {
      Intrinsic_Variant_T result { 5 };
      return result;
    }

    Intrinsic_Variant_T variantRoundtrip(Intrinsic_Variant_T variant)
    {
      return variant;
    }

    Intrinsic_Variant_T variantRoundtripReference(Intrinsic_Variant_T variant)
    {
      return variant;
    }

    Intrinsic_Variant_T variant_ = std::string("Initial value");
  };
}

void makeIntrinsicVariant()
{
  define_class<MyClass>("MyClass").
    define_constructor(Constructor<MyClass>()).
    define_method("variant_string", &MyClass::variantString).
    define_method("variant_complex", &MyClass::variantComplex).
    define_method("variant_vector", &MyClass::variantVector).
    define_method("variant_double", &MyClass::variantDouble).
    define_method("variant_bool_true", &MyClass::variantBoolTrue).
    define_method("variant_bool_false", &MyClass::variantBoolFalse).
    define_method("variant_int", &MyClass::variantInt).
    define_method("roundtrip", &MyClass::variantRoundtrip).
    define_attr("variant_attr", &MyClass::variant_);
}

namespace
{
  class MyClass1
  {
  public:
    MyClass1()
    {
      int a = 1;
    }

    std::string sayHello()
    {
      return "Hi from MyClass1";
    }
  };

  class MyClass2
  {
  public:
    MyClass2()
    {
      int a = 2;
    }

    std::string sayHello()
    {
      return "Hi from MyClass2";
    }
  };

  using Variant_T = std::variant<std::monostate, MyClass1, MyClass2>;

  Variant_T variant(bool myClass1)
  {
    if (myClass1)
    {
      return MyClass1();
    }
    else
    {
      return MyClass2();
    }
  }

  Variant_T roundTripVariant(Variant_T variant)
  {
    return variant;
  }

  Variant_T& roundTripVariantRef(Variant_T& variant)
  {
    return variant;
  }

  const Variant_T& roundTripConstVariantRef(const Variant_T& variant)
  {
    return variant;
  }
}

void makeClassVariant()
{
  define_class<MyClass1>("MyClass1").
    define_constructor(Constructor<MyClass1>()).
    define_method("say_hello", &MyClass1::sayHello);

  define_class<MyClass2>("MyClass2").
    define_constructor(Constructor<MyClass2>()).
    define_method("say_hello", &MyClass2::sayHello);

  define_global_function("variant_class", &variant);
  define_global_function("roundtrip_variant", &roundTripVariant);
  define_global_function("roundtrip_variant_ref", &roundTripVariantRef);
  define_global_function("roundtrip_const_variant_ref", &roundTripConstVariantRef);
}

SETUP(Variant)
{
  embed_ruby();
  makeIntrinsicVariant();
  makeClassVariant();
}

TEARDOWN(Variant)
{
  rb_gc_start();
}

TESTCASE(IntrinsicReturns)
{
  using namespace std::complex_literals;

  Module m = define_module("Testing");
  Object myClass = m.module_eval("MyClass.new");

  Object result = myClass.call("variant_string");
  ASSERT_EQUAL("a string", detail::From_Ruby<std::string>().convert(result));

  result = myClass.call("variant_complex");
  ASSERT_EQUAL(1i, detail::From_Ruby<std::complex<double>>().convert(result));

  result = myClass.call("variant_vector");
  std::vector<int> converted = detail::From_Ruby<std::vector<int>>().convert(result);
  ASSERT_EQUAL(3, converted.size());

  result = myClass.call("variant_double");
  ASSERT_EQUAL(3.3, detail::From_Ruby<double>().convert(result));

  result = myClass.call("variant_bool_true");
  ASSERT(detail::From_Ruby<bool>().convert(result));

  result = myClass.call("variant_bool_false");
  ASSERT(!detail::From_Ruby<bool>().convert(result));

  result = myClass.call("variant_int");
  ASSERT_EQUAL(5, detail::From_Ruby<int>().convert(result));
}

TESTCASE(IntrinsicRoundtrip)
{
  using namespace std::complex_literals;

  Module m = define_module("Testing");
  Object myClass = m.module_eval("MyClass.new");

  std::string code = R"(my_class = MyClass.new
                        my_class.roundtrip("roundtrip string"))";
  Object result = m.module_eval(code);
  ASSERT_EQUAL("roundtrip string", detail::From_Ruby<std::string>().convert(result));

  code = R"(my_class = MyClass.new
                        my_class.roundtrip(Complex(2, 3)))";
  result = m.module_eval(code);
  ASSERT_EQUAL((2.0 + 3i), detail::From_Ruby<std::complex<double>>().convert(result));

  code = R"(my_class = MyClass.new
                        my_class.roundtrip([1, 2, 3]))";
  result = m.module_eval(code);
  std::vector<int> expected = {1, 2, 3};
  ASSERT_EQUAL(expected, detail::From_Ruby<std::vector<int>>().convert(result));

  code = R"(my_class = MyClass.new
            my_class.roundtrip(44.4))";
  result = m.module_eval(code);
  ASSERT_EQUAL(44.4, detail::From_Ruby<double>().convert(result));

  code = R"(my_class = MyClass.new
            my_class.roundtrip(true))";
  result = m.module_eval(code);
  ASSERT(detail::From_Ruby<bool>().convert(result));

  code = R"(my_class = MyClass.new
            my_class.roundtrip(false))";
  result = m.module_eval(code);
  ASSERT(!detail::From_Ruby<bool>().convert(result));

  code = R"(my_class = MyClass.new
            my_class.roundtrip(45))";
  result = m.module_eval(code);
  ASSERT_EQUAL(45, detail::From_Ruby<int>().convert(result));
}

TESTCASE(VariantAttribute)
{
  Module m = define_module("Testing");
  Object myClass = m.module_eval("MyClass.new");

  Object result = myClass.call("variant_attr");
  ASSERT_EQUAL("Initial value", detail::From_Ruby<std::string>().convert(result));

  result = myClass.call("variant_attr=", "Second value");
  ASSERT_EQUAL("Second value", detail::From_Ruby<std::string>().convert(result));
  result = myClass.call("variant_attr");
  ASSERT_EQUAL("Second value", detail::From_Ruby<std::string>().convert(result));

  result = myClass.call("variant_attr=", 77.7);
  ASSERT_EQUAL(77.7, detail::From_Ruby<double>().convert(result));
  result = myClass.call("variant_attr");
  ASSERT_EQUAL(77.7, detail::From_Ruby<double>().convert(result));

  result = myClass.call("variant_attr=", true);
  ASSERT(detail::From_Ruby<bool>().convert(result));
  result = myClass.call("variant_attr");
  ASSERT(detail::From_Ruby<bool>().convert(result));

  result = myClass.call("variant_attr=", false);
  ASSERT(!detail::From_Ruby<bool>().convert(result));
  result = myClass.call("variant_attr");
  ASSERT(!detail::From_Ruby<bool>().convert(result));

  result = myClass.call("variant_attr=", 78);
  ASSERT_EQUAL(78, detail::From_Ruby<int>().convert(result));
  result = myClass.call("variant_attr");
  ASSERT_EQUAL(78, detail::From_Ruby<int>().convert(result));
}

TESTCASE(ClassReturns)
{
  Module m = define_module("Testing");

  Data_Object<MyClass1> myclass1 = m.module_eval("variant_class(true)");
  String hello = myclass1.call("say_hello");
  ASSERT_EQUAL("Hi from MyClass1", detail::From_Ruby<std::string>().convert(hello));

  Data_Object<MyClass2> myclass2 = m.module_eval("variant_class(false)");
  hello = myclass2.call("say_hello");
  ASSERT_EQUAL("Hi from MyClass2", detail::From_Ruby<std::string>().convert(hello));
}

TESTCASE(Roundtrip)
{
  Module m = define_module("Testing");

  Object instance = m.module_eval("MyClass1.new");
  Object instance2 = m.call("roundtrip_variant", instance);
  String hello = instance2.call("say_hello");
  ASSERT_EQUAL("Hi from MyClass1", detail::From_Ruby<std::string>().convert(hello));

  instance = m.module_eval("MyClass2.new");
  instance2 = m.call("roundtrip_variant", instance);
  hello = instance2.call("say_hello");
  ASSERT_EQUAL("Hi from MyClass2", detail::From_Ruby<std::string>().convert(hello));

  instance2 = m.call("roundtrip_variant", nullptr);
  ASSERT_EQUAL(Qnil, instance2.value());
}

/* This test case runs successfully on MSVC but not g++. Having stepped through the code with
  GDB, this sure seems due to a bug with g++. The issue is this variable in created operator():

        Arg_Ts nativeValues = this->getNativeValues(rubyValues, indices);

 And is then passed to invokeNativeFunction as a const Arg_Ts& nativeArgs where Arg_Ts& is
 std::tuple with one element, a reference to a variant. So it doesn't change and the address
 of the variable doesn't change. But for some reason g++ resets the
 the std::variant index to 0 thus breaking the test. Maybe something to do with storing
 a reference to a variant in a tuple? */

#ifdef _MSC_VER
TESTCASE(RoundtripRef)
{
  Module m = define_module("Testing");

  Object instance = m.module_eval("MyClass1.new");
  Object instance2 = m.call("roundtrip_variant_ref", instance);
  String hello = instance2.call("say_hello");
  ASSERT_EQUAL("Hi from MyClass1", detail::From_Ruby<std::string>().convert(hello));

  instance = m.module_eval("MyClass2.new");
  instance2 = m.call("roundtrip_variant_ref", instance);
  hello = instance2.call("say_hello");
  ASSERT_EQUAL("Hi from MyClass2", detail::From_Ruby<std::string>().convert(hello));

  instance2 = m.call("roundtrip_variant_ref", nullptr);
  ASSERT_EQUAL(Qnil, instance2.value());
}
#endif

TESTCASE(RoundtripConstRef)
{
  Module m = define_module("Testing");

  Object instance = m.module_eval("MyClass1.new");
  Object instance2 = m.call("roundtrip_const_variant_ref", instance);
  String hello = instance2.call("say_hello");
  ASSERT_EQUAL("Hi from MyClass1", detail::From_Ruby<std::string>().convert(hello));

  instance = m.module_eval("MyClass2.new");
  instance2 = m.call("roundtrip_const_variant_ref", instance);
  hello = instance2.call("say_hello");
  ASSERT_EQUAL("Hi from MyClass2", detail::From_Ruby<std::string>().convert(hello));

  instance2 = m.call("roundtrip_const_variant_ref", nullptr);
  ASSERT_EQUAL(Qnil, instance2.value());
}

namespace
{
  class MyClass4
  {
  public:
    size_t variantIndex(std::variant<std::vector<std::string>, std::vector<int>> variant)
    {
      return variant.index();
    }
  };
}

TESTCASE(VariantWithTwoVectors)
{
  using namespace std::complex_literals;

  define_class<MyClass4>("MyClass4").
    define_constructor(Constructor<MyClass4>()).
    define_method("variant_index", &MyClass4::variantIndex);

  Module m = define_module("Testing");

  std::string code = R"(vector = Std::Vector≺string≻.new
                          vector << "a" << "b" << "c"
                          my_class = MyClass4.new
                          my_class.variant_index(vector))";

  Object result = m.module_eval(code);
  ASSERT_EQUAL(0, detail::From_Ruby<size_t>().convert(result));

  code = R"(vector = Std::Vector≺int≻.new
              vector.push_back(4)
              my_class = MyClass4.new
              my_class.variant_index(vector))";
  result = m.module_eval(code);
  ASSERT_EQUAL(1, detail::From_Ruby<size_t>().convert(result));

  code = R"(my_class = MyClass4.new
              my_class.variant_index(["x", "y", "z"]))";
  result = m.module_eval(code);
  ASSERT_EQUAL(0, detail::From_Ruby<size_t>().convert(result));

  code = R"(my_class = MyClass4.new
              my_class.variant_index([5, 6]))";

  ASSERT_EXCEPTION_CHECK(
    Exception,
    m.module_eval(code),
    ASSERT_EQUAL("wrong argument type Integer (expected String)", ex.what())
  );
}

namespace
{
  class MyClass5
  {
  public:
    static inline int a = 10;
    static inline float b = 11.0;
  public:
    std::variant<Buffer<int>, Buffer<float>> variantBuffer(int index)
    {
      if (index == 0)
      {
        return Buffer<int>(&a);
      }
      else
      {
        return Buffer<float>(&b);
      }
    }
  };
}

TESTCASE(Buffer)
{
  define_buffer<int>();
  define_buffer<float>();
  using namespace std::complex_literals;

  define_class<MyClass5>("MyClass5").
    define_constructor(Constructor<MyClass5>()).
    define_method("buffer", &MyClass5::variantBuffer);

  Module m = define_module("Testing");

  std::string code = R"(myclass = MyClass5.new
                          myclass.buffer(0).to_ary(1).first)";

  Object result = m.module_eval(code);
  ASSERT_EQUAL(10, detail::From_Ruby<int>().convert(result));

  code = R"(myclass = MyClass5.new
              myclass.buffer(1).to_ary(1).first)";
  result = m.module_eval(code);
  ASSERT_EQUAL(11.0, detail::From_Ruby<float>().convert(result));
}
