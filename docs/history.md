# History

Rice originated as Excruby, a project to interface with C++-based trading software at Automated Trading Desk in Mount Pleasant, South Carolina. The Ruby bindings for Swig at the time were not suitable for the needs of the project.

Excruby started as a set of helper functions and classes for interfacing with the Ruby interpreter in an exception-safe manner. Over the course of five years, the project grew into wrappers for pieces of the Ruby API, but the original helper functions remained as part of the public interface.

This created confusion for the users of the library, because there were multiple ways of accomplishing most tasks -- directly through the C API, through a low-level wrapper around the C API and through a high-level abstraction of the lower-level interfaces.

Rice was born in 2008 as an attempt to clean up the interface. Since then Rice has continued to evolve. It has gone through several eras:

| Era       | Versions  | Theme                                                       |
|-----------|-----------|-------------------------------------------------------------|
| 2003-2008 | Excruby   | Ruby API wrappers                                           |
| 2008-2010 | 1.2-1.4   | Foundation, Ruby 1.9                                        |
| 2011-2014 | 1.4.3-1.6 | C++11, Ruby 2.x                                             |
| 2015-2020 | 1.7-2.2   | Maintenance                                                 |
| 2021      | 3.0-4.0   | Header-only rewrite, C++17                                  |
| 2022-2024 | 4.0.x-4.3 | STL expansion                                               |
| 2025      | 4.5-4.9   | OpenCV-driven evolution (buffers, pointers, overloads, etc) |

---

## 2008-2010: Foundation

Rice kept the lower-level wrappers, but as an implementation detail hidden behind C++ classes that provided a higher high-level abstraction around the Ruby C API.

| Version      | Date     | Major Features                                     |
|--------------|----------|----------------------------------------------------|
| First commit | Jan 2008 | Rice development begins, evolved from Excruby      |
| 1.2.0        | Oct 2009 | First tagged release                               |
| 1.4.0        | Aug 2010 | Ruby 1.9.2, constructor defaults, implicit casting |

---

## 2011-2014: Ruby 2.x and C++11

Rice kept pace with Ruby's evolution through the 2.x series and adopted C++11 features like unique_ptr.

| Version | Date     | Major Features             |
|---------|----------|----------------------------|
| 1.5.0   | May 2013 | Ruby 2.0                   |
| 1.6.0   | Feb 2014 | Ruby 2.1, C++11 unique_ptr |

---

## 2015-2020: Maintenance

A quiet period of maintenance. Rice started to show its limitations - it was difficult to use Rice to wrap C++ libraries. A major drawback was that Rice required its own compilation step to create its own library. At the time (and still today), there is not a standard way to install compiled C++ packages.

| Version | Date     | Major Features        |
|---------|----------|-----------------------|
| 1.7.0   | Jan 2015 | Ruby 2.2              |
| 2.0.0   | Nov 2015 | Deprecated Ruby < 2.0 |
| 2.2.0   | Jan 2020 | Ruby 2.7              |

---

## 2021: Header-Only Rewrite

Version 4, released in 2021, was a major rewrite that changed Rice into a header-only library. This made it much easier to use Rice to wrap C++ libraries. In addition, version 4 took full advantage of C++ template metaprogramming functionality in C++17 and higher to make it easier to create Ruby extensions for C++ libraries.

| Version | Date     | Major Features                                              |
|---------|----------|-------------------------------------------------------------|
| 3.0     | Jan 2021 | C++14 minimum, GitHub Actions CI                            |
| 4.0     | Apr 2021 | Header-only, C++17, From_Ruby/To_Ruby redesign, STL support |

---

## 2022-2024: STL Expansion

With the header-only foundation in place, Rice expanded its STL support to cover more container types and use cases.

| Version | Date     | Major Features                                 |
|---------|----------|------------------------------------------------|
| 4.1.0   | Apr 2023 | std::map, std::variant, iterators, enumerators |
| 4.2.0   | Jan 2024 | Ruby 3.3, Buffer class                         |
| 4.3     | Feb 2024 | STL containers with pointers, std::string_view |

---

## 2025: OpenCV-Driven Evolution

In 2025, Rice underwent rapid evolution based on learnings from wrapping the OpenCV library. OpenCV exposes a large C++ API that makes heavy use of C++ templates, overloaded methods and constructors, C style callbacks and other C++ features. To successfully wrap the library required making numerous changes to Rice.

| Version | Date     | Major Features                                                   |
|---------|----------|------------------------------------------------------------------|
| 4.5.0   | Feb 2025 | Method/constructor overloading, keyword args, callbacks, rvalues |
| 4.6.0   | Jun 2025 | Buffer rewrite, std::tuple/set/multimap, const-aware types       |
| 4.7.0   | Oct 2025 | Introspection API, RBS generation, API doc generation            |
| 4.8.0   | Dec 2025 | 2x faster compilation, 30% smaller binaries                      |
| 4.9.0   | Dec 2025 | Smart pointer redesign (Std::SharedPtr, Std::UniquePtr)          |
