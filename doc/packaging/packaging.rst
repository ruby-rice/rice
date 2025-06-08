.. _packaging:

========
Overview
========

Once you have wrapped a C++ library, the next step is to package it as a `RubyGem <https://guides.rubygems.org>`_. As part of the RubyGem, you should either include a pre-built binary or enable the extension to be built upon installation.

This is done via the ``extensions`` field in a Gemspec:

.. code-block:: ruby

    Gem::Specification.new do |spec|
      spec.extensions = ["ext/CMakeLists.txt"]
    end

The name of the file determines what extension builder ``RubyGems`` invokes on installation. There are two built-in choices:

* extconf.rb - uses :ref:`extconf.rb`
* CMakeLists.text - uses :ref:`CMake`

Almost all ``RubyGems`` that build extensions use extconf.rb, so you will find extensive documentation on how to use it. In contrast, very few, if any, ``RubyGems`` use CMake. Thus you will not find any documentation outside of what Rice provides.

In general, for simple extensions uses ``extconf.rb`` but otherwise use ``CMake``.

It is also possible to use a custom build system, but in that case you will need to create a RubyGem `plugin <https://guides.rubygems.org/plugins/>`_ to tell ``RubyGems`` how to make the build.

Header Library
==============
Rice is packaged as `header-only <https://en.wikipedia.org/wiki/Header-only>`_ library. That means you do not have to worry about linking a shared Rice library with your extension.

.. _compiler_settings:

Compiler Settings
=================
Rice extensions requires several compiler settings to be set.

For Clang and GCC:

.. code-block:: bash

    -std=c++17 -Wa,-mbig-obj -ftemplate-backtrace-limit=0

For MINGW:

.. code-block:: bash

    -std=c++17, -Wa,-mbig-obj

For Microsoft Visual C++:

.. code-block:: bash

    /std:c++17 /EHs /permissive- /bigobj /utf-8 -D_ALLOW_KEYWORD_MACROS -D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE

These options are described below:

C++17
-----
First, C++17 must be enabled since Rice makes heavy use of C++17 template and metaprogramming features.

Big Objects
-----------
Second, bigobject support needs to enabled. This tells the compiler to increase the number of sections allowed in a .obj file.

UTF-8
-----
Rice uses UTF-8 characters when :ref:`mapping<stl_class_names>` instantiated STL templates to Ruby class names.

Exception Handling Model
------------------------
For Visual C++, the default exception `model <https://learn.microsoft.com/en-us/cpp/build/reference/eh-exception-handling-model?view=msvc-170>`_ setting of ``/EHsc`` crashes Ruby when calling longjmp with optimizations enabled (/O2). Therefore use ``/EHs`` instead.

Template Backtrace
------------------
For g++, you must set ``-ftemplate-backtrace-limit=0`` to avoid compilation errors.




