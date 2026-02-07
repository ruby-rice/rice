#include "unittest.hpp"
#include "embed_ruby.hpp"
#include <rice/rice.hpp>
#include <rice/stl.hpp>
#include <ruby/version.h>

using namespace Rice;

TESTSUITE(Overloads);

namespace
{
  class MyClass;
  class MyClass2;
  class MyClass3;
  class MyClass4;
  class MyClass5;
  class MyClass6;
  class MyClass7;
  class MyClass8;
}

SETUP(Overloads)
{
  embed_ruby();

  Data_Type<MyClass>::unbind();
  Data_Type<MyClass2>::unbind();
  Data_Type<MyClass3>::unbind();
  Data_Type<MyClass4>::unbind();
  Data_Type<MyClass5>::unbind();
  Data_Type<MyClass6>::unbind();
  Data_Type<MyClass7>::unbind();
  Data_Type<MyClass8>::unbind();
  Rice::detail::Registries::instance.types.remove<MyClass>();
  Rice::detail::Registries::instance.types.remove<MyClass2>();
  Rice::detail::Registries::instance.types.remove<MyClass3>();
  Rice::detail::Registries::instance.types.remove<MyClass4>();
  Rice::detail::Registries::instance.types.remove<MyClass5>();
  Rice::detail::Registries::instance.types.remove<MyClass6>();
  Rice::detail::Registries::instance.types.remove<MyClass7>();
  Rice::detail::Registries::instance.types.remove<MyClass8>();
  Rice::detail::Registries::instance.natives.reset(Module(rb_mKernel));
}

TEARDOWN(Overloads)
{
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

  // Signed vs unsigned overloads
  std::string run(unsigned int)
  {
    return "run<unsigned int>";
  }

  std::string run(short)
  {
    return "run<short>";
  }

  std::string run(unsigned short)
  {
    return "run<unsigned short>";
  }

  std::string run(long long)
  {
    return "run<long long>";
  }

  std::string run(unsigned long long)
  {
    return "run<unsigned long long>";
  }

  std::string run(double)
  {
    return "run<double>";
  }
}

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
}

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

  std::string expected = R"(Could not resolve method call for MyClass#run
  6 overload(s) were evaluated based on the types of Ruby parameters provided:
     std::string AnonymousNamespace::MyClass*::run()
     std::string AnonymousNamespace::MyClass*::run(std::string)
     std::string AnonymousNamespace::MyClass*::run(int, float)
     std::string AnonymousNamespace::MyClass*::run(float, int)
     std::string AnonymousNamespace::MyClass*::run(int)
     std::string AnonymousNamespace::MyClass*::run(float))";

  ASSERT_EXCEPTION_CHECK(
    Exception,
    Rice::String result = m.module_eval(code),
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
}

Class createMyClass2()
{
  Class c = define_class<MyClass2>("MyClass2")
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

  std::string code = R"(MyClass2.new)";

  Rice::Object result = m.module_eval(code);
  Rice::String constructor = result.call("constructor");
  ASSERT_EQUAL("constructor<void>", constructor.str());
}

TESTCASE(constructor_one_parameter)
{
  Module m = define_module("Testing");
  Class c = createMyClass2();

  std::string code = R"(MyClass2.new(1))";

  Rice::Object result = m.module_eval(code);
  Rice::String constructor = result.call("constructor");
  ASSERT_EQUAL("constructor<int>", constructor.str());

  code = R"(my_class = MyClass2.new(1.0))";

  result = m.module_eval(code);
  constructor = result.call("constructor");
  ASSERT_EQUAL("constructor<float>", constructor.str());

  code = R"(my_class = MyClass2.new('hello'))";

  result = m.module_eval(code);
  constructor = result.call("constructor");
  ASSERT_EQUAL("constructor<string>", constructor.str());
}

TESTCASE(constructor_two_parameters)
{
  Module m = define_module("Testing");
  Class c = createMyClass2();

  std::string code = R"(MyClass2.new(1, 1.0))";

  Rice::Object result = m.module_eval(code);
  Rice::String constructor = result.call("constructor");
  ASSERT_EQUAL("constructor<int,float>", constructor.str());

  code = R"(my_class = MyClass2.new(1.0, 1))";

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

    std::string run(int&)
    {
      return "run<int&>";
    }

    std::string run(int*)
    {
      return "run<int*>";
    }
  };
}

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

  if constexpr (sizeof(long) == sizeof(int))
  {
    ASSERT_EQUAL("run<long long>", result.str());
  }
  else
  {
    ASSERT_EQUAL("run<long>", result.str());
  }

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

  code = R"(my_class = MyClass3.new
            value = 2**64
            my_class.run(value))";

#ifdef _MSC_VER 
  std::string expected2 = R"(Could not resolve method call for MyClass3#run
  5 overload(s) were evaluated based on the types of Ruby parameters provided:
     std::string AnonymousNamespace::MyClass3*::run(char*)
     std::string AnonymousNamespace::MyClass3*::run(unsigned char*)
     std::string AnonymousNamespace::MyClass3*::run(short)
     std::string AnonymousNamespace::MyClass3*::run(long)
     std::string AnonymousNamespace::MyClass3*::run(__int64))";
#else
  std::string expected2 = R"(Could not resolve method call for MyClass3#run
  5 overload(s) were evaluated based on the types of Ruby parameters provided:
     std::string AnonymousNamespace::MyClass3*::run(char*)
     std::string AnonymousNamespace::MyClass3*::run(unsigned char*)
     std::string AnonymousNamespace::MyClass3*::run(short)
     std::string AnonymousNamespace::MyClass3*::run(long)
     std::string AnonymousNamespace::MyClass3*::run(long long))";
#endif

  ASSERT_EXCEPTION_CHECK(
    Exception,
    result = m.module_eval(code),
    ASSERT_EQUAL(expected2, ex.what()));
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
  ASSERT_EQUAL("run<short>", result.str());

  code = R"(my_class = MyClass3.new
            value = 2**64
            my_class.run(value))";

  result = m.module_eval(code);
  ASSERT_EQUAL("run<float>", result.str());
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

#if (RUBY_API_VERSION_MAJOR == 3 && RUBY_API_VERSION_MINOR >= 4) || RUBY_API_VERSION_MAJOR >= 4
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
                        buffer = Rice::Buffer≺unsigned char≻.new("54")
                        my_class.run(buffer.data))";
  String result = m.module_eval(code);
  ASSERT_EQUAL("run<unsigned char*>", result.str());
}

TESTCASE(int_conversion_7)
{
  Class c = define_class<MyClass3>("MyClass3").
    define_constructor(Constructor<MyClass3>()).
    define_method<std::string(MyClass3::*)(int&)>("run", &MyClass3::run).
    define_method<std::string(MyClass3::*)(int*)>("run", &MyClass3::run);

  Module m = define_module("Testing");

  std::string code = R"(my_class = MyClass3.new
                        buffer = Rice::Buffer≺int≻.new("99")
                        my_class.run(buffer.data))";
  String result = m.module_eval(code);
  ASSERT_EQUAL("run<int*>", result.str());

  code = R"(my_class = MyClass3.new
            ref = Rice::Reference≺int≻.new(99)
            my_class.run(ref))";
  result = m.module_eval(code);
  ASSERT_EQUAL("run<int&>", result.str());
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

  define_class<MyClass6>("MyClass6").
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

  define_class<MyClass6>("MyClass6").
    define_constructor(Constructor<MyClass6>());

  m.define_function<std::string(*)(const MyClass6*)>("pointer", pointer).
    define_function<std::string(*)(MyClass6*)>("pointer", pointer, ArgBuffer("data"));

  std::string code = R"(my_class6 = MyClass6.new
                        buffer = Rice::Buffer≺AnonymousNamespace꞉꞉MyClass6≻.new(my_class6)
                        pointer(buffer.data))";

  String result = m.module_eval(code);
  ASSERT_EQUAL("pointerBuffer", result.str());
}

namespace
{
  class Pixel
  {
  public:
    Pixel(uint32_t rgb)
    {
      this->rgb = rgb;
    }

    Pixel(uint8_t red, uint8_t green, uint8_t blue)
    {
      this->rgb = (red << 16) | (green << 8) | blue;
    }

    uint32_t rgb = 0;
  };
}

TESTCASE(FloatToIntConversionWithOverload)
{
  Module m = define_module("Testing");

  // Define overloaded functions: one takes int, one takes no args
  // There is NO float overload
  m.define_module_function("hello_int_resolve", [](int x) -> int { return x; });
  m.define_module_function("hello_int_resolve", []() -> int { return 0; });

  // Calling with an integer should work
  std::string code = R"(hello_int_resolve(1))";
  Object result = m.module_eval(code);
  ASSERT_EQUAL(1, detail::From_Ruby<int>().convert(result.value()));

  // Calling with no args should work
  code = R"(hello_int_resolve())";
  result = m.module_eval(code);
  ASSERT_EQUAL(0, detail::From_Ruby<int>().convert(result.value()));

  // Calling with a float should convert to int (lossy but allowed)
  code = R"(hello_int_resolve(1.7))";
  result = m.module_eval(code);
  ASSERT_EQUAL(1, detail::From_Ruby<int>().convert(result.value()));
}

TESTCASE(Keywords)
{
  Module m = define_module("Testing");

  define_class<Pixel>("Pixel").
    define_constructor(Constructor<Pixel, uint32_t>()).
    define_constructor(Constructor<Pixel, uint8_t, uint8_t, uint8_t>(), Arg("red"), Arg("green"), Arg("blue")).
    define_attr("rgb", &Pixel::rgb);

  std::string code = R"(pixel = Pixel.new(0xFFFFFF)
                        pixel.rgb)";

  Object result = m.module_eval(code);
  ASSERT_EQUAL(0xFFFFFFull, detail::From_Ruby<size_t>().convert(result.value()));

  code = R"(pixel = Pixel.new(128, 128, 128)
            pixel.rgb)";

  result = m.module_eval(code);
  ASSERT_EQUAL(0x808080ull, detail::From_Ruby<size_t>().convert(result.value()));

  code = R"(pixel = Pixel.new(green: 80, red: 160, blue: 40)
            pixel.rgb)";

  result = m.module_eval(code);
  ASSERT_EQUAL(0xA05028ull, detail::From_Ruby<size_t>().convert(result.value()));
}

// ========== Signed vs Unsigned Preference Tests ==========

// Test: int should be preferred over unsigned int for Ruby Integer
TESTCASE(SignedPreferredOverUnsigned_Int)
{
  Module m = define_module("Testing");

  define_global_function<std::string(*)(int)>("run", &run);
  define_global_function<std::string(*)(unsigned int)>("run", &run);

  std::string code = R"(run(42))";
  String result = m.module_eval(code);
  ASSERT_EQUAL("run<int>", result.str());
}

// Test: short should be preferred over unsigned short for Ruby Integer
TESTCASE(SignedPreferredOverUnsigned_Short)
{
  Module m = define_module("Testing");

  define_global_function<std::string(*)(short)>("run", &run);
  define_global_function<std::string(*)(unsigned short)>("run", &run);

  std::string code = R"(run(42))";
  String result = m.module_eval(code);
  ASSERT_EQUAL("run<short>", result.str());
}

// Test: long long should be preferred over unsigned long long for Ruby Integer
TESTCASE(SignedPreferredOverUnsigned_LongLong)
{
  Module m = define_module("Testing");

  define_global_function<std::string(*)(long long)>("run", &run);
  define_global_function<std::string(*)(unsigned long long)>("run", &run);

  std::string code = R"(run(42))";
  String result = m.module_eval(code);
  ASSERT_EQUAL("run<long long>", result.str());
}

// Test: int should be preferred over float for Ruby Integer
TESTCASE(IntPreferredOverFloat_ForInteger)
{
  Module m = define_module("Testing");

  define_global_function<std::string(*)(int)>("run", &run);
  define_global_function<std::string(*)(float)>("run", &run);

  std::string code = R"(run(42))";
  String result = m.module_eval(code);
  ASSERT_EQUAL("run<int>", result.str());
}

// Test: float should be preferred over int for Ruby Float
TESTCASE(FloatPreferredOverInt_ForFloat)
{
  Module m = define_module("Testing");

  define_global_function<std::string(*)(int)>("run", &run);
  define_global_function<std::string(*)(float)>("run", &run);

  std::string code = R"(run(3.14))";
  String result = m.module_eval(code);
  ASSERT_EQUAL("run<float>", result.str());
}

// Test: unsigned int should be preferred over float for Ruby Integer (integers prefer integer types)
TESTCASE(UnsignedIntPreferredOverFloat_ForInteger)
{
  Module m = define_module("Testing");

  define_global_function<std::string(*)(unsigned int)>("run", &run);
  define_global_function<std::string(*)(float)>("run", &run);

  std::string code = R"(run(42))";
  String result = m.module_eval(code);
  ASSERT_EQUAL("run<unsigned int>", result.str());
}

// Test: double should be preferred over float for Ruby Float
TESTCASE(DoublePreferredOverFloat_ForFloat)
{
  Module m = define_module("Testing");

  define_global_function<std::string(*)(double)>("run", &run);
  define_global_function<std::string(*)(float)>("run", &run);

  std::string code = R"(run(3.14))";
  String result = m.module_eval(code);
  ASSERT_EQUAL("run<double>", result.str());
}

// Test: long long should be preferred over double for Ruby Integer
TESTCASE(LongLongPreferredOverDouble_ForInteger)
{
  Module m = define_module("Testing");

  define_global_function<std::string(*)(long long)>("run", &run);
  define_global_function<std::string(*)(double)>("run", &run);

  std::string code = R"(run(42))";
  String result = m.module_eval(code);
  ASSERT_EQUAL("run<long long>", result.str());
}

// Test: double should be preferred over long long for Ruby Float
TESTCASE(DoublePreferredOverLongLong_ForFloat)
{
  Module m = define_module("Testing");

  define_global_function<std::string(*)(long long)>("run", &run);
  define_global_function<std::string(*)(double)>("run", &run);

  std::string code = R"(run(3.14))";
  String result = m.module_eval(code);
  ASSERT_EQUAL("run<double>", result.str());
}

// Test: unsigned long long should be preferred over double for Ruby Integer (integers prefer integer types)
TESTCASE(UnsignedLongLongPreferredOverDouble_ForInteger)
{
  Module m = define_module("Testing");

  define_global_function<std::string(*)(unsigned long long)>("run", &run);
  define_global_function<std::string(*)(double)>("run", &run);

  std::string code = R"(run(42))";
  String result = m.module_eval(code);
  ASSERT_EQUAL("run<unsigned long long>", result.str());
}

namespace
{
  class MyClass8
  {
  public:
    MyClass8(std::string name): name_(name)
    {
    }

    MyClass8(std::string name, VALUE proc) : name_(name), proc_(proc)
    {
    }

    VALUE execute()
    {
      return detail::protect(rb_funcallv, this->proc_, Identifier("call"), 0, nullptr);
    }

    VALUE execute(VALUE proc)
    {
      return detail::protect(rb_funcallv, proc, rb_intern("call"), 0, nullptr);
    }

    std::string name_;
    VALUE proc_ = Qnil;
  };
}

TESTCASE(ConstructorProc)
{
  Class c = define_class<MyClass8>("MyClass8")
    .define_constructor(Constructor<MyClass8, std::string>())
    .define_constructor(Constructor<MyClass8, std::string, VALUE>(), Arg("name"), Arg("proc").setValue().keepAlive())
    .define_method<VALUE(MyClass8::*)()>("execute", &MyClass8::execute, Return().setValue())
    .define_method<VALUE(MyClass8::*)(VALUE)>("execute", &MyClass8::execute, Return().setValue())
    .define_attr("proc_", &MyClass8::proc_, Rice::AttrAccess::Read);

  Module m = define_module("Testing");

  std::string code = R"(my_proc = Proc.new do
                                    "Constructor Proc"
                                  end
                        myclass = MyClass8.new("proc", my_proc)
                        myclass.execute)";

  String result = m.module_eval(code);
  ASSERT_EQUAL("Constructor Proc", result.c_str());
}

TESTCASE(ConstructorBlock)
{
  Class c = define_class<MyClass8>("MyClass8")
    .define_constructor(Constructor<MyClass8, std::string>())
    .define_constructor(Constructor<MyClass8, std::string, VALUE>(), Arg("name"), Arg("proc").setValue().keepAlive())
    .define_method<VALUE(MyClass8::*)()>("execute", &MyClass8::execute, Return().setValue())
    .define_method<VALUE(MyClass8::*)(VALUE)>("execute", &MyClass8::execute, Arg("proc").setValue(), Return().setValue());

  Module m = define_module("Testing");

  std::string code = R"(myclass = MyClass8.new("block") do
                                    "Constructor Block"
                                  end
                        myclass.execute)";

  String result = m.module_eval(code);
  ASSERT_EQUAL("Constructor Block", result.c_str());
}

TESTCASE(ConstructorBlockKeywords)
{
  Class c = define_class<MyClass8>("MyClass8")
    .define_constructor(Constructor<MyClass8, std::string>())
    .define_constructor(Constructor<MyClass8, std::string, VALUE>(), Arg("name"), Arg("proc").setValue().keepAlive())
    .define_method<VALUE(MyClass8::*)()>("execute", &MyClass8::execute, Return().setValue())
    .define_method<VALUE(MyClass8::*)(VALUE)>("execute", &MyClass8::execute, Arg("proc").setValue(), Return().setValue());

  Module m = define_module("Testing");

  std::string code = R"(myclass = MyClass8.new(name: "blockkw") do
                                    "Constructor Block Keywords"
                                  end
                        myclass.execute)";

  String result = m.module_eval(code);
  ASSERT_EQUAL("Constructor Block Keywords", result.c_str());
}

TESTCASE(MethodBlock)
{
  Class c = define_class<MyClass8>("MyClass8")
    .define_constructor(Constructor<MyClass8, std::string>())
    .define_constructor(Constructor<MyClass8, std::string, VALUE>(), Arg("name"), Arg("proc").setValue().keepAlive())
    .define_method<VALUE(MyClass8::*)()>("execute", &MyClass8::execute, Return().setValue())
    .define_method<VALUE(MyClass8::*)(VALUE)>("execute", &MyClass8::execute, Arg("proc").setValue(), Return().setValue());

  Module m = define_module("Testing");

  std::string code = R"(myclass = MyClass8.new("MethodBlock")
                        myclass.execute do
                          "Method Block"
                        end)";

  String result = m.module_eval(code);
  ASSERT_EQUAL("Method Block", result.c_str());
}