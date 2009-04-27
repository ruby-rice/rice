require 'fileutils'
require 'find'

def process(file)
  puts "Post-processing #{file}"
  File.open(file) do |input|
    File.open("#{file}.pp", "w") do |output|
      input.each_line do |line|
        yield output, line
      end
    end
  end
  FileUtils.mv("#{file}.pp", "#{file}")
end

process("configure") do |out, line|
  # autoconf doesn't properly enclose $srcdir and $am_aux_dir in quotes
  line.gsub!(/([^"'])(\$srcdir)/, '\1"\2"')
  line.gsub!(/([^"'])(\$am_aux_dir)/, '\1\"\2\"')
  out.puts line
end
FileUtils.chmod(0700, "configure")
