#include "unittest.hpp"
#include "embed_ruby.hpp"
#include <rice/rice.hpp>
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
  
  // This is needed to make unittest compile (it uses ostream to report errors)
  inline std::ostream& operator<<(std::ostream& os, const Rice::Enum_Storage<Color>& storage)
  {
    os << storage.enumName;
    return os;
  }

  enum class Season { Spring, Summer, Fall, Winter };

  // This is needed to make unittest compile (it uses ostream to report errors)
  inline std::ostream& operator<<(std::ostream& os, const Season& season)
  {
    os << static_cast<std::underlying_type_t<Season>>(season);
    return os;
  }

  Enum<Season> define_season_enum()
  {
    static Enum<Season> seasons = define_enum<Season>("Season")
      .define_value("Spring", Season::Spring)
      .define_value("Summer", Season::Summer)
      .define_value("Fall", Season::Fall)
      .define_value("Winter", Season::Winter);

    return seasons;
  }
}

SETUP(Enum)
{
  embed_ruby();
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
  Array a = rb_eval_string("a = []; Color.each { |x| a << x }; a");
  ASSERT_EQUAL(3u, a.size());

  Enum<Color>::Value_T enum_0(a[0]);
  ASSERT_EQUAL(RED, enum_0->enumValue);
  
  Enum<Color>::Value_T enum_1(a[1]);
  ASSERT_EQUAL(BLACK, enum_1->enumValue);
  
  Enum<Color>::Value_T enum_2(a[2]);
  ASSERT_EQUAL(GREEN, enum_2->enumValue);
}

TESTCASE(each_seasons)
{
  Enum<Season> rb_cSeason = define_season_enum();
  Array a = rb_eval_string("a = []; Season.each { |x| a << x }; a");
  ASSERT_EQUAL(4u, a.size());

  Enum<Season>::Value_T enum_0(a[0]);
  ASSERT_EQUAL(Season::Spring, enum_0->enumValue);

  Enum<Season>::Value_T enum_1(a[1]);
  ASSERT_EQUAL(Season::Summer, enum_1->enumValue);

  Enum<Season>::Value_T enum_2(a[2]);
  ASSERT_EQUAL(Season::Fall, enum_2->enumValue);

  Enum<Season>::Value_T enum_3(a[3]);
  ASSERT_EQUAL(Season::Winter, enum_3->enumValue);
}

TESTCASE(to_s)
{
  Enum<Color> rb_cColor = define_color_enum();
  ASSERT_EQUAL(String("RED"), String(rb_eval_string("Color::RED.to_s")));
  ASSERT_EQUAL(String("BLACK"), String(rb_eval_string("Color::BLACK.to_s")));
  ASSERT_EQUAL(String("GREEN"), String(rb_eval_string("Color::GREEN.to_s")));
}

TESTCASE(to_i)
{
  Enum<Color> rb_cColor = define_color_enum();
  ASSERT_EQUAL(detail::to_ruby(int(RED)), rb_eval_string("Color::RED.to_i"));
  ASSERT_EQUAL(detail::to_ruby(int(BLACK)), rb_eval_string("Color::BLACK.to_i"));
  ASSERT_EQUAL(detail::to_ruby(int(GREEN)), rb_eval_string("Color::GREEN.to_i"));
}

TESTCASE(inspect)
{
  Enum<Color> rb_cColor = define_color_enum();
  ASSERT_EQUAL(String("#<Color::RED>"), String(rb_eval_string("Color::RED.inspect")));
  ASSERT_EQUAL(String("#<Color::BLACK>"), String(rb_eval_string("Color::BLACK.inspect")));
  ASSERT_EQUAL(String("#<Color::GREEN>"), String(rb_eval_string("Color::GREEN.inspect")));
}

TESTCASE(compare)
{
  Enum<Color> rb_cColor = define_color_enum();
  ASSERT_EQUAL(detail::to_ruby(-1), rb_eval_string("Color::RED <=> Color::BLACK"));
  ASSERT_EQUAL(detail::to_ruby(1), rb_eval_string("Color::GREEN <=> Color::RED"));
  ASSERT_EQUAL(detail::to_ruby(0), rb_eval_string("Color::BLACK <=> Color::BLACK"));
}

TESTCASE(eql)
{
  Enum<Color> rb_cColor = define_color_enum();
  ASSERT_EQUAL(detail::to_ruby(false), rb_eval_string("Color::RED == Color::BLACK"));
  ASSERT_EQUAL(detail::to_ruby(true), rb_eval_string("Color::GREEN == Color::GREEN"));
}

TESTCASE(invalid_to_i)
{
  Enum<Color> rb_cColor = define_color_enum();
  Enum<Color>::Value_T invalid(new Enum<Color>::Storage_T("Invalid Color", static_cast<Color>(42)));
  ASSERT_EQUAL(detail::to_ruby(42), invalid.call("to_i").value());
}

TESTCASE(invalid_compare)
{
  Enum<Color> rb_cColor = define_color_enum();
  Enum<Color>::Value_T invalid(new Enum<Color>::Storage_T("Invalid Color", static_cast<Color>(42)));
  Enum<Color>::Value_T red(new Enum<Color>::Storage_T("RED", RED));
  ASSERT_EQUAL(1, invalid.compare(red));
  ASSERT_EQUAL(-1, red.compare(invalid));
  ASSERT_EQUAL(0, invalid.compare(invalid));
}

TESTCASE(invalid_eql)
{
  Enum<Color> rb_cColor = define_color_enum();
  Enum<Color>::Value_T invalid(new Enum<Color>::Storage_T("Invalid Color", static_cast<Color>(42)));
  Enum<Color>::Value_T red(new Enum<Color>::Storage_T("RED", RED));
  ASSERT_EQUAL(false, invalid == red);
  ASSERT_EQUAL(false, red == invalid);
  ASSERT_EQUAL(true, invalid == invalid);
}

TESTCASE(different_objects_compare)
{
  Enum<Color> rb_cColor = define_color_enum();
  Enum<Color>::Value_T red1(new Enum<Color>::Storage_T("RED", RED));
  Enum<Color>::Value_T red2(new Enum<Color>::Storage_T("RED", RED));
  ASSERT_EQUAL(0, red1.compare(red2));
}

TESTCASE(different_objects_eql)
{
  Enum<Color> rb_cColor = define_color_enum();
  Enum<Color>::Value_T red1(new Enum<Color>::Storage_T("RED", RED));
  Enum<Color>::Value_T red2(new Enum<Color>::Storage_T("RED", RED));
  ASSERT_EQUAL(true, red1 == red2);
}

TESTCASE(hash)
{
  Enum<Color> rb_cColor = define_color_enum();
  Enum<Color>::Value_T red(new Enum<Color>::Storage_T("RED", RED));
  ASSERT_EQUAL(detail::to_ruby(int(RED)), red.call("hash").value());
}

TESTCASE(from_int)
{
  Enum<Color> rb_cColor = define_color_enum();
  Enum<Color>::Value_T color(rb_cColor.call("from_int", int(RED)));
  ASSERT_EQUAL(RED, color->enumValue);
}

TESTCASE(from_int_invalid)
{
  Enum<Color> rb_cColor = define_color_enum();
  ASSERT_EXCEPTION_CHECK(
    Exception,
    rb_cColor.call("from_int", -99999),
    ASSERT_EQUAL(Object(rb_eArgError),
                 Object(CLASS_OF(ex.value()))
    )
  );
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

  ASSERT_EQUAL(detail::to_ruby(int(0)), rb_eval_string("Inner::Props::VALUE1.to_i"));
  ASSERT_EQUAL(detail::to_ruby(int(1)), rb_eval_string("Inner::Props::VALUE2.to_i"));
  ASSERT_EQUAL(detail::to_ruby(int(2)), rb_eval_string("Inner::Props::VALUE3.to_i"));
}

namespace
{
  Color myFavoriteColor()
  {
    return RED;
  }

  bool isMyFavoriteColor(Color aColor)
  {
    return aColor == RED;
  }
}

TESTCASE(using_enums)
{
  Enum<Color> color = define_color_enum();
  color.define_singleton_function("my_favorite_color", &myFavoriteColor)
       .define_singleton_function("is_my_favorite_color", &isMyFavoriteColor)
       .define_singleton_function("is_my_favorite_color", &isMyFavoriteColor)
       .define_method("is_my_favorite_color", &isMyFavoriteColor);

  Module m = define_module("Testing");

  Object result = m.instance_eval("Color.my_favorite_color");
  ASSERT_EQUAL(RED, detail::From_Ruby<Color>::convert(result.value()));

  result = m.instance_eval("Color.is_my_favorite_color(Color::RED)");
  ASSERT_EQUAL(Qtrue, result.value());

  result = m.instance_eval("Color.is_my_favorite_color(Color::BLACK)");
  ASSERT_EQUAL(Qfalse, result.value());

  result = m.instance_eval("Color::RED.is_my_favorite_color");
  ASSERT_EQUAL(Qtrue, result.value());

  result = m.instance_eval("Color::BLACK.is_my_favorite_color");
  ASSERT_EQUAL(Qfalse, result.value());
}

namespace
{
  enum class Undefined { A, B, C };

  void undefinedArg(Undefined undefined)
  {
  }

  Undefined undefinedReturn()
  {
    return Undefined::B;
  }
}

TESTCASE(not_defined)
{
  ASSERT_EXCEPTION_CHECK(
    std::invalid_argument,
    define_global_function("undefined_arg", &undefinedArg),
    ASSERT_EQUAL("Enum type is not defined with Rice: enum `anonymous namespace'::Undefined", ex.what())
  );

  ASSERT_EXCEPTION_CHECK(
    std::invalid_argument,
    define_global_function("undefined_return", &undefinedReturn),
    ASSERT_EQUAL("Enum type is not defined with Rice: enum `anonymous namespace'::Undefined", ex.what())
  );
}
