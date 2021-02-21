#include <rice/rice.hpp>

void embed_ruby()
{
  static bool initialized__ = false;

  if (!initialized__)
  {
    int argc = 0;
    char* argv = (char*)malloc(1);
    argv[0] = 0;
    char** pArgv = &argv;

    ruby_sysinit(&argc, &pArgv);
    ruby_init();
    ruby_init_loadpath();

    initialized__ = true;

   // rb_eval_string("GC.stress = true");
  }
}
