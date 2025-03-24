#include <iostream>

#include "unittest.hpp"
#include "embed_ruby.hpp"

#include <rice/rice.hpp>

using namespace Rice;

TESTSUITE(Enum);

SETUP(Enum)
{
  embed_ruby();
}

TEARDOWN(Enum)
{
  Rice::detail::Registries::instance.types.clearUnverifiedTypes();
  rb_gc_start();
}


namespace
{
  enum Color { RED, BLACK, GREEN };

  enum class Season { Spring, Summer, Fall, Winter };

  // This is needed to make unittest compile (it uses ostream to report errors)
  inline std::ostream& operator<<(std::ostream& os, const Season& season)
  {
    os << static_cast<std::underlying_type_t<Season>>(season);
    return os;
  }
}

Enum<Color> define_color_enum()
{
  static Enum<Color> colors = define_enum<Color>("Color")
    .define_value("RED", RED)
    .define_value("BLACK", BLACK)
    .define_value("GREEN", GREEN);
  return colors;
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

  Array a = m.module_eval(code);
  ASSERT_EQUAL(3u, a.size());

  Data_Object<Color> enum_0(a[0]);
  ASSERT_EQUAL(RED, *enum_0);
  
  Data_Object<Color> enum_1(a[1]);
  ASSERT_EQUAL(BLACK, *enum_1);
  
  Data_Object<Color> enum_2(a[2]);
  ASSERT_EQUAL(GREEN, *enum_2);
}

TESTCASE(each_return)
{
  Module m = define_module("Testing");

  Enum<Color> colorEnum = define_color_enum();

  std::string code = R"(Color.each {|x|})";
  Object colorEnum2 = m.module_eval(code);
  ASSERT_EQUAL(colorEnum2, Enum<Color>().klass());
}

TESTCASE(to_enum)
{
  Module m = define_module("Testing");

  Enum<Color> colorEnum = define_color_enum();

  std::string code = R"(a = []
                        Color.each.with_index {|x, i| a << x }
                        a)";

  Array a = m.module_eval(code);
  ASSERT_EQUAL(3u, a.size());

  Data_Object<Color> enum_0(a[0]);
  ASSERT_EQUAL(RED, *enum_0);

  Data_Object<Color> enum_1(a[1]);
  ASSERT_EQUAL(BLACK, *enum_1);

  Data_Object<Color> enum_2(a[2]);
  ASSERT_EQUAL(GREEN, *enum_2);

  code = R"(Color.each)";
  Object enumerator = m.module_eval(code);
  ASSERT(enumerator.is_instance_of(rb_cEnumerator));
}

TESTCASE(each_seasons)
{
  Module m = define_module("Testing");

  Enum<Season> rb_cSeason = define_season_enum();
  std::string code = R"(a = []
                        Season.each { |x| a << x }
                        a)";

  Array a = m.module_eval(code);
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

TESTCASE(bitset_operators)
{
  Module m = define_module("TestingBitsetOperators");

  Enum<Season> rb_cSeason = define_season_enum();

  std::string code = R"(Season::Summer & Season::Winter)";
  Object value = m.module_eval(code);
  ASSERT_EQUAL(1, detail::From_Ruby<int>().convert(value));

  code = R"(Season::Spring | Season::Summer)";
  value = m.module_eval(code);
  ASSERT_EQUAL(1, detail::From_Ruby<int>().convert(value));

  code = R"(Season::Spring | Season::Summer | Season::Fall)";
  value = m.module_eval(code);
  ASSERT_EQUAL(3, detail::From_Ruby<int>().convert(value));

  code = R"(Season::Summer ^ Season::Winter)";
  value = m.module_eval(code);
  ASSERT_EQUAL(2, detail::From_Ruby<int>().convert(value));

  code = R"(~Season::Winter)";
  value = m.module_eval(code);
  ASSERT_EQUAL(-4, detail::From_Ruby<int>().convert(value));
}

TESTCASE(shift_operators)
{
  Module m = define_module("TestingShiftOperators");

  Enum<Season> rb_cSeason = define_season_enum();

  std::string code = R"(Season::Winter << 1)";
  Object value = m.module_eval(code);
  ASSERT_EQUAL(6, detail::From_Ruby<int>().convert(value));

  code = R"(Season::Winter >> 1)";
  value = m.module_eval(code);
  ASSERT_EQUAL(1, detail::From_Ruby<int>().convert(value));
}

TESTCASE(to_s)
{
  Module m = define_module("Testing");

  Enum<Color> colorEnum = define_color_enum();
  ASSERT_EQUAL(String("RED"), String(m.module_eval("Color::RED.to_s")));
  ASSERT_EQUAL(String("BLACK"), String(m.module_eval("Color::BLACK.to_s")));
  ASSERT_EQUAL(String("GREEN"), String(m.module_eval("Color::GREEN.to_s")));
}

TESTCASE(to_int)
{
  Module m = define_module("Testing");

  Enum<Color> colorEnum = define_color_enum();
  ASSERT_EQUAL(detail::to_ruby(int(RED)), m.module_eval("Color::RED.to_int").value());
  ASSERT_EQUAL(detail::to_ruby(int(BLACK)), m.module_eval("Color::BLACK.to_int").value());
  ASSERT_EQUAL(detail::to_ruby(int(GREEN)), m.module_eval("Color::GREEN.to_int").value());
}

TESTCASE(inspect)
{
  Module m = define_module("Testing");

  Enum<Color> colorEnum = define_color_enum();
  ASSERT_EQUAL(String("#<Color::RED>"), String(m.module_eval("Color::RED.inspect")));
  ASSERT_EQUAL(String("#<Color::BLACK>"), String(m.module_eval("Color::BLACK.inspect")));
  ASSERT_EQUAL(String("#<Color::GREEN>"), String(m.module_eval("Color::GREEN.inspect")));
}

TESTCASE(compare)
{
  Module m = define_module("Testing");

  Enum<Color> colorEnum = define_color_enum();
  ASSERT_EQUAL(detail::to_ruby(-1), m.module_eval("Color::RED <=> Color::BLACK").value());
  ASSERT_EQUAL(detail::to_ruby(1), m.module_eval("Color::GREEN <=> Color::RED").value());
  ASSERT_EQUAL(detail::to_ruby(0), m.module_eval("Color::BLACK <=> Color::BLACK").value());
}

TESTCASE(eql)
{
  Module m = define_module("Testing");

  Enum<Color> colorEnum = define_color_enum();
  ASSERT_EQUAL(detail::to_ruby(false), m.module_eval("Color::RED == Color::BLACK").value());
  ASSERT_EQUAL(detail::to_ruby(true), m.module_eval("Color::GREEN == Color::GREEN").value());
}

TESTCASE(compare_equal)
{
  Enum<Color> colorEnum = define_color_enum();
  Object color1 = colorEnum.const_get("RED");
  Object color2 = colorEnum.const_get("RED");
  Object result = color1.call("<=>", color2);
  ASSERT_EQUAL(0, detail::From_Ruby<int>().convert(result));
}

TESTCASE(compare_less)
{
  Enum<Color> colorEnum = define_color_enum();
  Object color1 = colorEnum.const_get("RED");
  Object color2 = colorEnum.const_get("BLACK");
  Object result = color1.call("<=>", color2);
  ASSERT_EQUAL(-1, detail::From_Ruby<int>().convert(result));
}

TESTCASE(compare_more)
{
  Enum<Color> colorEnum = define_color_enum();
  Object color1 = colorEnum.const_get("GREEN");
  Object color2 = colorEnum.const_get("BLACK");
  Object result = color1.call("<=>", color2);
  ASSERT_EQUAL(1, detail::From_Ruby<int>().convert(result));
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
  ASSERT_EQUAL((int)Color::GREEN, detail::From_Ruby<int>().convert(result));
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
  Data_Type<Inner> inner = define_class<Inner>("Inner");
  define_enum_under<Inner::Props>("Props", inner)
    .define_value("VALUE1", Inner::VALUE1)
    .define_value("VALUE2", Inner::VALUE2)
    .define_value("VALUE3", Inner::VALUE3);
  inner.define_constructor(Constructor<Inner>());

  Module m = define_module("Testing");

  ASSERT_EQUAL(detail::to_ruby(int(0)), m.module_eval("Inner::Props::VALUE1.to_int").value());
  ASSERT_EQUAL(detail::to_ruby(int(1)), m.module_eval("Inner::Props::VALUE2.to_int").value());
  ASSERT_EQUAL(detail::to_ruby(int(2)), m.module_eval("Inner::Props::VALUE3.to_int").value());
}

namespace
{
  Color myFavoriteColor()
  {
    return RED;
  }

  bool isMyFavoriteColor(Color color)
  {
    return color == RED;
  }

  bool myFavoriteColorAsInt(int color)
  {
    return color == RED;
  }
}

TESTCASE(using_enums)
{
  Enum<Color> colorEnum = define_color_enum();
  colorEnum.define_singleton_function("my_favorite_color", &myFavoriteColor)
           .define_singleton_function("is_my_favorite_color", &isMyFavoriteColor)
           .define_method("is_my_favorite_color", &isMyFavoriteColor);

  Module m = define_module("Testing");

  Object result = m.module_eval("Color.my_favorite_color");
  ASSERT_EQUAL(RED, detail::From_Ruby<Color>().convert(result.value()));

  result = m.module_eval("Color.is_my_favorite_color(Color::RED)");
  ASSERT_EQUAL(Qtrue, result.value());

  result = m.module_eval("Color.is_my_favorite_color(Color::BLACK)");
  ASSERT_EQUAL(Qfalse, result.value());

  result = m.module_eval("Color::RED.is_my_favorite_color");
  ASSERT_EQUAL(Qtrue, result.value());

  result = m.module_eval("Color::BLACK.is_my_favorite_color");
  ASSERT_EQUAL(Qfalse, result.value());
}

TESTCASE(enum_to_int)
{
  Enum<Color> colorEnum = define_color_enum();

  Module m = define_module("Testing");
  m.define_module_function("my_favorite_color_as_int", &myFavoriteColorAsInt);

  std::string code = R"(my_favorite_color_as_int(Color::RED))";
  Object result = m.module_eval(code);
  ASSERT_EQUAL(Qtrue, result.value());

  code = R"(my_favorite_color_as_int(Color::GREEN))";
  result = m.module_eval(code);
  ASSERT_EQUAL(Qfalse, result.value());
}

namespace
{
  bool isMyFavoriteSeasonAsInt(int season)
  {
    return ((Season)season == Season::Summer);
  }
}

TESTCASE(enum_class_to_int)
{
  define_season_enum();

  Module m = define_module("Testing");
  m.define_module_function("is_my_favorite_season_as_int", &isMyFavoriteSeasonAsInt);

  std::string code = R"(is_my_favorite_season_as_int(Season::Spring))";
  Object result = m.module_eval(code);
  ASSERT_EQUAL(Qfalse, result.value());

  code = R"(is_my_favorite_season_as_int(Season::Summer))";
  result = m.module_eval(code);
  ASSERT_EQUAL(Qtrue, result.value());
}

namespace
{
  Color defaultColor(Color aColor = BLACK)
  {
    return aColor;
  }
}

TESTCASE(default_argument)
{
  define_global_function("default_color", &defaultColor, Arg("aColor") = Color::BLACK);

  Module m = define_module("Testing");
  Object result = m.module_eval("default_color");
  ASSERT_EQUAL(Color::BLACK, detail::From_Ruby<Color>().convert(result.value()));
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
  Module m = define_module("TestingEnumNotDefined");

#ifdef _MSC_VER
  const char* message = "The following types are not registered with Rice:\n  enum `anonymous namespace'::Undefined\n";
#else
  const char* message = "The following types are not registered with Rice:\n  (anonymous namespace)::Undefined\n";
#endif

  m.define_module_function("undefined_arg", &undefinedArg);

  ASSERT_EXCEPTION_CHECK(
    std::invalid_argument,
    Rice::detail::Registries::instance.types.validateTypes(),
    ASSERT_EQUAL(message, ex.what())
  );

#ifdef _MSC_VER
  message = "Type is not registered with Rice: enum `anonymous namespace'::Undefined";
#else
  message = "Type is not registered with Rice: (anonymous namespace)::Undefined";
#endif

  m.define_module_function("undefined_return", &undefinedReturn);
  ASSERT_EXCEPTION_CHECK(
    Rice::Exception,
    m.call("undefined_return"),
    ASSERT_EQUAL(message, ex.what())
  );

#ifdef _MSC_VER
  message = "Type is not defined with Rice: enum `anonymous namespace'::Undefined";
#else
  message = "Type is not defined with Rice: (anonymous namespace)::Undefined";
#endif

  ASSERT_EXCEPTION_CHECK(
    Rice::Exception,
    m.call("undefined_arg", 1),
    ASSERT_EQUAL(message, ex.what())
  );
}