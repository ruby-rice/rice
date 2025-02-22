.. _std_exception:

std::exception
==============
Most of the time you do not have to worry about C++ exceptions. Rice :ref:`automatically <exceptions>` handles C++ exceptions and converts them to the appropriate Ruby exception.

However, although rare, some C++ APIs define methods that take exception parameters. For example, OpenCV's API includes:

.. code-block:: cpp

    class AsyncPromise
    {
    public:
        /** Stores exception.
        @param[in] exception exception to be raised in AsyncArray */
        void setException(const cv::Exception& exception);
    };

Thus Rice provides access to creating new C++ exception instances:

.. code-block:: cpp

   exception = Std::Exception.new

Note currently only supports creating instances of ``std::exception`` and none of its subtypes.

Ruby API
^^^^^^^^
The Ruby API for ``std::exception`` is:

* Exception#new (see `constructor <https://en.cppreference.com/w/cpp/error/exception/exception>`_)
* Exception#what (see `what <https://en.cppreference.com/w/cpp/error/exception/what>`_)
