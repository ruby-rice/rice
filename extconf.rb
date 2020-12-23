# To allow Rice to be installed on non-system installs of Ruby (rbenv or rvm),
# the rice gem uses the extconf version of deployment
# rather than configure. This file will get the appropriate values from CONFIG
# for the Ruby that this library is being installed into and generate
# the proper arguments to ./configure.
#
# With this, installing on any of the Ruby versions installed on a machine is as
# easy as /path/to/the/gem install rice.

$:.unshift File.expand_path(File.dirname(__FILE__))

require 'rbconfig'
require 'ruby/lib/version.rb'

prefix_dir = File.join(File.dirname(File.expand_path(__FILE__)), "ruby", "lib")
with_ruby = File.join(RbConfig::CONFIG["bindir"], RbConfig::CONFIG["RUBY_INSTALL_NAME"])

other_opts = ""
env = ""

if RbConfig::CONFIG["ENABLE_SHARED"] == "no"
  abort <<EOC.chomp
Unfortunately Rice does not build against a staticly linked Ruby.
You'll need to rebuild Ruby with --enable-shared to use this library.

If you're on rvm:    rvm reinstall [version] -- --enable-shared
If you're on rbenv:  CONFIGURE_OPTS="--enable-shared" rbenv install [version]
If you're on Heroku: upgrade your stack to heroku-16 or later
EOC
end

if RUBY_PLATFORM =~ /darwin(\d+)/
  check_iconv = 'echo "#include <iconv.h>" | g++ -xc -fsyntax-only -'
  if !system(check_iconv)
    abort <<EOM.chomp
-----
The file "/usr/include/iconv.h" is missing in your build environment,
which means you haven't installed Xcode Command Line Tools properly.
To install Command Line Tools, try running `xcode-select --install` on
terminal and follow the instructions.  If it fails, open Xcode.app,
select from the menu "Xcode" - "Open Developer Tool" - "More Developer
Tools" to open the developer site, download the installer for your OS
version and run it.
-----
EOM
  end

  darwin_version = $1.to_i

  if darwin_version >= 10
    arch = RbConfig::CONFIG["arch"].split("-")[0]

    if arch == "universal"
      arch = `uname -m`.strip
    end

    other_opts = "--disable-dependency-tracking"
  else
    arch = `uname -p`.chomp
  end

  env = if Gem::Version.new(RUBY_VERSION) >= Gem::Version.new('3.0.0')
          "ARCHFLAGS='-arch #{arch}' CPPFLAGS='-fdeclspec -arch #{arch}'"
        else
          "ARCHFLAGS='-arch #{arch}' CPPFLAGS='-arch #{arch}'"
        end
end

system "sh bootstrap"
system "#{env} sh configure --with-ruby=#{with_ruby} --prefix=#{prefix_dir} #{other_opts}"
