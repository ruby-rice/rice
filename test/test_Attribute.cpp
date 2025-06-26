#include <assert.h>

#include "unittest.hpp"
#include "embed_ruby.hpp"
#include <rice/rice.hpp>
#include <rice/stl.hpp>

using namespace Rice;

TESTSUITE(Attribute);

SETUP(Attribute)
{
  embed_ruby();
}

TEARDOWN(Attribute)
{
  Rice::detail::Registries::instance.types.clearUnverifiedTypes();
  rb_gc_start();
}

namespace
{
  class SomeClass
  {
  };

  class NotAssignable
  {
    NotAssignable& operator=(const NotAssignable&) = delete;
  };

  class NotCopyable
  {
  public:
    NotCopyable() = default;
    NotCopyable(const NotCopyable& other) = delete;
  };

  struct DataStruct
  {
    static inline float staticFloat = 1.0;
    static inline const std::string staticString = "Static string";
    static inline SomeClass someClassStatic;

    std::string readWriteString = "Read Write";
    int writeInt = 0;
    const char* readChars = "Read some chars!";
    const int constInt = 5;
    SomeClass someClass;
    NotAssignable notAssignable;
    NotCopyable notCopyable;

    std::string inspect()
    {
      return "";
    }
  };

  bool globalBool = true;
  const DataStruct* globalStruct = new DataStruct();

  class VecStruct
  {
  public:
    std::vector<double> vector;

    VecStruct(std::vector<double> aVector) : vector(aVector)
    {
    }

    size_t vecSize()
    {
      return this->vector.size();
    }
  };

} // namespace

TESTCASE(attributes)
{
  Class c = define_class<DataStruct>("DataStruct")
    .define_constructor(Constructor<DataStruct>())
    .define_method("inspect", &DataStruct::inspect)
    .define_attr("read_chars", &DataStruct::readChars, Rice::AttrAccess::Read)
    .define_attr("write_int", &DataStruct::writeInt, Rice::AttrAccess::Write)
    .define_attr("read_write_string", &DataStruct::readWriteString);

  Object o = c.call("new");
  DataStruct* dataStruct = detail::From_Ruby<DataStruct*>().convert(o);

  // Test readonly attribute
  Object result = o.call("read_chars");
  ASSERT_EQUAL("Read some chars!", detail::From_Ruby<char*>().convert(result));

  if constexpr (!oldRuby)
  {
    ASSERT_EXCEPTION_CHECK(
      Exception,
      o.call("read_char=", "some text"),
      ASSERT(std::string(ex.what()).find("undefined method `read_char='") == 0)
    );
  }
  // Test writeonly attribute
  result = o.call("write_int=", 5);
  ASSERT_EQUAL(5, detail::From_Ruby<int>().convert(result.value()));
  ASSERT_EQUAL(5, dataStruct->writeInt);

  if constexpr (!oldRuby)
  {
    ASSERT_EXCEPTION_CHECK(
      Exception,
      o.call("write_int", 3),
      ASSERT(std::string(ex.what()).find("undefined method `write_int'") == 0)
    );
  }

  // Test readwrite attribute
  result = o.call("read_write_string=", "Set a string");
  ASSERT_EQUAL("Set a string", detail::From_Ruby<std::string>().convert(result.value()));
  ASSERT_EQUAL("Set a string", dataStruct->readWriteString);

  result = o.call("read_write_string");
  ASSERT_EQUAL("Set a string", detail::From_Ruby<std::string>().convert(result.value()));
}

TESTCASE(vector)
{
  // See https ://github.com/ruby-rice/rice/issues/283
  Module m = define_module("Testing");

  define_class<VecStruct>("VecStruct")
    .define_constructor(Constructor<VecStruct, std::vector<double>>())
    .define_attr("vector", &VecStruct::vector, Rice::AttrAccess::Read)
    .define_method("vector_size", &VecStruct::vecSize);

  std::string code = R"(struct = VecStruct.new([1, 2])
                        # Access the attribute
                        array =  struct.vector.to_a
                        struct.vector_size)";

  Object result = m.module_eval(code);
  ASSERT_EQUAL(2, detail::From_Ruby<int>().convert(result));
}

TESTCASE(const_attribute)
{
  Data_Type<DataStruct> c = define_class<DataStruct>("DataStruct")
    .define_constructor(Constructor<DataStruct>());

  ASSERT_EXCEPTION_CHECK(
    std::exception,
    c.define_attr("const_int", &DataStruct::constInt),
    ASSERT_EQUAL(ex.what(), "Cannot define attribute writer for a const attribute: const_int")
  );

  c.define_attr("const_int", &DataStruct::constInt, AttrAccess::Read);
  Data_Object<DataStruct> o = c.call("new");
    
  if constexpr (!oldRuby)
  {
    ASSERT_EXCEPTION_CHECK(
      Exception,
      o.call("const_int=", 5),
      ASSERT(std::string(ex.what()).find("undefined method `const_int='") == 0)
    );
  }
}

TESTCASE(not_assignable)
{
  Class notAssignableClass = define_class<NotAssignable>("NotAssignable")
    .define_constructor(Constructor<NotAssignable>());
    
  Data_Type<DataStruct> c = define_class<DataStruct>("DataStruct")
    .define_constructor(Constructor<DataStruct>());

  ASSERT_EXCEPTION_CHECK(
    std::exception,
    c.define_attr("not_assignable", &DataStruct::notAssignable),
    ASSERT_EQUAL(ex.what(), "Cannot define attribute writer for a non assignable attribute: not_assignable")
  );

  c.define_attr("not_assignable", &DataStruct::notAssignable, AttrAccess::Read);
  Data_Object<NotAssignable> notAssignable = notAssignableClass.call("new");

  Data_Object<DataStruct> o = c.call("new");
  
  if constexpr (!oldRuby)
  {
    ASSERT_EXCEPTION_CHECK(
      Exception,
      o.call("not_assignable=", notAssignable),
      ASSERT(std::string(ex.what()).find("undefined method `not_assignable='") == 0)
    );
  }
}

TESTCASE(not_copyable)
{
  Class notCopyableClass = define_class<NotCopyable>("NotCopyable")
    .define_constructor(Constructor<NotCopyable>());

  Data_Type<DataStruct> c = define_class<DataStruct>("DataStruct")
    .define_constructor(Constructor<DataStruct>());

  ASSERT_EXCEPTION_CHECK(
    std::exception,
    c.define_attr("not_copyable", &DataStruct::notCopyable),
    ASSERT_EQUAL(ex.what(), "Cannot define attribute writer for a non copy constructible attribute: not_copyable")
  );

  c.define_attr("not_copyable", &DataStruct::notCopyable, AttrAccess::Read);
  Data_Object<NotCopyable> notCopyable = notCopyableClass.call("new");

  Data_Object<DataStruct> o = c.call("new");

  if constexpr (!oldRuby)
  {
    ASSERT_EXCEPTION_CHECK(
      Exception,
      o.call("not_assignable=", notCopyable),
      ASSERT(std::string(ex.what()).find("undefined method `not_copyable='") == 0)
    );
  }
}

TESTCASE(static_attributes)
{
  Class c = define_class<DataStruct>("DataStruct")
    .define_constructor(Constructor<DataStruct>())
    .define_singleton_attr("static_float", &DataStruct::staticFloat, Rice::AttrAccess::ReadWrite)
    .define_singleton_attr("static_string", &DataStruct::staticString, Rice::AttrAccess::Read);

  // Test readwrite attribute
  Object result = c.call("static_float=", 7.0);
  ASSERT_EQUAL(7.0, detail::From_Ruby<float>().convert(result.value()));
  ASSERT_EQUAL(7.0, DataStruct::staticFloat);
  result = c.call("static_float");
  ASSERT_EQUAL(7.0, detail::From_Ruby<float>().convert(result.value()));

  result = c.call("static_string");
  ASSERT_EQUAL("Static string", detail::From_Ruby<std::string>().convert(result.value()));

  if constexpr (!oldRuby)
  {
    ASSERT_EXCEPTION_CHECK(
      Exception,
      c.call("static_string=", true),
      ASSERT(std::string(ex.what()).find("undefined method `static_string='") == 0)
    );
  }
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
  DataStruct* aStruct = detail::From_Ruby<DataStruct*>().convert(result.value());
  ASSERT_EQUAL(aStruct, globalStruct);
}

TESTCASE(not_defined)
{
  Data_Type<DataStruct> c = define_class<DataStruct>("DataStruct");

#ifdef _MSC_VER
  const char* message = "The following types are not registered with Rice:\n  class `anonymous namespace'::SomeClass\n";
#else
  const char* message = "The following types are not registered with Rice:\n  (anonymous namespace)::SomeClass\n";
#endif

  c.define_singleton_attr("some_class_static", &DataStruct::someClassStatic);

  ASSERT_EXCEPTION_CHECK(
    std::invalid_argument,
    Rice::detail::Registries::instance.types.validateTypes(),
    ASSERT_EQUAL(message, ex.what())
  );

#ifdef _MSC_VER
  message = "Type is not registered with Rice: class `anonymous namespace'::SomeClass";
#else
  message = "Type is not registered with Rice: (anonymous namespace)::SomeClass";
#endif

  ASSERT_EXCEPTION_CHECK(
    Rice::Exception,
    c.call("some_class_static"),
    ASSERT_EQUAL(message, ex.what())
  );

  c.define_attr("some_class", &DataStruct::someClass);
  Object o = c.call("new");
  ASSERT_EXCEPTION_CHECK(
    Rice::Exception,
    o.call("some_class"),
    ASSERT_EQUAL(message, ex.what())
  );
}