.. _Memory Management:

Memory Management
=================

The trickiest part of wrapping a C++ API is correctly managing memory shared between C++ and Ruby. It is critical to get this right - otherwise your program *will* crash. The key to getting it right is being crystal clear on who owns each piece of memory.

Rice divides native types into Builtin types and external types. Builtin types are copied between C++ and Ruby while external types are wrapped. For additional information about builtin types please refer to the :doc:`Type Conversions <type_conversions>` section.

The rest of this section discusses how to manage memory of external types.

.. _cpp_to_ruby:

C++ to Ruby
-----------
By default Rice assumes that C++ instances passed to Ruby continue to be owned by C++. Thus there is no transfer of ownership. In this case, the transfer follows these rules:

====================== ================ =========
Method Return Type (T) C++ to Ruby      Cleanup
====================== ================ =========
Value (T)              Copy constructor Ruby frees the copy, C++ the original
Reference (T&)         No copy          C++ frees the C++ instance
Pointer (T*)           No copy          C++ frees the C++ instance
====================== ================ =========

However, many APIs transfer ownership of returned values to the caller. Thus Ruby take ownership of the value. In this case the transfer follows these rules:

====================== ================ =========
Method Return Type (T) C++ to Ruby      Cleanup
====================== ================ =========
Value (T)              Copy constructor Ruby frees the copy, C++ the original
Reference (T&)         Move constructor Ruby frees the C++ instance
Pointer (T*)           No copy          Ruby frees the C++ instance
====================== ================ =========

.. _ownership:

Let's look at an example:

.. code-block:: cpp

  class MyClass
  {
  }

  class Factory
  {
  public:
    static MyClass* create()
    {
      return new MyClass();;
    }
  }

  extern "C"
  void Init_test()
  {
    Data_Type<MyClass> rb_cMyClass = define_class<MyClass>("MyClass");

    Data_Type<Factory> rb_cFactory = define_class<Factory>("Factory")
        .define_function("create", &Factory::create); <--- WRONG, results in memory leak
  }

Each time ``Factory#create`` is called from Ruby, a new C++ instance of MyClass will be created. Using Rice's default rules, this will result in a memory leak because those instance will never be freed.

.. code-block:: ruby

  1_000.times do
    my_class = Factory.create
  end

To fix this, you need to tell Rice that it should take ownership of the returned instance:

.. code-block:: cpp

  define_function("create", &Factory::create, Return().takeOwnership());

Notice the addition of the ``Return().takeOwnership()``, which creates an instance of the Return class and tells it to take ownership of the instance returned from C++.

.. _ruby_to_cpp:

Ruby to C++
-----------

Sometimes it is necessary to tie the lifetime of one Ruby object to another. This often times happens with containers. For example, imagine we have a ``Listener`` and a ``ListenerContainer`` class.

.. code-block:: cpp

  class Listener
  {
  };

  class ListenerContainer
  {
    public:
      void addListener(Listener* listener)
      {
        mListeners.push_back(listener);
      }

      int process()
      {
        for(const Listener& listener : mListeners)
        {
        }
      }

    private:
      std::vector<Listener*> mListeners;
  };

Assuming these classes are wrapped with Rice, the following code crash:

.. code-block:: ruby

  @handler = ListenerContainer.new
  @handler.add_listener(Listener.new)
  GC.start
  @handler.process !!!! crash !!!!!

The Ruby garbage collector will notice that the ``Listener.new`` object is orphaned and will free it. That it turn frees the underlying C++ Listener object resulting in a crash when ``process`` is called.

To prevent this, we want to tie the lifetime of the Ruby listener instance to the container. This is done by calling ``keepAlive()`` in the argument list:

.. code-block:: ruby

  define_class<ListenerContainer>("ListenerContainer")
    .define_method("add_listener", &ListenerContainer::addListener, Arg("listener").keepAlive())

With this change, when a listener is added to the container, the container keeps a reference to it and will call ``rb_gc_mark`` to keep it alive. This is exactly the same thing Ruby's collection classes, such as Arrays and Hashes, do. The ``Listener`` object will not be freed until the container itself goes out of scope.

Another example is when a returned object is dependent upon the original object. For example:

.. code-block:: cpp

  class Column;

  class Database
  {
  public:
    Database()
    {
      // connect to Database
    }

    ~Database()
    {
      // disconnect from database
    }

    Column getColumn(uint32_t index)
    {
       return Column(*this, index);
    }

    std::string looupName(uint32_t index)
    {
      return some_name;
    }
  };

  class Column
  {
  public:
    Column(Database& database, uint32_t index): database_(database), index_(index)
    {
    }

    Column getName()
    {
      return this->database.lookupName(this->index_):
    }

  private:
    Database& database_;
    uint32_t index_;
  };

Assuming these classes are wrapped with Rice, then the following Ruby code will crash:

.. code-block:: ruby

  def get_column(column_index)
    database = Database.new(...)
    column = database.get_column(column_index)
  end

  column = get_column(0)
  puts column.name

The problem is that the instance of the Database class created in ``get_column`` will likely be garbage collected when the method returns. As a result, when ``Column#name`` is called it will have a dangling reference to the no longer valid database object.

Obviously this code could be rewritten to make sure the database object remains alive throughout the program. Alternatively, you can tell Rice that to tie the lifetime of the Database object to the Column object so that it will not be freed until the Column is freed:

.. code-block:: ruby

  define_class<Database>("Database")
    .define_method("get_column", &Database::getColumn, Return().keepAlive())

Note that ``Return().keepAlive()`` will work with external types only. An attempt to use it with builtin type will result in a runtime exception.

C++ Referencing Ruby Objects
----------------------------

When referencing Ruby objects from C++, you need to let Ruby know about them so they are not prematurely garbage collected.

There are several ways this can happen:

Stack
^^^^^
If you are working with VALUEs or Objects stored on the stack, the Ruby garbage collector will try to find them automatically. However, optimizing compilers may prevent them from doing so. Thus you may need to use Ruby's `RB_GC_GUARD <https://docs.ruby-lang.org/en/3.2/extension_rdoc.html#label-Appendix+E.+RB_GC_GUARD+to+protect+from+premature+GC>`_ macro

Heap
^^^^
If you allocate an Object on the heap or if it is a member of an object that might be allocated on the heap, use ``Rice::Address_Registration_Guard`` to register the object with the garbage collector.

Member Variables
^^^^^^^^^^^^^^^^
If you create classes or structures that reference Ruby objects, you need to implement a custom ``ruby_mark`` function:

.. code-block:: cpp

  class MyClass
  {
    VALUE value_;
  }

  namespace Rice
  {
    template<>
    ruby_mark(const MyClass* myClass)
    {
      rb_gc_mark(myClass->value_);
    }
  }

  Data_Type<MyClass> class = define_class<MyClass>("MyClass")
            .define_constructor(Constructor<MyClass>());
