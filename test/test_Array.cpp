#include "unittest.hpp"
#include "embed_ruby.hpp"
#include "rice/Array.hpp"
#include "rice/String.hpp"
#include "rice/global_function.hpp"

using namespace Rice;

TESTSUITE(Array);

SETUP(Array)
{
  embed_ruby();
}

TESTCASE(default_construct)
{
  Array a;
  ASSERT_EQUAL(T_ARRAY, rb_type(a));
  ASSERT_EQUAL(0, RARRAY_LEN(a.value()));
}

TESTCASE(construct_from_vector_of_int)
{
  std::vector<int> v;
  v.push_back(10);
  v.push_back(6);
  v.push_back(42);
  Array a(v.begin(), v.end());
  ASSERT_EQUAL(3u, a.size());
  ASSERT_EQUAL(to_ruby(10), a[0]);
  ASSERT_EQUAL(to_ruby(6), a[1]);
  ASSERT_EQUAL(to_ruby(42), a[2]);
}

TESTCASE(construct_from_c_array)
{
  int arr[] = { 10, 6, 42 };
  Array a(arr);
  ASSERT_EQUAL(3u, a.size());
  ASSERT_EQUAL(to_ruby(10), a[0]);
  ASSERT_EQUAL(to_ruby(6), a[1]);
  ASSERT_EQUAL(to_ruby(42), a[2]);
}


TESTCASE(push_no_items)
{
  Array a;
  ASSERT_EQUAL(0u, a.size());
}

TESTCASE(push_one_item)
{
  Array a;
  a.push(Rice::True);
  ASSERT_EQUAL(1u, a.size());
  ASSERT_EQUAL(Qtrue, a[0]);
}

TESTCASE(push_two_items)
{
  Array a;
  a.push(to_ruby(42));
  a.push(to_ruby(43));
  ASSERT_EQUAL(2u, a.size());
  ASSERT_EQUAL(42, from_ruby<int>(a[0].value()));
  ASSERT_EQUAL(43, from_ruby<int>(a[1].value()));
}

TESTCASE(push_three_items)
{
  Array a;
  a.push(to_ruby(42));
  a.push(to_ruby(43));
  a.push(to_ruby(44));
  ASSERT_EQUAL(3u, a.size());
  ASSERT_EQUAL(42, from_ruby<int>(a[0].value()));
  ASSERT_EQUAL(43, from_ruby<int>(a[1].value()));
  ASSERT_EQUAL(44, from_ruby<int>(a[2].value()));
}

TESTCASE(push_int)
{
  Array a;
  a.push(42);
  ASSERT_EQUAL(1u, a.size());
  ASSERT_EQUAL(to_ruby(42), a[0]);
}

TESTCASE(bracket_equals)
{
  Array a;
  a.push(to_ruby(42));
  a.push(to_ruby(43));
  a.push(to_ruby(44));
  a[1] = to_ruby(10);
  ASSERT_EQUAL(10, from_ruby<int>(a[1].value()));
}

TESTCASE(to_s)
{
  Array a;
  a.push(to_ruby(42));
  a.push(to_ruby(43));
  a.push(to_ruby(44));
  String s1(a.call("to_s"));
  String s2(a.to_s());
  ASSERT_EQUAL(s1.str(), s2.str());
}

TESTCASE(pop)
{
  Array a;
  a.push(to_ruby(42));
  a.push(to_ruby(43));
  a.push(to_ruby(44));
  VALUE result = a.pop();
  ASSERT_EQUAL(2u, a.size());
  ASSERT_EQUAL(42, from_ruby<int>(a[0].value()));
  ASSERT_EQUAL(43, from_ruby<int>(a[1].value()));
  ASSERT_EQUAL(44, from_ruby<int>(result));
}

TESTCASE(unshift)
{
  Array a;
  a.push(to_ruby(42));
  a.push(to_ruby(43));
  a.push(to_ruby(44));
  a.unshift(to_ruby(10));
  ASSERT_EQUAL(4u, a.size());
  ASSERT_EQUAL(10, from_ruby<int>(a[0].value()));
  ASSERT_EQUAL(42, from_ruby<int>(a[1].value()));
  ASSERT_EQUAL(43, from_ruby<int>(a[2].value()));
  ASSERT_EQUAL(44, from_ruby<int>(a[3].value()));
}

TESTCASE(unshift_int)
{
  Array a;
  a.unshift(42);
  ASSERT_EQUAL(1u, a.size());
  ASSERT_EQUAL(to_ruby(42), a[0]);
}

TESTCASE(shift)
{
  Array a;
  a.push(to_ruby(42));
  a.push(to_ruby(43));
  a.push(to_ruby(44));
  VALUE result = a.shift();
  ASSERT_EQUAL(2u, a.size());
  ASSERT_EQUAL(42, from_ruby<int>(result));
  ASSERT_EQUAL(43, from_ruby<int>(a[0].value()));
  ASSERT_EQUAL(44, from_ruby<int>(a[1].value()));
}

TESTCASE(iterate)
{
  Array a;
  a.push(to_ruby(42));
  a.push(to_ruby(43));
  a.push(to_ruby(44));
  int ca[] = { 42, 43, 44 };
  Array::iterator it = a.begin();
  Array::iterator end = a.end();
  for(int j = 0; it != end; ++j, ++it)
  {
    ASSERT_EQUAL(ca[j], from_ruby<int>(it->value()));
  }
}

TESTCASE(const_iterate)
{
  Array a;
  a.push(to_ruby(42));
  a.push(to_ruby(43));
  a.push(to_ruby(44));
  int ca[] = { 42, 43, 44 };
  Array::const_iterator it = a.begin();
  Array::const_iterator end = a.end();
  for(int j = 0; it != end; ++j, ++it)
  {
    ASSERT_EQUAL(ca[j], from_ruby<int>(*it));
  }
}

TESTCASE(iterate_and_change)
{
  Array a;
  a.push(to_ruby(42));
  a.push(to_ruby(43));
  a.push(to_ruby(44));
  Array::iterator it = a.begin();
  Array::iterator end = a.end();
  for(int j = 0; it != end; ++j, ++it)
  {
    int value = from_ruby<int>(it->value());
    *it = to_ruby(value + j);
  }
  ASSERT_EQUAL(42, from_ruby<int>(a[0].value()));
  ASSERT_EQUAL(44, from_ruby<int>(a[1].value()));
  ASSERT_EQUAL(46, from_ruby<int>(a[2].value()));
}

TESTCASE(iterate_and_call_member)
{
  Array a;
  a.push(to_ruby(42));
  a.push(to_ruby(43));
  a.push(to_ruby(44));
  Array::iterator it = a.begin();
  Array::iterator end = a.end();
  std::vector<Object> v;
  for(int j = 0; it != end; ++j, ++it)
  {
    v.push_back(it->to_s());
  }
  ASSERT_EQUAL(42, from_ruby<int>(a[0].value()));
  ASSERT_EQUAL(43, from_ruby<int>(a[1].value()));
  ASSERT_EQUAL(44, from_ruby<int>(a[2].value()));
  ASSERT_EQUAL(3u, v.size());
  ASSERT_EQUAL(Object(a[0]).to_s(), v[0]);
  ASSERT_EQUAL(Object(a[1]).to_s(), v[1]);
  ASSERT_EQUAL(Object(a[2]).to_s(), v[2]);
}

TESTCASE(assign_int)
{
  Array a;
  a.push(42);
  a[0] = 10;
  ASSERT_EQUAL(10, from_ruby<int>(a[0].value()));
}

/**
 * Issue 59 - Copy constructor compilation problem.
 */

namespace {
  void testArrayArg(Object self, Array string) {
  }
}

TESTCASE(use_array_in_wrapped_function) {
  define_global_function("test_array_arg", &testArrayArg);
}
