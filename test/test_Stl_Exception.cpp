#include <complex>
#include <memory>

#include "unittest.hpp"
#include "embed_ruby.hpp"
#include <rice/rice.hpp>
#include <rice/stl.hpp>

using namespace Rice;

TESTSUITE(StlException);

SETUP(StlException)
{
  embed_ruby();
}

TEARDOWN(StlException)
{
  rb_gc_start();
}

namespace
{
  class MyException: public std::exception
  {
  };

  void raiseMyException()
  {
    throw MyException();
  }

}

TESTCASE(StlExceptionCreate)
{
  Class MyExceptionKlass = define_class<MyException, std::exception>("MyException");

  auto handler = [MyExceptionKlass]()
    {
      try
      {
        throw;
      }
      catch (const MyException& exception)
      {
        Data_Object<MyException> wrapper(exception, true);
        rb_exc_raise(wrapper.value());
      }
    };
  detail::Registries::instance.handlers.set(handler);

  Module m = define_module("Testing");
  m.define_singleton_function("raise_my_exception", &raiseMyException);

  std::string code = R"(begin
                          raise_my_exception
                        rescue MyException => exception
                          $!
                        end)";

  Object object = m.instance_eval(code);
  ASSERT_EQUAL(MyExceptionKlass.value(), object.class_of().value());
}

namespace
{
  std::exception_ptr createExceptionPtr()
  {
    std::exception_ptr eptr;

    try
    {
      [[maybe_unused]]
      char ch = std::string().at(1); // this generates a std::out_of_range
    }
    catch (...)
    {
      eptr = std::current_exception(); // capture
    }

    return eptr;
  }

  void handleExceptionPtr(std::exception_ptr eptr)
  {
    std::rethrow_exception(eptr);
  }
}

TESTCASE(StlExceptionPtr)
{
  Module m = define_module("TestingModule");
  m.define_module_function("create_exception_ptr", createExceptionPtr).
    define_module_function("handle_exception_ptr", handleExceptionPtr);

  Data_Object<std::exception_ptr> exception = m.call("create_exception_ptr");
  VALUE value = exception.value();
  std::exception_ptr* ptr = exception.get();
  ASSERT((value != Qnil));
  ASSERT((ptr != nullptr));

  ASSERT_EXCEPTION_CHECK(
    Exception,
    m.call("handle_exception_ptr", exception),
    ASSERT_EQUAL(rb_eIndexError, ex.class_of())
  );
}
