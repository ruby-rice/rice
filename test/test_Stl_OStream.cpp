#include "unittest.hpp"
#include "embed_ruby.hpp"
#include <rice/rice.hpp>
#include <rice/stl.hpp>

#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>

using namespace Rice;

TESTSUITE(StdOStream);

SETUP(StdOStream)
{
  embed_ruby();
}

TEARDOWN(StdOStream)
{
  rb_gc_start();
}

TESTCASE(CreateOStringStream)
{
  define_ostringstream();

  Module m = define_module("TestOStream");

  std::string code = R"(
    Std::OStringStream.new
  )";
  Object result = m.module_eval(code);
  ASSERT_EQUAL("Std::OStringStream", result.class_name().c_str());
}

TESTCASE(CreateWithString)
{
  define_ostringstream();

  Module m = define_module("TestOStream");

  std::string code = R"(
    stream = Std::OStringStream.new("initial")
    stream.str
  )";
  Object result = m.module_eval(code);
  ASSERT_EQUAL("initial", detail::From_Ruby<std::string>().convert(result));
}

TESTCASE(Write)
{
  define_ostringstream();

  Module m = define_module("TestOStream");

  std::string code = R"(
    stream = Std::OStringStream.new
    stream.write(true)
    stream.write(" and ")
    stream.write(false)
    stream.write(" does not equal ")
    stream.write(11)
    stream.write(" or ")
    stream.write(22.22)
    stream.str
  )";
  Object result = m.module_eval(code);
  ASSERT_EQUAL("true and false does not equal 11 or 22.22", detail::From_Ruby<std::string>().convert(result));
}

TESTCASE(InsertOperator)
{
  define_ostringstream();

  Module m = define_module("TestOStream");

  std::string code = R"(
    stream = Std::OStringStream.new
    stream << "Hello" << " " << 123
    stream.str
  )";
  Object result = m.module_eval(code);
  ASSERT_EQUAL("Hello 123", detail::From_Ruby<std::string>().convert(result));
}

TESTCASE(SetStr)
{
  define_ostringstream();

  Module m = define_module("TestOStream");

  std::string code = R"(
    stream = Std::OStringStream.new
    stream.write("old content")
    stream.str = "new content"
    stream.str
  )";
  Object result = m.module_eval(code);
  ASSERT_EQUAL("new content", detail::From_Ruby<std::string>().convert(result));
}

TESTCASE(ToS)
{
  define_ostringstream();

  Module m = define_module("TestOStream");

  std::string code = R"(
    stream = Std::OStringStream.new
    stream.write("test content")
    stream.to_s
  )";
  Object result = m.module_eval(code);
  ASSERT_EQUAL("test content", detail::From_Ruby<std::string>().convert(result));
}

TESTCASE(StateGood)
{
  define_ostringstream();

  Module m = define_module("TestOStream");

  std::string code = R"(
    stream = Std::OStringStream.new
    stream.good?
  )";
  Object result = m.module_eval(code);
  ASSERT_EQUAL(Qtrue, result.value());
}

TESTCASE(Clear)
{
  define_ostringstream();

  Module m = define_module("TestOStream");

  std::string code = R"(
    stream = Std::OStringStream.new
    stream.clear
    stream.good?
  )";
  Object result = m.module_eval(code);
  ASSERT_EQUAL(Qtrue, result.value());
}

namespace
{
  void writeToStream(std::ostream& os, const std::string& message)
  {
    os << "Message: " << message;
  }

  std::string readFromStringStream(std::ostringstream& oss)
  {
    return oss.str();
  }
}

TESTCASE(PassToFunction)
{
  define_ostringstream();

  Module m = define_module("TestOStream");
  m.define_module_function("write_to_stream", &writeToStream);
  m.define_module_function("read_stream", &readFromStringStream);

  std::string code = R"(
    stream = Std::OStringStream.new
    write_to_stream(stream, "Hello from Ruby!")
    read_stream(stream)
  )";
  Object result = m.module_eval(code);
  ASSERT_EQUAL("Message: Hello from Ruby!", detail::From_Ruby<std::string>().convert(result));
}

TESTCASE(AutomaticRegistration)
{
  // Don't call define_ostringstream() - it should be auto-registered
  Module m = define_module("TestOStreamAuto");
  m.define_module_function("read_stream", &readFromStringStream);

  std::string code = R"(
    stream = Std::OStringStream.new
    stream.write("auto registered")
    read_stream(stream)
  )";
  Object result = m.module_eval(code);
  ASSERT_EQUAL("auto registered", detail::From_Ruby<std::string>().convert(result));
}

TESTCASE(OStreamBaseClass)
{
  define_ostream();

  Module m = define_module("TestOStreamBase");

  std::string code = R"(
    Std::OStream
  )";
  Object result = m.module_eval(code);
  ASSERT(result.is_a(rb_cClass));
}

TESTCASE(OStringStreamInheritsFromOStream)
{
  define_ostringstream();

  Module m = define_module("TestOStreamInherit");

  std::string code = R"(
    Std::OStringStream < Std::OStream
  )";
  Object result = m.module_eval(code);
  ASSERT_EQUAL(Qtrue, result.value());
}

TESTCASE(CoutConstant)
{
  define_ostream();

  Module m = define_module("TestCout");

  std::string code = R"(
    Std::COUT.is_a?(Std::OStream)
  )";
  Object result = m.module_eval(code);
  ASSERT_EQUAL(Qtrue, result.value());
}

TESTCASE(CerrConstant)
{
  define_ostream();

  Module m = define_module("TestCerr");

  std::string code = R"(
    Std::CERR.is_a?(Std::OStream)
  )";
  Object result = m.module_eval(code);
  ASSERT_EQUAL(Qtrue, result.value());
}

TESTCASE(OFStreamCreate)
{
  define_ofstream();

  Module m = define_module("TestOFStream");

  std::string code = R"(
    stream = Std::OFStream.new
    stream.is_a?(Std::OStream)
  )";
  Object result = m.module_eval(code);
  ASSERT_EQUAL(Qtrue, result.value());
}

TESTCASE(OFStreamOpenClose)
{
  define_ofstream();

  Module m = define_module("TestOFStreamOpenClose");

  std::string code = R"(
    stream = Std::OFStream.new
    stream.open?
  )";
  Object result = m.module_eval(code);
  ASSERT_EQUAL(Qfalse, result.value());
}

#ifndef _MSC_VER
TESTCASE(OFStreamWriteFile)
{
  define_ofstream();

  Module m = define_module("TestOFStreamWrite");

  std::string code = R"(
    stream = Std::OFStream.new("test_output.txt")
    stream.write("Hello from Ruby!")
    stream.flush
    stream.close
    stream.open?
  )";
  Object result = m.module_eval(code);
  ASSERT_EQUAL(Qfalse, result.value());

  // Verify file contents
  std::ifstream infile("test_output.txt");
  std::string content;
  std::getline(infile, content);
  ASSERT_EQUAL("Hello from Ruby!", content.c_str());
  infile.close();
  std::remove("test_output.txt");
}
#endif

TESTCASE(OFStreamOpenMethod)
{
  define_ofstream();

  Module m = define_module("TestOFStreamOpen");

  std::string code = R"(
    $stream = Std::OFStream.new
    $stream.open("test_output2.txt")
    $stream.open?
  )";
  Object result = m.module_eval(code);
  ASSERT_EQUAL(Qtrue, result.value());

  // Clean up
  std::string cleanup = R"(
    $stream.close
  )";
  m.module_eval(cleanup);
  std::remove("test_output2.txt");
}

TESTCASE(OFStreamInheritsFromOStream)
{
  define_ofstream();

  Module m = define_module("TestOFStreamInherit");

  std::string code = R"(
    Std::OFStream < Std::OStream
  )";
  Object result = m.module_eval(code);
  ASSERT_EQUAL(Qtrue, result.value());
}
