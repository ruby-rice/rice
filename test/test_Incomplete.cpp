#include "unittest.hpp"
#include "embed_ruby.hpp"
#include <rice/rice.hpp>

using namespace Rice;

TESTSUITE(IncompleteType);

SETUP(IncompleteType)
{
  embed_ruby();
}

TEARDOWN(IncompleteType)
{
  rb_gc_start();
}

// =======================================================================
// PIMPL Idiom
//
// The implementation class is forward-declared only - never defined in
// this file. This simulates a real PIMPL scenario where the impl is
// defined in a separate .cpp file that users never see.
// =======================================================================

namespace
{
  // Forward declaration only - WidgetImpl is never defined
  class WidgetImpl;

  struct WidgetImplReal
  {
    int value = 42;
  };

  class Widget
  {
  public:
    Widget() : pImpl_(reinterpret_cast<WidgetImpl*>(new WidgetImplReal()))
    {
    }

    ~Widget()
    {
      delete reinterpret_cast<WidgetImplReal*>(pImpl_);
    }

    void doSomething()
    {
      reinterpret_cast<WidgetImplReal*>(pImpl_)->value *= 2;
    }

    int getValue() const
    {
      return reinterpret_cast<WidgetImplReal*>(pImpl_)->value;
    }

    // Returns pointer to incomplete type
    WidgetImpl* getImpl()
    {
      return pImpl_;
    }

    // Takes pointer to incomplete type
    void setImpl(WidgetImpl* impl)
    {
      delete reinterpret_cast<WidgetImplReal*>(pImpl_);
      pImpl_ = impl;
    }

  private:
    WidgetImpl* pImpl_;
  };

  // Free functions that work with incomplete WidgetImpl
  int getImplValue(WidgetImpl* impl)
  {
    return reinterpret_cast<WidgetImplReal*>(impl)->value;
  }

  void setImplValue(WidgetImpl* impl, int value)
  {
    reinterpret_cast<WidgetImplReal*>(impl)->value = value;
  }
}

TESTCASE(PimplBasic)
{
  // Basic PIMPL usage - wrap the class without exposing the incomplete impl type
  define_class<Widget>("Widget")
    .define_constructor(Constructor<Widget>())
    .define_method("do_something", &Widget::doSomething)
    .define_method("get_value", &Widget::getValue);

  Module m = define_module("TestingModule");

  std::string code = R"(widget = Widget.new
                        value1 = widget.get_value
                        widget.do_something
                        value2 = widget.get_value
                        [value1, value2])";

  Array result = m.module_eval(code);

  ASSERT_EQUAL(42, detail::From_Ruby<int>().convert(result[0].value()));
  ASSERT_EQUAL(84, detail::From_Ruby<int>().convert(result[1].value()));
}

TESTCASE(PimplReturnIncomplete)
{
  // Register the incomplete type - Rice just needs to know it exists
  define_class<WidgetImpl>("WidgetImpl");

  // Test returning pointer to incomplete type WidgetImpl
  define_class<Widget>("Widget")
    .define_constructor(Constructor<Widget>())
    .define_method("do_something", &Widget::doSomething)
    .define_method("get_value", &Widget::getValue)
    .define_method("get_impl", &Widget::getImpl);

  define_global_function("get_impl_value", &getImplValue);

  Module m = define_module("TestingModule");

  std::string code = R"(widget = Widget.new
                        impl = widget.get_impl
                        get_impl_value(impl))";

  Object result = m.module_eval(code);
  ASSERT_EQUAL(42, detail::From_Ruby<int>().convert(result.value()));
}

TESTCASE(PimplPassIncomplete)
{
  // Register the incomplete type - Rice just needs to know it exists
  define_class<WidgetImpl>("WidgetImpl2");

  // Test passing pointer to incomplete type WidgetImpl between functions
  define_class<Widget>("Widget")
    .define_constructor(Constructor<Widget>())
    .define_method("get_value", &Widget::getValue)
    .define_method("get_impl", &Widget::getImpl);

  define_global_function("get_impl_value", &getImplValue);
  define_global_function("set_impl_value", &setImplValue);

  Module m = define_module("TestingModule");

  std::string code = R"(widget = Widget.new
                        impl = widget.get_impl
                        set_impl_value(impl, 999)
                        widget.get_value)";

  Object result = m.module_eval(code);
  ASSERT_EQUAL(999, detail::From_Ruby<int>().convert(result.value()));
}

// =======================================================================
// Opaque Handle Idiom
//
// Common in C libraries where a handle is passed around but its
// internal structure is never exposed. The struct is never defined.
// =======================================================================

namespace
{
  // Forward declaration only - OpaqueHandle is never defined
  struct OpaqueHandle;

  struct OpaqueHandleReal
  {
    int data = 100;
  };

  OpaqueHandle* createHandle()
  {
    return reinterpret_cast<OpaqueHandle*>(new OpaqueHandleReal());
  }

  void destroyHandle(OpaqueHandle* handle)
  {
    delete reinterpret_cast<OpaqueHandleReal*>(handle);
  }

  int getHandleData(OpaqueHandle* handle)
  {
    return reinterpret_cast<OpaqueHandleReal*>(handle)->data;
  }

  void setHandleData(OpaqueHandle* handle, int value)
  {
    reinterpret_cast<OpaqueHandleReal*>(handle)->data = value;
  }

  // Wrapper class that manages the opaque handle lifetime
  class HandleWrapper
  {
  public:
    HandleWrapper() : handle_(createHandle())
    {
    }

    ~HandleWrapper()
    {
      destroyHandle(handle_);
    }

    int getData() const
    {
      return getHandleData(handle_);
    }

    void setData(int value)
    {
      setHandleData(handle_, value);
    }

    // Exposes the incomplete type
    OpaqueHandle* getRawHandle()
    {
      return handle_;
    }

    void setRawHandle(OpaqueHandle* handle)
    {
      handle_ = handle;
    }

  private:
    OpaqueHandle* handle_;
  };

  // Free functions for Ruby bindings
  OpaqueHandle* createOpaqueHandle()
  {
    return createHandle();
  }

  void destroyOpaqueHandle(OpaqueHandle* handle)
  {
    destroyHandle(handle);
  }

  int getOpaqueHandleData(OpaqueHandle* handle)
  {
    return getHandleData(handle);
  }

  void setOpaqueHandleData(OpaqueHandle* handle, int value)
  {
    setHandleData(handle, value);
  }
}

TESTCASE(OpaqueHandleNotDefined)
{
  // Test that using an incomplete type without registering it throws an exception
  define_global_function("create_opaque_handle_unregistered", &createOpaqueHandle);

  Module m = define_module("TestingModule");

  ASSERT_EXCEPTION_CHECK(
    Exception,
    m.module_eval("create_opaque_handle_unregistered()"),
    ASSERT(std::string(ex.what()).find("OpaqueHandle") != std::string::npos)
  );
}

TESTCASE(OpaqueHandleCreateAndUse)
{
  // Register the incomplete type
  define_class<OpaqueHandle>("OpaqueHandle");

  // Test creating and using opaque handle via free functions
  define_global_function("create_opaque_handle", &createOpaqueHandle);
  define_global_function("destroy_opaque_handle", &destroyOpaqueHandle);
  define_global_function("get_opaque_handle_data", &getOpaqueHandleData);
  define_global_function("set_opaque_handle_data", &setOpaqueHandleData);

  Module m = define_module("TestingModule");

  std::string code = R"(handle = create_opaque_handle()
                        initial = get_opaque_handle_data(handle)
                        set_opaque_handle_data(handle, 999)
                        final = get_opaque_handle_data(handle)
                        destroy_opaque_handle(handle)
                        [initial, final])";

  Array result = m.module_eval(code);

  ASSERT_EQUAL(100, detail::From_Ruby<int>().convert(result[0].value()));
  ASSERT_EQUAL(999, detail::From_Ruby<int>().convert(result[1].value()));
}

TESTCASE(OpaqueHandleViaWrapper)
{
  // Register the incomplete type
  define_class<OpaqueHandle>("OpaqueHandle2");

  // Test opaque handle managed by wrapper class
  define_class<HandleWrapper>("OpaqueWrapper")
    .define_constructor(Constructor<HandleWrapper>())
    .define_method("get_data", &HandleWrapper::getData)
    .define_method("set_data", &HandleWrapper::setData)
    .define_method("get_raw_handle", &HandleWrapper::getRawHandle);

  define_global_function("get_opaque_handle_data", &getOpaqueHandleData);

  Module m = define_module("TestingModule");

  // Get raw handle from wrapper and use via free function
  std::string code = R"(wrapper = OpaqueWrapper.new
                        wrapper.set_data(555)
                        handle = wrapper.get_raw_handle
                        get_opaque_handle_data(handle))";

  Object result = m.module_eval(code);
  ASSERT_EQUAL(555, detail::From_Ruby<int>().convert(result.value()));
}

TESTCASE(OpaqueHandlePassToWrapper)
{
  // Register the incomplete type
  define_class<OpaqueHandle>("OpaqueHandle3");

  // Test passing opaque handle from free function into wrapper
  define_class<HandleWrapper>("OpaqueWrapper2")
    .define_constructor(Constructor<HandleWrapper>())
    .define_method("get_data", &HandleWrapper::getData)
    .define_method("set_raw_handle", &HandleWrapper::setRawHandle);

  define_global_function("create_opaque_handle", &createOpaqueHandle);
  define_global_function("set_opaque_handle_data", &setOpaqueHandleData);

  Module m = define_module("TestingModule");

  std::string code = R"(handle = create_opaque_handle()
                        set_opaque_handle_data(handle, 777)
                        wrapper = OpaqueWrapper2.new
                        wrapper.set_raw_handle(handle)
                        wrapper.get_data)";

  Object result = m.module_eval(code);
  ASSERT_EQUAL(777, detail::From_Ruby<int>().convert(result.value()));
}