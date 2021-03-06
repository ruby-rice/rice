#include <assert.h> 

#include "unittest.hpp"
#include "embed_ruby.hpp"
#include <rice/rice.hpp>

using namespace Rice;

TESTSUITE(Attribute);

SETUP(Attribute)
{
  embed_ruby();
}

namespace
{
  struct DataStruct
  {
    static inline float staticFloat = 1.0;
    static inline const std::string staticString = "Static string";

    std::string readWriteString = "Read Write";
    int writeInt = 0;
    const char* readChars = "Read some chars!";
  };

  bool globalBool = true;
  const DataStruct* globalStruct = new DataStruct();

} // namespace

TESTCASE(attributes)
{
  Class c = define_class<DataStruct>("DataStruct")
    .define_constructor(Constructor<DataStruct>())
    .define_attr("read_chars", &DataStruct::readChars, Rice::AttrAccess::Read)
    .define_attr("write_int", &DataStruct::writeInt, Rice::AttrAccess::Write)
    .define_attr("read_write_string", &DataStruct::readWriteString);

  Object o = c.call("new");
  DataStruct* dataStruct = detail::From_Ruby<DataStruct*>::convert(o);

  // Test readonly attribute
  Object result = o.call("read_chars");
  ASSERT_EQUAL("Read some chars!", detail::From_Ruby<char*>::convert(result));
  ASSERT_EXCEPTION_CHECK(
    Exception,
    o.call("read_char=", "some text"),
  );
  
  // Test writeonly attribute
  result = o.call("write_int=", 5);
  ASSERT_EQUAL(5, detail::From_Ruby<int>::convert(result.value()));
  ASSERT_EQUAL(5, dataStruct->writeInt);
  ASSERT_EXCEPTION_CHECK(
    Exception,
    o.call("write_int", 3),
    );

  // Test readwrite attribute
  result = o.call("read_write_string=", "Set a string");
  ASSERT_EQUAL("Set a string", detail::From_Ruby<std::string>::convert(result.value()));
  ASSERT_EQUAL("Set a string", dataStruct->readWriteString);

  result = o.call("read_write_string");
  ASSERT_EQUAL("Set a string", detail::From_Ruby<std::string>::convert(result.value()));
}

TESTCASE(static_attributes)
{
  Class c = define_class<DataStruct>("DataStruct")
    .define_constructor(Constructor<DataStruct>())
    .define_singleton_attr("static_float", &DataStruct::staticFloat, Rice::AttrAccess::ReadWrite)
    .define_singleton_attr("static_string", &DataStruct::staticString, Rice::AttrAccess::Read);

  // Test readwrite attribute
  Object result = c.call("static_float=", 7.0);
  ASSERT_EQUAL(7.0, detail::From_Ruby<float>::convert(result.value()));
  ASSERT_EQUAL(7.0, DataStruct::staticFloat);
  result = c.call("static_float");
  ASSERT_EQUAL(7.0, detail::From_Ruby<float>::convert(result.value()));

  result = c.call("static_string");
  ASSERT_EQUAL("Static string", detail::From_Ruby<std::string>::convert(result.value()));
  ASSERT_EXCEPTION_CHECK(
    Exception,
    c.call("static_string=", true),
  );
}

TESTCASE(global_attributes)
{
  Class c = define_class<DataStruct>("DataStruct")
    .define_constructor(Constructor<DataStruct>())
    .define_singleton_attr("global_bool", &globalBool, Rice::AttrAccess::ReadWrite)
    .define_singleton_attr("global_struct", &globalStruct, Rice::AttrAccess::Read);

  Object result = c.call("global_bool=", false);
  ASSERT_EQUAL(Qfalse, result.value());
  ASSERT_EQUAL(false, globalBool);
  result = c.call("global_bool");
  ASSERT_EQUAL(Qfalse, result.value());

  result = c.call("global_struct");
  DataStruct* aStruct = detail::From_Ruby<DataStruct*>::convert(result.value());
  ASSERT_EQUAL(aStruct, globalStruct);
}
