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
/*
namespace
{
  using Callback_T = char*(*)(int, double, bool, char*);
  Callback_T globalCallback;

  void registerCallback(char*(*callback)(int, double, bool, char*))
  {
    globalCallback = callback;
  }

  char* triggerCallback(int anInt, double aDouble, bool aBool, char* aString)
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

TESTCASE(ProcCallBack)
{
  Module m = define_module("TestingModuleBlock");
  m.define_module_function("register_callback", registerCallback).
    define_module_function("trigger_callback", triggerCallback);

  std::string code = R"(callback = Proc.new do |an_int, a_double, a_bool, a_string|
                                      values = [an_int, a_double, a_bool, a_string]
                                      values.map {|value| value.to_s}.join(" - ")
                                   end
                        register_callback(callback))";

  m.module_eval(code);

  String result = m.call("trigger_callback", 8, 4.4, true, "Hello proc");
  ASSERT_EQUAL("8 - 4.4 - true - Hello proc", result.c_str());
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
}*/

namespace
{
  using Callback_T2 = char*(*)();
  std::vector<Callback_T2> callbacks;

  void registerCallback(Callback_T2 callback1, Callback_T2 callback2)
  {
    callbacks.push_back(callback1);
    callbacks.push_back(callback2);
  }

  char* triggerCallback(int callBackIndex)
  {
    Callback_T2 callback = callbacks[callBackIndex];
    return callback();
  }
}

TESTCASE(MultipleCallbacks)
{
  Module m = define_module("TestingModuleMultipleCallbacks");
  m.define_module_function<void(*)(Callback_T2, Callback_T2)>("register_callback", registerCallback).
    define_module_function<char*(*)(int)>("trigger_callback", triggerCallback);

  std::string code = R"(proc1 = Proc.new do 
                                  STDOUT << 11111111111111 << "\n"  
                                  return "Proc 1"
                                end

                        proc2 = Proc.new do 
                                  STDOUT << 22222222 << "\n"  
                                  return "Proc 2"
                                end

                        #proc1 = lambda {"L1"}
                        #proc2 = lambda {"L2"}
                        register_callback(proc1, proc2))";

  m.module_eval(code);

  std::cout << "aaaaaaaaaaaaa\n";
  String result = m.call("trigger_callback", 0);
  ASSERT_EQUAL("L1", result.c_str());

  std::cout << "bbbbbbbbbbbbbb\n";
  result = m.call("trigger_callback", 1);
  ASSERT_EQUAL("L2", result.c_str());
}
