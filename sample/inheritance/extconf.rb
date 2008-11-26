require 'mkmf-rice'
$DEFLIBPATH.unshift(with_config('libpath')) if with_config('libpath')
create_makefile('animals')

