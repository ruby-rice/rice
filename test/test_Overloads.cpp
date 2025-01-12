#include "unittest.hpp"
#include "embed_ruby.hpp"
#include <rice/rice.hpp>
#include <rice/stl.hpp>

using namespace Rice;

TESTSUITE(Overloads);

SETUP(Overloads)
{
  embed_ruby();
}

namespace
{
  class MyClass;
  class MyClass2;
  class MyClass3;
}

TEARDOWN(Overloads)
{
  Data_Type<MyClass>::unbind();
  Data_Type<MyClass2>::unbind();
  Data_Type<MyClass3>::unbind();
  Rice::detail::Registries::instance.types.remove<MyClass>();
  Rice::detail::Registries::instance.types.remove<MyClass2>();
  Rice::detail::Registries::instance.types.remove<MyClass3>();
  rb_gc_start();
}

namespace
{
  std::string run()
  {
    return "run<void>";
  }

  std::string run(int a)
  {
    return "run<int>";
  }

  std::string run(float a)
  {
    return "run<float>";
  }

  std::string run(std::string a)
  {
    return "run<string>";
  }

  std::string run(int a, float b = 1.0)
  {
    return "run<int,float>";
  }

  std::string run(float a, int b = 1)
  {
    return "run<float,int>";
  }
} // namespace

void create_globals()
{
  define_global_function<std::string(*)()>("run", &run);
  define_global_function<std::string(*)(std::string)>("run", &run);
  define_global_function<std::string(*)(int, float)>("run", &run, Arg("a"), Arg("b") = 1.0f);
  define_global_function<std::string(*)(float, int)>("run", &run, Arg("a"), Arg("b") = 1);
  define_global_function<std::string(*)(int)>("run", &run);
  define_global_function<std::string(*)(float)>("run", &run);
}

TESTCASE(function_zero_parameters)
{
  Module m = define_module("Testing");
  create_globals();

  std::string code = R"(run())";

  Rice::String result = m.module_eval(code);
  ASSERT_EQUAL("run<void>", result.str());
}

TESTCASE(function_one_parameter)
{
  Module m = define_module("Testing");
  create_globals();

  std::string code = R"(run(1))";

  Rice::String result = m.module_eval(code);
  ASSERT_EQUAL("run<int>", result.str());

  code = R"(run(1.0))";

  result = m.module_eval(code);
  ASSERT_EQUAL("run<float>", result.str());

  code = R"(run('hello'))";

  result = m.module_eval(code);
  ASSERT_EQUAL("run<string>", result.str());
}

TESTCASE(function_two_parameters)
{
  Module m = define_module("Testing");
  create_globals();

  std::string code = R"(run(1, 1.0))";

  Rice::String result = m.module_eval(code);
  ASSERT_EQUAL("run<int,float>", result.str());

  code = R"(run(1.0, 1))";

  result = m.module_eval(code);
  ASSERT_EQUAL("run<float,int>", result.str());
}

namespace
{
  class MyClass
  {
  public:
    MyClass() = default;

    std::string run()
    {
      return "run<void>";
    }

    std::string run(int a)
    {
      return "run<int>";
    }

    std::string run(float a)
    {
      return "run<float>";
    }

    std::string run(std::string a)
    {
      return "run<string>";
    }

    std::string run(int a, float b = 1.0)
    {
      return "run<int,float>";
    }

    std::string run(float a, int b = 1)
    {
      return "run<float,int>";
    }
  };
} // namespace

Class createMyClass()
{
  using Data_Type_T = Data_Type<MyClass>;
  if (Data_Type_T::is_bound())
    detail::Registries::instance.natives.reset(Data_Type_T::klass());

  Class c = define_class<MyClass>("MyClass")
    .define_constructor(Constructor<MyClass>())
    .define_method<std::string(MyClass::*)()>("run", &MyClass::run)
    .define_method<std::string(MyClass::*)(std::string)>("run", &MyClass::run)
    .define_method<std::string(MyClass::*)(int, float)>("run", &MyClass::run, Arg("a"), Arg("b") = 1.0f)
    .define_method<std::string(MyClass::*)(float, int)>("run", &MyClass::run, Arg("a"), Arg("b") = 1)
    .define_method<std::string(MyClass::*)(int)>("run", &MyClass::run)
    .define_method<std::string(MyClass::*)(float)>("run", &MyClass::run);

  return c;
}

TESTCASE(method_zero_parameters)
{
  Module m = define_module("Testing");
  Class c = createMyClass();

  std::string code = R"(my_class = MyClass.new
                        my_class.run())";

  Rice::String result = m.module_eval(code);
  ASSERT_EQUAL("run<void>", result.str());
}

TESTCASE(method_one_parameter)
{
  Module m = define_module("Testing");
  Class c = createMyClass();

  std::string code = R"(my_class = MyClass.new
                        my_class.run(1))";

  Rice::String result = m.module_eval(code);
  ASSERT_EQUAL("run<int>", result.str());

  code = R"(my_class = MyClass.new
                        my_class.run(1.0))";

  result = m.module_eval(code);
  ASSERT_EQUAL("run<float>", result.str());

  code = R"(my_class = MyClass.new
                        my_class.run('hello'))";

  result = m.module_eval(code);
  ASSERT_EQUAL("run<string>", result.str());
}

TESTCASE(method_two_parameters)
{
  Module m = define_module("Testing");
  Class c = createMyClass();

  std::string code = R"(my_class = MyClass.new
                        my_class.run(1, 1.0))";

  Rice::String result = m.module_eval(code);
  ASSERT_EQUAL("run<int,float>", result.str());

  code = R"(my_class = MyClass.new
                        my_class.run(1.0, 1))";

  result = m.module_eval(code);
  ASSERT_EQUAL("run<float,int>", result.str());
}

TESTCASE(invalid_parameters)
{
  Module m = define_module("Testing");
  Class c = createMyClass();

  std::string code = R"(my_class = MyClass.new
                        my_class.run("abc", "def"))";

  ASSERT_EXCEPTION_CHECK(
    Exception,
    Rice::String result = m.module_eval(code),
    ASSERT_EQUAL("Could not resolve method call for MyClass#run\n  6 overload(s) were evaluated based on the types of Ruby parameters provided.", 
                 ex.what()));
}

namespace
{
  class MyClass2
  {
  public:
    MyClass2()
    {
      this->constructor = "constructor<void>";
    }

    MyClass2(int a)
    {
      this->constructor = "constructor<int>";
    }

    MyClass2(float a)
    {
      this->constructor = "constructor<float>";
    }

    MyClass2(std::string a)
    {
      this->constructor = "constructor<string>";
    }

    MyClass2(int a, float b)
    {
      this->constructor = "constructor<int,float>";
    }

    MyClass2(float a, int b)
    {
      this->constructor = "constructor<float,int>";
    }
   
    std::string constructor;
  };
} // namespace

Class createMyClass2()
{
  Class c = define_class<MyClass2>("MyClass")
    .define_constructor(Constructor<MyClass2>())
    .define_constructor(Constructor<MyClass2, int>())
    .define_constructor(Constructor<MyClass2, float>())
    .define_constructor(Constructor<MyClass2, std::string>())
    .define_constructor(Constructor<MyClass2, int, float>())
    .define_constructor(Constructor<MyClass2, float, int>())
    .define_attr("constructor", &MyClass2::constructor);

  return c;
}

TESTCASE(constructor_zero_parameters)
{
  Module m = define_module("Testing");
  Class c = createMyClass2();

  std::string code = R"(MyClass.new)";

  Rice::Object result = m.module_eval(code);
  Rice::String constructor = result.call("constructor");
  ASSERT_EQUAL("constructor<void>", constructor.str());
}

TESTCASE(constructor_one_parameter)
{
  Module m = define_module("Testing");
  Class c = createMyClass2();

  std::string code = R"(MyClass.new(1))";

  Rice::Object result = m.module_eval(code);
  Rice::String constructor = result.call("constructor");
  ASSERT_EQUAL("constructor<int>", constructor.str());

  code = R"(my_class = MyClass.new(1.0))";

  result = m.module_eval(code);
  constructor = result.call("constructor");
  ASSERT_EQUAL("constructor<float>", constructor.str());

  code = R"(my_class = MyClass.new('hello'))";

  result = m.module_eval(code);
  constructor = result.call("constructor");
  ASSERT_EQUAL("constructor<string>", constructor.str());
}

TESTCASE(constructor_two_parameters)
{
  Module m = define_module("Testing");
  Class c = createMyClass2();

  std::string code = R"(MyClass.new(1, 1.0))";

  Rice::Object result = m.module_eval(code);
  Rice::String constructor = result.call("constructor");
  ASSERT_EQUAL("constructor<int,float>", constructor.str());

  code = R"(my_class = MyClass.new(1.0, 1))";

  result = m.module_eval(code);
  constructor = result.call("constructor");
  ASSERT_EQUAL("constructor<float,int>", constructor.str());
}

namespace
{
  class MyClass3
  {
  public:
    std::string run(char value)
    {
      return "run<char>";
    }

    std::string run(unsigned char value)
    {
      return "run<unsigned char>";
    }

    std::string run(char* value)
    {
      return "run<char*>";
    }

    std::string run(unsigned char* value)
    {
      return "run<unsigned char*>";
    }

    std::string run(short value)
    {
      return "run<short>";
    }

    std::string run(int value)
    {
      return "run<int>";
    }

    std::string run(float value)
    {
      return "run<float>";
    }

    std::string run(double value)
    {
      return "run<double>";
    }

    std::string run(long value)
    {
      return "run<long>";
    }

    std::string run(long long value)
    {
      return "run<long long>";
    }
  };
} // namespace


TESTCASE(int_conversion_1)
{
  Class c = define_class<MyClass3>("MyClass3").
    define_constructor(Constructor<MyClass3>()).
    define_method<std::string(MyClass3::*)(char*)>("run", &MyClass3::run).
    define_method<std::string(MyClass3::*)(unsigned char*)>("run", &MyClass3::run).
    define_method<std::string(MyClass3::*)(short)>("run", &MyClass3::run).
    define_method<std::string(MyClass3::*)(long)>("run", &MyClass3::run).
    define_method<std::string(MyClass3::*)(long long)>("run", &MyClass3::run);

  Module m = define_module("Testing");

  std::string code = R"(my_class = MyClass3.new
                        value = 1
                        my_class.run(value))";
  String result = m.module_eval(code);
  ASSERT_EQUAL("run<long>", result.str());

  code = R"(my_class = MyClass3.new
            value = 2**60
            my_class.run(value))";
  result = m.module_eval(code);

#ifdef _MSC_VER
  const char* expected = "run<long long>";
#else
  const char* expected = "run<long>";
#endif

  ASSERT_EQUAL(expected, result.str());
}

TESTCASE(int_conversion_2)
{
  Class c = define_class<MyClass3>("MyClass3").
    define_constructor(Constructor<MyClass3>()).
    define_method<std::string(MyClass3::*)(short)>("run", &MyClass3::run).
    define_method<std::string(MyClass3::*)(float)>("run", &MyClass3::run);

  Module m = define_module("Testing");

  std::string code = R"(my_class = MyClass3.new
                        value = 1
                        my_class.run(value))";
  String result = m.module_eval(code);
  ASSERT_EQUAL("run<float>", result.str());

  code = R"(my_class = MyClass3.new
            value = 2**64
            my_class.run(value))";

  ASSERT_EXCEPTION_CHECK(
    Exception,
    result = m.module_eval(code),
    ASSERT_EQUAL("Could not resolve method call for MyClass3#run\n  2 overload(s) were evaluated based on the types of Ruby parameters provided.",
    ex.what()));
}

TESTCASE(int_conversion_3)
{
  Class c = define_class<MyClass3>("MyClass3").
    define_constructor(Constructor<MyClass3>()).
    define_method<std::string(MyClass3::*)(short)>("run", &MyClass3::run).
    define_method<std::string(MyClass3::*)(double)>("run", &MyClass3::run);

  Module m = define_module("Testing");

  std::string code = R"(my_class = MyClass3.new
                        value = 1
                        my_class.run(value))";
  String result = m.module_eval(code);
  ASSERT_EQUAL("run<double>", result.str());

  code = R"(my_class = MyClass3.new
            value = 2**32
            my_class.run(value))";

  result = m.module_eval(code);
  ASSERT_EQUAL("run<double>", result.str());
}

TESTCASE(int_conversion_4)
{
  Class c = define_class<MyClass3>("MyClass3").
    define_constructor(Constructor<MyClass3>()).
    define_method<std::string(MyClass3::*)(short)>("run", &MyClass3::run);

  Module m = define_module("Testing");

  std::string code = R"(my_class = MyClass3.new
                        value = 1
                        my_class.run(value))";
  String result = m.module_eval(code);
  ASSERT_EQUAL("run<short>", result.str());

  code = R"(my_class = MyClass3.new
            value = 2**42
            my_class.run(value))";

#ifdef _MSC_VER
  const char* expected = "bignum too big to convert into `long'";
#else
  const char* expected = "integer 4398046511104 too big to convert to `short'";
#endif

  ASSERT_EXCEPTION_CHECK(
    Exception,
    result = m.module_eval(code),
    ASSERT_EQUAL(expected, ex.what()));
}

TESTCASE(int_conversion_5)
{
  Class c = define_class<MyClass3>("MyClass3").
    define_constructor(Constructor<MyClass3>()).
    define_method<std::string(MyClass3::*)(unsigned char)>("run", &MyClass3::run).
    define_method<std::string(MyClass3::*)(unsigned char*)>("run", &MyClass3::run);

  Module m = define_module("Testing");

  std::string code = R"(my_class = MyClass3.new
                        value = "54"
                        my_class.run(value))";
  String result = m.module_eval(code);
  ASSERT_EQUAL("run<unsigned char*>", result.str());
}
