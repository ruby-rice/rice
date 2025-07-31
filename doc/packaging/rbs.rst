.. _rbs_files:

RBS Signature Files
===================
Ruby 3.0 added support for signature files, known as RBS files. `RBS <https://github.com/ruby/rbs>`_ is a language that describes the structure of a Ruby program including its type information.

Rice of course knows a lot about your extension's types! Starting with Rice 4.7, this information is available to Ruby via the :ref:`introspection_api` when it is :ref:`enabled <introspection_enable>`.

Generating RBS Files
--------------------
Use the ``rice_rbs`` script in the bin folder to generate RBS files for your extension.

To run it:

.. code-block:: bash

  rice_rbs [options] <rice_extension_library>

  Options:
    -o, --output=path                Output directory
    -v, --version                    Show version (1.1.0)
    -h, --help                       Show help message

The ``rice_extension_library`` should be the path to a compiled version of your extension. This will be a ``so`` file on Windows and Linux and a ``bundle`` on MacOS.