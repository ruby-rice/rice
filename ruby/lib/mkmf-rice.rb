require 'mkmf'

# The cpp_command is not overwridden in the C++ support for mkmf so do that
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

# Rice needs c++17
$CXXFLAGS += " -std=c++17"

# Rice needs to have the C++ library available
have_library("stdc++")

# And of course we need to know where the Rice header is
# TODO - right now this uses rice/rice.hpp instead of include/rice.hpp
find_header('rice.hpp', "../../rice")
