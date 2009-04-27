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

Find.find(".") do |file|
  if file =~ /Makefile.in$/ then
    process(file) do |out, line|
      # automake doesn't properly enclose $distdir in quotes
      if line !~ /echo / then
        line.gsub!(/([^"'])(\$\(distdir\))/, '\1"\2"')
      end
      line.gsub!(/(\$\(MKDIR_P\).*`)/, 'eval \1')
      out.puts line
    end
  end
end

