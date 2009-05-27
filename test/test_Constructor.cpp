#include "unittest.hpp"
#include "rice/Constructor.hpp"
#include "rice/Data_Type.hpp"
#include "rice/Constructor.hpp"

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

namespace {

  class AbstractClass
  {
    public:

    int doSomething()
    {
      return doSomethingImpl();
    }

    virtual int doSomethingImpl() = 0;
  };

  class Director : public AbstractClass
  {
    Rice::Object m_rbSelf;

    public:
      Director(Rice::Object self) : m_rbSelf(self) { }

      virtual int doSomethingImpl() {
        return from_ruby<int>( m_rbSelf.call("do_something_impl") );
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
    INT2NUM(8),
    result.value()
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
    INT2NUM(10),
    result.value()
  );
}
