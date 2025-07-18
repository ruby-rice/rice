$:.unshift File.expand_path(File.dirname(__FILE__))
require 'lib/rice/version'

$spec = Gem::Specification.new do |s|
  s.name = 'rice'
  s.version = Rice::VERSION
  s.license = "MIT"
  s.summary = 'Ruby Interface for C++ Extensions'
  s.homepage = 'https://github.com/ruby-rice/rice'
  s.authors = ['Paul Brannan', 'Jason Roelofs', 'Charlie Savage']
  s.email = ['curlypaul924@gmail.com', 'jasongroelofs@gmail.com', 'cfis@savagexi.com']

  s.description = <<-END
Rice is a C++ interface to Ruby's C API. It provides a type-safe and
exception-safe interface in order to make embedding Ruby and writing
Ruby extensions with C++ easier.
  END

  s.metadata = {
    "bug_tracker_uri"   => "https://github.com/ruby-rice/rice/issues",
    "changelog_uri"     => "https://github.com/ruby-rice/rice/blob/master/CHANGELOG.md",
    "documentation_uri" => "https://ruby-rice.github.io",
    "source_code_uri"   => "https://github.com/ruby-rice/rice",
  }

  s.test_files = Dir['test/ruby/*.rb']
  s.extra_rdoc_files = ['README.md']
  s.require_paths = ['lib']

  s.files = Dir[
    # Documentation
    'CHANGELOG.md',
    'CONTRIBUTORS.md',
    'COPYING',
    'README.md',

    # Ruby files
    'Gemfile',
    'Rakefile',
    'rice.gemspec',

    # CMake Files
    'CMakeLists.txt',
    'CMakePresets.json',
    'FindRuby.cmake',

    # rice.hpp
    'include/rice/rice.hpp',
    'include/rice/stl.hpp',

    # Source files
    'rice/**/*.?pp',

    # Ruby files
    'lib/**/*.rb',

    # Sample
    'sample/cpp/*.?pp',
    'sample/cpp/*.def',
    'sample/cpp/CMakeLists.txt',
    'sample/images/*.bmp',
    'sample/ruby/*.rb',

    # Test source files
    'test/*.?pp',
    'test/extconf.rb',
    'test/ext/t1/extconf.rb',
    'test/ext/t1/*.*pp',
    'test/ext/t2/extconf.rb',
    'test/ext/t2/*.*pp'
  ]

  s.required_ruby_version = ">= 3.1"
  s.add_dependency "ostruct"
  s.add_development_dependency "bundler"
  s.add_development_dependency "rake"
  s.add_development_dependency "minitest"
end
