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

  # Test source files
  'test/*.?pp',
]
puts patterns.collect { |p| Dir.glob(p) }.flatten.join("\n")

