ruby_config = $(shell ruby -rrbconfig -e "puts Config::CONFIG['$(strip $1)']")
RUBY_CFLAGS = -I$(call ruby_config, archdir)
RUBY_LDFLAGS = -L$(call ruby_config, libdir) \
               -L$(call ruby_config, archdir) \
               -lruby $(call ruby_config, LIBS) \
               $(call ruby_config, LDFLAGS)

CPPFLAGS=$(RUBY_CFLAGS) -I..
CXXFLAGS+=-fno-inline
CXXFLAGS+= -g
LDFLAGS+=$(RUBY_LDFLAGS) -L../rice -lrice

UT_OBJS = \
	unittest.o \
	test_Address_Registration_Guard.o \
	test_Allocation_Strategies.o \
	test_Array.o \
	test_Builtin_Object.o \
	test_Class.o \
	test_Constructor.o \
	test_Critical_Guard.o \
	test_Data_Object.o \
	test_Enum.o \
	test_Exception.o \
	test_Hash.o \
	test_Identifier.o \
	test_Jump_Tag.o \
	test_Module.o \
	test_Object.o \
	test_String.o \
	test_Struct.o \
	test_Symbol.o \
	test_To_From_Ruby.o \

all: unittest vm_unittest

unittest: $(UT_OBJS) ../rice/librice.a
	$(CXX) -o$@ $(UT_OBJS) $(LDFLAGS)

VM_UT_OBJS = \
	unittest.o \
	test_VM.o \
	
vm_unittest: $(VM_UT_OBJS) ../rice/librice.a
	$(CXX) -o$@ $(VM_UT_OBJS) $(LDFLAGS)

clean:
	rm -f $(UT_OBJS) $(VM_UT_OBJS) unittest

.PHONY: clean

CXXFLAGS+=-MMD

include $(OBJS:%.o=%.d)

