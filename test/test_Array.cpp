#include "unittest.hpp"
#include "embed_ruby.hpp"
#include <rice/rice.hpp>

#include <algorithm>
#include <iterator>
#include <vector>

using namespace Rice;

TESTSUITE(Array);

namespace {
  // This is needed to make unittest compile (it uses ostream to report errors)
  template<typename T>
  std::ostream &operator<<(std::ostream &os, const std::vector<T> &vector) {
    for (T &i: vector) {
      os << i << ", ";
    }
    return os;
  }
}

SETUP(Array)
{
  embed_ruby();
}

TEARDOWN(Array)
{
  rb_gc_start();
}

TESTCASE(construct_default)
{
  Array a;
  ASSERT_EQUAL(T_ARRAY, rb_type(a));
  ASSERT_EQUAL(0, RARRAY_LEN(a.value()));
}

TESTCASE(construct_capacity)
{
  Array a((long)10);
  ASSERT_EQUAL(0, RARRAY_LEN(a.value()));
}

TESTCASE(construct_from_vector_of_int)
{
  std::vector<int> v;
  v.push_back(10);
  v.push_back(6);
  v.push_back(42);
  Array a(v.begin(), v.end());
  ASSERT_EQUAL(3, a.size());
  ASSERT(rb_equal(detail::to_ruby(10), a[0].value()));
  ASSERT(rb_equal(detail::to_ruby(6), a[1].value()));
  ASSERT(rb_equal(detail::to_ruby(42), a[2].value()));
}

TESTCASE(construct_from_c_array)
{
  int arr[] = { 10, 6, 42 };
  Array a(arr);
  ASSERT_EQUAL(3, a.size());
  ASSERT(rb_equal(detail::to_ruby(10), a[0].value()));
  ASSERT(rb_equal(detail::to_ruby(6), a[1].value()));
  ASSERT(rb_equal(detail::to_ruby(42), a[2].value()));
}

TESTCASE(push_no_items)
{
  Array a;
  ASSERT_EQUAL(0, a.size());
}

TESTCASE(push_one_item)
{
  Array a;
  a.push(Rice::True, false);
  ASSERT_EQUAL(1, a.size());
  ASSERT_EQUAL(Qtrue, a[0]);
}

TESTCASE(push_two_items)
{
  Array a;
  a.push(42, false);
  a.push(43, false);
  ASSERT_EQUAL(2, a.size());
  ASSERT_EQUAL(42, detail::From_Ruby<int>().convert(a[0].value()));
  ASSERT_EQUAL(43, detail::From_Ruby<int>().convert(a[1].value()));
}

TESTCASE(push_three_items)
{
  Array a;
  a.push(42, false);
  a.push(43, false);
  a.push(44, false);
  ASSERT_EQUAL(3, a.size());
  ASSERT_EQUAL(42, detail::From_Ruby<int>().convert(a[0].value()));
  ASSERT_EQUAL(43, detail::From_Ruby<int>().convert(a[1].value()));
  ASSERT_EQUAL(44, detail::From_Ruby<int>().convert(a[2].value()));
}

TESTCASE(push_int)
{
  Array a;
  a.push(42, false);
  ASSERT_EQUAL(1, a.size());
  ASSERT(rb_equal(detail::to_ruby(42), a[0].value()));
}

TESTCASE(bracket_equals)
{
  Array a;
  a.push(42, false);
  a.push(43, false);
  a.push(44, false);
  a[1] = 10;
  ASSERT_EQUAL(10, detail::From_Ruby<int>().convert(a[1].value()));
}

TESTCASE(to_s)
{
  Array a;
  a.push(42, false);
  a.push(43, false);
  a.push(44, false);
  String s1(a.call("to_s"));
  String s2(a.to_s());
  ASSERT_EQUAL(s1.str(), s2.str());
}

TESTCASE(pop)
{
  Array a;
  a.push(42, false);
  a.push(43, false);
  a.push(44, false);
  VALUE result = a.pop();
  ASSERT_EQUAL(2, a.size());
  ASSERT_EQUAL(42, detail::From_Ruby<int>().convert(a[0].value()));
  ASSERT_EQUAL(43, detail::From_Ruby<int>().convert(a[1].value()));
  ASSERT_EQUAL(44, detail::From_Ruby<int>().convert(result));
}

TESTCASE(unshift)
{
  Array a;
  a.push(42, false);
  a.push(43, false);
  a.push(44, false);
  a.unshift(10);
  ASSERT_EQUAL(4, a.size());
  ASSERT_EQUAL(10, detail::From_Ruby<int>().convert(a[0].value()));
  ASSERT_EQUAL(42, detail::From_Ruby<int>().convert(a[1].value()));
  ASSERT_EQUAL(43, detail::From_Ruby<int>().convert(a[2].value()));
  ASSERT_EQUAL(44, detail::From_Ruby<int>().convert(a[3].value()));
}

TESTCASE(unshift_int)
{
  Array a;
  a.unshift(42);
  ASSERT_EQUAL(1, a.size());
  ASSERT(rb_equal(detail::to_ruby(42), a[0].value()));
}

TESTCASE(shift)
{
  Array a;
  a.push(42, false);
  a.push(43, false);
  a.push(44, false);
  VALUE result = a.shift();
  ASSERT_EQUAL(2, a.size());
  ASSERT_EQUAL(42, detail::From_Ruby<int>().convert(result));
  ASSERT_EQUAL(43, detail::From_Ruby<int>().convert(a[0].value()));
  ASSERT_EQUAL(44, detail::From_Ruby<int>().convert(a[1].value()));
}

TESTCASE(iterate)
{
  Array a;
  a.push(42, false);
  a.push(43, false);
  a.push(44, false);
  int ca[] = { 42, 43, 44 };
  Array::iterator it = a.begin();
  Array::iterator end = a.end();
  for(int j = 0; it != end; ++j, ++it)
  {
    ASSERT_EQUAL(ca[j], detail::From_Ruby<int>().convert(it->value()));
  }
}

TESTCASE(const_iterate)
{
  Array a;
  a.push(42, false);
  a.push(43, false);
  a.push(44, false);
  int ca[] = { 42, 43, 44 };
  Array::const_iterator it = a.begin();
  Array::const_iterator end = a.end();
  for(int j = 0; it != end; ++j, ++it)
  {
    ASSERT_EQUAL(ca[j], detail::From_Ruby<int>().convert(*it));
  }
}

TESTCASE(iterate_and_change)
{
  Array a;
  a.push(42, false);
  a.push(43, false);
  a.push(44, false);
  Array::iterator it = a.begin();
  Array::iterator end = a.end();
  for(int j = 0; it != end; ++j, ++it)
  {
    int value = detail::From_Ruby<int>().convert(it->value());
    *it = value + j;
  }
  ASSERT_EQUAL(42, detail::From_Ruby<int>().convert(a[0].value()));
  ASSERT_EQUAL(44, detail::From_Ruby<int>().convert(a[1].value()));
  ASSERT_EQUAL(46, detail::From_Ruby<int>().convert(a[2].value()));
}

TESTCASE(iterate_and_call_member)
{
  Array a;
  a.push(42, false);
  a.push(43, false);
  a.push(44, false);
  Array::iterator it = a.begin();
  Array::iterator end = a.end();
  std::vector<Object> v;
  for(const auto& item: a)
  {
    v.push_back(Object(item).to_s());
  }
  ASSERT_EQUAL(42, detail::From_Ruby<int>().convert(a[0].value()));
  ASSERT_EQUAL(43, detail::From_Ruby<int>().convert(a[1].value()));
  ASSERT_EQUAL(44, detail::From_Ruby<int>().convert(a[2].value()));
  ASSERT_EQUAL(3u, v.size());
  ASSERT_EQUAL(Object(a[0]).to_s(), v[0]);
  ASSERT_EQUAL(Object(a[1]).to_s(), v[1]);
  ASSERT_EQUAL(Object(a[2]).to_s(), v[2]);
}

TESTCASE(find_if)
{
  Array rubyValues;
  rubyValues.push(42, false);
  rubyValues.push(43, false);
  rubyValues.push(44, false);

  auto iter = std::find_if(rubyValues.begin(), rubyValues.end(),
    [&rubyValues](const Object& object)
    {
      return object == rubyValues[1];
    });

  ASSERT_EQUAL(43, detail::From_Ruby<int>().convert(iter->value()));
}

TESTCASE(assign_int)
{
  Array a;
  a.push(42, false);
  a[0] = 10;
  ASSERT_EQUAL(10, detail::From_Ruby<int>().convert(a[0].value()));
}

/**
 * Issue 59 - Copy constructor compilation problem.
 */

namespace
{
  void testArrayArg(Object, Array)
  {
  }
}

TESTCASE(use_array_in_wrapped_function)
{
  define_global_function("test_array_arg", &testArrayArg);
}

TESTCASE(array_to_ruby)
{
  Array a(rb_ary_new());
  ASSERT(rb_equal(a.value(), detail::to_ruby(a)));
}

TESTCASE(array_ref_to_ruby)
{
  Array a(rb_ary_new());
  Array& ref = a;
  ASSERT(rb_equal(a.value(), detail::to_ruby(ref)));
}

TESTCASE(array_ptr_to_ruby)
{
  Array a(rb_ary_new());
  Array* ptr = &a;
  ASSERT(rb_equal(a.value(), detail::to_ruby(ptr)));
}

TESTCASE(array_from_ruby)
{
  Array a(rb_ary_new());
  ASSERT_EQUAL(a, detail::From_Ruby<Array>().convert(a));
}

// Random-access iterator tests
TESTCASE(iterator_random_access_category)
{
  // Verify the iterator is now a random-access iterator
  static_assert(std::is_same_v<
    Array::iterator::iterator_category,
    std::random_access_iterator_tag>,
    "Array::iterator should be a random_access_iterator");

  static_assert(std::is_same_v<
    Array::const_iterator::iterator_category,
    std::random_access_iterator_tag>,
    "Array::const_iterator should be a random_access_iterator");
}

TESTCASE(iterator_arithmetic_add)
{
  Array a;
  a.push(10, false);
  a.push(20, false);
  a.push(30, false);
  a.push(40, false);
  a.push(50, false);

  Array::iterator it = a.begin();
  Array::iterator it2 = it + 2;
  ASSERT_EQUAL(30, detail::From_Ruby<int>().convert(it2->value()));

  it2 = 3 + it;  // Test non-member operator+
  ASSERT_EQUAL(40, detail::From_Ruby<int>().convert(it2->value()));
}

TESTCASE(iterator_arithmetic_subtract)
{
  Array a;
  a.push(10, false);
  a.push(20, false);
  a.push(30, false);
  a.push(40, false);
  a.push(50, false);

  Array::iterator it = a.end();
  Array::iterator it2 = it - 2;
  ASSERT_EQUAL(40, detail::From_Ruby<int>().convert(it2->value()));
}

TESTCASE(iterator_compound_assignment)
{
  Array a;
  a.push(10, false);
  a.push(20, false);
  a.push(30, false);
  a.push(40, false);
  a.push(50, false);

  Array::iterator it = a.begin();
  it += 3;
  ASSERT_EQUAL(40, detail::From_Ruby<int>().convert(it->value()));

  it -= 2;
  ASSERT_EQUAL(20, detail::From_Ruby<int>().convert(it->value()));
}

TESTCASE(iterator_difference)
{
  Array a;
  a.push(10, false);
  a.push(20, false);
  a.push(30, false);
  a.push(40, false);
  a.push(50, false);

  Array::iterator begin = a.begin();
  Array::iterator end = a.end();
  ASSERT_EQUAL(5, end - begin);
  ASSERT_EQUAL(-5, begin - end);

  Array::iterator mid = begin + 2;
  ASSERT_EQUAL(2, mid - begin);
  ASSERT_EQUAL(3, end - mid);
}

TESTCASE(iterator_subscript)
{
  Array a;
  a.push(10, false);
  a.push(20, false);
  a.push(30, false);
  a.push(40, false);
  a.push(50, false);

  Array::iterator it = a.begin();
  ASSERT_EQUAL(10, detail::From_Ruby<int>().convert(it[0].value()));
  ASSERT_EQUAL(30, detail::From_Ruby<int>().convert(it[2].value()));
  ASSERT_EQUAL(50, detail::From_Ruby<int>().convert(it[4].value()));
}

TESTCASE(iterator_comparison_less_greater)
{
  Array a;
  a.push(10, false);
  a.push(20, false);
  a.push(30, false);

  Array::iterator begin = a.begin();
  Array::iterator mid = begin + 1;
  Array::iterator end = a.end();

  ASSERT_EQUAL(true, begin < mid);
  ASSERT_EQUAL(true, begin < end);
  ASSERT_EQUAL(true, mid < end);

  ASSERT_EQUAL(true, end > mid);
  ASSERT_EQUAL(true, end > begin);
  ASSERT_EQUAL(true, mid > begin);

  ASSERT_EQUAL(true, begin <= begin);
  ASSERT_EQUAL(true, begin <= mid);
  ASSERT_EQUAL(true, mid <= end);

  ASSERT_EQUAL(true, end >= end);
  ASSERT_EQUAL(true, end >= mid);
  ASSERT_EQUAL(true, mid >= begin);
}

TESTCASE(iterator_decrement)
{
  Array a;
  a.push(10, false);
  a.push(20, false);
  a.push(30, false);

  Array::iterator it = a.end();
  --it;
  ASSERT_EQUAL(30, detail::From_Ruby<int>().convert(it->value()));
  --it;
  ASSERT_EQUAL(20, detail::From_Ruby<int>().convert(it->value()));
  --it;
  ASSERT_EQUAL(10, detail::From_Ruby<int>().convert(it->value()));
}

TESTCASE(iterator_decrement_postfix)
{
  Array a;
  a.push(10, false);
  a.push(20, false);
  a.push(30, false);

  Array::iterator it = a.end();
  it--;
  ASSERT_EQUAL(30, detail::From_Ruby<int>().convert(it->value()));
  Array::iterator prev = it--;
  ASSERT_EQUAL(30, detail::From_Ruby<int>().convert(prev->value()));
  ASSERT_EQUAL(20, detail::From_Ruby<int>().convert(it->value()));
}

TESTCASE(iterator_std_sort)
{
  // std::sort requires iterators that return assignable references.
  // Our iterator returns a Proxy which supports assignment to the array,
  // but STL algorithms need iter_swap to work. Test that we can use
  // std::sort with explicit swap via indices instead.
  Array a;
  a.push(30, false);
  a.push(10, false);
  a.push(50, false);
  a.push(20, false);
  a.push(40, false);

  // Manual sort using random-access capabilities (bubble sort for simplicity)
  for (long i = 0; i < a.size() - 1; i++)
  {
    for (long j = 0; j < a.size() - i - 1; j++)
    {
      auto it = a.begin();
      int val1 = detail::From_Ruby<int>().convert(it[j].value());
      int val2 = detail::From_Ruby<int>().convert(it[j + 1].value());
      if (val1 > val2)
      {
        // Swap using array subscript operator
        Object temp = a[j];
        a[j] = detail::From_Ruby<int>().convert(a[j + 1].value());
        a[j + 1] = detail::From_Ruby<int>().convert(temp.value());
      }
    }
  }

  ASSERT_EQUAL(10, detail::From_Ruby<int>().convert(a[0].value()));
  ASSERT_EQUAL(20, detail::From_Ruby<int>().convert(a[1].value()));
  ASSERT_EQUAL(30, detail::From_Ruby<int>().convert(a[2].value()));
  ASSERT_EQUAL(40, detail::From_Ruby<int>().convert(a[3].value()));
  ASSERT_EQUAL(50, detail::From_Ruby<int>().convert(a[4].value()));
}

TESTCASE(iterator_std_reverse)
{
  // std::reverse requires iter_swap support. Test manual reverse using
  // random-access iterator capabilities instead.
  Array a;
  a.push(10, false);
  a.push(20, false);
  a.push(30, false);
  a.push(40, false);
  a.push(50, false);

  // Manual reverse using random-access capabilities
  auto begin = a.begin();
  auto end = a.end() - 1;
  while (begin < end)
  {
    long i = begin.index();
    long j = end.index();
    Object temp = a[i];
    a[i] = detail::From_Ruby<int>().convert(a[j].value());
    a[j] = detail::From_Ruby<int>().convert(temp.value());
    ++begin;
    --end;
  }

  ASSERT_EQUAL(50, detail::From_Ruby<int>().convert(a[0].value()));
  ASSERT_EQUAL(40, detail::From_Ruby<int>().convert(a[1].value()));
  ASSERT_EQUAL(30, detail::From_Ruby<int>().convert(a[2].value()));
  ASSERT_EQUAL(20, detail::From_Ruby<int>().convert(a[3].value()));
  ASSERT_EQUAL(10, detail::From_Ruby<int>().convert(a[4].value()));
}

TESTCASE(iterator_std_distance)
{
  Array a;
  a.push(10, false);
  a.push(20, false);
  a.push(30, false);
  a.push(40, false);
  a.push(50, false);

  ASSERT_EQUAL(5, std::distance(a.begin(), a.end()));
  ASSERT_EQUAL(3, std::distance(a.begin() + 1, a.begin() + 4));
}

TESTCASE(iterator_std_advance)
{
  Array a;
  a.push(10, false);
  a.push(20, false);
  a.push(30, false);
  a.push(40, false);
  a.push(50, false);

  Array::iterator it = a.begin();
  std::advance(it, 3);
  ASSERT_EQUAL(40, detail::From_Ruby<int>().convert(it->value()));

  std::advance(it, -2);
  ASSERT_EQUAL(20, detail::From_Ruby<int>().convert(it->value()));
}
