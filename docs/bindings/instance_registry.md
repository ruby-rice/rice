# Instance Registry

## What It Does

The instance registry is a global `std::map<void*, VALUE>` that maps C++ instance addresses to their Ruby wrapper objects. Its purpose is deduplication: when the same C++ pointer or reference is returned to Ruby multiple times, the registry ensures the same Ruby object is returned each time rather than creating a new wrapper.

Without the registry, every time a C++ method returns a pointer or reference, Rice creates a new Ruby wrapper object. This means multiple Ruby objects may end up pointing at the same C++ object. For Ruby-owned objects, this creates a dangerous aliasing problem - when one Ruby wrapper is garbage collected it frees the C++ object, leaving the other wrappers with a dangling pointer.

## When It Applies

The registry only applies to pointer and references - cases where Ruby wraps a C++ object without copying it. These are the cases where multiple Ruby objects could alias the same C++ memory.

The registry does **not** apply to value returns. When C++ returns a value, Rice copies or moves it into wrapper-owned storage. Each Ruby object gets its own independent C++ copy, so there is no shared address to deduplicate.

## Modes

The registry has three modes:

```cpp
detail::Registries::instance.instances.mode = detail::InstanceRegistry::Mode::Owned; // default
```

| Mode    | Behavior                                                                |
|---------|-------------------------------------------------------------------------|
| `Off`   | No tracking. Every pointer/reference return creates a new Ruby wrapper. |
| `Owned` | Track only Ruby-owned C++ objects. **(default)**                        |
| `All`   | Track all wrapped C++ objects regardless of ownership.                  |

## Ruby-Owned vs C++-Owned Objects

Understanding the ownership distinction is essential:

- Ruby-owned: Ruby's garbage collector controls the C++ object's lifetime. When the Ruby wrapper is collected, it `delete`s the C++ object. This happens when a constructor creates the object, when `Return().takeOwnership()` is used or when a value is copied/moved into a wrapper.

- C++-owned: C++ controls the C++ object's lifetime. Ruby holds a reference to the C++ object but will not free it. This is the default for pointer and reference returns.

### Why the Default Mode is `Owned`

The default mode (`Owned`) only tracks Ruby-owned objects because this is the case where deduplication is both safe and necessary:

Ruby-owned objects (tracked): Ruby controls the lifetime, so the mapping from C++ address to Ruby VALUE remains valid for as long as the Ruby object is alive. Deduplication prevents the dangerous double-wrapper aliasing described above - if two Ruby objects both "own" the same C++ pointer, the first to be garbage collected will free the C++ object and the second will later crash.

C++-owned objects (not tracked): C++ controls the lifetime, so Rice cannot know when the C++ object is freed. If the registry held a mapping for a C++-owned object and C++ freed it, the mapping would become stale. Worse, if C++ then allocates a new object at the same address, the registry would incorrectly return the old (now-invalid) Ruby wrapper for the new C++ object. By not tracking C++-owned objects, this class of bugs is avoided entirely.

### What Happens When C++-Owned Objects Are Not Tracked

Without tracking, each time a C++ method returns the same pointer or reference (without transferring ownership), Rice creates a new Ruby wrapper. Multiple Ruby objects will reference the same C++ memory, but this is safe because none of them will free it - C++ retains ownership.

There is one exception: Rice detects when a C++ method returns a pointer/reference to `self` (the receiver object) and returns the existing Ruby `self` rather than creating a new wrapper. See [return self](methods.md#return-self).

## Mode::All

`Mode::All` tracks both Ruby-owned and C++-owned objects. This avoids creating duplicate wrappers for C++-owned objects and can improve performance by reducing Ruby object allocations.

However, it introduces the risks described above: if C++ frees a tracked object and a new object is allocated at the same address, the registry will return a stale Ruby wrapper. Use this mode only when you can guarantee that tracked C++ objects outlive their Ruby wrappers.

## Mode::Off

`Mode::Off` disables the registry entirely. Every pointer/reference return creates a new Ruby wrapper. This means Ruby-owned pointer returns are not deduplicated, which can lead to the double-wrapper aliasing problem described above. Use this mode only if you are certain that Ruby-owned C++ objects are never returned by pointer/reference more than once.

## Thread Safety

The registry is currently protected by Ruby's GVL. All `lookup`, `add`, and `remove` operations occur under the GVL in normal operation. Wrapper destructors (which call `remove`) run during garbage collection, which also holds the GVL.

However, the registry is not safe under Ractors. Ractors enable true parallel execution, bypassing the GVL. Since the registry is global shared mutable state (`Registries::instance`) with no internal synchronization, concurrent access from multiple Ractors is undefined behavior.

Additionally, the current caller code performs a non-atomic lookup-then-insert: it calls `lookup`, checks the result, creates a wrapper, then calls `add`. Under parallel execution, two Ractors wrapping the same C++ pointer can both see `Qnil` from `lookup`, both allocate a wrapper, and both call `add` — the second silently overwrites the first, leaking a wrapper and orphaning a Ruby object.
