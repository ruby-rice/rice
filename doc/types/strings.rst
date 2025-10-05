.. _type_strings:

Strings
=======
Ruby and C++ handle strings very differently. In Ruby, every string is associated with an encoding, such as UTF-8. Strings with encodings of binary, or USASCII-8bit, are treated as byte arrays

In contrast, C++ has no concept of encodings. It is up to the progammer to figure out how data is stored in a string.

Rice handles this difference by converting strings between the language as-is. In other words, it simply copies bytes from a Ruby string to a C++ string and vice versa. Thus any C++ string returned to Ruby will have its encoding set to binary.

If you know that a C++ API requires a certain encoding, such as UTF-8, then first convert the string in Ruby before passing it to C++. Or, if you know a C++ string is in a specific encoding then call ```String::force_encoding``` in Ruby.