.. _std_exception_ptr:

std::exception_ptr
==================
Introduced in C++11, ``std::exception_ptr`` enables C++ exceptions to be captured. They can then be moved between threads, handled in different parts of code base or rethrown. Generally you will not need to use them in Ruby, but some C++ APIs expose them.

For example, OpenCV's API includes:

.. code-block:: cpp

    class AsyncPromise
    {
    public:
        /** Stores exception.
        @param[in] exception exception to be raised in AsyncArray */
        void setException(std::exception_ptr exception);
    };

Ruby API
^^^^^^^^
Currently Rice simply wraps ``std::exception_ptr`` so that it can be passed between Ruby and C++. However, Rice does not expose any constructors or methods. If this functionality is needed it can be added in the future.