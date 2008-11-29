#include "unittest.hpp"
#include "rice/global_function.hpp"

using namespace Rice;

TESTSUITE(GlobalFunction);

SETUP(GlobalFunction)
{
  ruby_init();
}

namespace {

  bool method_to_wrap_called = false;
  void method_to_wrap(Object self) {
    method_to_wrap_called = true;
  }

  int method_with_args_arg0;

  void method_with_args(Object self, int arg) {
    method_with_args_arg0 = arg;
  } 

}

TESTCASE(exposes_global_functions)
{
  define_global_function("method_to_wrap", &method_to_wrap);
  Module m = Module(rb_mKernel);
  m.call("method_to_wrap");

  ASSERT(method_to_wrap_called);
}

TESTCASE(exposes_global_functions_with_arguments)
{
  define_global_function("method_with_args", &method_with_args);
  Module m = Module(rb_mKernel);
  m.call("method_with_args", 10);

  ASSERT_EQUAL(10, method_with_args_arg0);
}

