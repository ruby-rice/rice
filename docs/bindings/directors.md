# Directors

Inheritance becomes much more complex if you want to create Ruby classes that inherit from wrapped C++ classes. This introduces several problems:

* Ruby classes should be able to override C++ virtual methods
* Overridden virtual methods should be able to call `super` and invoke the overridden C++ method
* C++ code calling the virtual methods should invoke the overridden version in Ruby

Rice supports these use cases through the use of `Director` classes. `Directors` are proxies that correctly dispatch method invocations up or down a Class hierarchy.

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

    int doWork() override
    {
      int result = getSelf().call("do_work");
      return detail::From_Ruby<int>().convert(result);
    }

    int default_doWork()
    {
      return VirtualBase::doWork();
    }

    int processWorker() override
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

Next we implement `doWork`. The director class overrides it by forwarding the invocation to the Ruby instance.

```cpp
    int doWork() override
    {
      int result = getSelf().call("do_work");
      return detail::From_Ruby<int>().convert(result);
    }

    int default_doWork()
    {
      return VirtualBase::doWork();
    }
```

The director also implements `default_doWork` which enables Ruby to call the overridden virtual C++ method. The `default_` prefix is a naming convention to help keep straight which methods perform which functions.

If Ruby should never call the C++ method then the `default_` implementation should call `raisePureVirtual()`:

```cpp
int default_processWorker()
{
  raisePureVirtual();
}
```

The method `raisePureVirtual()` exists to allow wrapping a pure virtual method into Ruby (and ensuring compilation is possible) while also making sure any users of this extension get an exception if they try to call an unimplemented C++ method.

After defining the Director class is C++ it's time to wrap it for use in Ruby:

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

There are a couple of new things in this code.

First, note the addition of the `define_director` call which takes the `VirtualBaseProxy` as a template parameter.

Second, the `Constructor` template parameter must include the `VirtualBaseProxy` class to allow proper object construction and destruction of the derived objects.

Third, the `define_method` calls should point to the `default_*` implementations.

## Ruby Usage

Once the Director class is wrapped, Ruby classes can inherit from it and override virtual methods.

### Basic Subclassing

Create a Ruby class that inherits from the wrapped C++ class and implements the pure virtual method:

```ruby
class MyWorker < VirtualBase
  def process_worker
    42
  end
end

worker = MyWorker.new
worker.process_worker  # => 42
```

### Overriding Virtual Methods with Super

Ruby subclasses can override virtual methods and call `super` to invoke the original C++ implementation:

```ruby
class MyWorker < VirtualBase
  def do_work
    super * 2  # Calls C++ VirtualBase::doWork(), then doubles the result
  end
end

worker = MyWorker.new
worker.do_work  # => C++ result * 2
```

### Pure Virtual Methods

If a Ruby subclass calls `super` on a pure virtual method, Rice raises a `NotImplementedError`:

```ruby
class MyWorker < VirtualBase
  def process_worker
    super  # Raises NotImplementedError
  end
end
```

### Polymorphism from C++

The real power of Directors is that C++ code calling virtual methods will correctly invoke the Ruby implementations. For example, if you have a C++ class that processes workers:

```cpp
class Handler
{
  std::vector<Worker*> workers;

public:
  void addWorker(Worker* worker) { workers.push_back(worker); }

  int processWorkers(int start)
  {
    int result = start;
    for (auto* worker : workers)
    {
      result = worker->process(result);  // Calls Ruby override!
    }
    return result;
  }
};
```

Ruby subclasses passed to C++ will have their overridden methods called:

```ruby
class Doubler < Worker
  def process(num)
    num * 2
  end
end

class Adder < Worker
  def process(num)
    num + 10
  end
end

handler = Handler.new
handler.add_worker(Doubler.new)
handler.add_worker(Adder.new)
handler.process_workers(5)  # => (5 * 2) + 10 = 20
```

This enables true polymorphism where C++ code can work with Ruby-defined subclasses seamlessly.
