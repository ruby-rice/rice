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
require 'rubygems'
require 'ruby/lib/version.rb'

gem_name = "rice-#{Rice::VERSION}"
prefix_dir = File.join(Gem.default_dir, "gems", gem_name, "ruby", "lib")
with_ruby = File.join(Config::CONFIG["bindir"], Config::CONFIG["RUBY_INSTALL_NAME"])

other_opts = ""
env = ""

if RUBY_PLATFORM =~ /darwin10/
  other_opts = "--disable-dependency-tracking"
  env = "ARCHFLAGS='-arch x86_64'"
elsif RUBY_PLATFORM =~ /darwin9/
  env = "ARCHFLAGS='-arch #{`uname -p`.chomp}'"
end

system "#{env} sh configure --with-ruby=#{with_ruby} --prefix=#{prefix_dir} #{other_opts}"
