require 'mkmf'

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

# Rice needs c++17. Check if we are using msvc
if RbConfig::CONFIG['COMPILE_CXX'].match(/^cl /)
  $CXXFLAGS += " /std:c++17 /EHsc /permissive-"
  $CPPFLAGS += " -D_ALLOW_KEYWORD_MACROS"
else
  $CXXFLAGS += " -std=c++17"
end

# Rice needs to include its header. Let's setup the include path
# to make this easy
path = File.expand_path(File.join(__dir__, '../../include'))

find_header('rice.hpp', path)

