.. _Instance Tracking:

Instance Tracking
=================

Rice 4.1 added the ability to track which C++ objects have been previously wrapped by Ruby objects. In previous versions, when a C++ object was returned to Ruby a new Ruby object would be created with one exception for handling self. In that case, if a C++ member function returned ``this`` then the Ruby object (self) wrapping the C++ object would be returned.

Rice now has the ability to check if a C++ object has been wrapped by a live Ruby object. If yes, then that Ruby object is returned as opposed to creating a new one. The mapping is stored in a global ``std::map`` maintained by Rice.

It is possible that object tracking could introduce a small amount of overhead if tens of thousands of C++ objects are being returned to Ruby and thus need to be stored in the ``std::map``. However, in that case Rice would also avoid creating tens of thousands of additional Ruby objects, plus an internal wrapping class. Thus the net result would likely be quite a positive performance gain by avoiding fragmenting Ruby's memory and stressing the garbage collector.

By default, instance tracking is enabled. To turn it off:

.. code-block:: cpp

  INSTANCE_TRACKER.isEnabled = false;

Note that disabling object tracking will also disable ``to_enum`` support for STL containers.