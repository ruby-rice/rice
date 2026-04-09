#include "unittest.hpp"
#include "embed_ruby.hpp"

#include <rice/rice.hpp>

using namespace Rice;

TESTSUITE(Constructor);

SETUP(Constructor)
{
  embed_ruby();
}

TEARDOWN(Constructor)
{
  rb_gc_start();
}

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

namespace
{
  class TakesPointerParameter
  {
  public:
    TakesPointerParameter(int argc, const char *const* argv, bool enabled)
      : argc_(argc), argv_(argv), enabled_(enabled)
    {
    }

    int argc() const
    {
      return argc_;
    }

    const char *firstArg() const
    {
      return argv_[0];
    }

    bool enabled() const
    {
      return enabled_;
    }

  private:
    int argc_;
    const char *const *argv_;
    bool enabled_;
  };
}

TESTCASE(default_constructor)
{
  Data_Type<Default_Constructible> rb_cDefault_Constructible(anonymous_class());
  rb_cDefault_Constructible.define_constructor(Constructor<Default_Constructible>());
  Object o = rb_cDefault_Constructible.call("new");
  ASSERT_EQUAL(rb_cDefault_Constructible, o.class_of());
}

TESTCASE(constructor_accepts_pointer_parameters)
{
  Data_Type<TakesPointerParameter> rb_cTakesPointerParameter(anonymous_class());
  rb_cTakesPointerParameter.define_constructor(
    Constructor<TakesPointerParameter, int, const char *const*, bool>(),
    Arg("argc"), Arg("argv"), Arg("enabled"));
  ASSERT_EQUAL(rb_cTakesPointerParameter, rb_cTakesPointerParameter);
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

namespace
{
  int withArgsX;
  float withArgsY;
  bool withArgsYes;

  class WithDefaultArgs
  {
    public:
      WithDefaultArgs(int x, float y = 2.0, bool yes = false)
      {
        withArgsX = x;
        withArgsY = y;
        withArgsYes = yes;
      }
  };

  int withArgX;
  class WithOneArg
  {
    public:
      WithOneArg(int x = 14) {
        withArgX = x;
      }
  };
}

TESTCASE(constructor_supports_default_arguments)
{
  Class klass = define_class<WithDefaultArgs>("WithDefaultArgs").
    define_constructor(Constructor<WithDefaultArgs, int, float, bool>(),
           Arg("x"), Arg("y") = (float)2.0, Arg("yes") = (bool)false);

  klass.call("new", 4);
  ASSERT_EQUAL(4, withArgsX);
  ASSERT_EQUAL(2.0, withArgsY);
  ASSERT_EQUAL(false, withArgsYes);

  klass.call("new", 5, 3.0);
  ASSERT_EQUAL(5, withArgsX);
  ASSERT_EQUAL(3.0, withArgsY);
  ASSERT_EQUAL(false, withArgsYes);

  klass.call("new", 7, 12.0, true);
  ASSERT_EQUAL(7, withArgsX);
  ASSERT_EQUAL(12.0, withArgsY);
  ASSERT_EQUAL(true, withArgsYes);
}

TESTCASE(constructor_supports_single_default_argument)
{
  Class klass = define_class<WithOneArg>("WithOneArg").
    define_constructor(Constructor<WithOneArg, int>(),
          ( Arg("x") = 14 ));

  klass.call("new");
  ASSERT_EQUAL(14, withArgX);

  klass.call("new", 6);
  ASSERT_EQUAL(6, withArgX);
}

namespace
{
  class MyClass
  {
  public: 
    MyClass() = default;
    MyClass(const MyClass& other) = default;
    MyClass(MyClass&& other) = default;
    int value;
  };
}

TESTCASE(constructor_clone)
{
  Class c = define_class<MyClass>("MyClass")
    .define_constructor(Constructor<MyClass>())
    .define_constructor(Constructor<MyClass, const MyClass&>())
    .define_attr("value", &MyClass::value);

  // Default constructor
  Object o1 = c.call("new");
  o1.call("value=", 7);
  ASSERT_EQUAL(c, o1.class_of());

  // Clone
  Object o2 = o1.call("clone");
  Object value = o2.call("value");
  ASSERT_EQUAL(c, o2.class_of());
  ASSERT_EQUAL(7, detail::From_Ruby<int>().convert(value));
}

TESTCASE(constructor_dup)
{
  Class c = define_class<MyClass>("MyClass").
            define_constructor(Constructor<MyClass>()).
            define_constructor(Constructor<MyClass, const MyClass&>()).
            define_attr("value", &MyClass::value);

  // Default constructor
  Object o1 = c.call("new");
  o1.call("value=", 7);
  ASSERT_EQUAL(c, o1.class_of());

  // Clone
  Object o2 = o1.call("dup");
  Object value = o2.call("value");
  ASSERT_EQUAL(c, o2.class_of());
  ASSERT_EQUAL(7, detail::From_Ruby<int>().convert(value));
}

TESTCASE(constructor_move)
{
  Data_Type<MyClass> c = define_class<MyClass>("MyClass").
            define_constructor(Constructor<MyClass>());

  // This intentionally will not compile due to a static_assert
  //c.define_constructor(Constructor<MyClass, MyClass&&>());
}

namespace
{
  // Polymorphic (has virtual methods) but non-virtual destructor.
  // Deleting through a pointer is UB in this case. Rice's
  // Wrapper<T*>::~Wrapper should not delete such objects.
  class PolymorphicNonVirtualDtor
  {
  public:
    PolymorphicNonVirtualDtor() : value_(42) {}
    ~PolymorphicNonVirtualDtor() = default;

    virtual int value()
    {
      return value_;
    }

  private:
    int value_;
  };
}

TESTCASE(constructor_polymorphic_non_virtual_dtor)
{
  Data_Type<PolymorphicNonVirtualDtor> rb_cPolymorphicNonVirtualDtor(anonymous_class());
  rb_cPolymorphicNonVirtualDtor
    .define_constructor(Constructor<PolymorphicNonVirtualDtor>())
    .define_method("value", &PolymorphicNonVirtualDtor::value);

  Object o = rb_cPolymorphicNonVirtualDtor.call("new");
  ASSERT_EQUAL(rb_cPolymorphicNonVirtualDtor, o.class_of());

  Object value = o.call("value");
  ASSERT_EQUAL(42, detail::From_Ruby<int>().convert(value));
}

namespace
{
  class MoveOnlyValue
  {
  public:
    MoveOnlyValue() = default;
    MoveOnlyValue(const MoveOnlyValue&) = delete;

    MoveOnlyValue(MoveOnlyValue&& other) noexcept
      : moved_from_(other.moved_from_)
    {
      other.moved_from_ = true;
    }

    bool movedFrom() const
    {
      return moved_from_;
    }

  private:
    bool moved_from_ = false;
  };

  class TakesMoveOnlyValue
  {
  public:
    explicit TakesMoveOnlyValue(MoveOnlyValue&& value)
      : value_(std::move(value))
    {
    }

    bool hasValue() const
    {
      return !value_.movedFrom();
    }

  private:
    MoveOnlyValue value_;
  };
}

TESTCASE(constructor_forwards_rvalue_arguments)
{
  define_class<MoveOnlyValue>("MoveOnlyValue")
    .define_constructor(Constructor<MoveOnlyValue>())
    .define_method("moved_from?", &MoveOnlyValue::movedFrom);

  define_class<TakesMoveOnlyValue>("TakesMoveOnlyValue")
    .define_constructor(Constructor<TakesMoveOnlyValue, MoveOnlyValue&&>())
    .define_method("has_value?", &TakesMoveOnlyValue::hasValue);

  Module m(anonymous_module());
  Array result = m.module_eval(R"(
    value = MoveOnlyValue.new
    consumer = TakesMoveOnlyValue.new(value)
    [value.moved_from?, consumer.has_value?, consumer.class.name]
  )");

  ASSERT_EQUAL(Qtrue, result[0].value());
  ASSERT_EQUAL(Qtrue, result[1].value());
  ASSERT_EQUAL("TakesMoveOnlyValue", String(result[2].value()).str());
}
