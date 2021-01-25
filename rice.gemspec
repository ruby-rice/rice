$:.unshift File.expand_path(File.dirname(__FILE__))
require 'ruby/lib/version'

$spec = Gem::Specification.new do |s|
  s.name = 'rice'
  s.version = Rice::VERSION
  s.license = "MIT"
  s.summary = 'Ruby Interface for C++ Extensions'
  s.homepage = 'https://github.com/jasonroelofs/rice'
  s.authors = ['Paul Brannan', 'Jason Roelofs']
  s.email = ['curlypaul924@gmail.com', 'jasonroelofs@gmail.com']

  s.description = <<-END
Rice is a C++ interface to Ruby's C API.  It provides a type-safe and
exception-safe interface in order to make embedding Ruby and writing
Ruby extensions with C++ easier.  It is similar to Boost.Python in many
ways, but also attempts to provide an object-oriented interface to all
of the Ruby C API.
  END

  s.extensions = ['extconf.rb']

  s.test_files = [
    'test/test_rice.rb',
  ]

  s.extra_rdoc_files = [ 'README.md' ]

  s.require_paths = [ 'ruby/lib' ]

  patterns = [
    # Documentation
    'COPYING',
    'README',
    'README.mingw',

    # Doxygen
    'Doxyfile',
    'doxygen.ac',
    'doxygen.am',

    'extconf.rb',

    # Autoconf
    'bootstrap',
    'ax_cxx_compile_stdcxx.m4',
    'configure.ac',
    'configure',
    'config.guess',
    'config.sub',
    'depcomp',
    'doxygen.ac',
    'doxygen.am',
    'install-sh',
    'missing',
    'post-autoconf.rb',
    'post-automake.rb',
    'ruby.ac',
    'aclocal.m4',

    # Makefiles
    'Rakefile',
    'Makefile.am',
    'Makefile.in',
    'rice/Makefile.am',
    'rice/Makefile.in',
    'ruby/Makefile.am',
    'ruby/Makefile.in',
    'ruby/lib/Makefile.am',
    'ruby/lib/Makefile.in',
    'sample/Makefile.am',
    'sample/Makefile.in',
    'test/Makefile.am',
    'test/Makefile.in',

    # C++ source files
    'rice/*.?pp',
    'rice/*.rb',
    'rice/config.hpp.in',
    'rice/detail/*.?pp',
    'rice/detail/ruby_version_code.hpp.in',

    # Library files
    'ruby/lib/mkmf-rice.rb.in',
    'ruby/lib/version.rb',

    # Samples
    'sample/enum/extconf.rb',
    'sample/enum/*.?pp',
    'sample/enum/*.rb',
    'sample/map/extconf.rb',
    'sample/map/*.?pp',
    'sample/map/*.rb',
    'sample/inheritance/extconf.rb',
    'sample/inheritance/*.?pp',
    'sample/inheritance/*.rb',
    'sample/callbacks/extconf.rb',
    'sample/callbacks/*.?pp',
    'sample/callbacks/*.rb',

    # Test source files
    'test/*.?pp',
    'test/ext/Makefile.am',
    'test/ext/Makefile.in',
    'test/ext/t1/extconf.rb',
    'test/ext/t1/*.*pp',
    'test/ext/t2/extconf.rb',
    'test/ext/t2/*.*pp'
  ]

  s.files = patterns.collect { |p| Dir.glob(p) }.flatten

  s.required_ruby_version = ">= 2.4"

  s.add_development_dependency "bundler"
  s.add_development_dependency "rake"
  s.add_development_dependency "minitest"
end
