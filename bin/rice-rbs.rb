#! /usr/bin/env ruby

require 'bundler/setup'

# Require rice
require 'rice'

# Now setup option parser
require 'ostruct'
require 'optparse'

module Rice
	# == Synopsis
	#
	# Generates rbs files for a Rice extension
	#
	# == Usage
	# rice_rbs [options] <rice_extension_library>
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
				opts.banner = "rice-rbs #{Rice::VERSION}\n" +
											"Usage: rice-rbs [options] <rice_extension_library_path>"

				opts.separator ""
				opts.separator "Options:"

				opts.on('-o path', '--output=path',
								'Output generated Rbs files to the specified directory') do |dir|
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
				puts "Must specify a rice library"
				exit(-1)
			end

			self.option_parser.parse! ARGV

			if options.output.nil?
				puts "Must specify an output directory with -o/--outpu option"
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

rbs = Rice::Rbs.new(options.extension, options.output)
rbs.generate

