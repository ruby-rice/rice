all: rice_all test_all sample_all

rice_all:
	$(MAKE) -C rice

test_all:
	$(MAKE) -C test

sample_all:
	$(MAKE) -C sample

docs: rice/README.doxygen

rice/README.doxygen: README
	@echo Generating documentation
	@ruby -e 'File.open("README") { |i| File.open("rice/README.doxygen", "w") { |o| o.puts "/* #{i.gets}"; i.each_line { |l| o.puts " * #{l}" if l !~ /^\\comment/ }; o.puts " */" } }'

.PHONY: rice_all test_all samples_all docs

