require 'mkmf'
$CFLAGS = '-g -Wall -I../.. -fno-inline'
$LDFLAGS = '-L../../rice'
$LIBS = '-lrice'
create_makefile('map')

File.open('Makefile', 'a') do |mf|
  mf.puts 'LDSHARED = $(CXX) -shared'
end

