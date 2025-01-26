.. _callbacks:

Callbacks
=========
Starting with version 4.5, Rice supports callbacks.

C Style Callbacks
-----------------
C++ libraries oftentimes implement C style callbacks, which are simply pointers to functions. Rice enables Ruby methods, lambdas and procs to serve as callback functions.

As an example, OpenCV defines a ``MouseCallback`` callback function. The client registers the callback, which is then triggered whenever the user performs an action with the mouse. It is defined like this:

.. code-block:: cpp

  typedef void (*MouseCallback)(int event, int x, int y, int flags, void* userdata);
  void setMouseCallback(const String& winname, MouseCallback onMouse, void* userdata = 0);

First, expose the register function to Ruby:

.. code-block:: cpp

  // Create a new module
  Module rb_mCv = define_module("Cv");

  // Expose the register method
  rb_mCv.define_module_function("set_mouse_callback", &cv::setMouseCallback,
    Arg("winname"), Arg("on_mouse"), Arg("userdata") = static_cast<void *>(0));

Next, in Ruby, define a Proc to handle the callback and then register it:

.. code-block:: ruby

  # Create a Proc to handle the event
  on_mouse_event = Proc.new do |event, x, y, flags, user_data|
    if event == Cv::MouseEventTypes::EVENT_LBUTTONDOWN.to_i
      # Do Stuff
    end
  end

  # Register the proc
  Cv::set_mouse_callback("Starry", on_mouse_event)
  Module rb_mCv = define_module("Cv");

You can also use ``lambdas`` in addition to ``Procs``

User Data
---------
Notice that the ``MouseCallback`` callback defines a parameter called ``userdata`` which has a type of ``void*``. This is a common pattern in C style callbacks and allows clients to pass information into the callback - its a way of managing state.

Rice enables Ruby code to pass Ruby objects from the client to the callback. To do this, you must tell Rice that it should not try to convert the Ruby object to C++ or from C++ to Ruby when the callback is called. This is done by using the ``setOpaque`` method:

.. code-block:: cpp

  rb_mCv.define_module_function("set_mouse_callback", &cv::setMouseCallback,
    Arg("winname"), Arg("on_mouse"), Arg("userdata").setOpaque() = static_cast<void *>(0));

Notice the addition of ``Arg("userdata").setOpaque()``. Ruby code can now call register function like this:

.. code-block:: ruby

  Cv::set_mouse_callback("Starry", on_mouse_event, self)

This allows the current Ruby object, self, to pass a reference to itself to the callback method.

However, this only solves 1/2 the problem - passing Ruby objects unchanged to C++. When C++ invokes the callback, by default Rice translates all C++ parameters to their Ruby equivalents. Of course, this does not make sense for the self reference, so we need to tell Rice not to do it. This is done by using the ``define_callback`` function.

.. code-block:: cpp

  define_callback<cv::MouseCallback>(Arg("event"), Arg("x"), Arg("y"), Arg("flags"), Arg("user_data").setOpaque());

The important part to understand is the fifth argument, which is specified as ``Arg("user_data").setOpaque()``. This tells Rice to not covert the parameter from C++ to Ruby.

Finally, let's update the Ruby proc:

.. code-block:: ruby

  on_mouse_event = Proc.new do |event, x, y, flags, self|
    self.mouse_event_count+=1
  end

