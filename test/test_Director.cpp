#include "unittest.hpp"
#include "rice/Director.hpp"
#include "rice/Constructor.hpp"
#include "rice/Data_Type.hpp"

#include <vector>

using namespace Rice;

TESTSUITE(Director);

namespace {
  /**
   * Abstract base class
   */
  class Worker {
    public:
      virtual ~Worker() {  }

      int getNumber() { return 12; }

      virtual int doSomething(int num) { return num * 4; }

      virtual int process(int num) = 0; 
  };

  /**
   * Subclass that implements pure virtual
   */
  class MultiplyWorker : public Worker {
    public:
      virtual ~MultiplyWorker() { }

      virtual int process(int num) { return num * 2; }
  };

  /**
   * Class to handle workers
   */
  class Handler {
    std::vector<Worker*> mWorkers;

    public:

      void addWorker(Worker* worker) { mWorkers.push_back(worker); }

      int processWorkers() { 
        std::vector<Worker*>::iterator i = mWorkers.begin();
        int results = 0;

        for(; i != mWorkers.end(); i++) {
          results = (*i)->process(results);
        }

        return results;
      }
  };

  /**
   * Our Director wrapper of Worker
   */
  class WorkerDirector : public Worker, public Rice::Director {
    public:
      WorkerDirector(Object self) : Director(self) { }

      int process(int num) {
        /*
        if(goingUp("process")) {
          rb_raise(rb_eNotImpError, "can't super() into a pure virtual method");
        } else {
          return from_ruby<int>( getSelf().call("process", num) );
        }
        */
        return 3;
      }

      int doSomething(int num) {
        if(callIsFromRuby("do_something")) {
          return this->Worker::doSomething(num);
        } else {
          return from_ruby<int>( getSelf().call("do_something", num) );
        }
      }
  };
};

SETUP(Director)
{
  ruby_init();

  // Always need to tell Rice about the base class so that
  // casting works correctly
  define_class<Worker>("__Worker__");
}

TESTCASE(exposes_worker_as_instantiatable_class) 
{
  define_class<WorkerDirector, Worker>("Worker")
    .define_constructor(Constructor<WorkerDirector, Object>())
    .define_method("get_number", &Worker::getNumber);

  Module m = define_module("Testing");
  Object result = m.instance_eval("worker = Worker.new; worker.get_number");

  ASSERT_EQUAL(12, from_ruby<int>(result.value()));
}

TESTCASE(can_call_virtual_methods_on_base_class)
{
  define_class<WorkerDirector, Worker>("Worker")
    .define_constructor(Constructor<WorkerDirector, Object>())
    .define_method("get_number", &Worker::getNumber)
    .define_method("do_something", &WorkerDirector::doSomething)
    .define_method("process", &WorkerDirector::process);

  Module m = define_module("Testing");

  Object result = m.instance_eval("worker = Worker.new; worker.do_something(4)");

  ASSERT_EQUAL(16, from_ruby<int>(result.value()));
}

// super calls pass execution up the inheritance chain

// super calls on pure virtual raise error

// polymorphic calls head down the call chain

// mix of polymorphic calls and inheritance don't cause infinite loops
