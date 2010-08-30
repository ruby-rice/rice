$: << File.expand_path(File.dirname(__FILE__))
require 'rake/gempackagetask'
require 'rake/contrib/sshpublisher'
require 'yaml'
require 'ruby/lib/version'

PROJECT_NAME = "rice"
PROJECT_WEB_PATH = "/var/www/gforge-projects/rice"

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

desc "Upload documentation to the website. Requires rubyforge gem" 
task :upload_web => [:doc] do
  host = "jameskilton@rubyforge.org"
  Rake::SshDirPublisher.new(host, PROJECT_WEB_PATH, "doc/html").upload
end

# Gemspec kept externally
eval(File.read("rice.gemspec"))
Rake::GemPackageTask.new($spec) do |pkg|
end
