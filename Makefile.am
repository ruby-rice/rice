AUTOMAKE_OPTIONS = foreign 1.5
SUBDIRS = rice test sample ruby

include doxygen.am

doc: doxygen-doc

noinst_EXTRA = rice/README.doxygen

DX_EXTRA_DEPEND = rice/README.doxygen

EXTRA_DIST = \
	README.mingw \
	post-autoconf.rb \
	post-automake.rb \
	bootstrap \
	Doxyfile \
	ruby.ac \
	doxygen.ac \
	doxygen.am \
	doc

rice/README.doxygen: README
	@echo Generating documentation
	@$(RUBY) -e 'File.open("README") { |i| File.open("rice/README.doxygen", "w") { |o| o.puts "/*! #{i.gets}"; i.each_line { |l| o.puts " *  #{l}" if l !~ /^\\comment/ and l !~ /^vim:/ }; o.puts " */" } }'

