#include "unittest.hpp"
#include "embed_ruby.hpp"
#include <rice/rice.hpp>

using namespace Rice;

TESTSUITE(Iterator);

namespace
{
  class Container
  {
  public:
    Container(int* array, size_t length)
      : array_(array)
      , length_(length)
    {
    }

    // Custom names to make sure we call the function pointers rather than
    // expectable default names.
    int* beginFoo() { return array_; }
    int* endBar() { return array_ + length_; }

  private:
    int* array_;
    size_t length_;
  };
}

namespace
{
  struct Element
  {
    Element(uint32_t value)
    {
      this->index = value;
    }

    uint32_t index;
  };

  // Iterator that returns by value, not by reference (like cv::FileNodeIterator).
  // This tests the fix for MSVC warning C4239.
  class ValueReturningIterator
  {
  public:
    using iterator_category = std::input_iterator_tag;
    using value_type = Element;
    using difference_type = std::ptrdiff_t;
    using pointer = Element*;
    using reference = Element&;  // Note: standard expects reference, but we return by value

    ValueReturningIterator(std::vector<Element>* data, size_t index) : data_(data), index_(index) {}

    // Returns by VALUE, not by reference - this is the key characteristic being tested
    Element operator*() const { return (*data_)[index_]; }

    ValueReturningIterator& operator++() { ++index_; return *this; }
    ValueReturningIterator operator++(int) { auto tmp = *this; ++index_; return tmp; }
    bool operator==(const ValueReturningIterator& other) const { return index_ == other.index_; }
    bool operator!=(const ValueReturningIterator& other) const { return index_ != other.index_; }

  private:
    std::vector<Element>* data_;
    size_t index_;
  };

  class ContainerWithValueIterator
  {
  public:
    ContainerWithValueIterator()
    {
      this->data_ = { {10}, {20}, {30} };
    }

    ValueReturningIterator begin() { return ValueReturningIterator(&data_, 0); }
    ValueReturningIterator end() { return ValueReturningIterator(&data_, data_.size()); }

  private:
    std::vector<Element> data_;
  };

  class ContainerValues
  {
  public:
    ContainerValues()
    {
      this->data_ = { {1}, {2}, {3} };
    }

    std::vector<Element>::iterator begin()
    {
      return this->data_.begin();
    }

    std::vector<Element>::iterator end()
    {
      return this->data_.end();
    }

    std::vector<Element>::const_iterator cbegin() const
    {
      return this->data_.cbegin();
    }

    std::vector<Element>::const_iterator cend() const
    {
      return this->data_.cend();
    }

    std::vector<Element> data_;
  };

  class ContainerPointers
  {
  public:
    ContainerPointers()
    {
      this->data_ = { new Element{1}, new Element{2},  new Element{3} };
    }

    ~ContainerPointers()
    {
      for (Element* data : this->data_)
      {
        delete data;
      }
    }

    std::vector<Element*>::iterator begin()
    {
      return this->data_.begin();
    }

    std::vector<Element*>::iterator end()
    {
      return this->data_.end();
    }

    std::vector<Element*>::reverse_iterator rbegin()
    {
      return this->data_.rbegin();
    }

    std::vector<Element*>::reverse_iterator rend()
    {
      return this->data_.rend();
    }

    std::vector<Element*> data_;
  };
}


SETUP(Iterator)
{
  embed_ruby();
}

TEARDOWN(Iterator)
{
  rb_gc_start();
}

TESTCASE(ArrayIterator)
{
  define_class<Container>("Container")
    .define_constructor(Constructor<Container, int*, size_t>())
    .define_iterator(&Container::beginFoo, &Container::endBar);

  int array[] = { 1, 2, 3 };
  Container* container = new Container(array, 3);

  Object wrapped_container = Data_Object<Container>(container);

  Array a = wrapped_container.instance_eval("a = []; each() { |x| a << x }; a");
  ASSERT_EQUAL(3, a.size());
  ASSERT_EQUAL(Object(detail::to_ruby(1)), a[0]);
  ASSERT_EQUAL(Object(detail::to_ruby(2)), a[1]);
  ASSERT_EQUAL(Object(detail::to_ruby(3)), a[2]);
}

TESTCASE(Standard)
{
  define_class<Element>("Element")
    .define_constructor(Constructor<Element, uint32_t>());

  define_class<ContainerValues>("ContainerValues")
      .define_constructor(Constructor<ContainerValues>())
      .define_iterator(&ContainerValues::begin, &ContainerValues::end);

  ContainerValues* container = new ContainerValues();
  Data_Object<ContainerValues> wrapper(container);

  Array a = wrapper.instance_eval("each.to_a");
  ASSERT_EQUAL(3, a.size());

  Data_Object<Element> wrappedData(a[0]);
  ASSERT_EQUAL(1u, wrappedData->index);

  wrappedData = (Data_Object<Element>)a[1];
  ASSERT_EQUAL(2u, wrappedData->index);

  wrappedData = (Data_Object<Element>)a[2];
  ASSERT_EQUAL(3u, wrappedData->index);
}

TESTCASE(Lambda)
{
  define_class<Element>("Element")
    .define_constructor(Constructor<Element, uint32_t>());

  define_class<ContainerValues>("ContainerValues")
    .define_constructor(Constructor<ContainerValues>())
    .include_module(rb_mEnumerable)
    .define_method("each", [](VALUE self, VALUE /*proc*/)->VALUE
    {
      if (!detail::protect(rb_block_given_p))
      {
        static Identifier identifier("each");
        VALUE enumerator = detail::protect(rb_enumeratorize_with_size, self, identifier.to_sym(), 0, nullptr, nullptr);
        return enumerator;
      }
      else
      {
        ContainerValues* container = detail::From_Ruby<ContainerValues*>().convert(self);

        // The iterator returns references - we do NOT want to create a copy
        detail::To_Ruby<Element&> toRuby;

        auto it = container->begin();
        auto end = container->end();

        for (; it != end; ++it)
        {
          detail::protect(rb_yield, toRuby.convert(*it));
        }

        return self;
      }
    }, Arg("proc").setValue() = Qnil, Return().setValue());

  ContainerValues* container = new ContainerValues();
  Data_Object<ContainerValues> wrapper(container);

  Module m = define_module("TestIterator");
  std::string code = R"(result = []
                        container = ContainerValues.new
                        container.each do |data|
                          result << data
                        end
                        # Return container to keep it alive
                        [container, result])";

  Array result = m.module_eval(code);
  ASSERT_EQUAL(2, result.size());

  Array elements(result[1]);
  ASSERT_EQUAL(3, elements.size());

  Data_Object<Element> wrappedData(elements[0]);
  ASSERT_EQUAL(1u, wrappedData->index);

  wrappedData = (Data_Object<Element>)elements[1];
  ASSERT_EQUAL(2u, wrappedData->index);

  wrappedData = (Data_Object<Element>)elements[2];
  ASSERT_EQUAL(3u, wrappedData->index);

  code = R"(container = ContainerValues.new
            enumerator = container.each
            # Return container to keep it alive
            [container, enumerator.to_a])";

  result = m.module_eval(code);
  ASSERT_EQUAL(2, result.size());

  elements = Array(result[1]);
  ASSERT_EQUAL(3, elements.size());

  wrappedData = (Data_Object<Element>)elements[0];
  ASSERT_EQUAL(1u, wrappedData->index);

  wrappedData = (Data_Object<Element>)elements[1];
  ASSERT_EQUAL(2u, wrappedData->index);

  wrappedData = (Data_Object<Element>)elements[2];
  ASSERT_EQUAL(3u, wrappedData->index);
}

TESTCASE(ConstValue)
{
  define_class<Element>("Element")
      .define_constructor(Constructor<Element, uint32_t>());
    
  define_class<ContainerValues>("ContainerValues")
    .define_constructor(Constructor<ContainerValues>())
    .define_iterator(&ContainerValues::cbegin, &ContainerValues::cend);

  Module m = define_module("TestingModule");

  std::string code = R"(result = []
                        container = ContainerValues.new
                        container.each do |x|
                          result << x
                        end
                        # Return container to keep it alive
                        [container, result])";

  Array result = m.module_eval(code);
  ASSERT_EQUAL(2, result.size());

  Array elements = Array(result[1]);
  ASSERT_EQUAL(3, elements.size());

  Data_Object<Element> wrappedData(elements[0]);
  ASSERT_EQUAL(1u, wrappedData->index);

  wrappedData = (Data_Object<Element>)elements[1];
  ASSERT_EQUAL(2u, wrappedData->index);

  wrappedData = (Data_Object<Element>)elements[2];
  ASSERT_EQUAL(3u, wrappedData->index);
}

TESTCASE(Pointer)
{
  define_class<Element>("Element")
    .define_constructor(Constructor<Element, uint32_t>());
    
  define_class<ContainerPointers>("ContainerPointers")
    .define_constructor(Constructor<ContainerPointers>())
    .define_iterator(&ContainerPointers::begin, &ContainerPointers::end);

  ContainerPointers* container = new ContainerPointers();
  Object wrapper = Data_Object<ContainerPointers>(container);

  Module m = define_module("TestingModule");

  std::string code = R"(result = []
                        container = ContainerPointers.new
                        container.each do |x|
                          result << x
                        end
                        # Return container to keep it alive
                        [container, result])";

  Array a = m.module_eval(code);

  Array result = m.module_eval(code);
  ASSERT_EQUAL(2, result.size());

  Array elements = Array(result[1]);
  ASSERT_EQUAL(3, elements.size());

  Data_Object<Element> wrappedData(elements[0]);
  ASSERT_EQUAL(1u, wrappedData->index);

  wrappedData = (Data_Object<Element>)elements[1];
  ASSERT_EQUAL(2u, wrappedData->index);

  wrappedData = (Data_Object<Element>)elements[2];
  ASSERT_EQUAL(3u, wrappedData->index);
}

TESTCASE(TwoIteratorPointers)
{
  define_class<Element>("Element")
    .define_constructor(Constructor<Element, uint32_t>());

  define_class<ContainerPointers>("ContainerPointers")
    .define_constructor(Constructor<ContainerPointers>())
    .define_iterator(&ContainerPointers::begin, &ContainerPointers::end)
    .define_iterator(&ContainerPointers::rbegin, &ContainerPointers::rend, "reach");

  ContainerPointers* container = new ContainerPointers();
  Object wrapper = Data_Object<ContainerPointers>(container);

  Module m = define_module("TestingModule");

  std::string code = R"(result = []
                        container = ContainerPointers.new
                        container.each do |x|
                          result << x
                        end
                        container.reach do |x|
                          result << x
                        end
                        # Return container to keep it alive
                        [container, result])";

  Array a = m.module_eval(code);

  Array result = m.module_eval(code);
  ASSERT_EQUAL(2, result.size());

  Array elements = Array(result[1]);
  ASSERT_EQUAL(6, elements.size());

  Data_Object<Element> wrappedData(elements[0]);
  ASSERT_EQUAL(1u, wrappedData->index);

  wrappedData = (Data_Object<Element>)elements[1];
  ASSERT_EQUAL(2u, wrappedData->index);

  wrappedData = (Data_Object<Element>)elements[2];
  ASSERT_EQUAL(3u, wrappedData->index);

  wrappedData = (Data_Object<Element>)elements[3];
  ASSERT_EQUAL(3u, wrappedData->index);

  wrappedData = (Data_Object<Element>)elements[4];
  ASSERT_EQUAL(2u, wrappedData->index);

  wrappedData = (Data_Object<Element>)elements[5];
  ASSERT_EQUAL(1u, wrappedData->index);
}

TESTCASE(Map)
{
  define_class<Element>("Element")
    .define_constructor(Constructor<Element, uint32_t>())
    .define_attr("index", &Element::index, Rice::AttrAccess::Read);

  define_class<ContainerPointers>("ContainerPointers")
    .define_constructor(Constructor<ContainerPointers>())
    .define_iterator(&ContainerPointers::begin, &ContainerPointers::end);

  Module m = define_module("Testing");

  std::string code = R"(container = ContainerPointers.new
                        container.map do |data|
                          data.index * 2
                        end)";

  Array a = m.module_eval(code);
  ASSERT_EQUAL(3, a.size());

  Object element(a[0]);
  ASSERT_EQUAL(2, detail::From_Ruby<int>().convert(element));

  element = Object(a[1]);
  ASSERT_EQUAL(4, detail::From_Ruby<int>().convert(element));

  element = Object(a[2]);
  ASSERT_EQUAL(6, detail::From_Ruby<int>().convert(element));
}

TESTCASE(Enum)
{
  define_class<Element>("Element")
    .define_constructor(Constructor<Element, uint32_t>())
    .define_attr("index", &Element::index, Rice::AttrAccess::Read);

  define_class<ContainerPointers>("ContainerPointers")
    .define_constructor(Constructor<ContainerPointers>())
    .define_iterator(&ContainerPointers::begin, &ContainerPointers::end);

  Module m = define_module("TestingModule");

  std::string code = R"(container = ContainerPointers.new
                        container.each.map do |data|
                          data.index * 2
                        end)";

  Array a = m.module_eval(code);

  ASSERT_EQUAL(3, a.size());

  Object element(a[0]);
  ASSERT_EQUAL(2, detail::From_Ruby<int>().convert(element));

  element = Object(a[1]);
  ASSERT_EQUAL(4, detail::From_Ruby<int>().convert(element));

  element = Object(a[2]);
  ASSERT_EQUAL(6, detail::From_Ruby<int>().convert(element));
}

TESTCASE(ToArray)
{
  define_class<Element>("Element")
    .define_constructor(Constructor<Element, uint32_t>())
    .define_attr("index", &Element::index, Rice::AttrAccess::Read);

  define_class<ContainerPointers>("ContainerPointers")
    .define_constructor(Constructor<ContainerPointers>())
    .define_iterator(&ContainerPointers::begin, &ContainerPointers::end);

  Module m = define_module("TestingModule");

  std::string code = R"(container = ContainerPointers.new
                        # We have to return the container so it is not GCed
                        [container, container.to_a])";

  Array result = m.module_eval(code);
  ASSERT_EQUAL(2, result.size());

  Array elements(result[1]);

  ASSERT_EQUAL(3, elements.size());

  Object element(elements[0]);
  Object index = element.call("index");
  ASSERT_EQUAL(1, detail::From_Ruby<int>().convert(index));

  element = Object(elements[1]);
  index = element.call("index");
  ASSERT_EQUAL(2, detail::From_Ruby<int>().convert(index));

  element = Object(elements[2]);
  index = element.call("index");
  ASSERT_EQUAL(3, detail::From_Ruby<int>().convert(index));
}

TESTCASE(IterateNoCopy)
{
  define_class<Element>("Element")
    .define_constructor(Constructor<Element, uint32_t>());

  define_class<ContainerValues>("ContainerValues")
    .define_constructor(Constructor<ContainerValues>())
    .define_iterator(&ContainerValues::begin, &ContainerValues::end);

  Module m = define_module("Testing");

  ContainerValues container;
  Data_Object<ContainerValues> wrapper(container);
  Array a = wrapper.instance_eval("self.to_a");

  ASSERT_EQUAL(container.data_.size(), (size_t)a.size());

  for (size_t i = 0; i < container.data_.size(); i++)
  {
    Element& expected = container.data_[i];
    Data_Object<Element> actual(a[(long)i]);
    ASSERT_EQUAL(&expected, &(*actual));
  }
}

// Test iterator that returns by value (like cv::FileNodeIterator).
// This previously caused MSVC warning C4239 about binding rvalue to non-const reference.
TESTCASE(ValueReturningIterator)
{
  define_class<Element>("Element")
    .define_constructor(Constructor<Element, uint32_t>())
    .define_attr("index", &Element::index, Rice::AttrAccess::Read);

  define_class<ContainerWithValueIterator>("ContainerWithValueIterator")
    .define_constructor(Constructor<ContainerWithValueIterator>())
    .define_iterator(&ContainerWithValueIterator::begin, &ContainerWithValueIterator::end);

  Module m = define_module("TestingModule");

  std::string code = R"(result = []
                        container = ContainerWithValueIterator.new
                        container.each do |x|
                          result << x.index
                        end
                        result)";

  Array a = m.module_eval(code);

  ASSERT_EQUAL(3, a.size());
  ASSERT_EQUAL(10, detail::From_Ruby<int>().convert(a[0].value()));
  ASSERT_EQUAL(20, detail::From_Ruby<int>().convert(a[1].value()));
  ASSERT_EQUAL(30, detail::From_Ruby<int>().convert(a[2].value()));
}