# Inheritance

Rice supports creating child classes via the `define_class` method:

```cpp
class Base
{
public:
  virtual void foo();
};

class Derived
  : public Base
{
};

extern "C"
void Init_test()
{
  Data_Type<Base> rb_cBase =
    define_class<Base>("Base")
    .define_method("foo", &Base::foo);

  Data_Type<Derived> rb_cDerived =
    define_class<Derived, Base>("Derived");
}
```

This is done by adding a second template parameter to `define_class` that specifies that `Derived` inherits from `Base`.

Rice does not support multiple inheritance.

## Directors

Inheritance becomes much more complex if you want to create Ruby classes that inherit from wrapped C++ classes. This introduces several problems:

* Ruby classes should be able to override C++ virtual methods
* Overridden virtual methods should be able to call `super` and invoke the overridden C++ method
* C++ code calling the virtual methods should invoke the overridden version in Ruby

Rice supports these use cases through the use of `Director` classes. `Directors` are proxies that can correctly dispatch method invocations up or down a Class hierarchy.

!!! note
    The name Director comes from SWIG - for more information see <https://www.swig.org/Doc4.0/SWIGPlus.html#SWIGPlus_director_classes_introduction>.

Take the following class:

```cpp
class VirtualBase
{
  public:
    VirtualBase();
    virtual int doWork();
    virtual int processWorker() = 0;
};
```

Due to the abstract nature of this class, we cannot directly wrap it in Rice, as any C++ compiler will complain about trying to instantiate a virtual class. Even without the pure virtual function, any call to `VirtualBase::doWork` will stop at the C++ level and execution will not pass down into any Ruby subclasses.

To properly wrap both of these methods, use a `Rice::Director` subclass as a proxy and use this new proxy class as the type to wrap with `define_class`:

```cpp
#include <rice/rice.hpp>

class VirtualBaseProxy : public VirtualBase, public Rice::Director
{
  public:
    VirtualBaseProxy(Object self) : Rice::Director(self)
    { }

    virtual int doWork()
    {
      int result = getSelf().call("do_work");
      return detail::From_Ruby<int>().convert(result);
    }

    int default_doWork()
    {
      return VirtualBase::doWork();
    }

    virtual int processWorker()
    {
      int result = getSelf().call("process_worker");
      return detail::From_Ruby<int>().convert(result);
    }

    int default_processWorker()
    {
      raisePureVirtual();
    }
};
```

There is a lot going on here, so we'll go through each part.

```cpp
class VirtualBaseProxy : public Virtualbase, public Rice::Director { }
```

First, the class needs to subclass both the virtual class in question and `Rice::Director`.

```cpp
  public:
    VirtualBaseProxy(Object self) : Rice::Director(self) { }
```

For `Rice::Director` to work its magic, every instance of this class needs to have a handle to its Ruby instance. The constructor must take a `Rice::Object` as the first argument and pass it to `Rice::Director`.

Next we implement `doWork`. The director class overrides its by forwarding the invocation to the Ruby instance.

```cpp
    virtual int doWork()
    {
      int result = getSelf().call("do_work");
      return detail::From_Ruby<int>().convert(result);
    }

    int default_doWork()
    {
      return VirtualBase::doWork();
    }
```

It director also implements `default_doWork` which enables Ruby to call the overridden virtual C++ method. The `default_` prefix is a naming convention to help keep straight which methods perform which functions.

If Ruby should never call the C++ method then the `default_` implementation should call `raisePureVirtual()`:

```cpp
int default_processWorker()
{
  raisePureVirtual();
}
```

The method `raisePureVirtual()` exists to allow wrapping a pure virtual method into Ruby (and ensuring compilation is possible) while also making sure any users of this extension are informed quickly if there is nothing callable on the C++ side.

Once the Director class is built, it's time to wrap it into Ruby:

```cpp
extern "C"
void Init_virtual() {
  define_class<VirtualBase>("VirtualBase")
    .define_director<VirtualBaseProxy>()
    .define_constructor(Constructor<VirtualBaseProxy, Rice::Object>())
    .define_method("do_work", &VirtualBaseProxy::default_doWork)
    .define_method("process_worker", &VirtualBaseProxy::default_processWorker);
}
```

There are couple of new things in this code.

First, is the addition of the `define_director` call which takes the `VirtualBaseProxy` as a template parameter.

Second, the `Constructor` template parameter must also be the `VirtualBaseProxy` to allow proper object construction and destruction of the derived objects.

Third, the `define_method` calls should point to the `default_*` implementations.
