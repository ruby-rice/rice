#include "unittest.hpp"
#include "embed_ruby.hpp"
#include <rice/rice.hpp>

using namespace Rice;

TESTSUITE(Pin);

SETUP(Pin)
{
  embed_ruby();
}

TEARDOWN(Pin)
{
  rb_gc_start();
}

namespace
{
  // A C++ class that holds a Ruby VALUE as a member
  class Container
  {
  public:
    Container(VALUE value) : pin_(value)
    {
    }

    VALUE getValue() const
    {
      return pin_.get();
    }

    void setValue(VALUE value)
    {
      pin_.set(value);
    }

  private:
    Pin pin_;
  };
}

TESTCASE(prevents_gc_collection)
{
  Container* container = nullptr;

  // Create container with a Ruby string in a nested scope
  {
    VALUE str = rb_str_new_cstr("prevent gc test");
    container = new Container(str);
  }

  // Force garbage collection - the string should survive
  rb_gc_start();

  // Verify the string is still valid and accessible
  VALUE retrieved = container->getValue();
  ASSERT_EQUAL("prevent gc test", detail::From_Ruby<char*>().convert(retrieved));

  delete container;
}

TESTCASE(update_pinned_value)
{
  Container container(rb_str_new_cstr("first"));

  rb_gc_start();
  ASSERT_EQUAL("first", detail::From_Ruby<char*>().convert(container.getValue()));

  container.setValue(rb_str_new_cstr("second"));

  rb_gc_start();
  ASSERT_EQUAL("second", detail::From_Ruby<char*>().convert(container.getValue()));
}

TESTCASE(copy_container)
{
  Container c1(rb_str_new_cstr("shared value"));
  Container c2 = c1;

  rb_gc_start();

  // Both containers share the same anchor
  ASSERT_EQUAL("shared value", detail::From_Ruby<char*>().convert(c1.getValue()));
  ASSERT_EQUAL("shared value", detail::From_Ruby<char*>().convert(c2.getValue()));
}

TESTCASE(move_container)
{
  Container c1(rb_str_new_cstr("moved value"));
  Container c2 = std::move(c1);

  rb_gc_start();

  ASSERT_EQUAL("moved value", detail::From_Ruby<char*>().convert(c2.getValue()));
}
