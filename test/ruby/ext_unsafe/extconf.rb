require "mkmf-rice"

# Use the local Rice source — but WITHOUT RICE_RACTOR_SAFE.
# The InstanceRegistry will have no mutex protection.
rice_dir = File.expand_path("../../../../rice", __dir__)
$INCFLAGS.prepend("-I#{rice_dir} ")

# Intentionally NO -DRICE_RACTOR_SAFE

create_makefile("ractor_test_unsafe_ext")
