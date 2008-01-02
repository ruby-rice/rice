require 'mkmf'
$CFLAGS = '-g -Wall -I../..'
create_makefile('sample_enum')

File.open('Makefile', 'a') do |mf|
  mf.puts 'LDSHARED = $(CXX) -shared'
end

