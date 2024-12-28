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

/*TESTCASE(construct_from_exception_object)
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
}*/

namespace
{
  enum class ErrorCode
  {
    MEMORY,
    DISK,
    CPU
  };

  class CustomException
  {
  public:
    CustomException(ErrorCode code) : code(code)
    {
    }

    const char* what() const noexcept
    {
      return "My custom exception occurred!";
    }

    ErrorCode code;
  };

  void raiseCustomException(ErrorCode code)
  {
    throw CustomException(code);
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

Enum<ErrorCode> define_error_code_enum()
{
  static Enum<ErrorCode> errorCodes = define_enum<ErrorCode>("ErrorCode")
    .define_value("MEMORY", ErrorCode::MEMORY)
    .define_value("DISK", ErrorCode::DISK)
    .define_value("CPU", ErrorCode::CPU);
  
  return errorCodes;
}

TESTCASE(default_handler)
{
  define_error_code_enum();
  Module m = define_module("Testing");
  m.define_singleton_function("raise_custom_exception", &raiseCustomException);
  Class MyExceptionClass = define_class_under<CustomException>(rb_cObject, "CustomException", rb_eStandardError).
                           define_method("message", &CustomException::what);

  ASSERT_EXCEPTION_CHECK(
    Exception,
    m.call("raise_custom_exception", ErrorCode::MEMORY),
    ASSERT_EQUAL("Unknown C++ exception thrown", ex.what())
  );
}

TESTCASE(custom_handler_functor)
{
  define_error_code_enum();
  Class MyExceptionClass = define_class_under<CustomException>(rb_cObject, "CustomException", rb_eStandardError).
                           define_method("message", &CustomException::what);

  detail::Registries::instance.handlers.set(MyExceptionHandler());

  Module m = define_module("Testing");
  m.define_singleton_function("raise_custom_exception", &raiseCustomException);

  ASSERT_EXCEPTION_CHECK(
    Exception,
    m.call("raise_custom_exception", ErrorCode::DISK),
    ASSERT_EQUAL("My custom exception occurred!", ex.what())
  );

  std::string code = R"(begin
                          raise_custom_exception(ErrorCode::DISK)
                        rescue CustomException => exception
                          "Caught in Ruby"       
                        end)";

  String string = m.instance_eval(code);
  ASSERT_EQUAL("Caught in Ruby", string.c_str());
}

TESTCASE(custom_handler_lambda)
{
  define_error_code_enum();
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
    m.call("raise_custom_exception", ErrorCode::CPU),
    ASSERT_EQUAL("My custom exception occurred!", ex.what())
  );

  std::string code = R"(begin
                          raise_custom_exception(ErrorCode::CPU)
                        rescue CustomException => exception
                          $!
                        end)";

  Object object = m.instance_eval(code);
  ASSERT_EQUAL(rb_eCustomException.value(), object.class_of().value());
}

TESTCASE(subclasses)
{
  define_error_code_enum();
  Class rb_eCustomException = define_class("CustomException", rb_eStandardError).
                              define_method("message", &CustomException::what);
  Class rb_eMemoryException = define_class_under(rb_cObject, "MemoryException", rb_eCustomException);
  Class rb_eDiskException = define_class_under(rb_cObject, "DiskException", rb_eCustomException);
  Class rb_eCpuException = define_class_under(rb_cObject, "CpuException", rb_eCustomException);

  auto handler = [&]()
    {
      try
      {
        throw;
      }
      catch (const CustomException& exception)
      {
        Class exceptionKlass = rb_eCustomException;
        
        switch (exception.code)
        {
          case ErrorCode::MEMORY:
            exceptionKlass = rb_eMemoryException;
            break;
          case ErrorCode::DISK:
            exceptionKlass = rb_eDiskException;
            break;
          case ErrorCode::CPU:
            exceptionKlass = rb_eCpuException;
            break;
        }

        // Take ownership of the exception object and map it to the right subclass
        Data_Object<CustomException> wrapper(exception, true, exceptionKlass);
        rb_exc_raise(wrapper);
      }
    };

  detail::Registries::instance.handlers.set(handler);

  Module m = define_module("Testing");
  m.define_singleton_function("raise_custom_exception", &raiseCustomException);

  std::string code = R"(begin
                          raise_custom_exception(ErrorCode::MEMORY)
                        rescue => e
                          e.class.name
                        end)";

  String result = m.instance_eval(code);
  ASSERT_EQUAL("MemoryException", result.c_str());

  code = R"(begin
              raise_custom_exception(ErrorCode::MEMORY)
            rescue MemoryException => e
              e.class.name
            end)";

  result = m.instance_eval(code);
  ASSERT_EQUAL("MemoryException", result.c_str());

  code = R"(begin
              raise_custom_exception(ErrorCode::DISK)
            rescue DiskException => e
              e.class.name
            end)";

  result = m.instance_eval(code);
  ASSERT_EQUAL("DiskException", result.c_str());

  code = R"(begin
              raise_custom_exception(ErrorCode::CPU)
            rescue CpuException => e
              e.class.name
            end)";

  result = m.instance_eval(code);
  ASSERT_EQUAL("CpuException", result.c_str());

  code = R"(begin
              raise_custom_exception(ErrorCode::CPU)
            rescue RuntimeError => e
              e.class.name
            end)";

  ASSERT_EXCEPTION_CHECK(
    Exception,
    m.instance_eval(code),
    ASSERT_EQUAL("My custom exception occurred!", ex.what())
  );
}
