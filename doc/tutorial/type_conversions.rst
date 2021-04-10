=============================
User-defined type conversions
=============================

Rice provides default conversions for many built-in types. Sometimes,
however, the default conversion is not what is expected. For
example, consider a function:

.. code-block:: cpp

  void foo(char * x);

Is `x` a pointer to a single character or a pointer to the first character
of a null-terminated string or a pointer to the first character of an
array of char?

Because the second case is the most common use case (a pointer to the
first character of a C string), Rice provides a default conversion that
treats a `char *` as a C string. But suppose the above function actually
expects to receive a pointer to a single char instead?

If we write this:

.. code-block:: cpp

  extern "C"
  void Init_test()
  {
    define_global_function("foo", &foo);
  }

It will likely have the wrong behavior.

To avoid this problem, it is necessary to write a wrapper function where
the extension can be more explicit about how to handle the parameters:

.. code-block:: cpp

  Object wrap_foo(Object o)
  {
    char c = from_ruby<char>(o);
    foo(&c);
    return to_ruby(c);
  }

  extern "C"
  void Init_test()
  {
    define_global_function("foo", &wrap_foo);
  }

Note that the out parameter is returned from `wrap_foo`, as Ruby does not
have pass-by-variable-reference (it uses pass-by-object-reference).
