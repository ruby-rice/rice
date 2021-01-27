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

  s.test_files = Dir['test/ruby/*.rb']
  s.extra_rdoc_files = ['README.md']
  s.require_paths = ['ruby/lib']

  s.files = Dir[
    # Documentation
    'CHANGELOG.md',
    'CONTRIBUTORS.md',
    'COPYING',
    'README',
    'README.mingw',

    # Doxygen
    'Doxyfile',
    'doxygen.ac',
    'doxygen.am',

    # Other files
    'Gemfile',
    'Rakefile',

    # rice.hpp
    'include/rice.hpp',

    # Source files
    'rice/*.?pp',
    'rice/detail/*.?pp',

    # Ruby files
    'ruby/lib/mkmf-rice.rb',
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
    'test/extconf.rb',
    'test/ext/t1/extconf.rb',
    'test/ext/t1/*.*pp',
    'test/ext/t2/extconf.rb',
    'test/ext/t2/*.*pp'
  ]

  s.required_ruby_version = ">= 2.7"

  s.add_development_dependency "bundler"
  s.add_development_dependency "rake"
  s.add_development_dependency "minitest"
end
