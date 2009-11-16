RUBY_EXTCONF_OPTIONS = -I@RICE_ROOT@/ruby/lib
EXTCONF_OPTIONS = --with-cppflags="-I@RICE_ROOT@" --with-libpath="@RICE_ROOT@/rice"

EXTRA_DIST = \
  t1/extconf.rb \
  t1/t1.cpp \
  t2/extconf.rb \
  t2/t2.cpp

all:

t1/Makefile: t1/extconf.rb ../../config.status ../../ruby/lib/mkmf-rice.rb
	@RUBY@ $(RUBY_EXTCONF_OPTIONS) -C t1 extconf.rb $(EXTCONF_OPTIONS)

t2/Makefile: t2/extconf.rb ../../config.status ../../ruby/lib/mkmf-rice.rb
	@RUBY@ $(RUBY_EXTCONF_OPTIONS) -C t2 extconf.rb $(EXTCONF_OPTIONS)

all clean distclean: t1/Makefile t2/Makefile

all: t1_all t2_all

t1_all:
	make -C t1 all

t2_all:
	make -C t2 all

clean: t1_clean t2_clean

t1_clean:
	make -C t1 clean

t2_clean:
	make -C t2 clean

distclean: t1_distclean t2_distclean

t1_distclean:
	make -C t1 distclean

t2_distclean:
	make -C t2 distclean

