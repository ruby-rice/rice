# To allow Rice to be installed on non-system installs of Ruby (say the user has
# 1.8.7 and 1.9.0 installed), the rice gem uses the extconf version of deployment
# rather than configure. This file will get the appropriate values from CONFIG
# for the Ruby that this library is being installed into and generate
# the proper arguments to ./configure.
#
# With this, installing on any of the Ruby versions installed on a machine is as
# easy as /path/to/the/gem install rice.
#
# This isn't actually an extconf.rb file, all it needs to do is create a Makefile
# for gem to continue 'make'-ing. It needs to be named 'extconf.rb' to fit Rubygem's
# expectations

$:.unshift File.expand_path(File.dirname(__FILE__))

require 'rbconfig'
require 'ruby/lib/version.rb'

prefix_dir = File.join(File.dirname(File.expand_path(__FILE__)), "ruby", "lib")
with_ruby = File.join(RbConfig::CONFIG["bindir"], RbConfig::CONFIG["RUBY_INSTALL_NAME"])

if RbConfig::CONFIG["ENABLE_SHARED"] == "no"
  raise <<EOC
Unfortunately Rice does not work against a Ruby without any shared libraries.
You'll need to rebuild Ruby with --enable-shared to use this library.

If you're on rvm:   rvm reinstall [version] -- --enable-shared
If you're on rbenv: CONFIGURE_OPTS="--enable-shard" rbenv install [version]
If this is a host environment like Heroku you'll need to contact their support.
EOC
end

other_opts = ""
env = ""

if RUBY_PLATFORM =~ /darwin1[0-2]/ 
  arch = RbConfig::CONFIG["arch"].split("-")[0]

  if arch == "universal"
    arch = `uname -m`.strip
  end

  other_opts = "--disable-dependency-tracking"
  env = "ARCHFLAGS='-arch #{arch}' CPPFLAGS='-arch #{arch}'"
elsif RUBY_PLATFORM =~ /darwin9/
  arch = `uname -p`.chomp
  env = "ARCHFLAGS='-arch #{arch}' CPPFLAGS='-arch #{arch}'"
end

system "#{env} sh configure --with-ruby=#{with_ruby} --prefix=#{prefix_dir} #{other_opts}"
