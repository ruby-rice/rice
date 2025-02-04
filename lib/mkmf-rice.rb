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
path = File.expand_path(File.join(__dir__, '../include'))

unless find_header('rice/rice.hpp', path)
  raise("Could not find rice/rice.hpp header")
end

if !IS_DARWIN && !IS_MSWIN && !have_library('stdc++fs')
  have_library('stdc++')
end

# Copied from Ruby FFI bindings - see
# https://github.com/ffi/ffi/blob/1715332d553d53fae13fd9fcbbd9d2c1982a5c2f/ext/ffi_c/extconf.rb#L7C1-L27C6
def system_libffi_usable?
  # We need pkg_config or ffi.h
  libffi_ok = pkg_config("libffi") ||
      have_header("ffi.h") ||
      find_header("ffi.h", "/usr/local/include", "/usr/include/ffi",
                  "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/ffi",
                  "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/include/ffi") ||
      (find_header("ffi.h", `xcrun --sdk macosx --show-sdk-path`.strip + "/usr/include/ffi") rescue false)

  # Ensure we can link to ffi_prep_closure_loc
  libffi_ok &&= have_library("ffi", "ffi_prep_closure_loc", [ "ffi.h" ]) ||
                have_library("libffi", "ffi_prep_closure_loc", [ "ffi.h" ]) ||
                have_library("libffi-8", "ffi_prep_closure_loc", [ "ffi.h" ])

  if RbConfig::CONFIG['host_os'] =~ /mswin/
    have_library('libffi_convenience')
    have_library('shlwapi')
  end

  libffi_ok
end

def have_libffi
  # Check for libffi to support C style callacks.
  libffi_usable = system_libffi_usable?
  $CPPFLAGS += " -DHAVE_LIBFFI" if libffi_usable
  libffi_usable
end
