RUBY_EXTCONF_OPTIONS = -I@RICE_ROOT@/ruby/lib
EXTCONF_OPTIONS = --with-cppflags="-I@RICE_ROOT@" --with-libpath="@RICE_ROOT@/rice"

EXTRA_DIST = \
	enum/extconf.rb \
	enum/sample_enum.cpp \
	enum/test.rb \
	map/extconf.rb \
	map/map.cpp \
	map/test.rb \
	inheritance/extconf.rb \
	inheritance/animals.cpp \
	inheritance/test.rb

enum/Makefile: enum/extconf.rb ../config.status ../ruby/lib/mkmf-rice.rb
	@RUBY@ $(RUBY_EXTCONF_OPTIONS) -C enum extconf.rb $(EXTCONF_OPTIONS)

map/Makefile: map/extconf.rb ../config.status ../ruby/lib/mkmf-rice.rb
	@RUBY@ $(RUBY_EXTCONF_OPTIONS) -C map extconf.rb $(EXTCONF_OPTIONS)

inheritance/Makefile: inheritance/extconf.rb ../config.status ../ruby/lib/mkmf-rice.rb
	@RUBY@ $(RUBY_EXTCONF_OPTIONS) -C inheritance extconf.rb $(EXTCONF_OPTIONS)

all: enum/Makefile map/Makefile inheritance/Makefile all_extensions

all_extensions:
	@for sample in $(RICE_SAMPLES); \
	do \
	  $(MAKE) -C $${sample} all; \
	done

clean: enum/Makefile map/Makefile inheritance/Makefile clean_extensions

clean_extensions:
	@for sample in $(RICE_SAMPLES); \
	do \
	  $(MAKE) -C $${sample} clean; \
	done

distclean: enum/Makefile map/Makefile inheritance/Makefile distclean_extensions

distclean_extensions:
	@for sample in $(RICE_SAMPLES);
	do \
	  $(MAKE) -C $${sample} distclean; \
	done

