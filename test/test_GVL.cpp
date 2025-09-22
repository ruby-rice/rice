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

TESTCASE(FunctionNoReturn)
{
  detail::no_gvl(&slowFunction2);
  ASSERT_EQUAL(true, true);
}

TESTCASE(MemberFunction)
{
	Task task(84);
  int result = detail::no_gvl(&Task::run, task, false);
  ASSERT_EQUAL(84, result);
}
