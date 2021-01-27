require 'bundler/setup'
require 'rice'
require 'mkmf-rice'
require 'rbconfig'

target_exe = "unittest#{RbConfig::CONFIG['EXEEXT']}"
create_makefile(target_exe) do |conf|
  conf << "\n"
  conf << "#{target_exe}: $(OBJS)"
  conf << "\t$(ECHO) linking executable unittest"
  conf << "\t-$(Q)$(RM) $(@)"
  conf << "\t$(Q) $(CXX) -o $@ $(OBJS) $(LIBPATH) $(LOCAL_LIBS) $(LIBS)"
  conf << "\n"
end