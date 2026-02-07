#include <rice/rice.hpp>
#include <ruby/version.h>

void embed_ruby()
{
  static bool initialized__ = false;

  if (!initialized__)
  {
    RUBY_INIT_STACK;

    int argc = 0;
    char* argv = nullptr;
    char** pArgv = &argv;

    ruby_sysinit(&argc, &pArgv);

    ruby_init();
    ruby_init_loadpath();

#if (RUBY_API_VERSION_MAJOR == 3 && RUBY_API_VERSION_MINOR >= 1) || (RUBY_API_VERSION_MAJOR >= 4)
    // Force the prelude / builtins. In versions over 3.1 this is mandatory for Symbols, Arrays, and Hash methods to work
    const char* options[] = { "ruby", "-e;" };
    ruby_options(2, (char**)options);
#endif

    // Enable GC stress to help catch GC-related bugs
#if RICE_RELEASE
   // rb_funcall(rb_mGC, rb_intern("stress="), 1, Qtrue);
#endif

    initialized__ = true;
  }
}
