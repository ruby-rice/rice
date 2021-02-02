require 'mkmf'

# If we are on versions of Ruby before 2.7 then we need to copy in the experimental C++ support
# added in Ruby 2.7
unless MakeMakefile.methods.include?(:[])
  MakeMakefile.module_eval do
    @lang = Hash.new(self)

    def self.[](name)
      @lang.fetch(name)
    end

    def self.[]=(name, mod)
      @lang[name] = mod
    end

    MakeMakefile["C++"] = Module.new do
      include MakeMakefile
      extend self

      CONFTEST_CXX = "#{CONFTEST}.#{config_string('CXX_EXT') || CXX_EXT[0]}"

      TRY_LINK_CXX = config_string('TRY_LINK_CXX') ||
        ((cmd = TRY_LINK.gsub(/\$\(C(?:C|(FLAGS))\)/, '$(CXX\1)')) != TRY_LINK && cmd) ||
        "$(CXX) #{OUTFLAG}#{CONFTEST}#{$EXEEXT} $(INCFLAGS) $(CPPFLAGS) " \
                     "$(CXXFLAGS) $(src) $(LIBPATH) $(LDFLAGS) $(ARCH_FLAG) $(LOCAL_LIBS) $(LIBS)"

      def have_devel?
        unless defined? @have_devel
          @have_devel = true
          @have_devel = try_link(MAIN_DOES_NOTHING)
        end
        @have_devel
      end

      def conftest_source
        CONFTEST_CXX
      end

      def cc_command(opt="")
        conf = cc_config(opt)
        RbConfig::expand("$(CXX) #$INCFLAGS #$CPPFLAGS #$CXXFLAGS #$ARCH_FLAG #{opt} -c #{CONFTEST_CXX}",
                         conf)
      end

      def link_command(ldflags, *opts)
        conf = link_config(ldflags, *opts)
        RbConfig::expand(TRY_LINK_CXX.dup, conf)
      end
    end
  end
end

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

IS_MSWIN = !RbConfig::CONFIG['host_os'].match(/mswin/).nil?

# Rice needs c++17. Check if we are using msvc
if IS_MSWIN
  $CXXFLAGS += " /std:c++17 /EHsc /permissive-"
  $CPPFLAGS += " -D_ALLOW_KEYWORD_MACROS"
else
  $CXXFLAGS += " -std=c++17"
end

# Rice needs to include its header. Let's setup the include path
# to make this easy
path = File.expand_path(File.join(__dir__, '../../include'))

find_header('rice/rice.hpp', path)

