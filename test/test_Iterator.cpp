#include "unittest.hpp"
#include "embed_ruby.hpp"
#include <rice/rice.hpp>

using namespace Rice;

TESTSUITE(Iterator);

SETUP(Iterator)
{
  embed_ruby();
}

TEARDOWN(Iterator)
{
  rb_gc_start();
}

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
} // namespace

TESTCASE(define_array_iterator)
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

namespace
{
  struct Data
  {
    Data(uint32_t value)
    {
      this->index = value;
    }

    uint32_t index;
  };

  class ContainerValues
  {
  public:
    ContainerValues()
    {
      this->data_ = { {1}, {2}, {3} };
    }

    std::vector<Data>::iterator begin()
    {
      return this->data_.begin();
    }

    std::vector<Data>::iterator end()
    {
      return this->data_.end();
    }

    std::vector<Data>::const_iterator cbegin() const
    {
      return this->data_.cbegin();
    }

    std::vector<Data>::const_iterator cend() const
    {
      return this->data_.cend();
    }

    std::vector<Data> data_;
  };

  class ContainerPointers
  {
  public:
    ContainerPointers()
    {
      this->data_ = { new Data{1}, new Data{2},  new Data{3} };
    }

    ~ContainerPointers()
    {
      for (Data* data : this->data_)
      {
        delete data;
      }
    }

    std::vector<Data*>::iterator begin()
    {
      return this->data_.begin();
    }

    std::vector<Data*>::iterator end()
    {
      return this->data_.end();
    }

    std::vector<Data*>::reverse_iterator rbegin()
    {
      return this->data_.rbegin();
    }

    std::vector<Data*>::reverse_iterator rend()
    {
      return this->data_.rend();
    }

    std::vector<Data*> data_;
  };
}

TESTCASE(iterator_value)
{
  define_class<Data>("Data")
    .define_constructor(Constructor<Data, uint32_t>());

  define_class<ContainerValues>("ContainerValues")
      .define_constructor(Constructor<ContainerValues>())
      .define_iterator(&ContainerValues::begin, &ContainerValues::end);

  ContainerValues* container = new ContainerValues();
  Data_Object<ContainerValues> wrapper(container);

  Array a = wrapper.instance_eval("each.to_a");
  ASSERT_EQUAL(3, a.size());

  Data_Object<Data> wrappedData(a[0]);
  ASSERT_EQUAL(1u, wrappedData->index);

  wrappedData = (Data_Object<Data>)a[1];
  ASSERT_EQUAL(2u, wrappedData->index);

  wrappedData = (Data_Object<Data>)a[2];
  ASSERT_EQUAL(3u, wrappedData->index);
}

TESTCASE(const_iterator_value)
{
  define_class<Data>("Data")
      .define_constructor(Constructor<Data, uint32_t>());
    
  define_class<ContainerValues>("ContainerValues")
    .define_constructor(Constructor<ContainerValues>())
    .define_iterator(&ContainerValues::cbegin, &ContainerValues::cend);

  Module m = define_module("TestingModule");

  std::string code = R"(result = []
                        container = ContainerValues.new
                        container.each do |x|
                          result << x
                        end
                        result)";

  Array a = m.module_eval(code);

  Data_Object<Data> wrappedData(a[0]);
  ASSERT_EQUAL(1u, wrappedData->index);

  wrappedData = (Data_Object<Data>)a[1];
  ASSERT_EQUAL(2u, wrappedData->index);

  wrappedData = (Data_Object<Data>)a[2];
  ASSERT_EQUAL(3u, wrappedData->index);
}

TESTCASE(iterator_pointer)
{
  define_class<Data>("Data")
    .define_constructor(Constructor<Data, uint32_t>());
    
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
                        result)";

  Array a = m.module_eval(code);

  Data_Object<Data> wrappedData(a[0]);
  ASSERT_EQUAL(1u, wrappedData->index);

  wrappedData = (Data_Object<Data>)a[1];
  ASSERT_EQUAL(2u, wrappedData->index);

  wrappedData = (Data_Object<Data>)a[2];
  ASSERT_EQUAL(3u, wrappedData->index);
}

TESTCASE(two_iterator_pointer)
{
  define_class<Data>("Data")
    .define_constructor(Constructor<Data, uint32_t>());

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
                        result)";

  Array a = m.module_eval(code);

  ASSERT_EQUAL(6, a.size());

  Data_Object<Data> wrappedData(a[0]);
  ASSERT_EQUAL(1u, wrappedData->index);

  wrappedData = (Data_Object<Data>)a[1];
  ASSERT_EQUAL(2u, wrappedData->index);

  wrappedData = (Data_Object<Data>)a[2];
  ASSERT_EQUAL(3u, wrappedData->index);

  wrappedData = (Data_Object<Data>)a[3];
  ASSERT_EQUAL(3u, wrappedData->index);

  wrappedData = (Data_Object<Data>)a[4];
  ASSERT_EQUAL(2u, wrappedData->index);

  wrappedData = (Data_Object<Data>)a[5];
  ASSERT_EQUAL(1u, wrappedData->index);
}

TESTCASE(map)
{
  define_class<Data>("Data")
    .define_constructor(Constructor<Data, uint32_t>())
    .define_attr("index", &Data::index, Rice::AttrAccess::Read);

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

  Object element = a[0];
  ASSERT_EQUAL(2, detail::From_Ruby<int>().convert(element));

  element = a[1];
  ASSERT_EQUAL(4, detail::From_Ruby<int>().convert(element));

  element = a[2];
  ASSERT_EQUAL(6, detail::From_Ruby<int>().convert(element));
}

TESTCASE(to_enum)
{
  define_class<Data>("Data")
    .define_constructor(Constructor<Data, uint32_t>());

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

  Object element = a[0];
  ASSERT_EQUAL(2, detail::From_Ruby<int>().convert(element));

  element = a[1];
  ASSERT_EQUAL(4, detail::From_Ruby<int>().convert(element));

  element = a[2];
  ASSERT_EQUAL(6, detail::From_Ruby<int>().convert(element));
}

TESTCASE(to_a)
{
  define_class<Data>("Data")
    .define_constructor(Constructor<Data, uint32_t>())
    .define_attr("index", &Data::index, Rice::AttrAccess::Read);

  define_class<ContainerPointers>("ContainerPointers")
    .define_constructor(Constructor<ContainerPointers>())
    .define_iterator(&ContainerPointers::begin, &ContainerPointers::end);

  Module m = define_module("TestingModule");

  std::string code = R"(container = ContainerPointers.new
                        container.to_a)";

  Array a = m.module_eval(code);

  ASSERT_EQUAL(3, a.size());

  Object element = a[0];
  Object index = element.call("index");
  ASSERT_EQUAL(1, detail::From_Ruby<int>().convert(index));

  element = a[1];
  index = element.call("index");
  ASSERT_EQUAL(2, detail::From_Ruby<int>().convert(index));

  element = a[2];
  index = element.call("index");
  ASSERT_EQUAL(3, detail::From_Ruby<int>().convert(index));
}

TESTCASE(IterateNoCopy)
{
  define_class<Data>("Data")
    .define_constructor(Constructor<Data, uint32_t>());

  define_class<ContainerPointers>("ContainerValues")
    .define_constructor(Constructor<ContainerValues>())
    .define_iterator(&ContainerPointers::begin, &ContainerPointers::end);

  Module m = define_module("Testing");

  ContainerValues container;
  Data_Object<ContainerValues> wrapper(container);
  Array a = wrapper.instance_eval("self.to_a");

  ASSERT_EQUAL(container.data_.size(), (size_t)a.size());

  for (size_t i = 0; i < container.data_.size(); i++)
  {
    Data& expected = container.data_[i];
    Data_Object<Data> actual(a[(long)i]);
    ASSERT_EQUAL(&expected, &(*actual));
  }
}