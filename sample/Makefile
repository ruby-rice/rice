all: enum_all inheritance_all map_all

enum_all:
	cd enum && ruby extconf.rb
	$(MAKE) -C enum

inheritance_all:
	cd inheritance && ruby extconf.rb
	$(MAKE) -C inheritance

map_all:
	cd map && ruby extconf.rb
	$(MAKE) -C map

.PHONY: enum_all inheritance_all map_all

