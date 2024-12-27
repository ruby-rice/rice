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
        Data_Object<MyException> wrapped(exception);
        rb_exc_raise(wrapped.value());
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
