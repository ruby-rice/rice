ruby_config = $(shell ruby -rrbconfig -e "puts Config::CONFIG['$(strip $1)']")
RUBY_CFLAGS = -I$(call ruby_config, archdir)
RUBY_LDFLAGS = -L$(call ruby_config, libdir) \
               -L$(call ruby_config, archdir) \
               -lruby$(call ruby_config, LIBS) \
               $(call ruby_config, LDFLAGS)
RUBY = ruby

CPPFLAGS=$(RUBY_CFLAGS)
CXXFLAGS+=-fno-inline
CXXFLAGS+=-g

# librice.so: Object.o Exception.o detail/protect.o
# 	$(LD) $(LDFLAGS) -shared -o $@ $^

OBJS = Object.o Exception.o Struct.o Class.o Module.o \
			 Identifier.o Data_Type.o String.o Symbol.o VM.o \
			 detail/protect.o detail/check_ruby_type.o detail/method_data.o

all: librice.a

# TODO: should it be necessary to remove the archive file before
# creating it?  I'm not sure it even helps...
librice.a: $(OBJS)
	rm -f $@
	$(AR) cr $@ $^
	ranlib $@

clean:
	rm -f $(OBJS) librice.a

.PHONY: clean

# *.hpp *.cpp detail/*.hpp detail/*.cpp: generate_code

generate_code: generate_code.rb
	$(RUBY) generate_code.rb

CXXFLAGS+=-MMD

-include $(OBJS:%.o=%.d)

