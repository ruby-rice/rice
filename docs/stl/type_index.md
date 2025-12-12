# std::type_index

Introduced in C++11, `std::type_index` is a wrapper class around a `std::type_info` object, that can be used as index in associative and unordered associative containers.

Ruby does not have a concept of a type_index. Therefore, Rice wraps `std::type_index` which means that data is not copied between C++ and Ruby.

## Ruby API

The Ruby API for `std::type_index` is:

* TypeIndex#hash_code (see [hash_code](https://cppreference.com/w/cpp/types/type_index/hash_code.html))
* TypeIndex#name (see [name](https://cppreference.com/w/cpp/types/type_index/name.html))
