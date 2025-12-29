# Overview

Once you have wrapped a C++ library, the next step is to package it as a [RubyGem](https://guides.rubygems.org). As part of the RubyGem, you should either include a pre-built binary or enable the extension to be built upon installation.

This is done via the `extensions` field in a Gemspec:

```ruby
Gem::Specification.new do |spec|
  spec.extensions = ["ext/CMakeLists.txt"]
end
```

The name of the file determines what extension builder `RubyGems` invokes on installation. There are two built-in choices:

* extconf.rb - uses [extconf.rb](extconf.rb.md)
* CMakeLists.txt - uses [CMake](cmake.md)

Almost all `RubyGems` that build extensions use extconf.rb, so you will find extensive documentation on how to use it. In contrast, very few, if any, `RubyGems` use CMake. Thus you will not find any documentation outside of what Rice provides.

In general, for simple extensions use `extconf.rb` but otherwise use `CMake`.

It is also possible to use a custom build system, but in that case you will need to create a RubyGem [plugin](https://guides.rubygems.org/plugins/) to tell `RubyGems` how to make the build.

## Header Files

Rice is packaged as [header-only](https://en.wikipedia.org/wiki/Header-only) library. That means you do not have to worry about linking a shared Rice library with your extension.

The two headers are:

* rice/rice.hpp
* rice/stl.hpp

There are two ways to incorporate the headers into your build.

The first option is to install them by installing Rice - thus make Rice a required dependency of your Gem like this:

```ruby
Gem::Specification.new do |spec|
  spec.add_runtime_dependency('rice')
end
```

The second option is to simply copy them from [github](https://github.com/ruby-rice/rice/tree/master/include/rice) and copy them into your repo. Remember the provided link points to the master branch - you should pick a specific release tag and use those instead.

For information about required compiler and linker settings, see [Build Settings](build_settings.md).
