# std::variant

Introduced in C++17, `std::variant` is a type safe Union. A variant can hold a single value of one of its supported types.

Since a Ruby variable can point to a value of any type, Ruby does not need or have an equivalent type. Thus Rice unwraps `std::variant` instances and converts the stored value to the appropriate Ruby type.

When passing a Ruby instance to `std::variant`, Rice will convert a Ruby type into the appropriate C++ type and store it inside the variant.
