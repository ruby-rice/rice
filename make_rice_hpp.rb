require 'stringio'

LINE_ENDING = "\n"
RICE_INCLUDE_REGEX = %r{#include "(.*)"}
OTHER_INCLUDE_REGEX = %r{#include <(.*)>}

RICE_HEADER_GUARD_1 = %r{#ifndef Rice__}
RICE_HEADER_GUARD_2 = %r{#define Rice__}
RICE_HEADER_GUARD_3 = %r{#endif\s*//\s*Rice__}

def load_file(relative_path)
  File.read(relative_path, mode: 'rb')
end

def strip_includes(content)
  content.lines.find_all do |line|
    !line.match(RICE_INCLUDE_REGEX)
  end.join
end

def add_include(path, stream)
  basename = File.basename(path)
  basename_no_ext = File.basename(path, ".*")

  stream << "\r\n" << "// =========   #{File.basename(path)}   =========" << "\r\n"

  load_file(path).each_line do |line|
    if match = line.match(RICE_INCLUDE_REGEX)
      # Check for related includes, ie., Object.hpp, Object_defn.hpp and Object.ipp
      sub_include = File.basename(match[1])
      if ["#{basename_no_ext}_defn.hpp", "#{basename_no_ext}.ipp"].include?(sub_include)
        sub_include_path = File.join(File.dirname(path), match[1])
        stream << "\r\n" << "// ---------   #{File.basename(sub_include_path)}   ---------" << "\r\n"
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

def combine_header_files
  stream = StringIO.new

  load_file('rice/rice.hpp').each_line do |line|
    if matches = line.match(RICE_INCLUDE_REGEX)
      path = File.join("rice", matches[1])
      add_include(path, stream)
    end
  end

  File.open('include/rice.hpp', 'wb') do |file|
    file << stream.string
  end
end

combine_header_files

