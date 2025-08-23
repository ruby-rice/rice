#include <assert.h> 

#include "unittest.hpp"
#include "embed_ruby.hpp"
#include <rice/rice.hpp>
#include <rice/stl.hpp>

using namespace Rice;

TESTSUITE(Data_Type);

SETUP(Data_Type)
{
  embed_ruby();
}

TEARDOWN(Data_Type)
{
  Rice::detail::Registries::instance.types.clearUnverifiedTypes();
  rb_gc_start();
}

namespace
{
  class MyClass
  {
  public:
    static inline bool no_return_no_arg_called = false;
    static inline bool no_arg_called = false;
    static inline bool int_arg_called = false;
    static inline bool multiple_args_called = false;

    static void reset()
    {
      no_return_no_arg_called = false;
      no_arg_called = false;
      int_arg_called = false;
      multiple_args_called = false;
    }

    static Object singleton_method_object_int(Object object, int anInt)
    {
      return object;
    }

    static int singleton_function_int(int anInt)
    {
      return anInt;
    }

  public:
    MyClass() = default;
    MyClass(const MyClass& other) = delete;
    MyClass(MyClass&& other) = delete;

    void no_return_no_arg()
    {
      no_return_no_arg_called = true;
    }

    bool no_arg()
    {
      no_arg_called = true;
      return true;
    }

    int int_arg(int i)
    {
      int_arg_called = true;
      return i;
    }

    std::string multiple_args(int i, bool b, float f, std::string s, char* c)
    {
      multiple_args_called = true;
      return "multiple_args(" + std::to_string(i) + ", " + std::to_string(b) + ", " +
        std::to_string(f) + ", " + s + ", " + std::string(c) + ")";
    }
  };
}

TESTCASE(methods_with_member_pointers)
{
  Class c = define_class<MyClass>("MyClass")
    .define_constructor(Constructor<MyClass>())
    .define_method("no_return_no_arg", &MyClass::no_return_no_arg)
    .define_method("no_arg", &MyClass::no_arg)
    .define_method("int_arg", &MyClass::int_arg)
    .define_method("multiple_args", &MyClass::multiple_args);

  MyClass::reset();
  Object o = c.call("new");

  Object result = o.call("no_return_no_arg");
  ASSERT(MyClass::no_return_no_arg_called);
  ASSERT_EQUAL(Qnil, result.value());

  result = o.call("no_arg");
  ASSERT(MyClass::no_arg_called);
  ASSERT_EQUAL(Qtrue, result.value());

  result = o.call("int_arg", 42);
  ASSERT(MyClass::int_arg_called);
  ASSERT_EQUAL(42, detail::From_Ruby<int>().convert(result.value()));

  result = o.call("multiple_args", 81, true, 7.0, "a string", "a char");
  ASSERT(MyClass::multiple_args_called);
  ASSERT_EQUAL("multiple_args(81, 1, 7.000000, a string, a char)", detail::From_Ruby<std::string>().convert(result.value()));
}

namespace
{
  class MyClassOriginal
  {

  };

  namespace InnerNamespace
  {
    class MyClassOriginal
    {
    };
  }
}

TESTCASE(define_class_twice)
{
  Module module(rb_cObject);

  Class c1 = define_class<MyClassOriginal>("MyClassOriginal");
  bool result = module.const_defined("MyClassOriginal");
  ASSERT(result);
  String name = c1.name();
  ASSERT_EQUAL("MyClassOriginal", name.str());

  Class c2 = define_class<MyClassOriginal>("MyClassDuplicate");
  result = c2.is_equal(c1);
  ASSERT(result);

  result = module.const_defined("MyClassDuplicate");
  name = c2.name();
  ASSERT_EQUAL("MyClassOriginal", name.str());
}

TESTCASE(define_class_twice_under)
{
  Module inner = define_module("InnerNamespace");

  Class c1 = define_class_under<InnerNamespace::MyClassOriginal>(inner, "MyClassOriginal");
  bool result = inner.const_defined("MyClassOriginal");
  ASSERT(result);
  String name = c1.name();
  ASSERT_EQUAL("InnerNamespace::MyClassOriginal", name.str());

  Class c2 = define_class_under<InnerNamespace::MyClassOriginal>(inner, "MyClassDuplicate");
  result = c2.is_equal(c1);
  ASSERT(result);

  result = inner.const_defined("MyClassDuplicate");
  name = c2.name();
  ASSERT_EQUAL("InnerNamespace::MyClassOriginal", name.str());
}

TESTCASE(incorrect_number_of_args)
{
  Class c =
    define_class<MyClass>("MyClass")
    .define_constructor(Constructor<MyClass>())
    .define_method("int_arg", &MyClass::int_arg);

  Object o = c.call("new");

  ASSERT_EXCEPTION_CHECK(
    Exception,
    o.call("int_arg", 1, 2),
    ASSERT_EQUAL(rb_eArgError, ex.class_of())
  );
}

TESTCASE(incorrect_no_args)
{
  Class c =
    define_class<MyClass>("MyClass")
    .define_constructor(Constructor<MyClass>())
    .define_method("int_arg", &MyClass::int_arg);

  Object o = c.call("new");

  ASSERT_EXCEPTION_CHECK(
    Exception,
    o.call("int_arg"),
    ASSERT_EQUAL(rb_eArgError, ex.class_of())
  );
}

TESTCASE(methods_with_lambdas)
{
  Class c = define_class<MyClass>("MyClass")
    .define_constructor(Constructor<MyClass>())
    .define_method("no_return_no_arg", 
        [](MyClass& instance)
        {
          instance.no_return_no_arg();
        })
    .define_method("no_arg",
        [](MyClass& instance)
        {
          return instance.no_arg();
        })
    .define_method("int_arg", 
        [](MyClass& instance, int anInt)
        {
          return instance.int_arg(anInt);
        })
    .define_method("multiple_args",
        [](MyClass& instance, int anInt, bool aBool, float aFloat, std::string aString, char* aChar)
        {
          return instance.multiple_args(anInt, aBool, aFloat, aString, aChar);
        });

  MyClass::reset();
  Object o = c.call("new");

  Object result = o.call("no_return_no_arg");
  ASSERT(MyClass::no_return_no_arg_called);
  ASSERT_EQUAL(Qnil, result.value());

  result = o.call("no_arg");
  ASSERT(MyClass::no_arg_called);
  ASSERT_EQUAL(Qtrue, result.value());

  result = o.call("int_arg", 42);
  ASSERT(MyClass::int_arg_called);
  ASSERT_EQUAL(42, detail::From_Ruby<int>().convert(result.value()));

  result = o.call("multiple_args", 81, true, 7.0, "a string", "a char");
  ASSERT(MyClass::multiple_args_called);
  ASSERT_EQUAL("multiple_args(81, 1, 7.000000, a string, a char)", detail::From_Ruby<std::string>().convert(result.value()));
}

TESTCASE(static_singleton_method)
{
  Class c = define_class<MyClass>("MyClass")
    .define_constructor(Constructor<MyClass>())
    .define_singleton_method("singleton_method_object_int", &MyClass::singleton_method_object_int);

  MyClass::reset();

  Object result = c.call("singleton_method_object_int", 42);
  ASSERT_EQUAL(c, result);
}

TESTCASE(static_singleton_function)
{
  Class c = define_class<MyClass>("MyClass")
    .define_constructor(Constructor<MyClass>())
    .define_singleton_function("singleton_function_int", &MyClass::singleton_function_int);

  MyClass::reset();

  Object result = c.call("singleton_function_int", 42);
  ASSERT_EQUAL(42, detail::From_Ruby<int>().convert(result));
}

TESTCASE(static_singleton_method_lambda)
{
  Class c = define_class<MyClass>("MyClass")
    .define_constructor(Constructor<MyClass>())
    .define_singleton_method("singleton_method_object_int", [](Object object, int anInt)
      {
        return MyClass::singleton_method_object_int(object, anInt);
      });

  MyClass::reset();

  Object result = c.call("singleton_method_object_int", 42);
  ASSERT_EQUAL(c, result);
}

TESTCASE(static_singleton_function_lambda)
{
  Class c = define_class<MyClass>("MyClass")
    .define_constructor(Constructor<MyClass>())
    .define_singleton_function("singleton_function_int", [](int anInt)
      {
        return MyClass::singleton_function_int(anInt);
      });

  MyClass::reset();

  Object result = c.call("singleton_function_int", 42);
  ASSERT_EQUAL(42, detail::From_Ruby<int>().convert(result));
}

namespace {
  class BaseClass
  {
  public:
    BaseClass() {}

    bool some_method()
    {
      return true;
    }

    bool another_method()
    {
      return true;
    }
  };
}

TESTCASE(subclassing)
{
  Module m = define_module("Testing");
  define_class_under<BaseClass>(m, "BaseClass").
    define_constructor(Constructor<BaseClass>()).
    define_method("some_method", &BaseClass::some_method).
    define_method("another_method", &BaseClass::another_method);

    std::string code = R"(class ChildClass < BaseClass
                            def child_method
                              false
                            end

                            def another_method
                              super
                            end
                          end

                          instance = ChildClass.new
                          instance.some_method
                          instance.child_method
                          instance.another_method)";

  Object result = m.module_eval(code);
  ASSERT_EQUAL(Qtrue, result.value());
}

TESTCASE(subclass_override_initializer)
{
  Module m = define_module("Testing");
  define_class_under<BaseClass>(m, "BaseClass").
    define_constructor(Constructor<BaseClass>()).
    define_method("some_method", &BaseClass::some_method);

  std::string code = R"(class ChildClass < BaseClass
                          def initialize
                            # Note NO super call so class in incorrectly initialized
                          end
                        end

                        instance = ChildClass.new
                        instance.some_method)";

  ASSERT_EXCEPTION_CHECK(
    Exception,
    m.module_eval(code),
    ASSERT_EQUAL("Wrapped C++ object is nil. Did you override Testing::ChildClass#initialize and forget to call super?", ex.what())
  );
}

namespace {
  float with_reference_defaults_x;
  std::string with_reference_defaults_str;

  class DefaultArgsRefs
  {
  public:
    void with_reference_defaults(float x, std::string const& str = std::string("testing"))
    {
      with_reference_defaults_x = x;
      with_reference_defaults_str = str;
    }
  };
}

TESTCASE(define_method_works_with_reference_const_default_values)
{
  Class c = define_class<DefaultArgsRefs>("DefaultArgsRefs")
    .define_constructor(Constructor<DefaultArgsRefs>())
    .define_method("bar",
      &DefaultArgsRefs::with_reference_defaults,
      Arg("x"), Arg("str") = std::string("testing"));

  Object o = c.call("new");
  o.call("bar", 3);

  ASSERT_EQUAL(3, with_reference_defaults_x);
  ASSERT_EQUAL("testing", with_reference_defaults_str);
}

namespace
{
  class RefTest
  {
  public:
    RefTest() {}

    static std::string& getReference()
    {
      static std::string foo = "foo";
      return foo;
    }
  };
}

TESTCASE(define_singleton_method_returning_reference)
{
  Class c = define_class<RefTest>("RefTest")
    .define_constructor(Constructor<RefTest>())
    .define_singleton_function("get_reference", &RefTest::getReference);

  Module m(anonymous_module());

  Object result = m.module_eval("RefTest.get_reference");
  ASSERT_EQUAL(result, String("foo"));
}

namespace
{
  class RValue
  {
  public:
    RValue() {}

    RValue(RValue&& other) = default;

    // Move assignment operator.
    RValue& operator=(RValue&& other) noexcept
    {
      return *this;
    }

    bool takesRValue(RValue&& rvalue)
    {
      return true;
    }
  };
}

TESTCASE(rvalue_parameter)
{
  Class c = define_class<RValue>("RValue")
    .define_constructor(Constructor<RValue>())
    .define_method("takes_r_value", &RValue::takesRValue);

  Module m(anonymous_module());
  std::string code = R"(rvalue = RValue.new
                        rvalue.takes_r_value(rvalue))";

  Object result = m.module_eval(code);
  ASSERT_EQUAL(Qtrue, result.value());
}

TESTCASE(move_assignment)
{
  Class c = define_class<RValue>("RValue")
    .define_constructor(Constructor<RValue>())
    .define_method<RValue&(RValue::*)(RValue && other) noexcept>("=", &RValue::operator=);

  Module m(anonymous_module());
  std::string code = R"(object1 = RValue.new
                        object2 = RValue.new
                        object1 = object2)";

  Object result = m.module_eval(code);
  ASSERT_EQUAL(c, result.class_of());
}

namespace
{
  struct MyStruct
  {
    MyStruct* set(MyStruct* ptr)
    {
      assert(ptr == nullptr);
      return ptr;
    }

    MyStruct* get()
    {
      return nullptr;
    }
  };
}

TESTCASE(null_ptrs)
{
  Class c = define_class<MyStruct>("MyStruct")
    .define_constructor(Constructor<MyStruct>())
    .define_method("get", &MyStruct::get)
    .define_method("set", &MyStruct::set);

  Object o = c.call("new");

  Object result = o.call("get");
  ASSERT_EQUAL(Qnil, result.value());

  result = o.call("set", nullptr);
  ASSERT_EQUAL(Qnil, result.value());
}

namespace
{
  class Helper
  {
  public:

    Helper(int value) : value_(value)
    {
    }

    int value()
    {
      return this->value_;
    }

  private:
    int value_;
  };

  class MyClass2
  {
  public:
    Helper* passThrough(Helper* helper)
    {
      return helper;
    }

    const Helper* passThroughConst(const Helper* helper)
    {
      return helper;
    }

    Helper* passThrough(void* helper)
    {
      return static_cast<Helper*>(helper);
    }

    void* returnVoidHelper()
    {
      if (!this->helper_)
        this->helper_ = new Helper(4);
      
      return static_cast<void*>(this->helper_);
    }

    bool checkVoidHelper(void* helper)
    {
      return helper == this->helper_;
    }

  private:
    Helper* helper_ = nullptr;
  };
} // namespace

TESTCASE(pointers)
{
  Class helperClass = define_class<Helper>("Helper")
    .define_constructor(Constructor<Helper, int>())
    .define_method("value", &Helper::value);

  Class myClass = define_class<MyClass2>("MyClass2")
    .define_constructor(Constructor<MyClass2>())
    .define_method<Helper*(MyClass2::*)(Helper*)>("pass_through", &MyClass2::passThrough)
    .define_method<const Helper*(MyClass2::*)(const Helper*)>("pass_through_const", &MyClass2::passThroughConst)
    .define_method<Helper*(MyClass2::*)(void*)>("pass_through_void", &MyClass2::passThrough)
    .define_method<void*(MyClass2::*)()>("return_void_helper", &MyClass2::returnVoidHelper)
    .define_method<bool(MyClass2::*)(void*)>("check_void_helper", &MyClass2::checkVoidHelper);

  Object helper = helperClass.call("new", 5);
  Object object = myClass.call("new");

  Object result = object.call("pass_through", nullptr);
  ASSERT_EQUAL(Qnil, result.value());

  result = object.call("pass_through", helper);
  Object value = result.call("value");
  ASSERT_EQUAL(5, detail::From_Ruby<int>().convert(value));

  result = object.call("pass_through_const", helper);
  value = result.call("value");
  ASSERT_EQUAL(5, detail::From_Ruby<int>().convert(value));

  result = object.call("pass_through_void", nullptr);
  ASSERT_EQUAL(Qnil, result.value());

  result = object.call("pass_through_void", helper);
  value = result.call("value");
  ASSERT_EQUAL(5, detail::From_Ruby<int>().convert(value));

  helper = object.call("return_void_helper");
  result = object.call("check_void_helper", helper);
  ASSERT_EQUAL(Qtrue, result.value());
}

namespace
{
  class BigObject
  {
  public:
    BigObject(int value): value(value)
    {
    }

  public:
    int value;
  };

  class Processor
  {
  public:
    BigObject** createBigObjects(size_t size)
    {
      BigObject** result = new BigObject*[size];

      for (size_t i = 0; i < size; ++i)
      {
        result[i] = new BigObject(i + 5);
      }
      return result;
    }

    int sumBigObjects(BigObject** bigObjects, size_t size)
    {
      int result = 0;

      for (size_t i = 0; i < size; i++)
      {
        result += bigObjects[i]->value;
      }
      return result;
    }

    int sumBigObjectsConst(const BigObject**  bigObjects, size_t size)
    {
      int result = 0;

      for (size_t i = 0; i < size; i++)
      {
        result += bigObjects[i]->value;
      }
      return result;
    }

  private:
    BigObject** bigObjects_ = nullptr;
  };

}

TESTCASE(pointerToPointer)
{
  define_buffer<BigObject*>();

  Module m = define_module("DataTypePointerToPointer");

  Class BigObjectClass = define_class<BigObject>("BigObject")
    .define_attr("value", &BigObject::value);

  Class ProcessorClass = define_class<Processor>("ProcessorClass")
    .define_constructor(Constructor<Processor>())
    .define_method("create", &Processor::createBigObjects)
    .define_method("sum", &Processor::sumBigObjects,
                          Arg("bigObjects").setArray(), Arg("size"))
    .define_method("sum_const", &Processor::sumBigObjectsConst,
                                Arg("bigObjects").setArray(), Arg("size"));

  std::string code = R"(count = 2
                          processor = ProcessorClass.new
                          big_objects = processor.create(count)
                          processor.sum(big_objects, count))";

  Object result = m.module_eval(code);
  ASSERT_EQUAL(11, detail::From_Ruby<int>().convert(result));

  code = R"(count = 2
              processor = ProcessorClass.new
              big_objects = processor.create(count)
              processor.sum_const(big_objects, count))";

  result = m.module_eval(code);
  ASSERT_EQUAL(11, detail::From_Ruby<int>().convert(result));
}

namespace
{
  class UnknownClass
  {
  };

  void undefinedArg(UnknownClass unknownClass)
  {
  }

  void undefinedArg(UnknownClass& unknownClass)
  {
  }

  void undefinedArg(UnknownClass* unknownClass)
  {
  }

  UnknownClass undefinedReturn()
  {
    return UnknownClass();
  }
}

TESTCASE(not_defined)
{
  Module m = define_module("TestingDataTypeNotDefined");

#ifdef _MSC_VER
  const char* message = "The following types are not registered with Rice:\n  class `anonymous namespace'::UnknownClass\n";
#else
  const char* message = "The following types are not registered with Rice:\n  (anonymous namespace)::UnknownClass\n";
#endif

  m.define_module_function("undefined_return", &undefinedReturn);

  ASSERT_EXCEPTION_CHECK(
    std::invalid_argument,
    Rice::detail::Registries::instance.types.validateTypes(),
    ASSERT_EQUAL(message, ex.what())
  );

#ifdef _MSC_VER
  message = "Type is not registered with Rice: class `anonymous namespace'::UnknownClass";
#else
  message = "Type is not registered with Rice: (anonymous namespace)::UnknownClass";
#endif

  ASSERT_EXCEPTION_CHECK(
    Rice::Exception,
    m.call("undefined_return"),
    ASSERT_EQUAL(message, ex.what())
  );

#ifdef _MSC_VER
  message = "Type is not defined with Rice: class `anonymous namespace'::UnknownClass";
#else
  message = "Type is not defined with Rice: (anonymous namespace)::UnknownClass";
#endif

  m.define_module_function<void(*)(UnknownClass)>("undefined_arg_value", &undefinedArg);

  ASSERT_EXCEPTION_CHECK(
    Rice::Exception,
    m.call("undefined_arg_value", nullptr),
    ASSERT_EQUAL(message, ex.what())
  );

  m.define_module_function<void(*)(UnknownClass&)>("undefined_arg_reference", &undefinedArg);

  ASSERT_EXCEPTION_CHECK(
    Rice::Exception,
    m.call("undefined_arg_reference", nullptr),
    ASSERT_EQUAL(message, ex.what())
  );

  m.define_module_function<void(*)(UnknownClass*)>("undefined_arg_pointer", &undefinedArg);
  
  // This actually works because we pass a nullptr
  m.call("undefined_arg_pointer", nullptr);
}

namespace RangesTest
{
  class RangeCustom
  {
  public:
    RangeCustom(int x, int y) : x(x), y(y)
    {
    }

    RangeCustom(const RangeCustom& other) = default;

    int x;
    int y;
  };

  int sumRangesArray(int size, RangeCustom ranges[])
  {
    int result = 0;
    for (int i = 0; i < size; i++)
    {
      const RangeCustom& range = ranges[i];
      result += range.x + range.y;
    }

    return result;
  }

  int sumRanges(int size, const RangeCustom* ranges)
  {
    int result = 0;
    for (int i = 0; i < size; i++)
    {
      const RangeCustom& range = ranges[i];
      result += range.x + range.y;
    }

    return result;
  }

  int sumRanges(int size, const RangeCustom** ranges)
  {
    int result = 0;
    for (int i = 0; i < size; i++)
    {
      const RangeCustom* range = ranges[i];
      result += range->x + range->y;
    }

    return result;
  }
}

TESTCASE(array_of_ranges)
{
  define_buffer<RangesTest::RangeCustom>();

  Module m = define_module("CustomRanges");

  Class c = define_class_under<RangesTest::RangeCustom>(m, "RangeCustom")
    .define_constructor(Constructor<RangesTest::RangeCustom, int, int>())
    .define_attr("x", &RangesTest::RangeCustom::x)
    .define_attr("y", &RangesTest::RangeCustom::y);

  m.define_module_function("sum_ranges_array", RangesTest::sumRangesArray, 
                                               Arg("size"), Arg("ranges[]").setArray());

  std::string code = R"(range1 = RangeCustom.new(1, 2)
                        range2 = RangeCustom.new(3, 4)
                        range3 = RangeCustom.new(5, 6)
            
                        buffer = Rice::Buffer≺RangesTest꞉꞉RangeCustom≻.new([range1, range2, range3])
                        sum_ranges_array(buffer.size, buffer))";

  Object result = m.module_eval(code);
  ASSERT_EQUAL(21, detail::From_Ruby<int>().convert(result));
}

TESTCASE(pointer_of_ranges)
{
  define_buffer<RangesTest::RangeCustom>();

  Module m = define_module("CustomRanges");

  Class c = define_class_under<RangesTest::RangeCustom>(m, "RangeCustom")
    .define_constructor(Constructor<RangesTest::RangeCustom, int, int>())
    .define_attr("x", &RangesTest::RangeCustom::x)
    .define_attr("y", &RangesTest::RangeCustom::y);

  m.define_module_function<int(*)(int, const RangesTest::RangeCustom*)>("sum_ranges", RangesTest::sumRanges,
                                                                        Arg("size"), Arg("ranges*").setArray());

  std::string code = R"(range1 = RangeCustom.new(1, 2)
                        range2 = RangeCustom.new(3, 4)
                        range3 = RangeCustom.new(5, 6)

                        buffer = Rice::Buffer≺RangesTest꞉꞉RangeCustom≻.new([range1, range2, range3])
                        sum_ranges(buffer.size, buffer))";

  Object result = m.module_eval(code);
  ASSERT_EQUAL(21, detail::From_Ruby<int>().convert(result));
}

TESTCASE(pointer_of_ranges_wrong)
{
  define_buffer<RangesTest::RangeCustom>();

  Module m = define_module("CustomRanges");

  Class c = define_class_under<RangesTest::RangeCustom>(m, "RangeCustom")
    .define_constructor(Constructor<RangesTest::RangeCustom, int, int>())
    .define_attr("x", &RangesTest::RangeCustom::x)
    .define_attr("y", &RangesTest::RangeCustom::y);

  m.define_module_function<int(*)(int, const RangesTest::RangeCustom*)>("sum_ranges_wrong", RangesTest::sumRanges);

  std::string code = R"(range1 = RangeCustom.new(1, 2)
                        range2 = RangeCustom.new(3, 4)
                        range3 = RangeCustom.new(5, 6)

                        buffer = Rice::Buffer≺RangesTest꞉꞉RangeCustom≻.new([range1, range2, range3])
                        sum_ranges_wrong(buffer.size, buffer))";

  ASSERT_EXCEPTION_CHECK(
    Rice::Exception,
    m.module_eval(code),
    ASSERT_EQUAL("wrong argument type Rice::Buffer≺RangesTest꞉꞉RangeCustom≻ (expected CustomRanges::RangeCustom)", ex.what())
  );
}

TESTCASE(pointer_of_pointer_ranges)
{
  define_buffer<RangesTest::RangeCustom*>();

  Module m = define_module("CustomRanges");

  Class c = define_class_under<RangesTest::RangeCustom>(m, "RangeCustom")
    .define_constructor(Constructor<RangesTest::RangeCustom, int, int>())
    .define_attr("x", &RangesTest::RangeCustom::x)
    .define_attr("y", &RangesTest::RangeCustom::y);

  m.define_module_function<int(*)(int, const RangesTest::RangeCustom**)>("sum_ranges", RangesTest::sumRanges);

  std::string code = R"(range1 = RangeCustom.new(1, 2)
                        range2 = RangeCustom.new(3, 4)
                        range3 = RangeCustom.new(5, 6)
                        buffer = Rice::Buffer≺RangesTest꞉꞉RangeCustom≻.new([range1, range2, range3])
                        sum_ranges(buffer.size, buffer))";

  Object result = m.module_eval(code);
  ASSERT_EQUAL(21, detail::From_Ruby<int>().convert(result));
}
