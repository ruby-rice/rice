.. _pointers:

Pointers
========
C++ makes heavy use of pointers. In general, Rice hides pointers from end-users. A pointer to a C++ object is generally wrapped by a standard Ruby object.

However, as described in the :ref:`type_mapping` section, Rice sometimes wrap pointers in ``Pointer<T>`` classes. This happens for pointers to fundamental types, pointers to pointers and arrays.

By default, Rice will :doc:`auto generate <../types/naming>` Pointer Ruby classes. For example, a pointer to an int, ``int*``, will be wrapped by a Ruby class named ``Rice::Pointer≺int≻``.

The ``Pointer<T>`` classes have no functionality - there are no methods defined on them. However, they can be passed to a C++ API that takes a pointer.

You can create ``Pointers`` from Ruby via a :doc:`Buffer <buffers>` class. ``Buffer`` classes also allow you to read and updae the memory a pointer references.