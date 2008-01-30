spec = Gem::Specification.new do |s|
  s.name = 'rice'
  s.version = '1.0.0'
  s.summary = 'Ruby Interface for C++ Extensions'
  s.homepage = 'http://rubystuff.org/rice/'
  s.rubyforge_project = 'rice'
  s.author = 'Paul Brannan'
  s.email = 'curlypaul924@gmail.com'

  s.description = <<-END
Rice is a C++ interface to Ruby's C API.  It provides a type-safe and
exception-safe interface in order to make embedding Ruby and writing
Ruby extensions with C++ easier.  It is similar to Boost.Python in many
ways, but also attempts to provide an object-oriented interface to all
of the Ruby C API.
  END

  patterns = [
    # Documentation
    'COPYING',
    'README',

    # Doxygen
    'Doxyfile',

    # Autoconf
    'bootstrap',
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

    # Makefiles
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

    # Gem spec
    'rice.gemspec',

    # C++ source files
    'rice/*.?pp',
    'rice/*.rb',
    'rice/config.hpp.in',
    'rice/detail/*.?pp',

    # Library files
    'ruby/lib/mkmf-rice.rb.in',

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

    # Test source files
    'test/*.?pp',
  ]
  s.files = patterns.collect { |p| Dir.glob(p) }.flatten

  s.extensions = 'configure'

  s.test_files = [
    'test/unittest',
    'test/vm_unittest',
  ]

  s.extra_rdoc_files = [ 'README' ]
end

