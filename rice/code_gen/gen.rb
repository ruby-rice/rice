require 'object_call'
require 'protect'
require 'constructor'

# For each defined template generation system, we need to tell
# them to generate the C++ code and write it out to the appropriate file.
[
  ObjectCallHpp,
  ObjectCallIpp,
  ProtectHpp,
  ProtectIpp,
  ConstructorHpp,
].each do |klass|
  puts "Generating #{klass.out_file}"
  klass.render_to_file
end

puts "Code generation complete"
