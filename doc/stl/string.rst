std::string
-----------
Perhaps std::string is the most commonly used class in C++ standard template library. Rice treats std::strings as  Builtin types, which means strings are copied between C++ and Ruby. Thus, if you pass a std::string to Ruby and modify it in Ruby, C++ will not see the changes (and vice versa).

Unlike Ruby, C++ has very little support for encodings. Thus it is a guessing game to correctly translate strings between C++ and Ruby and its up to you - the programmer - to get it right.

When Rice converts a std::string to Ruby it is assumed to have the encoding specified by ``Encoding.default_external``. That is likely to be UTF-8 on Windows while on Linux and MacOS it is based on the operating system locale. If no external encoding is specified, the converted string will have no encoding at all which is signified by an encoding of ASCII-8BIT. If the encoding is incorrect, then you can fix it in Ruby by calling ``String#force_encoding``.

In contrast, when Rice converts a Ruby string to a ``std::string`` it simply passes the underlying ``char`` buffer to std::string for copying. Thus make sure the Ruby string has the correct encoding before it its converted to C++.

Note that Rice does not support ``std::wstring``.
