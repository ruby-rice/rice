#include "unittest.hpp"
#include "rice/Enum.hpp"
#include "rice/Array.hpp"
#include "rice/String.hpp"
#include "rice/Constructor.hpp"
#include <iostream>

using namespace Rice;

TESTSUITE(Enum);

namespace
{
  enum Color { RED, BLACK, GREEN };

  Enum<Color> define_color_enum()
  {
    static Enum<Color> colors = define_enum<Color>("Color")
      .define_value("RED", RED)
      .define_value("BLACK", BLACK)
      .define_value("GREEN", GREEN);
    return colors;
  }
}

SETUP(Enum)
{
  ruby_init();
}

TESTCASE(copy_construct)
{
  Enum<Color> rb_cColor(define_color_enum());
  ASSERT_EQUAL(define_color_enum().value(), rb_cColor.value());
}

TESTCASE(assignment)
{
  Enum<Color> rb_cColor;
  rb_cColor = define_color_enum();
  ASSERT_EQUAL(define_color_enum().value(), rb_cColor.value());
}

TESTCASE(each)
{
  Enum<Color> rb_cColor = define_color_enum();
  Array a = protect(rb_eval_string, "a = []; Color.each { |x| a << x }; a");
  ASSERT_EQUAL(3u, a.size());
  ASSERT_EQUAL(RED, from_ruby<Color>(a[0]));
  ASSERT_EQUAL(BLACK, from_ruby<Color>(a[1]));
  ASSERT_EQUAL(GREEN, from_ruby<Color>(a[2]));
}

TESTCASE(to_s)
{
  Enum<Color> rb_cColor = define_color_enum();
  ASSERT_EQUAL(String("RED"), String(protect(rb_eval_string, "Color::RED.to_s")));
  ASSERT_EQUAL(String("BLACK"), String(protect(rb_eval_string, "Color::BLACK.to_s")));
  ASSERT_EQUAL(String("GREEN"), String(protect(rb_eval_string, "Color::GREEN.to_s")));
}

TESTCASE(to_i)
{
  Enum<Color> rb_cColor = define_color_enum();
  ASSERT_EQUAL(to_ruby(int(RED)), Object(protect(rb_eval_string, "Color::RED.to_i")));
  ASSERT_EQUAL(to_ruby(int(BLACK)), Object(protect(rb_eval_string, "Color::BLACK.to_i")));
  ASSERT_EQUAL(to_ruby(int(GREEN)), Object(protect(rb_eval_string, "Color::GREEN.to_i")));
}

TESTCASE(inspect)
{
  Enum<Color> rb_cColor = define_color_enum();
  ASSERT_EQUAL(String("#<Color::RED>"), String(protect(rb_eval_string, "Color::RED.inspect")));
  ASSERT_EQUAL(String("#<Color::BLACK>"), String(protect(rb_eval_string, "Color::BLACK.inspect")));
  ASSERT_EQUAL(String("#<Color::GREEN>"), String(protect(rb_eval_string, "Color::GREEN.inspect")));
}

TESTCASE(compare)
{
  Enum<Color> rb_cColor = define_color_enum();
  ASSERT_EQUAL(to_ruby(-1), Object(protect(rb_eval_string, "Color::RED <=> Color::BLACK")));
  ASSERT_EQUAL(to_ruby(1), Object(protect(rb_eval_string, "Color::GREEN <=> Color::RED")));
  ASSERT_EQUAL(to_ruby(0), Object(protect(rb_eval_string, "Color::BLACK <=> Color::BLACK")));
}

TESTCASE(eql)
{
  Enum<Color> rb_cColor = define_color_enum();
  ASSERT_EQUAL(to_ruby(false), Object(protect(rb_eval_string, "Color::RED == Color::BLACK")));
  ASSERT_EQUAL(to_ruby(true), Object(protect(rb_eval_string, "Color::GREEN == Color::GREEN")));
}

TESTCASE(invalid_to_s)
{
  Enum<Color> rb_cColor = define_color_enum();
  Data_Object<Color> invalid(new Color(static_cast<Color>(42)));
  ASSERT_EQUAL(String("INVALID(42)"), invalid.to_s());
}

TESTCASE(invalid_to_i)
{
  Enum<Color> rb_cColor = define_color_enum();
  Data_Object<Color> invalid(new Color(static_cast<Color>(42)));
  ASSERT_EQUAL(to_ruby(42), invalid.call("to_i"));
}

TESTCASE(invalid_inspect)
{
  Enum<Color> rb_cColor = define_color_enum();
  Data_Object<Color> invalid(new Color(static_cast<Color>(42)));
  ASSERT_EQUAL(String("#<Color::INVALID(42)>"), invalid.inspect());
}

TESTCASE(invalid_compare)
{
  Enum<Color> rb_cColor = define_color_enum();
  Data_Object<Color> invalid(new Color(static_cast<Color>(42)));
  Data_Object<Color> red(new Color(RED));
  ASSERT_EQUAL(1, invalid.compare(red));
  ASSERT_EQUAL(-1, red.compare(invalid));
  ASSERT_EQUAL(0, invalid.compare(invalid));
}

TESTCASE(invalid_eql)
{
  Enum<Color> rb_cColor = define_color_enum();
  Data_Object<Color> invalid(new Color(static_cast<Color>(42)));
  Data_Object<Color> red(new Color(RED));
  ASSERT_EQUAL(false, invalid == red);
  ASSERT_EQUAL(false, red == invalid);
  ASSERT_EQUAL(true, invalid == invalid);
}

TESTCASE(different_objects_compare)
{
  Enum<Color> rb_cColor = define_color_enum();
  Data_Object<Color> red1(new Color(RED));
  Data_Object<Color> red2(new Color(RED));
  ASSERT_EQUAL(0, red1.compare(red2));
}

TESTCASE(different_objects_eql)
{
  Enum<Color> rb_cColor = define_color_enum();
  Data_Object<Color> red1(new Color(RED));
  Data_Object<Color> red2(new Color(RED));
  ASSERT_EQUAL(true, red1 == red2);
}

/*
TESTCASE(hash)
{
  Enum<Color> rb_cColor = define_color_enum();
  Data_Object<Color> red(new Color(RED));
  ASSERT_EQUAL(to_ruby(int(RED)), red.call("hash"));
}
*/

TESTCASE(from_int)
{
  Enum<Color> rb_cColor = define_color_enum();
  Data_Object<Color> color(rb_cColor.call("from_int", int(RED)));
  ASSERT_EQUAL(RED, *color);
}

namespace
{
  class Inner
  {
    public:
      enum Props
      {
        VALUE1,
        VALUE2,
        VALUE3
      };
  };
}

TESTCASE(nested_enums)
{
  {
    Data_Type<Inner> inner = define_class<Inner>("Inner");
    define_enum<Inner::Props>("Props", inner)
      .define_value("VALUE1", Inner::VALUE1)
      .define_value("VALUE2", Inner::VALUE2)
      .define_value("VALUE3", Inner::VALUE3);
    inner.define_constructor(Constructor<Inner>());
  }

  ASSERT_EQUAL(to_ruby(int(0)), Object(protect(rb_eval_string, "Inner::Props::VALUE1.to_i")));
  ASSERT_EQUAL(to_ruby(int(1)), Object(protect(rb_eval_string, "Inner::Props::VALUE2.to_i")));
  ASSERT_EQUAL(to_ruby(int(2)), Object(protect(rb_eval_string, "Inner::Props::VALUE3.to_i")));
}

