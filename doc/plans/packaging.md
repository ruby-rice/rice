# Packaging Strategies For Rice Extensions

## Summary

Packaging is a separate concern from type sharing.

Rice should define a cross-extension type-sharing mechanism that works regardless of gem topology. Binding authors should then be free to choose the packaging model that fits their library, audience, and maintenance goals.

This document describes the main packaging shapes Rice should support and the tradeoffs between them.

## Design Principles

Packaging should not define type identity.

Whether a binding ships as one gem, multiple gems, or one gem with multiple native extensions, shared type identity should still come from:

* Ruby class path
* cached normalized C++ type key

The packaging layer should remain policy chosen by the binding author.

## Supported Packaging Shapes

Rice should support at least these publication patterns.

### One Gem, One Native Extension

This is the simplest model.

Characteristics:

* one gemspec
* one native extension target
* one primary `require`

Advantages:

* easiest to explain to users
* simplest release process
* no cross-extension attach requirements inside the project itself

Costs:

* larger native binary
* larger build surface
* weak support for optional feature subsets

### One Gem, Many Native Extensions

One gem ships several native libraries and a Ruby loader coordinates them.

Characteristics:

* one gemspec
* one published artifact
* multiple native extension targets
* runtime can eager-load or lazy-load extension libraries

Advantages:

* keeps installation simple for users
* lets the project split build/runtime boundaries internally
* supports conditional builds based on available upstream modules

Costs:

* loader logic becomes more complex
* type sharing inside the gem still matters
* shipped source payload may remain large even if native binaries are split

### Umbrella Gem Plus Module Gems

A small top-level gem depends on one or more module gems.

Characteristics:

* one user-facing umbrella package
* real implementation split across subgems
* dependency graph controls which modules are installed by default

Advantages:

* can reduce install/build surface for optional modules
* keeps a simple top-level entrypoint
* versioning can stay coordinated while artifacts remain split

Costs:

* more release management
* more gemspecs and packaging metadata
* more chances for dependency skew if versions drift

### Only Module Gems

Users install the exact pieces they need.

Characteristics:

* no umbrella package required
* module boundaries are explicit in the package list

Advantages:

* smallest install footprint for targeted users
* packaging aligns closely with module boundaries

Costs:

* highest complexity for users
* weakest “just install this” story
* version compatibility between modules must be managed carefully

## Runtime Loading

Once a project has more than one native extension, it still needs a runtime loading policy.

The common choices are:

* eager-load everything that was built
* load a minimal foundation, then lazy-load feature modules
* load only core by default and require explicit module entrypoints

Rice should not force one policy globally.

## Wrapper Namespaces And Packaging

Wrapper namespace choice affects packaging freedom.

If generated helper/container types stay global in `Rice::*` and `Std::*`:

* wrapper sharing becomes effectively process-global
* unrelated projects are more likely to collide
* packaging flexibility exists, but global namespace pressure remains

If generated wrapper types move under the binding family's public namespace:

* packaging becomes more independent across projects
* one library family can choose one gem while another chooses many gems
* unrelated projects are less likely to interfere with each other

So packaging and wrapper namespaces are separate decisions, but they influence each other strongly.

## Example: OpenCV

`opencv-ruby` can reasonably choose:

* one published gem
* multiple internal native module libraries
* CMake deciding which pieces can be built from the locally available OpenCV installation
* runtime lazy-loading of feature modules

This is a good fit when:

* the author values a simple install command
* shipped source volume is not a major concern
* local build variability is acceptable

## Example: Qt

A Qt binding author may reasonably choose:

* multiple gems
* package boundaries closer to upstream Qt modules
* explicit dependencies between those gems

This is a good fit when:

* package boundaries are important to the author
* optional install surface matters
* upstream module structure is meaningful to users

## Recommendation

Rice should not prescribe one universal packaging model.

The preferred Rice-level position is:

* type sharing is a mechanism Rice provides
* packaging is a policy binding authors choose
* docs should present supported patterns and tradeoffs, not one mandatory topology

## Non-Goals

This document does not attempt to:

* define a mandatory gem naming convention
* require one packaging shape for all Rice users
* make packaging decisions part of type identity
