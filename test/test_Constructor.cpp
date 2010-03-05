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

/*
namespace
{
  class WithDefaultArgs
  {
    public:
      WithDefaultArgs(int x, float y = 2.0, bool yes = false)
      {
        x_ = x;
        y_ = y;
        yes_ = yes;
      }

      WithDefaultArgs(int x = 14) {
        x_ = x;
        y_ = 1.0;
        yes_ = false;
      }

      int getX() { return x_; }
      float getY() { return y_; }
      bool getYes() { return yes_; }

    private:

      int x_;
      float y_;
      bool yes_;
  };
}

TESTCASE(constructor_supports_default_arguments)
{
  Class klass = define_class<WithDefaultArgs>("WithDefaultArgs").
    define_constructor(Constructor<WithDefaultArgs, int, float, bool>(
          ( Arg("x"), Arg("y") = (float)2.0, Arg("yes") = (bool)false )));

  Data_Object<WithDefaultArgs> obj;

  obj= klass.call("new", 4);
  ASSERT_EQUAL(4, obj->getX());
  ASSERT_EQUAL(2.0, obj->getY());
  ASSERT_EQUAL(false, obj->getYes());

  obj= klass.call("new", 5, 3.0);
  ASSERT_EQUAL(5, obj->getX());
  ASSERT_EQUAL(3.0, obj->getY());
  ASSERT_EQUAL(false, obj->getYes());

  obj= klass.call("new", 7, 12.0, true);
  ASSERT_EQUAL(7, obj->getX());
  ASSERT_EQUAL(12.0, obj->getY());
  ASSERT_EQUAL(true, obj->getYes());
}

TESTCASE(constructor_supports_single_default_argument)
{

}
*/
