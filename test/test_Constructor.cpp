#include "unittest.hpp"
#include "rice/Constructor.hpp"
#include "rice/Data_Type.hpp"
#include "rice/Constructor.hpp"

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

