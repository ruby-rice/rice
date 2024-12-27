#include "unittest.hpp"
#include "embed_ruby.hpp"
#include <rice/rice.hpp>

using namespace Rice;

TESTSUITE(Exception);

SETUP(Exception)
{
  embed_ruby();
}

TEARDOWN(Exception)
{
  rb_gc_start();
}

TESTCASE(construct_from_exception_object)
{
  VALUE v = detail::protect(rb_exc_new2, rb_eRuntimeError, "foo");
  Exception ex(v);
  ASSERT_EQUAL(ex.value(), v);
}

TESTCASE(copy_construct)
{
  VALUE v = detail::protect(rb_exc_new2, rb_eRuntimeError, "foo");
  Exception ex1(v);
  Exception ex2(v);
  ASSERT_EQUAL(ex2.value(), v);
}

TESTCASE(construct_from_format_string)
{
  Exception ex(rb_eRuntimeError, "%s", "foo");
  ASSERT_EQUAL(rb_eRuntimeError, ex.class_of());
}

TESTCASE(message)
{
  Exception ex(rb_eRuntimeError, "%s", "foo");
  ASSERT_EQUAL("foo", ex.what());
}

TESTCASE(what)
{
  const char* foo = "foo";
  Exception ex(rb_eRuntimeError, "%s", "foo");
  ASSERT_EQUAL(foo, ex.what());
}

namespace
{
  class CustomException
  {
  public:
    const char* what() const noexcept
    {
      return "My custom exception occurred!";
    }
  };

  void raiseCustomException()
  {
    throw CustomException();
  }

  class MyExceptionHandler
  {
  public:
    void operator()()
    {
      try
      {
        throw;
      }
      catch (const CustomException& exception)
      {
        Data_Object<CustomException> wrapper(exception, true);
        rb_exc_raise(wrapper);
      }
    }
  };
}

TESTCASE(default_handler)
{
  Module m = define_module("Testing");
  m.define_singleton_function("raise_custom_exception", &raiseCustomException);
  Class MyExceptionClass = define_class_under<CustomException>(rb_cObject, "CustomException", rb_eStandardError).
                           define_method("message", &CustomException::what);

  ASSERT_EXCEPTION_CHECK(
    Exception,
    m.call("raise_custom_exception"),
    ASSERT_EQUAL("Unknown C++ exception thrown", ex.what())
  );
}

TESTCASE(custom_handler_functor)
{
  Class MyExceptionClass = define_class_under<CustomException>(rb_cObject, "CustomException", rb_eStandardError).
                           define_method("message", &CustomException::what);

  detail::Registries::instance.handlers.set(MyExceptionHandler());

  Module m = define_module("Testing");
  m.define_singleton_function("raise_custom_exception", &raiseCustomException);

  ASSERT_EXCEPTION_CHECK(
    Exception,
    m.call("raise_custom_exception"),
    ASSERT_EQUAL("My custom exception occurred!", ex.what())
  );

  std::string code = R"(begin
                          raise_custom_exception
                        rescue CustomException => exception
                          "Caught in Ruby"       
                        end)";

  String string = m.instance_eval(code);
  ASSERT_EQUAL("Caught in Ruby", string.c_str());
}

TESTCASE(custom_handler_lambda)
{
  Class rb_eCustomException = define_class("CustomException", rb_eStandardError).
                              define_method("message", &CustomException::what);

  auto handler = []()
    {
      try
      {
        throw;
      }
      catch (const CustomException& exception)
      {
        Data_Object<CustomException> wrapper(exception);
        rb_exc_raise(wrapper);
      }
    };

  detail::Registries::instance.handlers.set(handler);

  Module m = define_module("Testing");
  m.define_singleton_function("raise_custom_exception", &raiseCustomException);

  ASSERT_EXCEPTION_CHECK(
    Exception,
    m.call("raise_custom_exception"),
    ASSERT_EQUAL("My custom exception occurred!", ex.what())
  );

  std::string code = R"(begin
                          raise_custom_exception
                        rescue CustomException => exception
                          $!
                        end)";

  Object object = m.instance_eval(code);
  ASSERT_EQUAL(rb_eCustomException.value(), object.class_of().value());
}
