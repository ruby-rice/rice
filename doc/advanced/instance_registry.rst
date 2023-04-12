.. _Instance Registry:

Instance Registry
=================

Rice 4.1 added an instance registry which tracks which C++ objects have been wrapped by Ruby objects. This done via a global ``std::map`` maintained by Rice.

Enabled
-------
When the instance registry is enabled, Rice will check if a C++ instance has been wrapped by a Ruby instance. If it has, then the existing Ruby instance is returned.

By default, instance tracking is disabled. To turn it on:

.. code-block:: cpp

  detail::Internal::intance.instances.isEnabled = true;

Disabled
--------
When the instance registry is disabled, Rice will wrap a C++ instance in a new Ruby instance regardless of whether it is already wrapped by a Ruby instance. There is one exception to this rule, which happens when a C++ method returns back itself. Rice recognizes that the C++ object is wrapped by the Ruby object making the call, and thus it is returning self.

Why is Tracking Disabled?
-------------------------
Enabling the instance registry can significantly increase performance. Although tracking introduces a small amount of overhead, it avoids creating duplicate Ruby objects and C++ wrapper objects.

However, its unknown if tracking is fully reliable. There are a few potential issues.

First, the implementation is not thread-safe. Due to Ruby's GIL, this is not considered an issue.

Second, pairs in the global map are removed when a Ruby object is freed by the garbage collector. There could be a window where a Ruby object is marked for deletion but the underlying C++ object is returned back to Ruby. Then the Ruby object would be freed resulting in a crash. It is unknown if this really happens, it has never been observed.

Third, a C++ instance wrapped by Ruby instance may be freed on the C++ side. As long as ownership rules have been correctly setup, this is fine. However, if a new C++ instance is created that has the same address as the deleted C++ object and then is passed to Ruby the instance tracker will return the old deleted object. This has been observed in the Rice tests. It is unknown if this is due to how the tests are written or is a more general problem.