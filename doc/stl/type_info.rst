.. _std_type_info:

std::type_info
==============
Instances of the class ``std:::type_info`` are returned by C++'s ``typeid`` operator and provides implementation-specific information about a type.

Ruby API
^^^^^^^^
Rice wraps ``std:::type_info``. The exposed API is:

* TypeInfo#hash_code (see `hash_code <https://en.cppreference.com/w/cpp/types/type_info/hash_code>`_)
* TypeInfo#name (see `name <https://en.cppreference.com/w/cpp/types/type_info/name>`_)
