#include "unittest.hpp"
#include "embed_ruby.hpp"
#include <rice/rice.hpp>

#include <chrono>
#include <thread>

using namespace Rice;

TESTSUITE(GVL);

SETUP(GVL)
{
  embed_ruby();
}

TEARDOWN(GVL)
{
  rb_gc_start();
}

namespace
{
  int slowFunction1()
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return 83;
  }

  void slowFunction2()
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  void functionException()
  {
		throw std::runtime_error("This is an exception from functionException");
  }

  class Task
  {
    public:
			Task(int id) : id_(id)
      {
      }

      int run(bool arg1)
      {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        return this->id_;
		  }

    private:
      int id_;
  };
}

TESTCASE(Function)
{
  int result = detail::no_gvl(&slowFunction1);
  ASSERT_EQUAL(83, result);
}

TESTCASE(FunctionViaRuby)
{
  Module m = define_module("Testing");
  m.define_module_function("slow_function_1", &slowFunction1, NoGVL());
  
  std::string code = R"(slow_function_1)";
  Object result = m.module_eval(code);
  ASSERT_EQUAL(83, detail::From_Ruby<int>().convert(result));
}

TESTCASE(FunctionNoReturn)
{
  detail::no_gvl(&slowFunction2);
  ASSERT_EQUAL(true, true);
}

TESTCASE(FunctionWithException)
{
  ASSERT_EXCEPTION_CHECK(
    std::runtime_error,
    detail::no_gvl(&functionException),
    ASSERT_EQUAL("This is an exception from functionException", ex.what())
  );
}

TESTCASE(MemberFunction)
{
	Task task(84);
  int result = detail::no_gvl(&Task::run, task, false);
  ASSERT_EQUAL(84, result);
}

TESTCASE(MemberFunctionNoGvl)
{
  Module m = define_module("Testing");

  Class task = define_class_under<Task>(m, "Task")
    .define_constructor(Constructor<Task, int>())
    .define_method("run", &Task::run, NoGVL());

  std::string code = R"(task = Task.new(85)
                        task.run(true))";

  Object result = m.module_eval(code);
  ASSERT_EQUAL(85, detail::From_Ruby<int>().convert(result));
}
