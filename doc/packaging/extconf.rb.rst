.. _extconf.rb:

extconf.rb
==========
The default way to build ``RubyGem`` extensions is to use ``extconf.rb``. However, ``extconf.rb`` was designed for ``C`` and not `C++`. Thus, Rice provides a file called ``mkmf-rice`` that adds additional functionality to ``mkmf`` including:

* Uses a C++ compiler instead of a C compiler
* Includes Rice header files automatically
* Sets required :ref:`compiler_settings`
* Links to the standard C++ library

Enabling
--------
To build your extension using ``extconf.rb``, add the following to your Gemspec (assuming the extension directory is ``ext``):

.. code-block:: ruby

    Gem::Specification.new do |spec|
      spec.extensions = ["ext/extconf.rb"]
    end

Example
-------
Below is an example ``extconf.rb``:

.. code-block:: ruby

    require 'mkmf-rice'

    dir_config('...')
    have_library('...')
    create_makefile('my_extension')

Disadvantages
-------------
Although ``extconf.rb`` is widely used to build Ruby extensions, it is not designed for C++. This means it can be tricky to get it to work for C++ extensions.

Aother disadvantage is that ``extconf.rb`` creates a ``Makefile`` that is then built either by ``make`` or ``nmake``. By default, ``make`` will only compile a single file at a time. For larger C++ extensions, such as ``ruby-opencv`` this results is very *long* compilation times.