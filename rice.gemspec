$:.unshift File.expand_path(File.dirname(__FILE__))
require 'lib/version'

$spec = Gem::Specification.new do |s|
  s.name = 'rice'
  s.version = Rice::VERSION
  s.license = "MIT"
  s.summary = 'Ruby Interface for C++ Extensions'
  s.homepage = 'https://github.com/ruby-rice/rice'
  s.authors = ['Paul Brannan', 'Jason Roelofs', 'Charlie Savage']
  s.email = ['curlypaul924@gmail.com', 'jasonroelofs@gmail.com', 'cfis@savagexi.com']

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
    'README',
    'README.mingw',

    # Other files
    'Gemfile',
    'Rakefile',

    # rice.hpp
    'include/rice/rice.hpp',
    'include/rice/stl.hpp',

    # Source files
    'rice/*.?pp',
    'rice/detail/*.?pp',

    # Ruby files
    'lib/mkmf-rice.rb',
    'lib/version.rb',

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

  s.required_ruby_version = ">= 3.0"

  s.add_development_dependency "bundler"
  s.add_development_dependency "rake"
  s.add_development_dependency "minitest"
end
