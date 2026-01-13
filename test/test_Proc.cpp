#include <functional>

#include "unittest.hpp"
#include "embed_ruby.hpp"
#include <rice/rice.hpp>

using namespace Rice;

TESTSUITE(Proc);

SETUP(Proc)
{
  embed_ruby();
}

TEARDOWN(Proc)
{
  rb_gc_start();
}

namespace
{
  int squareWithBlock(int i, VALUE block)
  {
    // You must include a value parameter at the end that will be set to a proc
    // created from the passed in block. You can actually ingore the proc and yield
    // to the block if you really want too...although not sure that is a good idea
    VALUE result = detail::protect(rb_yield, detail::To_Ruby<int>().convert(i));
    return detail::From_Ruby<int>().convert(result);
  }

  int squareWithProc(int i, VALUE proc)
  {
    VALUE result = detail::protect(rb_funcall, proc, rb_intern("call"), 1, detail::To_Ruby<int>().convert(i));
    return detail::From_Ruby<int>().convert(result);
  }

  int square(int i)
  {
    return i * i;
  }

  // Returns a function pointer that Ruby wraps in NativeProc
  auto squareProc()
  {
    return square;
  }
}

TESTCASE(SquareProc)
{
  Module m = define_module("TestingModuleMakeProc");
  m.define_module_function("square_proc", squareProc);

  std::string code = R"(proc = square_proc
                        proc.call(9))";

  Object result = m.module_eval(code);
  ASSERT_EQUAL(81, detail::From_Ruby<int>().convert(result));
}

TESTCASE(SquareWithBlock)
{
  Module m = define_module("TestingModuleMakeBlock");
  m.define_module_function("square_with_block", squareWithBlock, Arg("i"), Arg("block").setValue());

  std::string code = R"(square_with_block(7) do |i|
                          i * i
                        end)";

  Object result = m.module_eval(code);
  ASSERT_EQUAL(49, detail::From_Ruby<int>().convert(result));
}

TESTCASE(SquareWithCapturedBlock)
{
  Module m = define_module("TestingModuleMakeBlock");
  m.define_module_function("square_with_captured_block", squareWithProc, Arg("i"), Arg("proc").setValue());

  std::string code = R"(square_with_captured_block(4) do |i|
                          i * i
                        end)";

  Object result = m.module_eval(code);
  ASSERT_EQUAL(16, detail::From_Ruby<int>().convert(result));
}

TESTCASE(SquareWithProc)
{
  Module m = define_module("TestingModuleMakeBlock");
  m.define_module_function("square_with_proc", squareWithProc, Arg("i"), Arg("proc").setValue());

  std::string code = R"(proc = Proc.new do |i|
                          i * i
                        end
                        square_with_proc(4, proc))";

  Object result = m.module_eval(code);
  ASSERT_EQUAL(16, detail::From_Ruby<int>().convert(result));
}