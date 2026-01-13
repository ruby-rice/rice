#include "unittest.hpp"
#include "embed_ruby.hpp"
#include <rice/rice.hpp>
#include <rice/stl.hpp>

#include <functional>
#include <string>
#include <vector>
#include <algorithm>

using namespace Rice;

TESTSUITE(StdFunction);

SETUP(StdFunction)
{
  embed_ruby();
}

TEARDOWN(StdFunction)
{
  rb_gc_start();
}

TESTCASE(CreateFunction)
{
  define_stl_function<int(int, int)>("FunctionInt");

  Module m = define_module("TestFunction");

  std::string code = R"(
    proc = Proc.new do |first, second|
      first * second
    end
    Std::FunctionInt.new(proc)
  )";
  Object result = m.module_eval(code);
  ASSERT_EQUAL("Std::FunctionInt", result.class_name().c_str());
}

TESTCASE(CallProc)
{
  define_stl_function<int(int, int)>("FunctionInt");

  Module m = define_module("TestFunction");

  std::string code = R"(
    proc = Proc.new do |first, second|
      first * second
    end
    function = Std::FunctionInt.new(proc)
    function.call(9, 11)
  )";
  Object result = m.module_eval(code);
  ASSERT_EQUAL(99, detail::From_Ruby<int>().convert(result));
}

TESTCASE(CallBlock)
{
  define_stl_function<short(short, short)>();
  Module m = define_module("TestFunction");

  std::string code = R"(
    function = Std::Function≺short❨short‚ short❩≻.new do |first, second|
      first * second
    end
    function.call(8, 2)
  )";
  Object result = m.module_eval(code);
  ASSERT_EQUAL(16, detail::From_Ruby<int>().convert(result));
}

TESTCASE(CallLambda)
{
  define_stl_function<int(int, int)>("FunctionInt");

  Module m = define_module("TestFunction");

  std::string code = R"(
    lamb = lambda do |first, second|
      first * second
    end
    function = Std::FunctionInt.new(lamb)
    function.call(5, 4)
  )";
  Object result = m.module_eval(code);
  ASSERT_EQUAL(20, detail::From_Ruby<int>().convert(result));
}

TESTCASE(CallStabbyLambda)
{
  define_stl_function<int(int, int)>("FunctionInt");

  Module m = define_module("TestFunction");

  std::string code = R"(
    lamb = ->(first, second) {first * second}
    function = Std::FunctionInt.new(lamb)
    function.call(3, 8)
  )";
  Object result = m.module_eval(code);
  ASSERT_EQUAL(24, detail::From_Ruby<int>().convert(result));
}

TESTCASE(Raises)
{
  define_stl_function<short(short, short)>();
  Module m = define_module("TestFunction");

  std::string code = R"(
    function = Std::Function≺short❨short‚ short❩≻.new do |first, second|
      raise("Intentional exception!")
    end
    function.call(8, 2)
  )";

  ASSERT_EXCEPTION_CHECK(
    Exception,
    m.module_eval(code),
    ASSERT_EQUAL("Intentional exception!", ex.what())
  );
}

namespace
{
  std::function<int(int, int)> getAdder()
  {
    return [](int a, int b) { return a + b; };
  }

  int invokeIntFunction(std::function<int(int, int)> func, int a, int b)
  {
    return func(a, b);
  }

  int invokeShortFunction(std::function<short(short, short)> func, short a, short b)
  {
    return func(a, b);
  }
}

TESTCASE(Call)
{
  Module m = define_module("TestFunction");
  m.define_module_function("get_adder", getAdder);

  std::string code = R"(
    func = get_adder
    func.call(7, 6)
  )";

  Object result = m.module_eval(code);
  ASSERT_EQUAL(13, detail::From_Ruby<int>().convert(result));
}

TESTCASE(Roundtrip)
{
  Module m = define_module("TestFunction");
  m.define_module_function("get_adder", getAdder);
  m.define_module_function("invoke", invokeIntFunction);

  std::string code = R"(
    func = get_adder
    invoke(func, 2, 3)
  )";

  Object result = m.module_eval(code);
  ASSERT_EQUAL(5, detail::From_Ruby<int>().convert(result));
}

TESTCASE(RawProc)
{
  Module m = define_module("TestFunctionRaw");
  m.define_module_function("invoke", invokeIntFunction);

  std::string code = R"(
    proc = Proc.new do |first, second|
      first * second
    end
    invoke(proc, 5, 1)
  )";

  ASSERT_EXCEPTION_CHECK(
    Exception,
    m.module_eval(code),
    ASSERT_EQUAL("The Ruby object is a proc or lambda and does not wrap a C++ object", 
                 ex.what())
  );
}

TESTCASE(ReturnVoid)
{
  define_stl_function<void(double)>();

  Module m = define_module("TestFunction");

  std::string code = R"(
    function = Std::Function≺void❨double❩≻.new do |ptr|
      # Do absolutely nothing
    end
    function.call(4.3)
  )";
  Object result = m.module_eval(code);
  ASSERT_EQUAL(Qnil, result.value());
}

namespace
{
  std::function<int(int, int)> throwsException()
  {
    throw std::runtime_error("Intentional exception!");
  }
}

TESTCASE(CallThrows)
{
  Module m = define_module("TestFunction");
  m.define_module_function("throws_exception", throwsException);

  std::string code = R"(
    func = throws_exception
    func.call(7, 6)
  )";

  ASSERT_EXCEPTION_CHECK(
    Exception,
    m.module_eval(code),
    ASSERT_EQUAL("Intentional exception!", ex.what())
  );
}

namespace
{
  const std::function<int(int)> constFunction()
  {
    return [](int a)
      {
        return a;
      };
  }

  int invokeConst(const std::function<int(int)>& func, int value)
  {
    return func(value);
  }
}

TESTCASE(ConstFunctionReference)
{
  Module m = define_module("TestConstFunction");
  m.define_module_function("const_function", constFunction);
  m.define_module_function("invoke_const", invokeConst);

  std::string code = R"(
    func = const_function
    invoke_const(func, 9)
  )";

  Object result = m.module_eval(code);
  ASSERT_EQUAL(9, detail::From_Ruby<int>().convert(result));
}