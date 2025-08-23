require 'bundler/setup'

# Require rice
require 'rice'

# Now setup option parser
require 'ostruct'
require 'optparse'

# Load needed files
require_relative '../lib/rice/doc/cpp_reference'
require_relative '../lib/rice/doc/doxygen'
require_relative '../lib/rice/doc/rice'
require_relative '../lib/rice/doc/ruby'
require_relative '../lib/rice/doc/mkdocs'

module Rice
	# == Synopsis
	#
	# Generates Mkdocs for a Rice extension
	#
	# == Usage
	# rice-doc [options] <rice_extension_library>
	#
	# Options:
	#    -o, --output=path                Output directory
	#    -v, --version                    Show version (1.1.0)
	#    -h, --help                       Show help message

	class Cmd
		# :enddoc:
		attr_accessor :options

		def option_parser
			OptionParser.new do |opts|
				opts.banner = "rice-doc #{Rice::VERSION}\n" +
											"Usage: rice-doc [options] <rice_extension_library_path>"

				opts.separator ""
				opts.separator "Options:"

				opts.on('-o path', '--output=path',
								'Output generated mkdoc files to the specified directory') do |dir|
					options.output = File.expand_path(dir)
				end

				opts.on_tail("-v version", "--version", "Show version (#{Rice::VERSION})") do
					puts "ruby_prof " + Rice::VERSION
					exit
				end

				opts.on_tail("-h", "--help", "Show help message") do
					puts opts
					exit
				end
			end
		end

		def parse_args
			@options = OpenStruct.new
			options.extension = nil
			options.output = nil

			# Make sure the user specified at least one file
			if ARGV.length < 1 and not options.exec
				puts self.option_parser
				puts ""
				puts "Must specify a rice extension"
				exit(-1)
			end

			self.option_parser.parse! ARGV

			if options.output.nil?
				puts "Must specify an output directory with -o/--output option"
				exit(-1)
			elsif !Dir.exist?(options.output)
				puts "Creating output directory '#{options.output}'"
				Dir.mkdir(options.output)
			end

			options.extension = ARGV.shift

			options
		rescue OptionParser::InvalidOption, OptionParser::InvalidArgument, OptionParser::MissingArgument => e
			puts self.option_parser
			puts e.message
			exit(-1)
		end
	end
end

# Parse command line options
cmd = Rice::Cmd.new
options = cmd.parse_args

# Load the extension
require options.extension

# Get modules and classes
modules = Rice::Registries.instance.modules.modules
klasses = Rice::Registries.instance.types.klasses

# Doxygen documentation
doxygen_root = "https://docs.opencv.org/4.x"
doxygen_index = "https://docs.opencv.org/4.x/opencv.tag"
doxygen_index = "c:/Users/cfis/Downloads/opencv.tag"
doxygen_resolver = Rice::Doc::Doxygen.new(doxygen_root, doxygen_index)

# Setup resolvers per namespace
resolvers = {nil => Rice::Doc::Ruby.new,
						 'Cv' => doxygen_resolver,
						 'Rice' => Rice::Doc::Rice.new,
						 'Std' => Rice::Doc::CppReference.new}

generator = Rice::Doc::Mkdocs.new(modules + klasses, resolvers, options.output)
generator.generate
