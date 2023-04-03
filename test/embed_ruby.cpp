#include <rice/rice.hpp>
#include <ruby/version.h>

// See https://bugs.ruby-lang.org/issues/17643. Note there is a function called 
// rb_call_builtin_inits that properly intializes the GC and other modules.
// It is exported on Windows builds (MSVC and MinGW) by the ruby shared library
// so we can use it although it is not defined in any header.
extern "C" {
  void rb_call_builtin_inits(void);
}

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

    if constexpr (RUBY_API_VERSION_MAJOR < 3)
    {
      rb_eval_string("GC.stress = true");
    }
#ifdef _WIN64
    else
    {
      rb_call_builtin_inits();
      rb_eval_string("GC.stress = true");
    }
#endif
  }
}
