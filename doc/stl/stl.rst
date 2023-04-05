.. _stl:

STL
===

Rice provides partial support for C++'s standard template library (STL). To enable this support you need to include the ``rice/stl.hpp`` header file after ``rice/rice.hpp``.

.. _stl_class_names:

Automatically Generated Ruby Classes
------------------------------------
Remember that STL classes are template classes, and thus must be instantiated with concrete types to define actual C++ classes. A ``std::vector<int>`` is a different class than a ``std::vector<std::string>``. This means there can be a large number of STL instantianted classes in a C++ code base.

To avoid having to manually define these classes, Rice automatically generates Ruby wrapper classes. These Ruby classes are added to the ``Rice::Std`` module. Automatic classes work well in cases where Ruby code is accessing, or modifying, wrapped objects created in C++.

Automatic class creation does not work quite as well if Ruby needs to create new instances of these classes. That is because the generated class names are generated from C++ runtime type information and therefore end up long and ugly. For example, the type ``std::pair<std::string, double>`` ends up named:

.. code-block:: ruby

    Rice::Std::Pair__basic_string__char_char_traits__char___allocator__char_____double__

Yikes!  So the rule of thumb - use generated classes if you do not need to create instances of them from Ruby. Otherwise use manually created classes.

Its easy to manually create class names. The way to do this varies per STL type, but follows a simple naming scheme - ``define_pair``, ``define_vector``, etc. Please refer to the documentation for each supported STL type.

Note manual class names can be defined *after* any auto generated classes. What happens is there is still only one class but it is referred to by multiple constants. For example if you call ``define_pair<std::pair<std::string, double>>(StringDoublePair)`` after the pair has been registered, in Ruby you will have two constants pointing to the class:

.. code-block:: ruby

    Rice::Std::Pair__basic_string__char_char_traits__char___allocator__char_____double__
    Object::StringDoubelPair
