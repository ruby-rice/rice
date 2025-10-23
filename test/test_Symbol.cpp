#include "unittest.hpp"
#include "embed_ruby.hpp"
#include <rice/rice.hpp>

using namespace Rice;

TESTSUITE(Symbol);

SETUP(Symbol)
{
  embed_ruby();
}

TEARDOWN(Symbol)
{
  rb_gc_start();
}

TESTCASE(construct_from_symbol)
{
  VALUE v = ID2SYM(rb_intern("foo"));
  Symbol symbol(v);
  ASSERT_EQUAL(v, symbol.value());
}

TESTCASE(construct_from_identifier)
{
  Identifier identifier("FOO");
  Symbol symbol(identifier);
  ASSERT_EQUAL(ID2SYM(rb_intern("FOO")), symbol.value());
}

TESTCASE(construct_from_string)
{
  Symbol symbol("Foo");
  ASSERT_EQUAL(ID2SYM(rb_intern("Foo")), symbol.value());
}

TESTCASE(construct_from_string_view)
{
  std::string_view view("Foo");
  Symbol symbol(view);
  ASSERT_EQUAL(ID2SYM(rb_intern("Foo")), symbol.value());
}

TESTCASE(default_construct)
{
  Symbol symbol;
  ASSERT_EQUAL(ID2SYM(rb_intern("")), symbol.value());
}

TESTCASE(copy_construct)
{
  Symbol symbol1("Foo");
  Symbol symbol2(symbol1);
  ASSERT_EQUAL(ID2SYM(rb_intern("Foo")), symbol2.value());
}

TESTCASE(c_str)
{
  Symbol symbol("Foo");
  ASSERT_EQUAL("Foo", symbol.c_str());
}

TESTCASE(str)
{
  Symbol symbol("Foo");
  ASSERT_EQUAL(std::string("Foo"), symbol.str());
}

TESTCASE(to_id)
{
  Symbol symbol("Foo");
  ASSERT_EQUAL(Identifier("Foo"), symbol.to_id());
}

namespace
{
  void testSymbolArg(Object self, Symbol string)
  {
  }
}

TESTCASE(use_symbol_in_wrapped_function)
{
  define_global_function("test_symbol_arg", &testSymbolArg);
}
