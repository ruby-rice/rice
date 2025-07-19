require 'stringio'

LINE_ENDING = "\n"
RICE_INCLUDE_REGEX = %r{#include "(.*)"}
OTHER_INCLUDE_REGEX = %r{#include <(.*)>}

RICE_HEADER_GUARD_1 = %r{#ifndef Rice__}
RICE_HEADER_GUARD_2 = %r{#define Rice__}
RICE_HEADER_GUARD_3 = %r{#endif\s*//\s*Rice__}
SHARED_METHODS_REGEX = %r{#include "((?:cpp_api\/)?shared_methods.hpp)"}

def load_file(relative_path)
  content = File.read(relative_path, mode: 'rb')

  # Special case shared_methods.hpp which if requested we want to
  # merge into the current file
  match = content.match(SHARED_METHODS_REGEX)
  if match
    shared_path = File.join(File.dirname(relative_path), match[1])
    content.gsub!(SHARED_METHODS_REGEX, File.read(shared_path, mode: 'rb'))
  end

  content
end

def strip_includes(content)
  content.lines.find_all do |line|
    !line.match(RICE_INCLUDE_REGEX)
  end.join
end

def add_include(path, stream)
  basename = File.basename(path)
  basename_no_ext = File.basename(path, ".*")

  stream << "\n" << "// =========   #{File.basename(path)}   =========" << "\n"

  load_file(path).each_line do |line|
    if match = line.match(RICE_INCLUDE_REGEX)
      # Check for related includes, ie., Object.hpp, Object_defn.hpp and Object.ipp
      sub_include = File.basename(match[1])
      if ["#{basename_no_ext}_defn.hpp", "#{basename_no_ext}.ipp"].include?(sub_include)
        sub_include_path = File.join(File.dirname(path), match[1])
        stream << "\n" << "// ---------   #{File.basename(sub_include_path)}   ---------" << "\n"
        stream << strip_includes(load_file(sub_include_path))
      end
    elsif line.match(RICE_HEADER_GUARD_1) || line.match(RICE_HEADER_GUARD_2) || line.match(RICE_HEADER_GUARD_3)
      # Skip the header guard
    else
      # Include the line in the output
      stream << line
    end
  end
end

def combine_headers(filename)
  stream = StringIO.new

  load_file("rice/#{filename}").each_line do |line|
    if matches = line.match(RICE_INCLUDE_REGEX)
      path = File.join("rice", matches[1])
      add_include(path, stream)
    else
      stream << line
    end
  end

  File.open("include/rice/#{filename}", 'wb') do |file|
    file << stream.string
  end
end

puts "Building rice.hpp"
combine_headers('rice.hpp')

puts "Building stl.hpp"
combine_headers('stl.hpp')

puts "Building ruby_api.hpp"
combine_headers('ruby_api.hpp')

puts "Success"
