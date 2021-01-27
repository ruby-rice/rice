require 'rubygems/package_task'
require "rake/testtask"
require 'rbconfig'
require 'open3'

# ---------  Binaries --------------
def run_command(*args)
  Open3.popen2(*args) do |stdin_child, stdout_child, status_thread|
    stdout_child.each_line do |line|
      $stdout << line
    end
    raise "Failed"  unless status_thread.value.success?
  end
end

unittest = File.join(__dir__, "test", "unittest#{RbConfig::CONFIG['EXEEXT']}")
sample_enum = File.join(__dir__, "sample", "enum", "sample_enum.so")
sample_inheritance = File.join(__dir__, "sample", "inheritance", "animals.so")
sample_map = File.join(__dir__, "sample", "map", "map.so")
test_ext1 = File.join(__dir__, "test", "ext", "t1", "t1.so")
test_ext2 = File.join(__dir__, "test", "ext", "t2", "t2.so")
binaries = [unittest, sample_enum, sample_inheritance, sample_map, test_ext1, test_ext2]

binaries.each do |asset|
  file asset  do
    dir = File.dirname(asset)
    cd dir do
      run_command(Gem.ruby, "extconf.rb")
      run_command("make")
    end
  end
end

task :build => binaries

task :clean do
  binaries.each do |asset|
    dir = File.dirname(asset)
    cd dir do
      run_command("make", "clean")
    end
  end
end

# ---------  Testing --------------
task :test_cpp => :build do
  cd "test" do
    run_command(unittest)
  end
end

Rake::TestTask.new do |t|
  t.libs += %w(test)
  t.test_files = Dir['test/ruby/*.rb']
  t.verbose = true
  t.warning = true
end

task :test => :test_cpp

# ---------  Header  --------------
rice_hpp = File.join(__dir__, 'include', 'rice.hpp')

desc "Build rice.hpp"
file rice_hpp do
  Dir.mkdir('include')
  path = File.join(__dir__, 'make_rice_hpp.rb')
  run_command(Gem.ruby, path)
end


# ---------  Documentation  --------------
desc "Build the documentation"
task :doc do
  sh "make doc"
end

task :default => :test

# ---------  Packaging  --------------
spec = Gem::Specification.load("rice.gemspec")
Gem::PackageTask.new(spec) do |pkg|
end

task :package => rice_hpp