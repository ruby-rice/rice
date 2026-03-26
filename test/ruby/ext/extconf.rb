require "mkmf-rice"

# Use the local Rice source (with RICE_RACTOR_SAFE support)
rice_dir = File.expand_path("../../../../rice", __dir__)
$INCFLAGS.prepend("-I#{rice_dir} ")

$CXXFLAGS << " -DRICE_RACTOR_SAFE"

create_makefile("ractor_test_ext")
