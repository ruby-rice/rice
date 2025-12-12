require 'libxml-ruby'

# Require rice
require_relative '../lib/rice'
require_relative '../lib/rice/doc'

# Now setup option parser
require 'optparse'

module Rice
	# == Synopsis
	#
	# Generates Mkdocs for a Rice extension
	#
	# == Usage
	# rice-doc [options] <config_file>
	#
	# Options:
	#    -v, --version                    Show version
	#    -h, --help                       Show help message

	class Cmd
		attr_accessor :options

		def option_parser
			OptionParser.new do |opts|
				opts.banner = "rice-doc #{Rice::VERSION}\n" +
											"Usage: rice-doc [options] <config_file.yaml>"

				opts.separator ""
				opts.separator "Options:"

				opts.on_tail("-v", "--version", "Show version (#{Rice::VERSION})") do
					puts "rice-doc " + Rice::VERSION
					exit
				end

				opts.on_tail("-h", "--help", "Show help message") do
					puts opts
					puts ""
					puts "Config file format (YAML):"
					puts "  extension: path/to/extension.so"
					puts "  output: ./docs/api"
					puts "  resolvers:"
					puts "    Std:"
					puts "      type: cppreference"
					puts "    MyNamespace:"
					puts "      type: doxygen"
					puts "      root: https://example.com/docs"
					puts "      tagfile: https://example.com/docs/tagfile.xml"
					puts "      type_mappings:"
					puts "        - pattern: \"^enum\\\\s*\""
					puts "          replacement: \"\""
					puts "      method_mappings:"
					puts "        MyNamespace::MyClass:"
					puts "          ruby_method: cppMethod"
					puts ""
					puts "Note: Ruby core types are automatically linked to docs.ruby-lang.org"
					exit
				end
			end
		end

		def parse_args
			@options = Struct.new(:config_file).new
			options.config_file = nil

			self.option_parser.parse! ARGV

			if ARGV.length < 1
				puts self.option_parser
				puts ""
				puts "Must specify a config file"
				exit(-1)
			end

			options.config_file = ARGV.shift

			unless File.exist?(options.config_file)
				puts "Config file not found: #{options.config_file}"
				exit(-1)
			end

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

# Load configuration
config = Rice::Doc::Config.new(options.config_file)

# Validate configuration
if config.extension.nil?
	puts "Config file must specify 'extension'"
	exit(-1)
end

if config.output.nil?
	puts "Config file must specify 'output'"
	exit(-1)
end

# Create output directory if needed
unless Dir.exist?(config.output)
	puts "Creating output directory '#{config.output}'"
	FileUtils.mkdir_p(config.output)
end

# Load the extension
require config.extension

# Get modules and classes
modules = Rice::Registries.instance.modules.modules
klasses = Rice::Registries.instance.types.klasses

# Generate documentation
generator = Rice::Doc::Mkdocs.new(modules + klasses, config.resolvers, config.output)
generator.generate
