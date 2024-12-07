#include <assert.h> 

#include "unittest.hpp"
#include "embed_ruby.hpp"
#include <rice/rice.hpp>
#include <rice/stl.hpp>

using namespace Rice;

TESTSUITE(Template);

SETUP(Template)
{
  embed_ruby();
}

TEARDOWN(Template)
{
  rb_gc_start();
}

namespace
{
  template<typename T>
  class MyVector
  {
  public:
    MyVector() = default;

    void add(T& element)
    {
      this->vector_.push_back(element);
      this->empty = false;
    }

    size_t size()
    {
      return this->vector_.size();
    }

    bool empty = true;
  private:
    std::vector<T> vector_;
  };
}

template<typename T>
void builder_function(Data_Type<T>& klass)
{
  klass.define_constructor(Constructor<T>())
    .define_method("add", &T::add)
    .define_method("size", &T::size)
    .define_attr("empty", &T::empty, Rice::AttrAccess::Read);
}

TESTCASE(my_vector)
{
  Class C1 = define_class_template<MyVector, int>("MyVecInt", builder_function);
  Object o1 = C1.create();
  
  Object result1 = o1.instance_eval("empty");
  ASSERT_EQUAL(Qtrue, result1.value());

  result1 = o1.instance_eval("size");
  ASSERT_EQUAL(0, detail::From_Ruby<int>().convert(result1.value()));

  o1.call("add", 5);
  result1 = o1.instance_eval("empty");
  ASSERT_EQUAL(Qfalse, o1.instance_eval("empty").value());

  result1 = o1.instance_eval("size");
  ASSERT_EQUAL(1, detail::From_Ruby<int>().convert(result1.value()));

  Class C2 = define_class_template<MyVector, std::string>("MyVecString", &builder_function);
  Object o2 = C2.create();
  Object result2 = o2.instance_eval("empty");
  ASSERT_EQUAL(Qtrue, result2.value());

  result2 = o2.instance_eval("size");
  ASSERT_EQUAL(0, detail::From_Ruby<int>().convert(result2.value()));

  o2.call("add", "five");
  result2 = o2.instance_eval("empty");
  ASSERT_EQUAL(Qfalse, result2.value());

  result2 = o2.instance_eval("size");
  ASSERT_EQUAL(1, detail::From_Ruby<int>().convert(result2.value()));
}

TESTCASE(my_vector_lambda)
{
  auto builder_lambda = [](auto& klass)
    {
      using T = std::remove_reference_t<decltype(klass)>::type;
      klass.define_constructor(Constructor<T>())
        .define_method("add", &T::add)
        .define_method("size", &T::size)
        .define_attr("empty", &T::empty, Rice::AttrAccess::Read);
    };

  Class C = define_class_template<MyVector, float>("MyVecFloat", builder_lambda);
  Object o = C.create();

  Object result = o.instance_eval("empty");
  ASSERT_EQUAL(Qtrue, result.value());

  result = o.instance_eval("size");
  ASSERT_EQUAL(0, detail::From_Ruby<int>().convert(result.value()));

  o.call("add", 5.0);
  result = o.instance_eval("empty");
  ASSERT_EQUAL(Qfalse, o.instance_eval("empty").value());

  result = o.instance_eval("size");
  ASSERT_EQUAL(1, detail::From_Ruby<int>().convert(result.value()));
}

namespace
{
  template<typename T, int x, int y>
  class Matrix
  {
    Matrix(T something)
    {
    }

    Matrix<T, x, y> col(int i) const
    {
      return Matrix<T, x, y>();
    }
  };
}

TESTCASE(matrix)
{
  auto builder_lambda = [](auto& klass)
    {
      using T = std::remove_reference_t<decltype(klass)>::type;
      klass.define_constructor(Constructor<T>())
        .define_method("add", &T::add)
        .define_method("size", &T::size)
        .define_attr("empty", &T::empty, Rice::AttrAccess::Read);
    };

  Class C = define_class_template<MyVector, float>("MyVecFloat", builder_lambda);
  Object o = C.create();

  Object result = o.instance_eval("empty");
  ASSERT_EQUAL(Qtrue, result.value());

  result = o.instance_eval("size");
  ASSERT_EQUAL(0, detail::From_Ruby<int>().convert(result.value()));

  o.call("add", 5.0);
  result = o.instance_eval("empty");
  ASSERT_EQUAL(Qfalse, o.instance_eval("empty").value());

  result = o.instance_eval("size");
  ASSERT_EQUAL(1, detail::From_Ruby<int>().convert(result.value()));
}