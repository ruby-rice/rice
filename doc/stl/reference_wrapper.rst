.. _std_reference_wrapper:

std::reference_wrapper
======================
Introduced in C++11, ``std::reference_wrapper`` wraps C++ references in a copyable, assignable object. This allows them to be stored in containers such as ``std::vector`` or other types such as ``std::variant``.

Since Ruby does not have an equivalent type, Rice unwraps ``std::reference_wrapper`` instances. If the ``std::reference_wrapper`` points to a Builtin type, then it will be converted to the appropriate Ruby type. It it points to a non Builtin type, then the value will be wrapped by Ruby.

When passing a Ruby instance to ``std::reference_wrapper``, Rice will convert a Ruby type into the appropriate C++ type and store it inside the wrapper. Note this can be dangerous because Rice must maintain the original memory location for the reference to remain valid. For wrapped types this means the reference will remain valid as long as the wrapping Ruby object is valid and not garbage collected. For BuiltIn types, Rice only guarantees the reference to be valid through the lifetime of a method call. Thus if called C++ code stores the reference, and later tries to use it, an exception will happen.
