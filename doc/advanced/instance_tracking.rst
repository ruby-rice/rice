.. _Instance Tracking:

Instance Tracking
=================

Rice 4.1 added the ability to track which C++ objects have been previously wrapped by Ruby objects. By default this ability is disabled.

Enabled
-------
When tracking is enabled, Rice will check if a C++ object has been wrapped by a live Ruby object. If yes, then that Ruby object is returned as opposed to creating a new one. The mapping is stored in a global ``std::map`` maintained by Rice.

By default, instance tracking is disabled. To turn it on:

.. code-block:: cpp

  INSTANCE_TRACKER.isEnabled = true;

Disabled
--------
When tracking is disabled, Rice will wrap a C++ object in a new Ruby object regardless of whether it is already wrapped by a live Ruby object. There is one exception to this rule, which happens when a C++ method returns back itself. Rice recognizes that the C++ object is wrapped by the Ruby object making the call, and thus it is returning self.

Why is Tracking Disabled?
-------------------------
Enabling object tracking can potentially significantly increase performance. Although tracking introduces a small amount of overhead, it avoids creating duplicate Ruby objects and C++ wrapper objects.

However, its unknown if tracking is fully reliable. There are a few potential issues.

First, the implementation is not currently thread-safe. Due to Ruby's GIL, this is not considered an issue.

Second, entries from the tracking map are removed when a Ruby object is freed by the garbage collector. There could be a window where a Ruby object is marked for deletion but the underlying C++ object is returned back to Ruby before the Ruby object is freed. Thus Ruby code would be using an invalid Ruby object. It is unknown if this really happens.

Third, a C++ object wrapped by Ruby may be freed on the C++ side. This is not a problem, but if a new C++ instance is created that has the same address as the deleted C++ object and then passed to Ruby then tracking map will return the wrong object. This does occasionally happen in the Rice tests. It is unknown if this is just due to how the tests are written or a more general issue.