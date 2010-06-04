#include "unittest.hpp"
#include "rice/Data_Type.hpp"
#include "rice/Exception.hpp"
#include "rice/Constructor.hpp"
#include "rice/global_function.hpp"

using namespace Rice;

TESTSUITE(Data_Type);

/**
 * The tests here are for the feature of taking an instance
 * of a Ruby-subclass of a Rice wrapped class and passing
 * that instance back into the Rice wrapper. While that
 * might be confusing, the test code is pretty straight foward
 * to see what we're talking about.
 */

namespace {

  /**
   * The class we will subclass in Ruby
   */
  class Listener {
    public:
      Listener() {  }

      virtual ~Listener() {  }

      virtual int getValue() { return 4; }
  };

  /**
   * This class will recieve a new Listener instance
   * from Ruby
   */
  class ListenerHandler {

    public:

      ListenerHandler() {  }

      void addListener(Listener* newList) {
        mListeners.push_back(newList);
      }

      int process() {
        std::vector<Listener*>::iterator i = mListeners.begin();
        int accum = 0; 
        for(; i != mListeners.end(); i++) {
          accum += (*i)->getValue();
        }

        return accum;
      }

      int listenerCount() { return mListeners.size(); }

    private:
      std::vector<Listener*> mListeners;
  };
}

SETUP(Data_Type)
{
  ruby_init();

  define_class<Listener>("Listener")
    .define_constructor(Constructor<Listener>())
    .define_method("get_value", &Listener::getValue);

  define_class<ListenerHandler>("ListenerHandler")
    .define_constructor(Constructor<ListenerHandler>())
    .define_method("add_listener", &ListenerHandler::addListener)
    .define_method("process", &ListenerHandler::process)
    .define_method("listener_count", &ListenerHandler::listenerCount);

}

TESTCASE(can_send_ruby_instance_back_into_rice)
{
  Module m = define_module("TestingModule");
  Object handler = m.instance_eval("@handler = ListenerHandler.new");

  ASSERT_EQUAL(INT2NUM(0), handler.call("listener_count").value());

  m.instance_eval("class MyListener < Listener; end;");
  m.instance_eval("@handler.add_listener(MyListener.new)");

  ASSERT_EQUAL(INT2NUM(1), handler.call("listener_count").value());
  ASSERT_EQUAL(INT2NUM(4), handler.call("process").value());

  m.instance_eval("@handler.add_listener(Listener.new)");

  ASSERT_EQUAL(INT2NUM(2), handler.call("listener_count").value());
  ASSERT_EQUAL(INT2NUM(8), handler.call("process").value());
}

/**
 * The following test SEGFAULTs right now
 */
/*
TESTCASE(no_super_in_constructor_still_works) 
{
  Module m = define_module("TestingModule");
  Object handler = m.instance_eval("@handler = ListenerHandler.new");

  ASSERT_EQUAL(INT2NUM(0), handler.call("listener_count").value());

  // Because of this, there's a constructor but no super call
  m.instance_eval("class MyListener < Listener; def initialize; @val = 10; end; end;");
  m.instance_eval("@handler.add_listener(MyListener.new)");

  ASSERT_EQUAL(INT2NUM(1), handler.call("listener_count").value());
  ASSERT_EQUAL(INT2NUM(4), handler.call("process").value());

  m.instance_eval("@handler.add_listener(MyListener.new)");

  ASSERT_EQUAL(INT2NUM(2), handler.call("listener_count").value());
  ASSERT_EQUAL(INT2NUM(8), handler.call("process").value());
}
*/

/**
 * Implicit Casting across unrelated types
 *
 * Two ways of defining if types are implicitly castable
 *
 * 1) operator
 * 2) constructor 
 */

/**
 * Examples here taken from Ogre's Math library.
 * This uses the constructor method of casting types.
 */
namespace 
{
  const int degree2Radians = (3.14 / 180.0);
  const int radian2Degrees = (180.0 / 3.14);

  class Radian;

  class Degree
  {
    public:
      explicit Degree(float d) : val_(d) {}
      Degree(const Radian& r);

      float valueDegrees() const { return val_; }
      float valueRadians() const { return val_ * degree2Radians; }

    private:
      float val_;
  };

  class Radian
  {
    public:
      explicit Radian(float r) : val_(r) {} 
      Radian(const Degree& d) : val_(d.valueRadians()) {}

      float valueRadians() const { return val_; }
      float valueDegrees() const { return val_ * radian2Degrees; }

    private:
      float val_;
  };

  // Due to circular dependencies, need to define some
  // methods down here
  Degree::Degree(const Radian& r)
  {
    val_ = r.valueDegrees();
  }

  /**
   * And now some methods that work w/ the above two classes
   */
  bool isAcute(Degree degree) {
    return degree.valueDegrees() < 90;
  }

  bool isObtuse(Radian radian) {
    return radian.valueDegrees() > 90 && radian.valueDegrees() <= 180;
  }

  bool isRight(Degree* degree) {
    return degree->valueDegrees() == 90;
  }
}

TESTCASE(can_define_implicit_type_conversions_across_wrapped_types)
{
  define_class<Degree>("Degree")
    .define_constructor(Constructor<Degree, float>());

  define_class<Radian>("Radian")
    .define_constructor(Constructor<Radian, float>());

  define_implicit_cast<Degree, Radian>();
  define_implicit_cast<Radian, Degree>();

  define_global_function("is_acute", &isAcute);
  define_global_function("is_obtuse", &isObtuse);
  define_global_function("is_right", &isRight);

  Module m = define_module("TestingModule");
  Object result;

  // ACUTE
  result = m.instance_eval("is_acute(Degree.new(75))");
  ASSERT(from_ruby<bool>(result.value()));

  result = m.instance_eval("is_acute(Radian.new(2.0))");
  ASSERT(!from_ruby<bool>(result.value()));

  // OBTUSE
  result = m.instance_eval("is_obtuse(Degree.new(75))");
  ASSERT(!from_ruby<bool>(result.value()));

  result = m.instance_eval("is_obtuse(Radian.new(2.0))");
  ASSERT(from_ruby<bool>(result.value()));

  // RIGHT
  result = m.instance_eval("is_right(Degree.new(90))");
  ASSERT(from_ruby<bool>(result.value()));

  result = m.instance_eval("is_right(Radian.new(2.0))");
  ASSERT(!from_ruby<bool>(result.value()));
}

namespace {
  class Explicit
  {
    public:
      Explicit(float v) {
        value = v;
      }

      Explicit(const Degree &d) {
        value = d.valueDegrees();
      }

      float getValue() { return value; }

    private:
      float value;
  };

  float getExplicitValue(Explicit* v) {
    return v->getValue();
  }
}

TESTCASE(supports_multiple_implicit_conversions_for_a_type)
{
  define_class<Degree>("Degree")
    .define_constructor(Constructor<Degree, float>());

  define_class<Radian>("Radian")
    .define_constructor(Constructor<Radian, float>());

  define_class<Explicit>("Explicit")
    .define_constructor(Constructor<Explicit, float>());

  define_global_function("is_obtuse", &isObtuse);
  define_global_function("explicit_value", &getExplicitValue);

  define_implicit_cast<Radian, Degree>();
  define_implicit_cast<Degree, Radian>();
  define_implicit_cast<Degree, Explicit>();

  Module m = define_module("TestingModule");
  Object result;

  result = m.instance_eval("is_obtuse(Degree.new(75))");
  ASSERT(!from_ruby<bool>(result.value()));

  result = m.instance_eval("explicit_value(Degree.new(75))");
  ASSERT_EQUAL(75.0, from_ruby<float>(result.value()));
}

/**
 * Sample taken and modified from boost::python::implicit:
 * http://www.boost.org/doc/libs/1_41_0/libs/python/doc/v2/implicit.html
 *
 * This is the operator version of casting and shows that this works for
 * base types as well as defined types
 */
/*
namespace {
  struct Real
  {
    Real(int x)
      : v(x)
    {}

    operator int() const
    {
      return v;
    }

    int v;
  };

  int realValue(Real const& x)
  {
    return x.v;
  }

  Real makeReal(int n)
  {
    return Real(n);
  }
}

TESTCASE(can_define_implicit_type_conversions_to_base_types)
{
  define_class<Real>("Real")
    .define_constructor(Constructor<Real, int>());

  // Define the conversion rules
  define_implicit_cast<Real, int>();
  define_implicit_cast<int, Real>();

  define_global_function("real_value", &realValue);
  define_global_function("make_real", &makeReal);

  Module m = define_module("TestingModule");

  // As Real object
  Object result = m.instance_eval("real_value( Real.new(4) )");
  ASSERT_EQUAL(4, from_ruby<int>(result.value()));

  // As fixnum (int)
  result = m.instance_eval("real_value(4)");
  ASSERT_EQUAL(4, from_ruby<int>(result.value()));

  // As Real object
  result = m.instance_eval("r = make_real( Real.new(6) ); real_value(r)");
  ASSERT_EQUAL(6, from_ruby<int>(result.value()));

  // As fixnum (int)
  result = m.instance_eval("r = make_real(6); real_value(r)");
  ASSERT_EQUAL(6, from_ruby<int>(result.value()));
}
*/
