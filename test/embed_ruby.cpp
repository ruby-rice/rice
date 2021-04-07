#include <rice/rice.hpp>
#include <ruby/version.h>

void embed_ruby()
{
  static bool initialized__ = false;

  if (!initialized__)
  {
    int argc = 0;
    char* argv = nullptr;
    char** pArgv = &argv;

    ruby_sysinit(&argc, &pArgv);
    ruby_init();
    ruby_init_loadpath();

    initialized__ = true;

    // Because Ruby 3 no longer initializes the GC module when embedding, calling GC.stress 
    // results in a crash.
    // See https://bugs.ruby-lang.org/issues/17643
    if (RUBY_API_VERSION_MAJOR == 3 &&
        RUBY_API_VERSION_MINOR == 0 &&
        RUBY_API_VERSION_TEENY == 0)
    {
      // do nothing
    }
    else
    {
      rb_eval_string("GC.stress = true");
    }
  }
}
