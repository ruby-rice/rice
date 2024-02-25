.. _std_string_view:

std::string_view
-----------------
``std::string_view`` is a read-only reference to a sequence of `char`s. It provides a way of passing strings without the overhead of copying `std::string`. 

On input, Rice treats ``std::string_view`` as a Builtin type which means it copies the portion of the ``char`` sequence that a ``std::string_view`` references in C++ to Ruby. Please refer to the :ref:`_std_string_view` documentation to learn about how Rice handles encodings.my-refere

On output, Rice creates a ``std::string_view`` that references a Ruby string's underlying `char` buffer. Note this is DANGEROUS. Sooner or later the Ruby string will be garbage collected or moved as part of compaction, thus invalidating the `char` buffer. So use this with caution.