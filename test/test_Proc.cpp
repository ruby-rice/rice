#include <functional>

#include "unittest.hpp"
#include "embed_ruby.hpp"
#include <rice/rice.hpp>
#include <rice/stl.hpp>

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
  int square(int i)
  {
    return i * i;
  }

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
