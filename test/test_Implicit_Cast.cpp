#include "unittest.hpp"
#include "embed_ruby.hpp"
#include <rice/rice.hpp>

using namespace Rice;

TESTSUITE(Implicit_Cast);

SETUP(Implicit_Cast)
{
  embed_ruby();
}

/**
 * Examples here taken from Ogre's Math library.
 * This uses the constructor method of casting types.
 */
namespace
{
  const float degree2Radians = (float)(3.14 / 180.0);
  const float radian2Degrees = (float)(180.0 / 3.14);

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

TESTCASE(constructor_cast)
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

  Object result = m.instance_eval("is_acute(Degree.new(75))");
  ASSERT(detail::From_Ruby<bool>::convert(result.value()));

  result = m.instance_eval("is_acute(Radian.new(2.0))");
  ASSERT(!detail::From_Ruby<bool>::convert(result.value()));

  // OBTUSE
  result = m.instance_eval("is_obtuse(Degree.new(75))");
  ASSERT(!detail::From_Ruby<bool>::convert(result.value()));

  result = m.instance_eval("is_obtuse(Radian.new(2.0))");
  ASSERT(detail::From_Ruby<bool>::convert(result.value()));

  // RIGHT
  result = m.instance_eval("is_right(Degree.new(90))");
  ASSERT(detail::From_Ruby<bool>::convert(result.value()));

  result = m.instance_eval("is_right(Radian.new(2.0))");
  ASSERT(!detail::From_Ruby<bool>::convert(result.value()));
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

TESTCASE(multiple_constructor_casts)
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

  Object result = m.instance_eval("is_obtuse(Degree.new(75))");
  ASSERT(!detail::From_Ruby<bool>::convert(result.value()));

  result = m.instance_eval("explicit_value(Degree.new(75))");
  ASSERT_EQUAL(75.0, detail::From_Ruby<float>::convert(result.value()));
}

namespace
{
  class Celsius;

  class Fahrenheit
  {
  public:

    explicit Fahrenheit(double value) : value_(value)
    {
    }

    operator Celsius();

    double value_;
  };

  class Celsius
  {
  public:

    explicit Celsius(double value) : value_(value)
    {
    }

    operator Fahrenheit()
    {
      return Fahrenheit(32 + (value_ * 9.0 / 5.0));
    }

    double value_;
  };

  Fahrenheit::operator Celsius()
  {
    return Celsius((value_ - 32) * 5.0 / 9.0);
  }

  double fahrenheitValue(Fahrenheit data)
  {
    return data.value_;
  }

  double celsiusValue(Celsius data)
  {
    return data.value_;
  }
}

TESTCASE(operator_casts)
{
  define_class<Fahrenheit>("Fahrenheit")
    .define_constructor(Constructor<Fahrenheit, double>());

  define_class<Celsius>("Celsius")
    .define_constructor(Constructor<Celsius, double>());

  define_implicit_cast<Fahrenheit, Celsius>();
  define_implicit_cast<Celsius, Fahrenheit>();

  define_global_function("fahrenheit_value", &fahrenheitValue);
  define_global_function("celsius_value", &celsiusValue);

  Module m = define_module("TestingModule");

  Object result = m.instance_eval("fahrenheit_value(Fahrenheit.new(32))");
  ASSERT_EQUAL(32.0, detail::From_Ruby<double>::convert(result));

  result = m.instance_eval("celsius_value(Fahrenheit.new(32))");
  ASSERT_EQUAL(0, detail::From_Ruby<double>::convert(result));

  result = m.instance_eval("fahrenheit_value(Celsius.new(100))");
  ASSERT_EQUAL(212.0, detail::From_Ruby<double>::convert(result));

  result = m.instance_eval("celsius_value(Celsius.new(100))");
  ASSERT_EQUAL(100, detail::From_Ruby<double>::convert(result));
}

/**
 * Sample taken and modified from boost::python::implicit:
 * http://www.boost.org/doc/libs/1_41_0/libs/python/doc/v2/implicit.html
 *
 * This is the operator version of casting and shows that this works for
 * base types as well as defined types
 */
/*namespace
{
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

TESTCASE(primitive_casts)
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
  ASSERT_EQUAL(4, detail::From_Ruby<int>::convert(result.value()));

  // As fixnum (int)
  result = m.instance_eval("real_value(4)");
  ASSERT_EQUAL(4, detail::From_Ruby<int>::convert(result.value()));

  // As Real object
  result = m.instance_eval("r = make_real( Real.new(6) ); real_value(r)");
  ASSERT_EQUAL(6, detail::From_Ruby<int>::convert(result.value()));

  // As fixnum (int)
  result = m.instance_eval("r = make_real(6); real_value(r)");
  ASSERT_EQUAL(6, detail::From_Ruby<int>::convert(result.value()));
}*/