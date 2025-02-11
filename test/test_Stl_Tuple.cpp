#include "unittest.hpp"
#include "embed_ruby.hpp"
#include <rice/rice.hpp>
#include <rice/stl.hpp>

#include <tuple>
#include <complex>

using namespace Rice;
using namespace std::complex_literals;

TESTSUITE(Tuple);

SETUP(Tuple)
{
  embed_ruby();
}

TEARDOWN(Tuple)
{
  rb_gc_start();
}

namespace
{
  /*inline std::ostream& operator<<(std::ostream& stream, Tuple_T const& tuple)
  {
    stream << "Tuple(" << "index: " << tuple.index() << ")";
    return stream;
  }*/

  class MyTupleClass
  {
  public:
    using Tuple_T = std::tuple<std::string,
                               std::complex<double>,
                               std::vector<int>,
                               double,
                               bool,
                               int>;

    Tuple_T someTuple()
    {
      Tuple_T result{ "Hello", 1i, std::vector<int>{1, 2, 3}, 3.3, true, 5 };
      return result;
    }

    bool validateTuple(Tuple_T tuple)
    {
      ASSERT_EQUAL("Ruby", std::get<0>(tuple).c_str());

      std::complex<double> complex = std::get<1>(tuple);
      ASSERT_EQUAL(2, complex.real());
      ASSERT_EQUAL(3, complex.imag());
       
      std::vector<int> vec = std::get<2>(tuple);
      ASSERT_EQUAL(3, vec.size());
      ASSERT_EQUAL(3, vec[0]);
      ASSERT_EQUAL(4, vec[1]);
      ASSERT_EQUAL(5, vec[2]);


      ASSERT_EQUAL(4.4, std::get<3>(tuple));
      ASSERT_EQUAL(false, std::get<4>(tuple));
      ASSERT_EQUAL(9, std::get<5>(tuple));

      return true;
    }
  };
}

Data_Type<MyTupleClass> defineMyTupleClass()
{
  return define_class<MyTupleClass>("MyTupleClass").
    define_constructor(Constructor<MyTupleClass>()).
    define_method("some_tuple", &MyTupleClass::someTuple).
    define_method("validate_tuple", &MyTupleClass::validateTuple);
}

TESTCASE(TupleToRuby)
{
  Data_Type<MyTupleClass> klass = defineMyTupleClass();

  Module m = define_module("TupleTesting");

  std::string code = R"(my_class = MyTupleClass.new
                        my_class.some_tuple)";

  Array result = m.module_eval(code);
  
  ASSERT_EQUAL(6, result.size());

  ASSERT_EQUAL("Hello", detail::From_Ruby<std::string>().convert(result[0].value()).c_str());
  ASSERT_EQUAL(1i, detail::From_Ruby<std::complex<double>>().convert(result[1].value()));
  
  std::vector<int> converted = detail::From_Ruby<std::vector<int>>().convert(result[2].value());
  ASSERT_EQUAL(3, converted.size());

  ASSERT_EQUAL(3.3, detail::From_Ruby<double>().convert(result[3].value()));
  ASSERT_EQUAL(Qtrue, result[4].value());
  ASSERT_EQUAL(5, detail::From_Ruby<int>().convert(result[5].value()));
}

TESTCASE(TupleFromRuby)
{
  Data_Type<MyTupleClass> klass = defineMyTupleClass();

  Module m = define_module("TupleTesting");

  std::string code = R"(my_class = MyTupleClass.new
                        array = ["Ruby", Complex(2, 3), [3, 4, 5], 4.4, false, 9]
                        my_class.validate_tuple(array))";

  Object result = m.module_eval(code);
  ASSERT_EQUAL(Qtrue, result.value());
}
