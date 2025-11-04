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

  s.bindir = "bin"
  s.executables = ["rice-doc.rb", "rice-rbs.rb"]

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

    # Include files
    'include/rice/*.hpp',

    # Bin files
    'bin/rice-rbs.rb',

    # Source files
    'rice/**/*.?pp',

    # Library files
    'lib/**/*.rb',

    # Test source files
    'test/*.?pp',
    'test/extconf.rb'
  ]

  s.required_ruby_version = ">= 3.1"
  s.add_development_dependency "bundler"
  s.add_development_dependency "rake"
  s.add_development_dependency "minitest"
end
