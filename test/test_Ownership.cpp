#include "unittest.hpp"
#include "embed_ruby.hpp"
#include <rice/rice.hpp>

#include <memory>
#include <unordered_set>

using namespace Rice;

TESTSUITE(Ownership);

namespace
{
  class MyCopyableClass
  {
  public:
    static inline int constructorCalls = 0;
    static inline int copyConstructorCalls = 0;
    static inline int destructorCalls = 0;
    static inline int methodCalls = 0;
    static inline std::unordered_set<MyCopyableClass*> liveInstances;

    static void reset()
    {
      constructorCalls = 0;
      copyConstructorCalls = 0;
      destructorCalls = 0;
      methodCalls = 0;
      liveInstances.clear();
    }

  public:
    int flag = 0;
    char* payload = nullptr;

  public:
    MyCopyableClass()
    {
      constructorCalls++;
      payload = new char[8];
      payload[0] = 0;
      liveInstances.insert(this);
    }

    ~MyCopyableClass()
    {
      liveInstances.erase(this);
      delete[] payload;
      payload = (char*)0x1;
      destructorCalls++;
    }

    MyCopyableClass(const MyCopyableClass& other): flag(other.flag)
    {
      copyConstructorCalls++;
      payload = new char[8];
      payload[0] = other.payload ? other.payload[0] : 0;
      liveInstances.insert(this);
    }

    MyCopyableClass(MyCopyableClass&& other) = delete;

    int32_t process()
    {
      this->assertAlive();
      payload[0] = (char)flag;
      methodCalls++;
      return methodCalls;
    }

    void setFlag(int value)
    {
      this->assertAlive();
      this->flag = value;
      payload[0] = (char)value;
    }

  private:
    void assertAlive()
    {
      if (liveInstances.find(this) == liveInstances.end())
      {
        volatile int* p = nullptr;
        *p = 1;
      }
    }
  };

  class MyMovableClass
  {
  public:
    static inline int constructorCalls = 0;
    static inline int moveConstructorCalls = 0;
    static inline int destructorCalls = 0;
    static inline int methodCalls = 0;

    static void reset()
    {
      constructorCalls = 0;
      moveConstructorCalls = 0;
      destructorCalls = 0;
      methodCalls = 0;
    }

  public:
    int flag = 0;

  public:
    MyMovableClass()
    {
      constructorCalls++;
    }

    ~MyMovableClass()
    {
      destructorCalls++;
    }

    MyMovableClass(const MyMovableClass& other) = delete;

    MyMovableClass(MyMovableClass&& other) : flag(other.flag)
    {
      moveConstructorCalls++;
    }

    int32_t process()
    {
      methodCalls++;
      return methodCalls;
    }

    void setFlag(int value)
    {
      this->flag = value;
    }
  };

  class Factory
  {
  public:
    static void reset()
    {
      delete Factory::instance_;
      Factory::instance_ = nullptr;
    }

  public:
    MyCopyableClass* transferPointerToRuby()
    {
      return new MyCopyableClass();
    }

    MyCopyableClass* keepPointer()
    {
      return this->instance();
    }

    MyCopyableClass& keepReference()
    {
      return *this->instance();
    }

    MyCopyableClass value()
    {
      return MyCopyableClass();
    }

    MyMovableClass moveValue()
    {
      return MyMovableClass();
    }

    void transferPointerToCpp(MyCopyableClass* myClass)
    {
      delete myClass;
    }

    MyCopyableClass* instance()
    {
      if (!instance_)
      {
        instance_ = new MyCopyableClass();
      }
      return instance_;
    }

  public:
    static inline MyCopyableClass* instance_ = nullptr;
  };

  class OwnerBox;

  class AliasBox
  {
  public:
    explicit AliasBox(OwnerBox* owner) : owner_(owner)
    {}

    OwnerBox* owner()
    {
      return owner_;
    }

  private:
    OwnerBox* owner_ = nullptr;
  };

  class OwnerBox
  {
  public:
    OwnerBox() : alias_(std::make_unique<AliasBox>(this))
    {
    }

    AliasBox* alias()
    {
      return alias_.get();
    }

    void setFlag(int value)
    {
      this->flag_ = value;
    }

    int flag() const
    {
      return this->flag_;
    }

  private:
    int flag_ = 0;
    std::unique_ptr<AliasBox> alias_;
  };

  class RotatingStore
  {
  public:
    RotatingStore() : current_(std::make_unique<MyCopyableClass>())
    {
    }

    MyCopyableClass* current()
    {
      return current_.get();
    }

    void rotate()
    {
      current_.reset(new MyCopyableClass());
    }

    void clear()
    {
      current_.reset();
    }

  private:
    std::unique_ptr<MyCopyableClass> current_;
  };

  class SharedFactory
  {
  public:
    SharedFactory() : shared_(new MyCopyableClass())
    {
    }

    ~SharedFactory() = default;

    MyCopyableClass* createOwned()
    {
      return shared_;
    }

    MyCopyableClass* getBorrowed()
    {
      return shared_;
    }

  private:
    MyCopyableClass* shared_ = nullptr;
  };
}

SETUP(Ownership)
{
  embed_ruby();

  define_class<MyCopyableClass>("MyCopyableClass").
    define_constructor(Constructor<MyCopyableClass>()).
    define_method("process", &MyCopyableClass::process).
    define_method("set_flag", &MyCopyableClass::setFlag);

  define_class<MyMovableClass>("MyMovableClass").
    define_constructor(Constructor<MyMovableClass>()).
    define_method("process", &MyMovableClass::process).
    define_method("set_flag", &MyMovableClass::setFlag);

  define_class<Factory>("Factory").
    define_constructor(Constructor<Factory>()).
    define_method("value", &Factory::value).
    define_method("move_value", &Factory::moveValue).
    define_method("transfer_pointer_to_ruby", &Factory::transferPointerToRuby, Return().takeOwnership()).
    define_method("transfer_pointer_to_cpp", &Factory::transferPointerToCpp, Arg("myClass").takeOwnership()).
    define_method("keep_pointer", &Factory::keepPointer).
    define_method("copy_reference", &Factory::keepReference, Return().takeOwnership()).
    define_method("keep_reference", &Factory::keepReference);

  define_class<OwnerBox>("OwnerBox")
    .define_constructor(Constructor<OwnerBox>())
    .define_method("set_flag", &OwnerBox::setFlag)
    .define_method("flag", &OwnerBox::flag);

  define_class<AliasBox>("AliasBox")
    .define_method("owner", &AliasBox::owner);

  Data_Type<OwnerBox>().define_method("alias", &OwnerBox::alias);

  define_class<RotatingStore>("RotatingStore")
    .define_constructor(Constructor<RotatingStore>())
    .define_method("current", &RotatingStore::current)
    .define_method("rotate", &RotatingStore::rotate)
    .define_method("clear", &RotatingStore::clear);

  define_class<SharedFactory>("SharedFactory")
    .define_constructor(Constructor<SharedFactory>())
    .define_method("create_owned", &SharedFactory::createOwned, Return().takeOwnership())
    .define_method("get_borrowed", &SharedFactory::getBorrowed);
}

TEARDOWN(Ownership)
{
  rb_gc_start();
}

TESTCASE(TransferPointerToRuby)
{
  Factory::reset();
  MyCopyableClass::reset();

  Module m = define_module("TestingModule");

  std::string code = R"(factory = Factory.new
                        10.times do |i|
                          my_class = factory.transfer_pointer_to_ruby
                          my_class.set_flag(i)
                          my_class = nil
                        end)";

  m.module_eval(code);
  rb_gc_start();

  ASSERT_EQUAL(10, MyCopyableClass::constructorCalls);
  ASSERT_EQUAL(0, MyCopyableClass::copyConstructorCalls);
  ASSERT_EQUAL(10, MyCopyableClass::destructorCalls);
  ASSERT(!Factory::instance_);
}

TESTCASE(TransferPointerToCpp)
{
  Factory::reset();
  MyCopyableClass::reset();

  Module m = define_module("TestingModule");

  std::string code = R"(myClass = MyCopyableClass.new
                        factory = Factory.new
                        factory.transfer_pointer_to_cpp(myClass))";

  m.module_eval(code);
  rb_gc_start();

  ASSERT(true);
}

TESTCASE(KeepPointer)
{
  Factory::reset();
  MyCopyableClass::reset();

  Module m = define_module("TestingModule");

  // Create ruby objects that point to the same instance of MyClass
  std::string code = R"(factory = Factory.new
                        10.times do |i|
                          my_class = factory.keep_pointer
                          my_class.set_flag(i)
                        end)";

  m.module_eval(code);
  rb_gc_start();

  ASSERT_EQUAL(1, MyCopyableClass::constructorCalls);
  ASSERT_EQUAL(0, MyCopyableClass::copyConstructorCalls);
  ASSERT_EQUAL(0, MyCopyableClass::destructorCalls);
  ASSERT_EQUAL(9, Factory::instance_->flag);
}

TESTCASE(KeepReference)
{
  Factory::reset();
  MyCopyableClass::reset();

  Module m = define_module("TestingModule");

  // Create ruby objects that point to the same instance of MyClass
  std::string code = R"(factory = Factory.new
                        10.times do |i|
                          my_class = factory.keep_reference
                          my_class.set_flag(i)
                        end)";

  m.module_eval(code);
  rb_gc_start();

  ASSERT_EQUAL(1, MyCopyableClass::constructorCalls);
  ASSERT_EQUAL(0, MyCopyableClass::copyConstructorCalls);
  ASSERT_EQUAL(0, MyCopyableClass::destructorCalls);
  ASSERT_EQUAL(9, Factory::instance_->flag);
}

TESTCASE(CopyReference)
{
  Factory::reset();
  MyCopyableClass::reset();

  Module m = define_module("TestingModule");

  // Create ruby objects that point to the same instance of MyClass
  std::string code = R"(factory = Factory.new
                        10.times do |i|
                          my_class = factory.copy_reference
                          my_class.set_flag(i)
                        end)";

  m.module_eval(code);
  rb_gc_start();

  ASSERT_EQUAL(1, MyCopyableClass::constructorCalls);
  ASSERT_EQUAL(10, MyCopyableClass::copyConstructorCalls);
  ASSERT_EQUAL(10, MyCopyableClass::destructorCalls);
  ASSERT_EQUAL(0, Factory::instance_->flag);
}

TESTCASE(TransferValue)
{
  Factory::reset();
  MyCopyableClass::reset();

  Module m = define_module("TestingModule");

  std::string code = R"(factory = Factory.new
                        10.times do |i|
                          my_class = factory.value
                          my_class.set_flag(i)
                        end)";

  m.module_eval(code);
  rb_gc_start();

  ASSERT_EQUAL(10, MyCopyableClass::constructorCalls);
  ASSERT_EQUAL(10, MyCopyableClass::copyConstructorCalls);
  ASSERT_EQUAL(20, MyCopyableClass::destructorCalls);
  ASSERT(!Factory::instance_);
}

TESTCASE(MoveValue)
{
  Factory::reset();
  MyCopyableClass::reset();

  Module m = define_module("TestingModule");

  std::string code = R"(factory = Factory.new
                        10.times do |i|
                          my_class = factory.move_value
                          my_class.set_flag(i)
                        end)";

  m.module_eval(code);
  rb_gc_start();

  ASSERT_EQUAL(10, MyMovableClass::constructorCalls);
  ASSERT_EQUAL(10, MyMovableClass::moveConstructorCalls);
  ASSERT_EQUAL(20, MyMovableClass::destructorCalls);
  ASSERT(!Factory::instance_);
}

namespace
{
  class MyClassNotCopyable
  {
  public:
    MyClassNotCopyable() = default;
    MyClassNotCopyable(const MyClassNotCopyable& other) = delete;
    MyClassNotCopyable(MyClassNotCopyable&& other) = delete;
    int value;
  };
}

TESTCASE(NotCopyable)
{
  Class c = define_class<MyClassNotCopyable>("MyClassNotCopyable")
    .define_constructor(Constructor<MyClassNotCopyable>())
    .define_attr("value", &MyClassNotCopyable::value);

  MyClassNotCopyable instance;

#ifdef _MSC_VER
  const char* expected = "Rice was directed to take ownership of a C++ object but it does not have an accessible copy or move constructor. Type: class `anonymous namespace'::MyClassNotCopyable";
#else
  const char* expected = "Rice was directed to take ownership of a C++ object but it does not have an accessible copy or move constructor. Type: (anonymous namespace)::MyClassNotCopyable";
#endif

  // Trying to take ownership should fail
  ASSERT_EXCEPTION_CHECK(
    std::runtime_error,
    Data_Object<MyClassNotCopyable>(instance, true),
    ASSERT_EQUAL(expected, ex.what())
  );
}

TESTCASE(MultipleOwnerReferences)
{
  detail::Registries::instance.instances.mode = detail::InstanceRegistry::Mode::All;

  Module m = define_module("TestingModule");
  std::string code = R"(
    box1 = OwnerBox.new
    alias_obj = box1.alias
    box2 = alias_obj.owner
    box2.set_flag(99)
    box1 = nil
    GC.start
    box2.flag
  )";
  
  Object result = m.module_eval(code);
  ASSERT_EQUAL(99, detail::From_Ruby<int>().convert(result.value()));

  detail::Registries::instance.instances.mode = detail::InstanceRegistry::Mode::Off;
  result = m.module_eval(code);
  ASSERT_NOT_EQUAL(99, detail::From_Ruby<int>().convert(result.value()));
}
