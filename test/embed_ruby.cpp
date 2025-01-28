#include <rice/rice.hpp>
#include <ruby/version.h>

void embed_ruby()
{
  static bool initialized__ = false;

  if (!initialized__)
  {
    RUBY_INIT_STACK;
    ruby_init();
    ruby_init_loadpath();

#if RUBY_API_VERSION_MAJOR == 3 && RUBY_API_VERSION_MINOR >= 1
    // Force the prelude / builtins
    const char* opts[] = { "ruby", "-e;" };
    ruby_options(2, (char**)opts);
#endif

    initialized__ = true;
  }
}
