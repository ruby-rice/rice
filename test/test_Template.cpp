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
    int rows() const
    {
      return Rows;
    }

    int cols() const
    {
      return Columns;
    }
  };

  template<typename T, int N>
  class Scalar : public Matrix<T, N, 1>
  {
  public:
    int size() const
    {
      return N;
    }
  };
}

template<typename Data_Type_T, typename T, int Rows, int Columns>
void Matrix_builder(Data_Type_T& klass)
{
  klass.define_constructor(Constructor<Matrix<T, Rows, Columns>>())
    .define_method("rows", &Matrix<T, Rows, Columns>::rows)
    .define_method("cols", &Matrix<T, Rows, Columns>::cols);
}

template<typename Data_Type_T, typename T, int N>
void Scalar_builder(Data_Type_T& klass)
{
  klass.define_constructor(Constructor<Scalar<T, N>>())
    .define_method("size", &Scalar<T, N>::size);
}

TESTCASE(matrix)
{
  Class C = define_class<Matrix<float, 5, 4>>("Matrixf54").
    define(&Matrix_builder<Data_Type<Matrix<float, 5, 4>>, float, 5, 4>);

  Object o = C.create();

  Object result = o.instance_eval("cols");
  ASSERT_EQUAL(4, detail::From_Ruby<int>().convert(result.value()));
}

TESTCASE(duplicate_template)
{
  Class C1 = define_class<Matrix<float, 6, 4>>("MatrixFirst").
    define(&Matrix_builder<Data_Type<Matrix<float, 6, 4>>, float, 6, 4>);

  String name = C1.name();
  ASSERT_EQUAL("MatrixFirst", name.str());

  Object aClass1 = Object(rb_cObject).instance_eval("MatrixFirst");
  bool result = aClass1.is_equal(C1);
  ASSERT(result);

  Class C2 = define_class<Matrix<float, 6, 4>>("MatrixSecond").
    define(&Matrix_builder<Data_Type<Matrix<float, 6, 4>>, float, 6, 4>);

  // The first definition name is the one that wins!
  name = C2.name();
  ASSERT_EQUAL("MatrixFirst", name.str());

  Object aClass2 = Object(rb_cObject).instance_eval("MatrixSecond");
  result = aClass2.is_equal(C2);
  ASSERT(result);

  result = C1.is_equal(C2);
  ASSERT(result);
}

TESTCASE(template_inheritance)
{
  Class MatrixClass = define_class<Matrix<float, 5, 1>>("Matrixf51").
    define(&Matrix_builder<Data_Type<Matrix<float, 5, 1>>, float, 5, 1>);

  Class ScalarClass = define_class<Scalar<float, 5>, Matrix<float, 5, 1>>("Scalarf5").
    define(&Scalar_builder<Data_Type<Scalar<float, 5>>, float, 5>);

  Object o = ScalarClass.create();

  Object result = o.instance_eval("size");
  ASSERT_EQUAL(5, detail::From_Ruby<int>().convert(result.value()));

  result = o.instance_eval("rows");
  ASSERT_EQUAL(5, detail::From_Ruby<int>().convert(result.value()));

  result = o.instance_eval("cols");
  ASSERT_EQUAL(1, detail::From_Ruby<int>().convert(result.value()));
}