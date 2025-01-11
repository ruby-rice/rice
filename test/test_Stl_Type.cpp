#include "unittest.hpp"
#include "embed_ruby.hpp"

#include <rice/rice.hpp>
#include <rice/stl.hpp>

using namespace Rice;

TESTSUITE(TypeInfo);

SETUP(TypeInfo)
{
  embed_ruby();
}

TEARDOWN(TypeInfo)
{
  rb_gc_start();
}

namespace
{
  const std::type_info& typeInfo(int value)
  {
    return typeid(value);
  }

  const std::string typeInfoName(const std::type_info& value)
  {
    return value.name();
  }

  const std::type_index typeIndex(const std::type_info& value)
  {
    return std::type_index(value);
  }
}

TESTCASE(TypeInfoName)
{
  Module m = define_module("TestingModule");
  m.define_module_function("get_type_info", typeInfo);

  std::string code = R"(int = 5
                        type_info = get_type_info(5)
                        type_info.name)";

  String result = m.module_eval(code);
  ASSERT_EQUAL("int", result.c_str());
}

TESTCASE(TypeInfoHash)
{
  Module m = define_module("TestingModule");
  m.define_module_function("get_type_info", typeInfo);

  std::string code = R"(int = 5
                        type_info = get_type_info(5)
                        type_info.hash)";

  Object result = m.module_eval(code);
  size_t hash = detail::From_Ruby<size_t>().convert(result);
  ASSERT((hash > 0));
}

TESTCASE(TypeInfoParameter)
{
  Module m = define_module("TestingModule");
  m.define_module_function("get_type_info", typeInfo).
    define_module_function("get_type_info_name", typeInfoName);

  std::string code = R"(int = 5
                        type_info = get_type_info(5)
                        get_type_info_name(type_info))";

  String result = m.module_eval(code);
  ASSERT_EQUAL("int", result.c_str());
}

TESTCASE(TypeIndexCreate)
{
  Module m = define_module("TestingModule");
  m.define_module_function("get_type_info", typeInfo).
    define_module_function("get_type_index", typeIndex);

  std::string code = R"(int = 5
                        type_info = get_type_info(5)
                        Rice::Std::TypeIndex.new(type_info))";

  Data_Object<std::type_index> result = m.module_eval(code);
  ASSERT_EQUAL("int", result->name());
}

TESTCASE(TypeIndexName)
{
  Module m = define_module("TestingModule");
  m.define_module_function("get_type_info", typeInfo).
    define_module_function("get_type_index", typeIndex);

  std::string code = R"(int = 5
                        type_info = get_type_info(5)
                        type_index = get_type_index(type_info)
                        type_index.name)";

  String result = m.module_eval(code);
  ASSERT_EQUAL("int", result.str());
}

TESTCASE(TypeIndexHash)
{
  Module m = define_module("TestingModule");
  m.define_module_function("get_type_info", typeInfo).
    define_module_function("get_type_index", typeIndex);

  std::string code = R"(int = 5
                        type_info = get_type_info(5)
                        type_index = get_type_index(type_info)
                        type_index.hash)";

  Object result = m.module_eval(code);
  size_t hash = detail::From_Ruby<size_t>().convert(result);
  ASSERT((hash > 0));
}
