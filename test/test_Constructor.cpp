#include "unittest.hpp"
#include "rice/Constructor.hpp"
#include "rice/Data_Type.hpp"

#include "rice/detail/env.hpp"

#include <iostream>
using namespace std;

using namespace Rice;

TESTSUITE(Constructor);

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

TESTCASE(default_constructor)
{
  Data_Type<Default_Constructible> rb_cDefault_Constructible(
      anonymous_class());
  rb_cDefault_Constructible
    .define_constructor(Constructor<Default_Constructible>());
  Object o = rb_cDefault_Constructible.call("new");
  ASSERT_EQUAL(rb_cDefault_Constructible, o.class_of());
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
