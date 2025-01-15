#include <functional>

#include "unittest.hpp"
#include "embed_ruby.hpp"
#include <rice/rice.hpp>
#include <rice/stl.hpp>

using namespace Rice;

TESTSUITE(Callback);

SETUP(Callback)
{
  embed_ruby();
}

TEARDOWN(Callback)
{
  rb_gc_start();
}

namespace
{
  using Callback_T = std::string(*)(int, double, bool, std::string);
  Callback_T globalCallback;

  void registerCallback(Callback_T callback)
  {
    globalCallback = callback;
  }

  std::string triggerCallback(int anInt, double aDouble, bool aBool, std::string aString)
  {
    if (globalCallback)
    {
      return globalCallback(anInt, aDouble, aBool, aString);
    }
    throw std::runtime_error("Callback has not been registered");
  }
}

TESTCASE(LambdaCallBack)
{
  Module m = define_module("TestingModuleLambda");
  m.define_module_function("register_callback", registerCallback).
    define_module_function("trigger_callback", triggerCallback);

  ASSERT_EQUAL(globalCallback, nullptr);

  std::string code = R"(callback = lambda do |an_int, a_double, a_bool, a_string|
                                      values = [an_int, a_double, a_bool, a_string]
                                      values.map {|value| value.to_s}.join(" - ")
                                   end
                        register_callback(callback))";


  m.module_eval(code);
  ASSERT((globalCallback != nullptr));

  String result = m.call("trigger_callback", 1, 2, true, "hello");
  ASSERT_EQUAL("1 - 2.0 - true - hello", result.c_str());
}

TESTCASE(BlockCallBack)
{
  Module m = define_module("TestingModuleBlock");
  m.define_module_function("register_callback", registerCallback).
    define_module_function("trigger_callback", triggerCallback);

  std::string code = R"(register_callback do |an_int, a_double, a_bool, a_string|
                          values = [an_int, a_double, a_bool, a_string]
                          values.map {|value| value.to_s}.join(" - ")
                        end)";

  m.module_eval(code);

  String result = m.call("trigger_callback", 4, 5.5, false, "Hello block");
  ASSERT_EQUAL("4 - 5.5 - false - Hello block", result.c_str());
}

namespace
{
  int functionArg(int i, int(*f)(int))
  {
    return f(i);
  }
}

TESTCASE(FunctionArg)
{
  Module m = define_module("TestingModuleFunctionArg");
  m.define_module_function("function_arg", functionArg);

  std::string code = R"(function_arg(4) do |i|
                          i * i
                        end)";

  Object result = m.module_eval(code);
  ASSERT_EQUAL(16, detail::From_Ruby<int>().convert(result));
}
