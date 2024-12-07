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

template<typename Data_Type_T, typename T>
void MyVector_builder(Data_Type_T& klass)
{
  klass.define_constructor(Constructor<MyVector<T>>())
    .define_method("add", &MyVector<T>::add)
    .define_method("size", &MyVector<T>::size)
    .define_attr("empty", &MyVector<T>::empty, Rice::AttrAccess::Read);
}

TESTCASE(my_vector)
{
  Class C1 = define_class<MyVector<int>>("MyVecInt").
             define(&MyVector_builder<Data_Type<MyVector<int>>, int>);

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

  Class C2 = define_class<MyVector< std::string>>("MyVecInt").
             define(&MyVector_builder<Data_Type<MyVector<std::string>>, std::string>);

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

namespace
{
  template<typename T, int Rows, int Columns>
  class Matrix
  {
  public:
    Matrix()
    {
    }

    int cols() const
    {
      return Columns;
    }
  };
}

template<typename Data_Type_T, typename T, int Rows, int Columns>
void Matrix_builder(Data_Type_T& klass)
{
  klass.define_constructor(Constructor<Matrix<T, Rows, Columns>>())
    .define_method("cols", &Matrix<T, Rows, Columns>::cols);
}

TESTCASE(matrix)
{
  Class C = define_class<Matrix<float, 5, 4>>("Matrixf54").
    define(&Matrix_builder<Data_Type<Matrix<float, 5, 4>>, float, 5, 4>);

  Object o = C.create();

  Object result = o.instance_eval("cols");
  ASSERT_EQUAL(4, detail::From_Ruby<int>().convert(result.value()));
}