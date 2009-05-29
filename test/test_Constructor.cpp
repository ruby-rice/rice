#include "unittest.hpp"
#include "rice/Constructor.hpp"
#include "rice/Data_Type.hpp"

#include "rice/detail/env.hpp"

#include <iostream>
using namespace std;

using namespace Rice;

TESTSUITE(Constructor);

namespace
{
  class Default_Constructible
  {
  public:
    Default_Constructible()
    {
    }
  };
}

TESTCASE(default_constructor)
{
  Data_Type<Default_Constructible> rb_cDefault_Constructible(
      anonymous_class());
  rb_cDefault_Constructible
    .define_constructor(Constructor<Default_Constructible>());
  Object o = rb_cDefault_Constructible.call("new");
  ASSERT_EQUAL(rb_cDefault_Constructible, o.class_of());
}


namespace
{
  class Non_Default_Constructible
  {
  public:
    Non_Default_Constructible(int i)
      : i_(i)
    {
    }

    int i() const
    {
      return i_;
    }

  private:
    int i_;
  };
}

TESTCASE(non_default_constructor)
{
  Data_Type<Non_Default_Constructible> rb_cNon_Default_Constructible(
      anonymous_class());
  rb_cNon_Default_Constructible
    .define_constructor(Constructor<Non_Default_Constructible, int>());
  Data_Object<Non_Default_Constructible> o =
    rb_cNon_Default_Constructible.call("new", 42);
  ASSERT_EQUAL(rb_cNon_Default_Constructible, o.class_of());
  ASSERT_EQUAL(42, o->i());
}
/*
namespace {

  class AbstractClass
  {
    public:

    int doSomething()
    {
      return doSomethingImpl();
    }

    int doSomethingParam(int a, int b)
    {
      return doSomethingParamImpl(a, b);
    }

    virtual int doSomethingImpl() = 0;

    virtual int doSomethingParamImpl(int a, int b) = 0;
  };

  class Director : public AbstractClass
  {
    Rice::Object m_rbSelf;

    public:
      Director(Rice::Object self) : m_rbSelf(self) { }

      virtual int doSomethingImpl() 
      {
        cout << endl << "In Impl, frame's last_func is " << rb_id2name(ruby_frame->last_func) << endl;
        cout << endl << "In Impl, frame's orig_func is " << rb_id2name(ruby_frame->orig_func) << endl;
        cout << endl << "In Impl, frame's last_class is " << Class(ruby_frame->last_class).name().str() << endl;

        cout << endl << "In Impl, frame's prev's last_func is " << rb_id2name(ruby_frame->prev->last_func) << endl;
        cout << endl << "In Impl, frame's prev's orig_func is " << rb_id2name(ruby_frame->prev->orig_func) << endl;

        cout << endl << "We've got classname (frame) " << rb_obj_classname(ruby_frame->self) << " vs (self) " << rb_obj_classname(m_rbSelf.value()) << endl;
        cout << endl << "We've got classname (frame prev) " << rb_obj_classname(ruby_frame->prev->self) << " vs (self) " << rb_obj_classname(m_rbSelf.value()) << endl;

        // Need to get the Ruby callee, compare that to m_rbSelf, and 
        // if they're equal, go UP the chain, otherwise go DOWN
        if(ruby_frame->self == m_rbSelf.value()) { 
          // As doSomethingImpl is pure virtual, we need to implement a default here so
          // that super calls don't blow things up
          return 100;
        } else {
          return from_ruby<int>( m_rbSelf.call("do_something_impl") );
        }

      }

      virtual int doSomethingParamImpl(int a, int b) 
      {
        return from_ruby<int>( m_rbSelf.call("do_something_param_impl", a, b) );
      }
  };
}

TESTCASE(director_system_polymorphic_calls)
{
  Data_Type<AbstractClass> a = define_class<AbstractClass>("__AbstractClass__");

  Data_Type<Director> d = define_class<Director, AbstractClass>("AbstractClass");
  d.define_constructor(Constructor<Director, Rice::Object>());
  d.define_method("do_something", &AbstractClass::doSomething);
  d.define_method("do_something_impl", &Director::doSomethingImpl);

  Module m = define_module("Testing");
  m.instance_eval(
    "class MyWorker < AbstractClass;"
    "def do_something_impl; 8; end;"
    "end"
  );

  Object result = m.instance_eval("worker = MyWorker.new; worker.do_something");

  ASSERT_EQUAL(
    8, from_ruby<int>(result.value())
  );
}

TESTCASE(director_system_polymorphic_calls_with_parameters)
{
  Data_Type<AbstractClass> a = define_class<AbstractClass>("__AbstractClass__");

  Data_Type<Director> d = define_class<Director, AbstractClass>("AbstractClass");
  d.define_constructor(Constructor<Director, Rice::Object>());
  d.define_method("do_something_param", &AbstractClass::doSomethingParam);
  d.define_method("do_something_param_impl", &Director::doSomethingParamImpl);

  Module m = define_module("Testing");
  m.instance_eval(
    "class MyWorker < AbstractClass;"
    "def do_something_param_impl(a, b); a * b; end;"
    "end"
  );

  Object result = m.instance_eval("worker = MyWorker.new; worker.do_something_param(2, 10)");

  ASSERT_EQUAL(
    20, from_ruby<int>(result.value())
  );
}

TESTCASE(director_system_polymorphic_calls_very_deep)
{
  Data_Type<AbstractClass> a = define_class<AbstractClass>("__AbstractClass__");

  Data_Type<Director> d = define_class<Director, AbstractClass>("AbstractClass");
  d.define_constructor(Constructor<Director, Rice::Object>());
  d.define_method("do_something", &AbstractClass::doSomething);
  d.define_method("do_something_impl", &Director::doSomethingImpl);

  Module m = define_module("Testing");
  m.instance_eval(
    "class MyWorker < AbstractClass;"
    "def do_something_impl; 4; end;"
    "end;"
    "class MyWorker2 < MyWorker;"
    "def do_something_impl; 6; end;"
    "end;"
    "class MyWorker3 < MyWorker2;"
    "def do_something_impl; 8; end;"
    "end;"
    "class MyWorker4 < MyWorker3;"
    "def do_something_impl; 10; end;"
    "end;"
  );

  Object result = m.instance_eval("worker = MyWorker4.new; worker.do_something");

  ASSERT_EQUAL(
    10, from_ruby<int>(result.value())
  );
}

TESTCASE(director_system_super_calls_dont_infinite_loop) 
{
  Data_Type<AbstractClass> a = define_class<AbstractClass>("__AbstractClass__");

  Data_Type<Director> d = define_class<Director, AbstractClass>("AbstractClass");
  d.define_constructor(Constructor<Director, Rice::Object>());
  d.define_method("do_something", &AbstractClass::doSomething);
  d.define_method("do_something_impl", &Director::doSomethingImpl);

  Module m = define_module("Testing");
  m.instance_eval(
    "class MyWorker < AbstractClass;"
    "@@call_count = 0;"
    "def do_something_impl;"
    "@@call_count += 1;"
    "if @@call_count == 1; super; else; -1; end;"
    "end; end;"
  );

  Object result = m.instance_eval("worker = MyWorker.new; worker.do_something");

  // -1 means that the do_something_impl got called twice, aka we were in an infinite loop
  // had the class var check not been there
  ASSERT_NOT_EQUAL(
    -1,
    from_ruby<int>(result.value())
  );

  ASSERT_EQUAL(
    100,
    from_ruby<int>(result.value())
  );
}
*/
