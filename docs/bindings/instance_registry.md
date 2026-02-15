# Instance Registry

Rice 4.1 added an instance registry which tracks which C++ objects have been wrapped by Ruby objects. This done via a global `std::map` maintained by Rice.

## Enabled

When instance tracking is enabled (that is, mode is not `Off`), Rice will check if a C++ instance has already been wrapped by Ruby. If it has, then the existing Ruby instance is returned.

Instance tracking is configured only via mode:

```cpp
detail::Registries::instance.instances.mode = detail::InstanceRegistry::Mode::Owned; // default
// Off   -> disable tracking
// Owned -> track only Ruby-owned wrapped instances
// All   -> track both Ruby-owned and borrowed instances
```

## Disabled

When mode is `Off`, Rice will wrap a C++ instance in a new Ruby instance regardless of whether it is already wrapped by a Ruby instance. Therefore if you make multiple calls to a C++ method that returns the same C++ object each time via a reference or pointer, multiple wrapping Ruby objects will be created. By default having multiple Ruby objects wrap a C++ object is fine since the Ruby objects do not own the C++ object. For more information please carefully read the [C++ to Ruby](memory_management.md#c-to-ruby) topic.

There is one exception to this rule, which happens when a C++ method returns back itself. Rice recognizes that the C++ object is wrapped by the Ruby object making the call, and thus it is returning self (see [return self](methods.md#return-self)).

## Why is Tracking Disabled?

Enabling the instance registry can significantly increase performance. Although tracking introduces a small amount of overhead, it avoids creating duplicate Ruby objects and C++ wrapper objects.

However, its unknown if tracking is fully reliable. There are a few potential issues.

First, the implementation is not thread-safe. Due to Ruby's GIL, this is not considered an issue.

Second, pairs in the global map are removed when a Ruby object is freed by the garbage collector. There could be a window where a Ruby object is marked for deletion but the underlying C++ object is returned back to Ruby. Then the Ruby object would be freed resulting in a crash. It is unknown if this really happens, it has never been observed.

Third, a C++ instance wrapped by Ruby instance may be freed on the C++ side. As long as ownership rules have been correctly setup, this is fine. However, if a new C++ instance is created that has the same address as the deleted C++ object and then is passed to Ruby the instance tracker will return the old deleted object. This has been observed in the Rice tests. It is unknown if this is due to how the tests are written or is a more general problem.

## Rule for Ruby-Owned Objects

A useful practical rule is: if Ruby owns a C++ object, there should be only one Ruby object wrapping that C++ instance. Multiple Ruby wrappers for the same Ruby-owned C++ object can create lifetime bugs where one wrapper frees the underlying C++ object while another wrapper still exists and later dereferences a dangling pointer.

The instance registry addresses this by mapping C++ instance addresses to existing Ruby wrappers and reusing the same Ruby object instead of creating duplicates. In other words, when ownership is on the Ruby side, enabling instance tracking helps enforce a single-wrapper identity and avoids this class of aliasing/lifetime errors.

Value-return wrappers do not participate in instance tracking. When C++ returns values, Rice copies or moves them into wrapper-owned storage, so there is no stable shared native address identity to reuse. Instance tracking is therefore only meaningful for pointer/reference-style aliasing cases, not value copies.
