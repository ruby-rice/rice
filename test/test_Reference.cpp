#include "unittest.hpp"
#include "embed_ruby.hpp"
#include <rice/rice.hpp>

using namespace Rice;

TESTSUITE(Reference);

SETUP(Reference)
{
  embed_ruby();
}

TEARDOWN(Reference)
{
  rb_gc_start();
}

namespace
{
  void updateIntRef(int& ref)
  {
    ref = 42;
  }

  int readIntRef(int& ref)
  {
    return ref;
  }

  double readDoubleRef(double& ref)
  {
    return ref;
  }

  bool readBoolRef(bool& ref)
  {
    return ref;
  }

  char readCharRef(char& ref)
  {
    return ref;
  }

  long readLongRef(long& ref)
  {
    return ref;
  }

  long long readLongLongRef(long long& ref)
  {
    return ref;
  }

  unsigned long long readUnsignedLongLongRef(unsigned long long& ref)
  {
    return ref;
  }
}

TESTCASE(pass_fundamental_to_reference)
{
  Module m = define_module("Testing");
  m.define_module_function("read_int_ref", &readIntRef);

  std::string code = R"(read_int_ref(7))";
  Object result = m.module_eval(code);
  ASSERT_EQUAL(7, detail::From_Ruby<int>().convert(result));
}

TESTCASE(pass_reference_object)
{
  Module m = define_module("Testing");
  m.define_module_function("read_int_ref", &readIntRef);

  std::string code = R"(ref = Rice::Reference≺int≻.new(7)
                        read_int_ref(ref))";

  Object result = m.module_eval(code);
  ASSERT_EQUAL(7, detail::From_Ruby<int>().convert(result));
}

TESTCASE(modify_reference_object)
{
  Module m = define_module("Testing");
  m.define_module_function("update_int_ref", &updateIntRef);

  std::string code = R"(ref = Rice::Reference≺int≻.new(7)
                        update_int_ref(ref)
                        ref.value)";

  Object result = m.module_eval(code);
  ASSERT_EQUAL(42, detail::From_Ruby<int>().convert(result));
}

TESTCASE(double_reference)
{
  Module m = define_module("Testing");
  m.define_module_function("read_double_ref", &readDoubleRef);

  std::string code = R"(read_double_ref(2.5))";
  Object result = m.module_eval(code);
  ASSERT_EQUAL(2.5, detail::From_Ruby<double>().convert(result));
}

TESTCASE(bool_reference)
{
  Module m = define_module("Testing");
  m.define_module_function("read_bool_ref", &readBoolRef);

  std::string code = R"(read_bool_ref(true))";
  Object result = m.module_eval(code);
  ASSERT_EQUAL(Qtrue, result.value());

  code = R"(read_bool_ref(false))";
  result = m.module_eval(code);
  ASSERT_EQUAL(Qfalse, result.value());
}

TESTCASE(reference_preserves_value)
{
  Module m = define_module("Testing");
  m.define_module_function("read_int_ref", &readIntRef);

  std::string code = R"(read_int_ref(0))";
  Object result = m.module_eval(code);
  ASSERT_EQUAL(0, detail::From_Ruby<int>().convert(result));

  code = R"(read_int_ref(-1))";
  result = m.module_eval(code);
  ASSERT_EQUAL(-1, detail::From_Ruby<int>().convert(result));

  code = R"(read_int_ref(1000000))";
  result = m.module_eval(code);
  ASSERT_EQUAL(1000000, detail::From_Ruby<int>().convert(result));
}

TESTCASE(char_reference)
{
  Module m = define_module("Testing");
  m.define_module_function("read_char_ref", &readCharRef);

  std::string code = R"(read_char_ref(65))";
  Object result = m.module_eval(code);
  ASSERT_EQUAL('A', detail::From_Ruby<char>().convert(result));
}

TESTCASE(long_reference)
{
  Module m = define_module("Testing");
  m.define_module_function("read_long_ref", &readLongRef);

  std::string code = R"(read_long_ref(123456789))";
  Object result = m.module_eval(code);
  ASSERT_EQUAL(123456789, detail::From_Ruby<long>().convert(result));
}

TESTCASE(long_long_reference)
{
  Module m = define_module("Testing");
  m.define_module_function("read_long_long_ref", &readLongLongRef);

  std::string code = R"(read_long_long_ref(9223372036854775807))";
  Object result = m.module_eval(code);
  ASSERT_EQUAL(9223372036854775807LL, detail::From_Ruby<long long>().convert(result));
}

TESTCASE(unsigned_long_long_reference)
{
  // Special case: VALUE is unsigned long long on Windows x64, so we can't
  // use Reference<unsigned long long>. Instead, the old pattern is used
  // which stores the converted value directly.
  Module m = define_module("Testing");
  m.define_module_function("read_unsigned_long_long_ref", &readUnsignedLongLongRef);

  std::string code = R"(read_unsigned_long_long_ref(73))";
  Object result = m.module_eval(code);
  ASSERT_EQUAL(73ULL, detail::From_Ruby<unsigned long long>().convert(result));
}

