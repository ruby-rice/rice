# History

Rice originated as Excruby, a project to interface with C++-based trading software at Automated Trading Desk in Mount Pleasant, South Carolina. The Ruby bindings for Swig were at the time less mature than they are today, and did not suit the needs of the project.

Excruby was written as a set of helper functions and classes for interfacing with the Ruby interpreter in an exception-safe manner. Over the course of five years, the project grew into wrappers for pieces of the Ruby API, but the original helper functions remained as part of the public interface.

This created confusion for the users of the library,because there were multiple ways of accomplishing most tasks -- directly through the C API, through a low-level wrapper around the C API and through a high-level abstraction of the lower-level interfaces.

Rice was then born in 2008 as an attempt to clean up the interface. Rice keeps the lower-level wrappers, but as an implementation detail; the public interface is truly a high-level abstraction around the Ruby C API.

However, it was still difficult to use Rice to wrap C++ libraries. A major drawback was that Rice required its own compilation step to create is own library. At the time (and still today), there is not a standard way to install compiled C++ packages.

To address this, version 4, released in 2021, was a major rewrite that changed Rice into a header-only library. This made it much easier to use Rice to wrap C++ libraries. In addition, version 4 took full advantage of all the C++ template metaprogramming functionality in C++17 and higher to make it easier to create Ruby extensions for C++ libraries.

In 2025, Rice version 4.5 was released. Version 4.5 was based on learnings from wrapping the OpenCV library. OpenCV exposes a large C++ API that makes heavy use of C++ templates, overridden methods and constructors, C style callbacks and other C++ features. To successfully wrap the library required making numerous changes to Rice which are described in the [CHANGELOG.md](https://github.com/ruby-rice/rice/blob/master/CHANGELOG.md).
