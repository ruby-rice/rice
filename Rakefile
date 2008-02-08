require 'rake/gempackagetask'
require 'rake/contrib/sshpublisher'
require 'yaml'
require 'rubyforge'

PROJECT_NAME = "rice"
RICE_VERSION = "1.0.0"
PROJECT_WEB_PATH = "/var/www/gforge-projects/rice"

task :default => :test

desc "Build Rice locally. Delete the top level Makefile to force this to run"
task :build do 
  if !File.exist?("Makefile")
    sh "bootstrap"
    sh "configure"
    sh "make"
  end
end

desc "Run unit tests" 
task :test => :build do
  cd "test" do
    ruby "test_rice.rb"
  end
end

desc "Upload documentation to the website. Requires rubyforge gem" 
task :upload_web => :build do
  sh "make doc"

  config = YAML.load(File.read(File.expand_path("~/.rubyforge/user-config.yml")))
  host = "#{config["username"]}@rubyforge.org"

  Rake::SshDirPublisher.new(host, PROJECT_WEB_PATH, "doc/html").upload
end

spec = Gem::Specification.new do |s|
  s.name = PROJECT_NAME
  s.version = RICE_VERSION
  s.summary = 'Ruby Interface for C++ Extensions'
  s.homepage = 'http://rice.rubyforge.org/'
  s.rubyforge_project = PROJECT_NAME
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
    'README.mingw',

    # Doxygen
    'Doxyfile',
    'doxygen.ac',
    'doxygen.am',

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
    'test/test_rice.rb',
  ]

  s.extra_rdoc_files = [ 'README' ]

  s.require_paths = [ 'ruby/lib' ]
end

Rake::GemPackageTask.new(spec) do |pkg|
  pkg.need_zip = true
  pkg.need_tar = true
end

desc "Create a new release to Rubyforge" 
task :release => :package do
  rf = RubyForge.new
  puts "Logging into rubyforge"
  rf.login

  pkg = "pkg/#{PROJECT_NAME}-#{RICE_VERSION}"

  c = rf.userconfig

  files = [
    "#{pkg}.tgz",
    "#{pkg}.zip",
    "#{pkg}.gem"
  ]

  puts "Releasing #{PROJECT_NAME} v. #{RICE_VERSION}"
  begin
    rf.add_release spec.rubyforge_project, PROJECT_NAME, RICE_VERSION, *files
  rescue => ex
    puts "You may not be configured with rubyforge. Please run `rubyforge config` and run this task again."
    puts "Error is #{ex.inspect}"
  end
end

