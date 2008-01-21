require 'mkmf-rice'

$CFLAGS = '-I../..'
$LDFLAGS = '-L../../rice'
have_library('rice') or raise "Could not find Rice library"
create_makefile('sample_enum')

