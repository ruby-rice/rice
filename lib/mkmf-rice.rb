require 'mkmf'

IS_MSWIN = /mswin/ =~ RUBY_PLATFORM
IS_MINGW = /mingw/ =~ RUBY_PLATFORM
IS_DARWIN = RbConfig::CONFIG['host_os'].match?(/darwin/)

# The cpp_command is not overwritten in the experimental mkmf C++ support.
# See https://bugs.ruby-lang.org/issues/17578
MakeMakefile['C++'].module_eval do
  def cpp_command(outfile, opt="")
    conf = cc_config(opt)
    if $universal and (arch_flag = conf['ARCH_FLAG']) and !arch_flag.empty?
      conf['ARCH_FLAG'] = arch_flag.gsub(/(?:\G|\s)-arch\s+\S+/, '')
    end
    RbConfig::expand("$(CXX) -E #$INCFLAGS #$CPPFLAGS #$CFLAGS #{opt} #{CONFTEST_CXX} #{outfile}",
                     conf)
  end
end

# Now pull in the C++ support
include MakeMakefile['C++']

# Rice needs c++17.
if IS_MSWIN
  $CXXFLAGS += " /std:c++17 /EHs /permissive- /bigobj /utf-8"
  $CPPFLAGS += " -D_ALLOW_KEYWORD_MACROS -D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE"
elsif IS_MINGW
  $CXXFLAGS += " -std=c++17 -Wa,-mbig-obj"
else
  $CXXFLAGS += " -std=c++17"
end

# Rice needs to include its header. Let's setup the include path
# to make this easy
path = File.expand_path(File.join(__dir__, '..'))

unless find_header('rice/rice.hpp', path)
  raise("Could not find rice/rice.hpp header")
end

if !IS_DARWIN && !IS_MSWIN && !have_library('stdc++fs')
  have_library('stdc++')
end

# Check for libffi to support C style callacks
if have_library("ffi")
  $CPPFLAGS += " -DHAVE_LIBFFI"
end

