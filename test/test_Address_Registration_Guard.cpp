#include "unittest.hpp"
#include "rice/Address_Registration_Guard.hpp"

using namespace Rice;

TESTSUITE(Address_Registration_Guard);

SETUP(Address_Registration_Guard)
{
  ruby_init();
}

TESTCASE(register_address)
{
  VALUE v = Qnil;
  Address_Registration_Guard g(&v);
}

TESTCASE(register_object)
{
  Object o;
  Address_Registration_Guard g(&o);
}

TESTCASE(get_address)
{
  VALUE v = Qnil;
  Address_Registration_Guard g(&v);
  ASSERT_EQUAL(&v, g.address());
}

TESTCASE(swap)
{
  VALUE v = Qnil;
  VALUE v2 = Qnil;
  Address_Registration_Guard g(&v);
  Address_Registration_Guard g2(&v2);
  g.swap(g2);
  ASSERT_EQUAL(&v, g2.address());
  ASSERT_EQUAL(&v2, g.address());
  // TODO: ensure addresses are still registered
}

