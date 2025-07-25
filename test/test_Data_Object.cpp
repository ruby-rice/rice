#include "unittest.hpp"
#include "embed_ruby.hpp"
#include <rice/rice.hpp>

using namespace Rice;

TESTSUITE(Data_Object);

namespace
{
  bool test_destructor_called = false;
  bool test_ruby_mark_called = false;

  struct MyDataType
  {
    MyDataType(int value) : x(value)
    {
    }

    ~MyDataType()
    {
      test_destructor_called = true;
    }
    
    int x;
  };

  static MyDataType myDataTypes[] = { 1,2,3 };
  MyDataType* dataTypes()
  {
    return myDataTypes;
  }

  int dataTypesCount()
  {
    return sizeof(myDataTypes)/sizeof(MyDataType);
  }

  struct Bar
  {
  };
}

namespace Rice
{
  template<>
  void ruby_mark(MyDataType* myDataType)
  {
    test_ruby_mark_called = true;
  }
}

SETUP(Data_Object)
{
  embed_ruby();

  if (!Data_Type<MyDataType>::is_bound())
  {
    Class object(rb_cObject);
    if (object.const_defined("MyDataType"))
    {
      object.remove_const("MyDataType");
    }

    define_class<MyDataType>("MyDataType");
    define_class<Bar>("Bar");
  }
}

TEARDOWN(Data_Object)
{
  rb_gc_start();
}

TESTCASE(data_object_construct_from_pointer)
{
  MyDataType* myDataType = new MyDataType(42);
  Data_Object<MyDataType> wrapped_foo(myDataType);
  ASSERT_EQUAL(myDataType, wrapped_foo.get());
  ASSERT_EQUAL(Data_Type<MyDataType>::klass(), wrapped_foo.class_of());
  ASSERT_EQUAL(myDataType, detail::unwrap<MyDataType>(wrapped_foo, Data_Type<MyDataType>::ruby_data_type(), false));
}

TESTCASE(data_object_construct_from_ruby_object)
{
  MyDataType * myDataType = new MyDataType(42);
  VALUE wrapped_foo = detail::wrap(Data_Type<MyDataType>::klass(), Data_Type<MyDataType>::ruby_data_type(), myDataType, true);

  Data_Object<MyDataType> data_object_foo(wrapped_foo);
  ASSERT_EQUAL(myDataType, data_object_foo.get());
  ASSERT_EQUAL(Data_Type<MyDataType>::klass(), data_object_foo.class_of());
  ASSERT_EQUAL(RTYPEDDATA(wrapped_foo), RTYPEDDATA(data_object_foo.value()));
  ASSERT_EQUAL(myDataType, detail::unwrap<MyDataType>(wrapped_foo, Data_Type<MyDataType>::ruby_data_type(), false));
}

TESTCASE(data_object_construct_from_ruby_object_and_wrong_class)
{
  MyDataType * myDataType = new MyDataType(42);
  VALUE wrapped_foo = detail::wrap(Data_Type<MyDataType>::klass(), Data_Type<MyDataType>::ruby_data_type(), myDataType, true);

  ASSERT_EXCEPTION_CHECK(
    Exception,
    Data_Object<Bar> bar(wrapped_foo),
    ASSERT_EQUAL(rb_eTypeError, CLASS_OF(ex.value())));

  ASSERT_EXCEPTION_CHECK(
    Exception,
    Data_Object<Bar> bar(wrapped_foo),
    ASSERT_EQUAL("Wrong argument type. Expected: Bar. Received: MyDataType.", ex.what()));
}

TESTCASE(data_object_copy_construct)
{
  MyDataType * myDataType = new MyDataType(42);
  VALUE wrapped_foo = detail::wrap(Data_Type<MyDataType>::klass(), Data_Type<MyDataType>::ruby_data_type(), myDataType, true);
  Data_Object<MyDataType> orig_data_object_foo(wrapped_foo);
  Data_Object<MyDataType> data_object_foo(orig_data_object_foo);

  ASSERT_EQUAL(myDataType, data_object_foo.get());
  ASSERT_EQUAL(Data_Type<MyDataType>::klass(), data_object_foo.class_of());
  ASSERT_EQUAL(RTYPEDDATA(wrapped_foo), RTYPEDDATA(data_object_foo.value()));
  ASSERT_EQUAL(myDataType, detail::unwrap<MyDataType>(wrapped_foo, Data_Type<MyDataType>::ruby_data_type(), false));
}

TESTCASE(data_object_move_construct)
{
  MyDataType* myDataType = new MyDataType(42);

  Data_Object<MyDataType> wrapper1(myDataType);
  Data_Object<MyDataType> wrapper2(std::move(wrapper1));

  ASSERT_EQUAL(myDataType, wrapper2.get());
  ASSERT((wrapper1.get() == nullptr));
}

TESTCASE(data_object_move_assign)
{
  MyDataType* foo1 = new MyDataType(42);
  Data_Object<MyDataType> wrapper1(foo1);

  MyDataType* foo2 = new MyDataType(42);
  Data_Object<MyDataType> wrapper2(foo2);

  wrapper2 = std::move(wrapper1);

  ASSERT_EQUAL(foo1, wrapper2.get());
  ASSERT((wrapper1.get() == nullptr));
}

TESTCASE(data_object_dereference)
{
  Data_Type<MyDataType> rb_cFoo;
  MyDataType * myDataType = new MyDataType(42);
  Data_Object<MyDataType> wrapped_foo(myDataType);
  ASSERT_EQUAL(myDataType, &*wrapped_foo);
}

TESTCASE(data_object_arrow)
{
  Data_Type<MyDataType> rb_cFoo;
  MyDataType * myDataType = new MyDataType(42);
  Data_Object<MyDataType> wrapped_foo(myDataType);
  ASSERT_EQUAL(42, myDataType->x);
}

TESTCASE(data_object_get)
{
  Data_Type<MyDataType> rb_cFoo;
  MyDataType * myDataType = new MyDataType(42);
  Data_Object<MyDataType> wrapped_foo(myDataType);
  ASSERT_EQUAL(myDataType, wrapped_foo.get());
}

TESTCASE(data_object_to_ruby)
{
  Data_Type<MyDataType> rb_cFoo;
  MyDataType * myDataType = new MyDataType(42);
  Data_Object<MyDataType> wrapped_foo(myDataType);
  ASSERT_EQUAL(String("MyDataType"), wrapped_foo.class_name());
}

TESTCASE(data_object_from_ruby)
{
  Data_Type<MyDataType> rb_cFoo;
  MyDataType* myDataType = new MyDataType(42);
  Data_Object<MyDataType> wrapped_foo(myDataType);
  ASSERT_EQUAL(myDataType, wrapped_foo.get());
}

TESTCASE(data_object_from_ruby_const_ref)
{
  Data_Type<MyDataType> rb_cFoo;
  MyDataType * myDataType = new MyDataType(42);
  Data_Object<MyDataType> wrapped_foo(myDataType);
  ASSERT_EQUAL(myDataType->x, detail::From_Ruby<MyDataType const &>().convert(wrapped_foo).x);
}

TESTCASE(data_object_from_ruby_copy)
{
  Data_Type<MyDataType> rb_cFoo;
  MyDataType * myDataType = new MyDataType(42);
  Data_Object<MyDataType> wrapped_foo(myDataType);
  ASSERT_EQUAL(myDataType->x, detail::From_Ruby<MyDataType>().convert(wrapped_foo).x);
}

TESTCASE(data_object_return_array)
{
  define_buffer<MyDataType>();

  Module m = define_module("DataObjectTest").
    define_module_function("data_types", &dataTypes, Return().setArray()).
    define_module_function("data_types_count", &dataTypesCount);

  std::string code = R"(buffer = data_types
                        count = data_types_count
                        buffer.to_ary(count))";

  Array dataTypes = m.module_eval(code);
  ASSERT_EQUAL(3, dataTypes.size());

  std::vector<MyDataType> vector = dataTypes.to_vector<MyDataType>();
  ASSERT_EQUAL(1, vector[0].x);
  ASSERT_EQUAL(2, vector[1].x);
  ASSERT_EQUAL(3, vector[2].x);
}

TESTCASE(data_object_update_buffer)
{
  define_buffer<MyDataType>();

  Class c = define_class<MyDataType>("MyDataType")
    .define_constructor(Constructor<MyDataType, int>());

  Module m = define_module("DataObjectTest").
    define_module_function("data_types", &dataTypes, Return().setArray()).
    define_module_function("data_types_count", &dataTypesCount);

  std::string code = R"(buffer = data_types
                        my_data_type = MyDataType.new(100)
                        buffer.size = 3
                        buffer[2] = my_data_type
                        buffer)";

  Object result = m.module_eval(code);
  Data_Object<Buffer<MyDataType>> dataObject(result);
  Buffer<MyDataType>* buffer = dataObject.get();
  MyDataType myDataType = buffer->operator[](2);
  ASSERT_EQUAL(100, myDataType.x);
}

TESTCASE(data_object_ruby_custom_mark)
{
  test_ruby_mark_called = false;

  MyDataType* myDataType = new MyDataType(42);
  Data_Object<MyDataType> wrapped_foo(myDataType);
  rb_gc_start();

  ASSERT_EQUAL(true, test_ruby_mark_called);
}

TESTCASE(data_object_ruby_custom_free)
{
  test_ruby_mark_called = false;
  test_destructor_called = false;

  {
    // Put this code in a block so wrapped_foo is destroyed at the end of it.
    // That will set its value field to Qnil allowing myDataType to be freed
    MyDataType* myDataType = new MyDataType(42);
    Data_Object<MyDataType> wrapped_foo(myDataType, true);

    // Force a mark
    rb_gc_start();
    ASSERT_EQUAL(true, test_ruby_mark_called);
  }

  // Force a free
  rb_gc_start();

  // Some versions of Ruby's and compilers think the Ruby value in wrapped_foo is still
  // alive. Thus the rb_gc_start call results in a mark and not a free
#if defined(__MINGW64__) && RUBY_API_VERSION_MAJOR == 3 && RUBY_API_VERSION_MINOR == 2
  // do nothing
#elif defined(__APPLE__) && RUBY_API_VERSION_MAJOR == 2 && RUBY_API_VERSION_MINOR == 7
  // do nothing
#else
//  ASSERT_EQUAL(true, test_destructor_called);
#endif
}
