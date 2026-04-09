# Type Sharing Across Rice Extensions

## Summary

Rice type sharing matters whenever more than one Rice extension is loaded into the same Ruby process. This includes libraries split into multiple native modules or gems, but it also includes independently authored Rice extensions that need to interoperate correctly. Modular packaging is only one instance of the broader cross-extension type-sharing problem.

Rice should improve cross-extension correctness without trying to rescue extensions that define public classes in namespaces they do not own. Ruby modules and class paths remain the primary ownership mechanism.

## Why This Matters

There are at least two important use cases:

* one project split into multiple native extensions, such as OpenCV or Qt bindings
* multiple independent Rice extensions from different authors loaded into the same Ruby process

Both cases run into the same underlying problem: one extension may need to recognize, accept, or return Ruby classes and C++ types that were first bound by another extension.

## Current Behavior

Rice has two distinct layers of type state:

* `Data_Type<T>` local static binding state inside a compiled extension
* `TypeRegistry` local runtime lookup/cache state used for conversion, verification, and inheritance

These are related but not the same thing.

Historically, split Rice-based bindings could appear to work on Linux because default symbol visibility sometimes allowed Rice globals to be shared implicitly across `.so` files. In practice, the first loaded extension could become the master `TypeRegistry`. Issue [#355](https://github.com/ruby-rice/rice/issues/355) shows this clearly: split Qt bindings worked on Linux but failed on Windows.

That accidental Linux behavior should not be treated as architecture. The default `CMakePresets.json` now tells GCC and Clang not to export symbols by default, so implicit Rice-global sharing should no longer be assumed even on Linux.

The main current failure is not that `bind()` is too simple. The main failure is that lazy access to a locally unbound `Data_Type<T>` fails too early:

* `klass()` and `ruby_data_type()` call `check_is_bound()` and raise
* `is_descendant()` silently returns `false` if local state is missing

## Identity And Compatibility

The canonical identity for a shared binding is the Ruby class path.

If two extensions define classes in a namespace they do not own and collide, that is developer error. Rice relies on normal Ruby namespace ownership rules rather than introducing a second ownership model.

The compatibility check for a shared binding is:

* same Ruby class path
* same normalized C++ type key cached on that Ruby class

This same rule can also be used when discovering derived classes through Ruby subclass relationships.

## Lazy Attach Design

The primary change should be a non-throwing lazy attach path, such as `try_bind()`, not a redesign of `bind()` itself.

`try_bind()` should:

* return immediately if local static state is already attached
* look up the existing Ruby class for `T`
* validate the cached normalized C++ type key
* hydrate local `klass_`
* hydrate local `rb_data_type_`
* add the local `TypeRegistry` entry
* update any waiting `unbound_instances()`
* return success or failure without throwing

Then:

* `check_is_bound()` should call `try_bind()` before throwing
* `klass()` and `ruby_data_type()` continue to rely on `check_is_bound()`
* `is_descendant()` should call `try_bind()` before returning `false`

Caching should be asymmetric:

* cache success locally
* do not cache failure

Load order will still matter. A miss can simply mean that the owning extension has not been loaded yet, so negative caching would be incorrect.

## Inherited Types

Arguments typed as a base class are already mostly fine. `From_Ruby<Base*>` and similar conversions rely on Ruby subclass checks, so another extension does not need to know every concrete subclass just to accept a `Base`.

The harder case is polymorphic returns typed as `Base*` or `Base&`.

Today, `TypeRegistry::figureType()` already behaves like this:

* return the most-derived locally known type if it is in the local registry
* otherwise fall back to the requested/base type

That base fallback is acceptable as the baseline behavior for v1.

The preferred enhancement is:

* on a local derived-type miss
* walk the Ruby subclass tree of the known base class
* compare each subclass's cached normalized C++ type key to the runtime type
* import matching descendants into the local cache
* retry lookup before falling back to base

This keeps `TypeRegistry` local while still improving cross-extension polymorphic returns for classes that are already loaded somewhere in Ruby.

## Shared `TypeRegistry` As An Alternative

The obvious alternative is to export or share one process-wide `TypeRegistry`.

That explains historical Linux behavior, where the first loaded extension could effectively become the master registry. It also explains why split bindings could appear to work without any explicit attach mechanism.

But this approach has major drawbacks:

* it is load-order dependent
* it is not portable to Windows
* it no longer matches the default hidden-symbol build configuration
* it is still not sufficient by itself, because local `Data_Type<T>` state may remain unbound
* it creates tighter native coupling between extensions

So shared `TypeRegistry` is a real historical behavior, not a hypothetical one, but it should not be the preferred design.

## Wrapper Namespace Options

Rice auto-generates essential helper and container types for large bindings. For libraries such as OpenCV, manually naming every instantiated template type is not realistic.

Examples include:

* `Rice::Pointer<T>`
* `Rice::Buffer<T>`
* `Std::Vector<T>`
* `Std::Map<K, V>`
* `Std::SharedPtr<T>`

There are two viable namespace models.

### Option 1: Global Rice-Owned Wrapper Namespaces

Generated helper/container classes stay in global public namespaces:

* `Rice::*`
* `Std::*`

Under this model:

* generated classes are process-global by Ruby class path
* sharing is mandatory for matching `Rice::*` and `Std::*` class paths
* one gem or many gems still share the same generated wrapper space

Advantages:

* closest to current Rice behavior and docs
* no new namespace configuration surface
* existing examples remain valid

Costs:

* unrelated Rice extensions can collide in the same wrapper namespace
* sharing is mandatory, not policy-controlled
* one global generated-wrapper universe is forced onto the whole process

### Option 2: Binding-Family-Owned Wrapper Namespaces

Generated helper/container classes live under the binding family's public namespace.

For OpenCV, examples could be:

* `Cv::Rice::Pointer<T>`
* `Cv::Rice::Buffer<T>`
* `Cv::Std::Vector<int>`
* `Cv::Std::Vector<Cv::Mat>`

Qt could choose its own parallel structure.

Under this model:

* generated wrappers are shared within one binding family
* unrelated projects no longer collide just because both use Rice
* packaging choices become freer because wrapper identity is scoped to the family namespace

Advantages:

* much better isolation between unrelated extensions
* better fit for modular multi-gem or multi-extension libraries
* same Ruby class path is a stronger identity rule because the namespace is author-owned

Costs:

* larger public API change
* Rice needs configuration for generated wrapper namespace roots
* generated wrapper names become part of each binding family's public API

Preferred long-term direction:

* binding-family-owned wrapper namespaces

## Packaging

Packaging should remain author policy, not a Rice-level requirement.

The type-sharing mechanism should work regardless of whether a project chooses:

* one gem, one native extension
* one gem, many native extensions
* umbrella gem plus module gems
* only module gems

Packaging tradeoffs and examples are documented separately in [packaging.md](packaging.md).

## Load Order

Load order will still matter.

Before the owning extension is loaded:

* `try_bind()` can miss
* hard-use sites such as `klass()` and `ruby_data_type()` will still raise
* soft-probe sites such as `is_descendant()` should simply return `false`

After the owning extension is loaded:

* the next `try_bind()` can succeed
* local state attaches once
* future accesses become cheap

The goal is not to eliminate load-order sensitivity. The goal is to make it predictable and avoid poisoning local state with cached misses.

## Non-Goals

This design does not attempt to:

* rescue extensions that define public classes in namespaces they do not own
* solve arbitrary unrelated collisions in the global namespace
* infer full native compatibility beyond Ruby class path plus normalized C++ type key
* force one packaging topology on all Rice users
