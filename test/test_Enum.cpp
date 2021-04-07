#include <iostream>

#include "unittest.hpp"
#include "embed_ruby.hpp"

#include <rice/rice.hpp>

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
  Enum<Color> colorEnum(define_color_enum());
  ASSERT_EQUAL(define_color_enum().value(), colorEnum.value());
}

TESTCASE(assignment)
{
  Enum<Color> colorEnum;
  colorEnum = define_color_enum();
  ASSERT_EQUAL(define_color_enum().value(), colorEnum.value());
}

TESTCASE(each)
{
  Module m = define_module("Testing");

  Enum<Color> colorEnum = define_color_enum();

  std::string code = R"(a = []
                        Color.each { |x| a << x }
                        a)";
  Array a = m.instance_eval(code);
  ASSERT_EQUAL(3u, a.size());

  Data_Object<Color> enum_0(a[0]);
  ASSERT_EQUAL(RED, *enum_0);
  
  Data_Object<Color> enum_1(a[1]);
  ASSERT_EQUAL(BLACK, *enum_1);
  
  Data_Object<Color> enum_2(a[2]);
  ASSERT_EQUAL(GREEN, *enum_2);
}

TESTCASE(each_seasons)
{
  Module m = define_module("Testing");

  Enum<Season> rb_cSeason = define_season_enum();
  std::string code = R"(a = []
                        Season.each { |x| a << x }
                        a)";

  Array a = m.instance_eval(code);
  ASSERT_EQUAL(4u, a.size());

  Data_Object<Season> enum_0(a[0]);
  ASSERT_EQUAL(Season::Spring, *enum_0);

  Data_Object<Season> enum_1(a[1]);
  ASSERT_EQUAL(Season::Summer, *enum_1);

  Data_Object<Season> enum_2(a[2]);
  ASSERT_EQUAL(Season::Fall, *enum_2);

  Data_Object<Season> enum_3(a[3]);
  ASSERT_EQUAL(Season::Winter, *enum_3);
}

TESTCASE(to_s)
{
  Enum<Color> colorEnum = define_color_enum();
  ASSERT_EQUAL(String("RED"), String(detail::protect(rb_eval_string, "Color::RED.to_s")));
  ASSERT_EQUAL(String("BLACK"), String(detail::protect(rb_eval_string, "Color::BLACK.to_s")));
  ASSERT_EQUAL(String("GREEN"), String(detail::protect(rb_eval_string, "Color::GREEN.to_s")));
}

TESTCASE(to_i)
{
  Enum<Color> colorEnum = define_color_enum();
  ASSERT_EQUAL(detail::to_ruby(int(RED)), rb_eval_string("Color::RED.to_i"));
  ASSERT_EQUAL(detail::to_ruby(int(BLACK)), rb_eval_string("Color::BLACK.to_i"));
  ASSERT_EQUAL(detail::to_ruby(int(GREEN)), rb_eval_string("Color::GREEN.to_i"));
}

TESTCASE(inspect)
{
  Enum<Color> colorEnum = define_color_enum();
  ASSERT_EQUAL(String("#<Color::RED>"), String(rb_eval_string("Color::RED.inspect")));
  ASSERT_EQUAL(String("#<Color::BLACK>"), String(rb_eval_string("Color::BLACK.inspect")));
  ASSERT_EQUAL(String("#<Color::GREEN>"), String(rb_eval_string("Color::GREEN.inspect")));
}

TESTCASE(compare)
{
  Enum<Color> colorEnum = define_color_enum();
  ASSERT_EQUAL(detail::to_ruby(-1), rb_eval_string("Color::RED <=> Color::BLACK"));
  ASSERT_EQUAL(detail::to_ruby(1), rb_eval_string("Color::GREEN <=> Color::RED"));
  ASSERT_EQUAL(detail::to_ruby(0), rb_eval_string("Color::BLACK <=> Color::BLACK"));
}

TESTCASE(eql)
{
  Enum<Color> colorEnum = define_color_enum();
  ASSERT_EQUAL(detail::to_ruby(false), rb_eval_string("Color::RED == Color::BLACK"));
  ASSERT_EQUAL(detail::to_ruby(true), rb_eval_string("Color::GREEN == Color::GREEN"));
}

TESTCASE(compare_equal)
{
  Enum<Color> colorEnum = define_color_enum();
  Object color1 = colorEnum.const_get("RED");
  Object color2 = colorEnum.const_get("RED");
  Object result = color1.call("<=>", color2);
  ASSERT_EQUAL(0, detail::From_Ruby<int>::convert(result));
}

TESTCASE(compare_less)
{
  Enum<Color> colorEnum = define_color_enum();
  Object color1 = colorEnum.const_get("RED");
  Object color2 = colorEnum.const_get("BLACK");
  Object result = color1.call("<=>", color2);
  ASSERT_EQUAL(-1, detail::From_Ruby<int>::convert(result));
}

TESTCASE(compare_more)
{
  Enum<Color> colorEnum = define_color_enum();
  Object color1 = colorEnum.const_get("GREEN");
  Object color2 = colorEnum.const_get("BLACK");
  Object result = color1.call("<=>", color2);
  ASSERT_EQUAL(1, detail::From_Ruby<int>::convert(result));
}

TESTCASE(different_objects_eql)
{
  Enum<Color> colorEnum = define_color_enum();
  Object black1 = colorEnum.const_get("BLACK");
  Object black2 = colorEnum.const_get("BLACK");
  Object result = black1.call("==", black2);
  ASSERT_EQUAL(Qtrue, result.value());
}

TESTCASE(hash)
{
  Enum<Color> colorEnum = define_color_enum();
  Object color = colorEnum.const_get("GREEN");
  Object result = color.call("hash");
  ASSERT_EQUAL((int)Color::GREEN, detail::From_Ruby<int>::convert(result));
}

TESTCASE(from_int)
{
  Enum<Color> colorEnum = define_color_enum();
  Object result = colorEnum.call("from_int", 2);
  ASSERT_EQUAL(result, colorEnum.const_get("GREEN"));
}

TESTCASE(from_int_invalid)
{
  Enum<Color> colorEnum = define_color_enum();

  ASSERT_EXCEPTION_CHECK(
    Exception,
    Object result = colorEnum.call("from_int", 22),
    ASSERT_EQUAL("Unknown enum value: 22", ex.what())
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
  Enum<Color> colorEnum = define_color_enum();
  colorEnum.define_singleton_function("my_favorite_color", &myFavoriteColor)
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
#ifdef _MSC_VER
  const char* message = "Type is not defined with Rice: enum `anonymous namespace'::Undefined";
#else
  const char* message = "Type is not defined with Rice: (anonymous namespace)::Undefined";
#endif

  ASSERT_EXCEPTION_CHECK(
    std::invalid_argument,
    define_global_function("undefined_arg", &undefinedArg),
    ASSERT_EQUAL(message, ex.what())
  );

  ASSERT_EXCEPTION_CHECK(
    std::invalid_argument,
    define_global_function("undefined_return", &undefinedReturn),
    ASSERT_EQUAL(message, ex.what())
  );
}