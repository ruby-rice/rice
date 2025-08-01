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
    raise "Failed" unless status_thread.value.success?
  end
end

is_mswin = !RbConfig::CONFIG['host_os'].match(/mswin/).nil?
MAKE = is_mswin ? 'nmake' : 'make'

unittest = File.join(__dir__, "test", "unittest#{RbConfig::CONFIG['EXEEXT']}")
binaries = [unittest]

binaries.each do |asset|
  file asset  do
    dir = File.dirname(asset)
    cd dir do
      run_command(Gem.ruby, "extconf.rb")
      run_command(MAKE)
    end
  end
end

desc "Build test binaries"
task :build => [:headers, *binaries]

desc "Clean test binaries"
task :clean do
  binaries.each do |asset|
    dir = File.dirname(asset)
    cd dir do
      begin
        run_command(MAKE, "clean")
      rescue
        # If clean crashes, that's probably means there's no Makefile
        # which is fine. That means this is a fresh checkout or the directory
        # already otherwise got cleaned.
      end
    end
  end
end

# ---------  Testing --------------
task :test_cpp => [:clean, :build] do
  cd "test" do
    run_command(unittest)
  end
end

# NOTE - The problem with this is that it breaks with Github actions since it sees
# build artifacts such as *.so files and dies. Needs to be updated to just check
# for the include headers.
#task :confirm_headers => :build do
#  begin
#    run_command("git", "diff", "--quiet")
#  rescue
#    puts "", "Git working tree is not clean. Did the updated header files get checked in?", ""
#    run_command("git", "status", "--short")
#    exit 1
#  end
#end

Rake::TestTask.new do |t|
  t.libs += %w(test)
  t.test_files = Dir['test/ruby/*.rb']
  t.verbose = true
  t.warning = true
end

task :test => [:test_cpp]#, :confirm_headers]

# ---------  Header  --------------
include_dir = File.join(__dir__, 'include', 'rice')
FileUtils.mkdir_p(include_dir) #unless File.exists?('include')

desc "Update rice header files"
task :headers do
  FileUtils.rm_rf(File.join(include_dir, "rice", "*"))
  path = File.join(__dir__, 'lib', 'make_rice_headers.rb')
  # Execute make_rice_headers.rb
  run_command(Gem.ruby, path)
end

# ---------  Documentation  --------------
desc "Build the documentation"
task :doc do
  build_dir = File.expand_path(File.join("doc", "_build"))
  FileUtils.rm_r(build_dir, secure: true)
  cmd = ["sphinx-build", "--builder", "html", "doc", build_dir]
  sh *cmd
end

task :default => :test

# ---------  Packaging  --------------
spec = Gem::Specification.load("rice.gemspec")
Gem::PackageTask.new(spec) do |pkg|
end

task :package => :headers
