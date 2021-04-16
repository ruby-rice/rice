===
STL
===

Rice provides partial support for C++'s standard template library (STL. To enable this support you need to include the ``rice/stl.hpp`` header file after ``rice/rice.hpp``.

Automatically Generated Ruby Class
-----------------------------------
Remember that STL classes are template classes, and thus must be instantiated with concrete types to define actual C++ classes. A ``std::vector<int>`` is a different class than a ``std::vector<std::string>``. This means there can be a large number of different container classes in a C++ code base.

To avoid having to manually define all these classes, Rice automatically generates Ruby classes that wrap these C++ classes. These Ruby classes are added to the ``Rice::Std`` namespace. Automatic classes work well in cases where Ruby code is accessing, or modifying, wrapped objects created in C++.

Automatic class creation does not work quite as well if Ruby needs to create a new instance of a container. That is because the generated class names are generated from C++ runtime type information and therefore end up long and ugly. For example, the pair ``std::pair<std::string, double`` ends up named:

    Rice::Std::Pair__basic_string__char_char_traits__char___allocator__char_____double__

Yikes!  So the rule of thumb - use generated class if you do not need to create instances from Ruby otherwise use manually created classes.

Not something you want to remember, or use, in your code.

As a result, Rice also let's you manually define Ruby class wrapper. The way to do this varies per STL type, but follows a simple naming scheme - ``define_pair``, ``define_vector``, etc. Please refer to the documentation for each supported STL type.

Note manual class names must be defined *before* any ``define_method``, ``define_function`` or ``define_attr`` are called that use either return or take a STL container type.