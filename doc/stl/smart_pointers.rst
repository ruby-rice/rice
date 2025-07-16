.. _smart_pointers:

Smart Pointers
==============
Smart pointers are key tool in modern C++ to write memory safe code. Rice provides support for ``std::unique_ptr`` and ``std::shared_ptr``. Rice can also be easily extended to support custom smart pointer types.

std::unique_ptr
--------------- 
When a native method returns a ``std::unique_ptr``, Rice copies it via its move constructor. Therefore, Rice takes ownership of the ``std::unique_ptr``, which in turns owns its underlying object.  

Use of the ``std::unique_ptr`` is transparent to Ruby code. As far a Ruby is concerned, its wrapping the type managed by the unique pointer, thus ``std::unique_ptr::element_type``. As a result, there is not a Ruby visible API to the ``std::unique_ptr`` itself.

Rice supports ``std::unique_ptr`` out of the box - there is no additional work you have to do except include the "rice/stl.hpp" header. Let's take a look at an example:

.. code-block:: cpp

  class MyClass
  {
  public:
    int flag = 0;

  public:
    void setFlag(int value)
    {
      this->flag = value;
    }
  };

  class Factory
  {
  public:
    std::unique_ptr<MyClass> transfer()
    {
      return std::make_unique<MyClass>();
    }
  };

  int extractFlagUniquePtrRef(std::unique_ptr<MyClass>& myClass)
  {
    return myClass->flag;
  }

  void setupRice()
  {
    define_class<MyClass>("MyClass").
      define_method("set_flag", &MyClass::setFlag);

    define_class<Factory>("Factory").
      define_constructor(Constructor<Factory>()).
      define_method("transfer", &Factory::transfer);

    define_global_function("extract_flag_unique_ptr_ref", &extractFlagUniquePtrRef);
  }

And in Ruby:

.. code-block:: cpp

  factory = Factory.new
  my_instance = factor.transfer
  my_instance.set_flag(5)
  flag = extract_flag_unique_ptr_ref(my_instance)

When ``myInstance`` goes out of scope and is garbage collected, it will free the ``std::unique_ptr`` it wraps which will in turn free the C++ MyClass instance that it manages.

Note Rice does not support transferring ownership of the ``std::unique_ptr`` back to C++ by passing it as a function parameter to a native function. In other words, once a ``std::unique_ptr`` is transferred to Ruby it will be freed by Ruby. You can, however, pass back a reference as shown in the example above. Note that passing a reference to ``std::unique_ptr`` does *not* not transer its ownership.

std::shared_ptr
--------------- 
When a native method returns a ``std::shared_ptr``, Rice copies it via its move constructor and therefore owns one instance of the shared_pointer. Of course it is likely there are other copies of the ``std::shared_ptr`` owned by other objects.

Use of the ``std::shared_ptr`` is transparent to Ruby code. As far a Ruby is concerned, its wrapping the type managed by the shared pointer, thus ``std::shared_ptr::element_type``. As a result, there is not a Ruby visible API to the ``std::unique_ptr`` itself.

Rice supports ``std::shared_ptr`` out of the box - there is no additional work you have to do except include the "rice/stl.hpp" header. Let's take a look at an example:

.. code-block:: cpp

  class MyClass
  {
  public:
    int flag = 0;

  public:
    void setFlag(int value)
    {
      this->flag = value;
    }
  };

  class Factory
  {
  public:
    std::shared_ptr<MyClass> share()
    {
      if (!instance_)
      {
        instance_ = std::make_shared<MyClass>();
      }
      return instance_;
    }

  public:
    static inline std::shared_ptr<MyClass> instance_;
  };

  int extractFlagSharedPtr(std::shared_ptr<MyClass> myClass)
  {
    return myClass->flag;
  }

  int extractFlagSharedPtrRef(std::shared_ptr<MyClass>& myClass)
  {
    return myClass->flag;
  }

  void setupRice()
  {
    embed_ruby();

    define_class<MyClass>("MyClass").
      define_method("set_flag", &MyClass::setFlag);

    define_class<Factory>("Factory").
      define_constructor(Constructor<Factory>()).
      define_method("share", &Factory::share);

    define_global_function("extract_flag_shared_ptr", &extractFlagSharedPtr);
    define_global_function("extract_flag_shared_ptr_ref", &extractFlagSharedPtrRef);
  }

And in Ruby:

.. code-block:: ruby
  
  factory = Factory.new
  my_instance = factor.share
  my_instance.set_flag(5)
  flag = extract_flag_shared_ptr(my_instance)
  flag = extract_flag_shared_ptr_ref(my_instance)
  
When ``myInstance`` goes out of scope and is garbage collected, it will free the ``std::shared_ptr`` it wraps. That may or may not free the underlying C++ MyClass instance depending if there are other ``std::smart_pointer`` instances managing it.

Unlike ``std::unique_ptr``, you can pass a copy of a ``std::shared_ptr`` back to native code via a function parameter. However, Ruby will always maintain one copy of the shared pointer until the wrapper Ruby object is freed.

Custom Smart Pointer
--------------------
It is possible to extend Rice to support additional smart pointer types. Start by looking at ``stl/smart_ptr.hpp``. It defines the following template class that is used to store smart pointers:

.. code-block:: cpp

  namespace Rice::detail
  {
    template <template <typename, typename...> typename SmartPointer_T, typename...Arg_Ts>
    class WrapperSmartPointer : public Wrapper
    {
    public:
      WrapperSmartPointer(SmartPointer_T<Arg_Ts...>& data);
      void* get() override;
      SmartPointer_T<Arg_Ts...>& data();

    private:
      SmartPointer_T<Arg_Ts...> data_;
    };
  }

Assuming that your smart pointer fits into the template class, you need to tell Rice three things:

* How to wrap it in a Ruby object
* How to extract it from a Ruby object
* How to access its managed type

Let's start by telling Rice how to wrap the smart pointer. Here is how that is done for ``std::unique_ptr``:

.. code-block:: cpp

  namespace Rice::detail
  {
    template <typename T>
    struct To_Ruby<std::unique_ptr<T>>
    {
      VALUE convert(std::unique_ptr<T>& data)
      {
        std::pair<VALUE, rb_data_type_t*> rubyTypeInfo = detail::Registries::instance.types.figureType<T>(*data);

        // Use custom wrapper type 
        using Wrapper_T = WrapperSmartPointer<std::unique_ptr, T>;
        return detail::wrap<std::unique_ptr<T>, Wrapper_T>(rubyTypeInfo.first, rubyTypeInfo.second, data, true);
      }
    };
  }

First we specialize ``detail::To_Ruby`` for our smart pointer type, in this case ``std::unique_ptr``.
Next we pass the pointer to Rice's TypeRegistry to look up what Ruby class wraps the C++ class. Then we instantiate a Wrapper_T class from the ``WrapperSmartPointer``. Finally we pass the type information and pointer to a helper method that will first create an instance of Wrapper_T and then on a Ruby object to store it.

The second step is to tell Rice how to extract the smart pointer from Ruby.

.. code-block:: cpp

  namespace Rice::detail
  {
    template <typename T>
    struct From_Ruby<std::unique_ptr<T>&>
    {
      static std::unique_ptr<T>& convert(VALUE value)
      {
        Wrapper* wrapper = detail::getWrapper(value, Data_Type<T>::ruby_data_type());

        using Wrapper_T = WrapperSmartPointer<std::unique_ptr, T>;
        Wrapper_T* smartWrapper = dynamic_cast<Wrapper_T*>(wrapper);
        if (!smartWrapper)
        {
          std::string message = "Invalid smart pointer wrapper";
          throw std::runtime_error(message.c_str());
        }
        return smartWrapper->data();
      }
    };
  }

Similarly to above, we specialize ``detail::From_Ruby`` for our smart pointer type, in this case ``std::unique_ptr``.

Next we use a helper method to get back a pointer to the ``Wrapper`` instance we created in the ``detail::To_Ruby`` method. Note ``detail::Wrapper`` is the base class for our custom wrapper type, so we have to typecast it to ``Wrapper_T``. Finally we call its ``data()`` method to access to stored smart pointer.

Last, we have to tell Rice how to verify the type our smart pointer contains. That is done like this:

.. code-block:: cpp

    namespace Rice::detail
    {
      template<typename T>
      struct Type<std::unique_ptr<T>>
      {
        static bool verify()
        {
          return Type<T>::verify();
        }
      };
  }
