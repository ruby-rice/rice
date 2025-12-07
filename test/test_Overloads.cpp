#include "unittest.hpp"
#include "embed_ruby.hpp"
#include <rice/rice.hpp>
#include <rice/stl.hpp>
#include <ruby/version.h>

RICE_USE_NAMESPACE

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
  detail::Registries::instance.types.remove<MyClass>();
  detail::Registries::instance.types.remove<MyClass2>();
  detail::Registries::instance.types.remove<MyClass3>();
  rb_gc_start();
}

namespace
{
  class Container
  {
  public:
    size_t capacity()
    {
      return this->capacity_;
    }

    void capacity(size_t value)
    {
      this->capacity_ = value;
    }

  private:
    size_t capacity_;
  };
}

TESTCASE(OverloadsWithTemplateParameter)
{
  Class c = define_class<Container>("Container")
    .define_constructor(Constructor<Container>())
    .define_method<size_t(Container::*)()>("capacity", &Container::capacity)
    .define_method<void(Container::*)(size_t)>("capacity=", &Container::capacity);


  Module m = define_module("Testing");

  std::string code = R"(container = Container.new
                        container.capacity = 5
                        container.capacity)";

  Object result = m.module_eval(code);
  ASSERT_EQUAL(5, detail::From_Ruby<int>().convert(result.value()));
}

TESTCASE(OverloadsWithUsing)
{
  using Getter_T = size_t(Container::*)();
  using Setter_T = void(Container::*)(size_t);

  Class c = define_class<Container>("Container")
    .define_constructor(Constructor<Container>())
    .define_method("capacity", (Getter_T)&Container::capacity)
    .define_method("capacity=", (Setter_T)&Container::capacity);

  Module m = define_module("Testing");

  std::string code = R"(container = Container.new
                        container.capacity = 6
                        container.capacity)";

  Object result = m.module_eval(code);
  ASSERT_EQUAL(6, detail::From_Ruby<int>().convert(result.value()));
}

TESTCASE(OverloadsWithTypedef)
{
  typedef size_t(Container::* Getter_T)();
  typedef void (Container::* Setter_T)(size_t);

  Class c = define_class<Container>("Container")
    .define_constructor(Constructor<Container>())
    .define_method("capacity", (Getter_T)&Container::capacity)
    .define_method("capacity=", (Setter_T)&Container::capacity);

  Module m = define_module("Testing");

  std::string code = R"(container = Container.new
                        container.capacity = 7
                        container.capacity)";

  Object result = m.module_eval(code);
  ASSERT_EQUAL(7, detail::From_Ruby<int>().convert(result.value()));
}

namespace
{
  std::string run()
  {
    return "run<void>";
  }

  std::string run(int)
  {
    return "run<int>";
  }

  std::string run(float)
  {
    return "run<float>";
  }

  std::string run(std::string)
  {
    return "run<string>";
  }

  std::string run(int, float = 1.0)
  {
    return "run<int,float>";
  }

  std::string run(float, int = 1)
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

  String result = m.module_eval(code);
  ASSERT_EQUAL("run<void>", result.str());
}

TESTCASE(function_one_parameter)
{
  Module m = define_module("Testing");
  create_globals();

  std::string code = R"(run(1))";

  String result = m.module_eval(code);
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

  String result = m.module_eval(code);
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

    std::string run(int)
    {
      return "run<int>";
    }

    std::string run(float)
    {
      return "run<float>";
    }

    std::string run(std::string)
    {
      return "run<string>";
    }

    std::string run(int, float = 1.0)
    {
      return "run<int,float>";
    }

    std::string run(float, int = 1)
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

  String result = m.module_eval(code);
  ASSERT_EQUAL("run<void>", result.str());
}

TESTCASE(method_one_parameter)
{
  Module m = define_module("Testing");
  Class c = createMyClass();

  std::string code = R"(my_class = MyClass.new
                        my_class.run(1))";

  String result = m.module_eval(code);
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

  String result = m.module_eval(code);
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

  std::string expected = R"(Could not resolve method call for MyClass#run
  6 overload(s) were evaluated based on the types of Ruby parameters provided:
     std::string AnonymousNamespace::MyClass*::run(int, float)
     std::string AnonymousNamespace::MyClass*::run(float, int)
     std::string AnonymousNamespace::MyClass*::run()
     std::string AnonymousNamespace::MyClass*::run(std::string)
     std::string AnonymousNamespace::MyClass*::run(int)
     std::string AnonymousNamespace::MyClass*::run(float))";

  ASSERT_EXCEPTION_CHECK(
    Exception,
    String result = m.module_eval(code),
    ASSERT_EQUAL(expected.c_str(), ex.what()));
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

    MyClass2(int)
    {
      this->constructor = "constructor<int>";
    }

    MyClass2(float)
    {
      this->constructor = "constructor<float>";
    }

    MyClass2(std::string)
    {
      this->constructor = "constructor<string>";
    }

    MyClass2(int, float)
    {
      this->constructor = "constructor<int,float>";
    }

    MyClass2(float, int)
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

  Object result = m.module_eval(code);
  String constructor = result.call("constructor");
  ASSERT_EQUAL("constructor<void>", constructor.str());
}

TESTCASE(constructor_one_parameter)
{
  Module m = define_module("Testing");
  Class c = createMyClass2();

  std::string code = R"(MyClass.new(1))";

  Object result = m.module_eval(code);
  String constructor = result.call("constructor");
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

  Object result = m.module_eval(code);
  String constructor = result.call("constructor");
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
    std::string run(char)
    {
      return "run<char>";
    }

    std::string run(unsigned char)
    {
      return "run<unsigned char>";
    }

    std::string run(char*)
    {
      return "run<char*>";
    }

    std::string run(unsigned char*)
    {
      return "run<unsigned char*>";
    }

    std::string run(short)
    {
      return "run<short>";
    }

    std::string run(int)
    {
      return "run<int>";
    }

    std::string run(float)
    {
      return "run<float>";
    }

    std::string run(double)
    {
      return "run<double>";
    }

    std::string run(long)
    {
      return "run<long>";
    }

    std::string run(long long)
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

#ifdef _WIN32
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

  std::string expected = R"(Could not resolve method call for MyClass3#run
  2 overload(s) were evaluated based on the types of Ruby parameters provided:
     std::string AnonymousNamespace::MyClass3*::run(short)
     std::string AnonymousNamespace::MyClass3*::run(float))";

  ASSERT_EXCEPTION_CHECK(
    Exception,
    result = m.module_eval(code),
    ASSERT_EQUAL(expected.c_str(), ex.what()));
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

#ifdef _WIN32

#if RUBY_API_VERSION_MAJOR == 3 && RUBY_API_VERSION_MINOR >= 4
  const char* expected = "bignum too big to convert into 'long'";
#else
  const char* expected = "bignum too big to convert into `long'";
#endif

#else

#if RUBY_API_VERSION_MAJOR == 3 && RUBY_API_VERSION_MINOR >= 4
  const char* expected = "integer 4398046511104 too big to convert to 'short'";
#else
  const char* expected = "integer 4398046511104 too big to convert to `short'";
#endif

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
  ASSERT_EQUAL("run<unsigned char>", result.str());
}

TESTCASE(int_conversion_6)
{
  Class c = define_class<MyClass3>("MyClass3").
    define_constructor(Constructor<MyClass3>()).
    define_method<std::string(MyClass3::*)(unsigned char)>("run", &MyClass3::run).
    define_method<std::string(MyClass3::*)(unsigned char*)>("run", &MyClass3::run, ArgBuffer("value"));

  Module m = define_module("Testing");

  std::string code = R"(my_class = MyClass3.new
                        buffer = RiceTest::Buffer≺unsigned char≻.new("54")
                        my_class.run(buffer.data))";
  String result = m.module_eval(code);
  ASSERT_EQUAL("run<unsigned char*>", result.str());
}

namespace
{
  class MyClass4
  {
  public:
    static const MyClass4& constRef()
    {
      if (!instance_)
      {
        instance_ = new MyClass4();
      }
      return *instance_;
    }

  private:
    static inline MyClass4* instance_ = nullptr;
  };

  class MyClass5
  {
  public:
    MyClass5(MyClass4&)
    {
      this->result = "non-const ref";
    }

    MyClass5(const MyClass4&)
    {
      this->result = "const ref";
    }

  public:
    std::string result = "";
  };
}

TESTCASE(NonConstRef)
{
  Class c4 = define_class<MyClass4>("MyClass4").
    define_constructor(Constructor<MyClass4>());

  Class c5 = define_class<MyClass5>("MyClass5").
    define_constructor(Constructor<MyClass5, const MyClass4&>()).
    define_constructor(Constructor<MyClass5, MyClass4&>()).
    define_attr("result", &MyClass5::result);

  Module m = define_module("Testing");

  std::string code = R"(my_class4 = MyClass4.new
                        my_class5 = MyClass5.new(my_class4)
                        my_class5.result)";
  String result = m.module_eval(code);
  ASSERT_EQUAL("non-const ref", result.str());
}

TESTCASE(ConstRef)
{
  Class c4 = define_class<MyClass4>("MyClass4").
    define_singleton_function("const_instance", MyClass4::constRef);

  Class c5 = define_class<MyClass5>("MyClass5").
    define_constructor(Constructor<MyClass5, const MyClass4&>()).
    define_constructor(Constructor<MyClass5, MyClass4&>()).
    define_attr("result", &MyClass5::result);

  Module m = define_module("Testing");

  std::string code = R"(my_class4 = MyClass4.const_instance
                        my_class5 = MyClass5.new(my_class4)
                        my_class5.result)";
  String result = m.module_eval(code);
  ASSERT_EQUAL("const ref", result.str());
}

namespace
{
  class MyClass6
  {
  public:
    static const MyClass6* constPointer()
    {
      if (!instance_)
      {
        instance_ = new MyClass6();
      }
      return instance_;
    }

  private:
    static inline MyClass6* instance_ = nullptr;
  };

  class MyClass7
  {
  public:
    MyClass7(MyClass6*)
    {
      this->result = "non-const pointer";
    }

    MyClass7(const MyClass6*)
    {
      this->result = "const pointer";
    }
  public:
    std::string result = "";
  };
}

TESTCASE(NonConstPointer)
{
  Class c6 = define_class<MyClass6>("MyClass6").
    define_constructor(Constructor<MyClass6>());

  Class c7 = define_class<MyClass7>("MyClass7").
    define_constructor(Constructor<MyClass7, const MyClass6*>()).
    define_constructor(Constructor<MyClass7, MyClass6*>()).
    define_attr("result", &MyClass7::result);

  Module m = define_module("Testing");

  std::string code = R"(my_class6 = MyClass6.new
                          my_class7 = MyClass7.new(my_class6)
                          my_class7.result)";
  String result = m.module_eval(code);
  ASSERT_EQUAL("non-const pointer", result.str());
}

TESTCASE(ConstPointer)
{
  Class c6 = define_class<MyClass6>("MyClass6").
    define_singleton_function("const_instance", MyClass6::constPointer);

  Class c7 = define_class<MyClass7>("MyClass7").
    define_constructor(Constructor<MyClass7, const MyClass6*>()).
    define_constructor(Constructor<MyClass7, MyClass6*>()).
    define_attr("result", &MyClass7::result);

  Module m = define_module("Testing");

  std::string code = R"(my_class6 = MyClass6.const_instance
                        my_class7 = MyClass7.new(my_class6)
                        my_class7.result)";
  String result = m.module_eval(code);
  ASSERT_EQUAL("const pointer", result.str());
}

namespace
{
  std::string pointer(const MyClass6*)
  {
    return "pointer";
  }

  std::string pointer(MyClass6*)
  {
    return "pointerBuffer";
  }
}

TESTCASE(PointerNotBuffer)
{
  Module m = define_module("Testing");

  define_class<MyClass6>("MyClass").
    define_constructor(Constructor<MyClass6>());

  m.define_module_function<std::string(*)(const MyClass6*)>("pointer", pointer).
    define_module_function<std::string(*)(MyClass6*)>("pointer", pointer, ArgBuffer("data"));

  std::string code = R"(my_class6 = MyClass6.new
                        pointer(my_class6))";

  String result = m.module_eval(code);
  ASSERT_EQUAL("pointer", result.str());
}

TESTCASE(PointerBuffer)
{
  Module m = define_module("Testing");

  define_class<MyClass6>("MyClass").
    define_constructor(Constructor<MyClass6>());

  m.define_function<std::string(*)(const MyClass6*)>("pointer", pointer).
    define_function<std::string(*)(MyClass6*)>("pointer", pointer, ArgBuffer("data"));

  std::string code = R"(my_class6 = MyClass6.new
                        buffer = RiceTest::Buffer≺AnonymousNamespace꞉꞉MyClass6≻.new(my_class6)
                        pointer(buffer.data))";

  String result = m.module_eval(code);
  ASSERT_EQUAL("pointerBuffer", result.str());
}
