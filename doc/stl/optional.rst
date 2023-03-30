std::optional
-------------
Introduced in C++17, ``std::optional`` provides C++ code an additional way (besides pointers) to return values from functions that may not be set.

Since Ruby does not have an equivalent type, Rice unwraps ``std::optional`` instances. If the optional is empty, thus has a value of ``std::nullopt``, Rice converts it to ``nil`` in Ruby. If the optional has a value, and that value is a Builtin type, then it will be converted to the appropriate Ruby type. If it is not a Builtin type, then the value will be wrapped by Ruby.

When passing a Ruby instance to ``std::optional``, Rice will convert ``nil`` values to ``std::nullopt`` and non ``nil`` values to the appropriate C++ type.