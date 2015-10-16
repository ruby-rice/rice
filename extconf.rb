# To allow Rice to be installed on non-system installs of Ruby (rbenv or rvm),
# the rice gem uses the extconf version of deployment
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

other_opts = ""
env = ""

if RUBY_PLATFORM =~ /darwin(\d+)/
  # TY Nokogiri!
  if !File.exist?('/usr/include/iconv.h')
    abort <<'EOM'.chomp
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
    env = "ARCHFLAGS='-arch #{arch}' CPPFLAGS='-arch #{arch}'"
  else
    arch = `uname -p`.chomp
    env = "ARCHFLAGS='-arch #{arch}' CPPFLAGS='-arch #{arch}'"
  end
end

system "sh bootstrap"
system "#{env} sh configure --with-ruby=#{with_ruby} --prefix=#{prefix_dir} #{other_opts}"
