.. _callbacks:

Callbacks
=========
Starting with version 4.5, Rice supports callbacks.

C Style Callbacks
-----------------
C++ libraries sometimes implement C style callbacks, which are pointers to functions. Rice enables Ruby methods, lambdas and procs to serve as callback functions.

As an example, OpenCV defines a ``MouseCallback`` signature:

.. code-block:: cpp

  typedef void (*MouseCallback)(int event, int x, int y, int flags, void* userdata);

It then provides a function to register a callback:

.. code-block:: cpp

  void setMouseCallback(const String& winname, MouseCallback onMouse, void* userdata = 0);

Once clients register a callback, then it will be called whenever the user performs an action with the mouse.

To wrap this code in Rice, first expose the register function to Ruby:

.. code-block:: cpp

  // Create a new module
  Module rb_mCv = define_module("Cv");

  // Expose the register method
  rb_mCv.define_module_function("set_mouse_callback", &cv::setMouseCallback,
    Arg("winname"), Arg("on_mouse"), Arg("userdata") = static_cast<void *>(0));

Next, in Ruby, define a Proc to handle the callback and then call the register exposed register function:

.. code-block:: ruby

  # Create a Proc to handle the event
  on_mouse_event = Proc.new do |event, x, y, flags, user_data|
    if event == Cv::MouseEventTypes::EVENT_LBUTTONDOWN.to_i
      # Do Stuff
    end
  end

  # Register the proc
  Cv::set_mouse_callback("Starry", on_mouse_event)

You can also use ``lambdas`` in addition to ``Procs``

User Data
^^^^^^^^^
Notice that the ``MouseCallback`` callback defines a parameter called ``userdata`` which has a type of ``void*``. This is a common pattern in C style callbacks and allows clients to pass information into the callback - it's a way of handling state.

Rice enables Ruby code to pass Ruby objects from the client to the callback. To do this, you must tell Rice that it should not try to convert the Ruby object to C++ or from C++ to Ruby. This is done by using the ``setOpaque`` method:

.. code-block:: cpp

  Module rb_mCv = define_module("Cv");
  rb_mCv.define_module_function("set_mouse_callback", &cv::setMouseCallback,
    Arg("winname"), Arg("on_mouse"), Arg("userdata").setOpaque() = static_cast<void *>(0));

Notice the addition of ``Arg("userdata").setOpaque()``. Ruby code can now call the register function like this:

.. code-block:: ruby

  Cv::set_mouse_callback("Starry", on_mouse_event, self)

This allows the current Ruby object, ``self``, to pass a reference to itself to the callback method.

However, this only solves 1/2 the problem - passing a Ruby object unchanged to C++. When C++ later invokes the callback, Rice will try to translate it from C++ to Ruby. Of course, that does not make sense for the self reference, so we need to tell Rice not to do it. This is done by using the ``define_callback`` function.

.. code-block:: cpp

  define_callback<cv::MouseCallback>(Arg("event"), Arg("x"), Arg("y"), Arg("flags"), Arg("user_data").setOpaque());

Notice the fifth argument, which is specified as ``Arg("user_data").setOpaque()``. This tells Rice to not covert the parameter from C++ to Ruby.

Finally, let's update the Ruby proc:

.. code-block:: ruby

  on_mouse_event = Proc.new do |event, x, y, flags, self|
    self.mouse_event_count+=1
  end

Associating Callbacks with Procs
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Remember that C style callbacks are simply function pointers, they do not have a place to store any state. Thus when a C style callback is invoked, Rice needs to determine what Ruby ``Proc`` to call. This is a tricky problem to solve.

Simple Callback
"""""""""""""""
In the simplest case, a callback is only used once in a code base. Thus there is a one-to-one mapping between a callback and its associated Ruby ``Proc``.

This is easy to implement - Rice generates a new C++ class based on the callback's signature using the `NativeCallbackSimple <https://github.com/ruby-rice/rice/blob/master/rice/detail/NativeCallbackSimple.hpp>`_ class template. The generated class has a static member field that stores the ``Proc``. Thus every callback is associated with a single instantiation of the ``NativeCallbackSimple`` template.

LibFFI Callback
"""""""""""""""
However, a library often times use a callback in multiple places. For example:

.. code-block:: cpp

  void setMouseClickCallback(MouseCallback_T onSingleClick, MouseCallback_T onDoubleClick);
  void setMouseEnterExitCallback(MouseCallback_T onEnterExit);

The above code uses the same callback type 3 different times, thus the one-to-one mapping between callback type and C++ class is broken. Therefore the simple solution of using a static member variable to store the Ruby proc no longer works. Instead, we need to store 3 different ``Procs`` and figure out which one to call when the callback is invoked.

In this case, Rice uses libffi's `closure <https://github.com/libffi/libffi/blob/master/src/closures.c>`_ API. The closure API associates a piece of user data, in this case the ``Proc``, with a callback and then dynamically generates a new function which is what is invoked by the callback function.

Since you are working with Ruby, it is highly likely that LibFFI is already installed since the `Fiddle <https://github.com/ruby/fiddle>`_ gem requires it. The default build script will check for LibFFI and if it is found compile it into your bindings.

If you are using CMake, you will need to add a C++ preprocessor define called ``HAVE_LIBFFI`` and link to libffi.
