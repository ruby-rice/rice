#include <rice/rice.hpp>
#include <ruby/version.h>

void embed_ruby()
{
  static bool initialized__ = false;

  if (!initialized__)
  {
    ruby_init();
    ruby_init_loadpath();
    ruby_script("unittest");

    // Force the prelude / builtins
    char *opts[] = { "ruby", "-e;" };
    ruby_options(2, opts);

    initialized__ = true;
  }
}
