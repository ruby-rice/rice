$: << File.expand_path(File.dirname(__FILE__))
require 'rubygems'
require 'rubygems/package_task'
require 'yaml'
require 'ruby/lib/version'

task :default => :test

desc "Run unit tests"
task :test do
  cd "test" do
    ruby "test_rice.rb"
  end
end

desc "Build the documentation"
task :doc do
  sh "make doc"
end

# Gemspec kept externally
eval(File.read("rice.gemspec"))
Gem::PackageTask.new($spec) do |pkg|
end
