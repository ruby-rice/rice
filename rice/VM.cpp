#include "VM.hpp"
#include "detail/ruby.hpp"
#include <env.h>
#include <stdexcept>

Rice::VM::
VM(char * app_name)
{
  init_stack();
  init(1, &app_name);
}

Rice::VM::
VM(int argc, char * argv[])
{
  init_stack();
  init(argc, argv);
}

Rice::VM::
VM(std::vector<char *> const & args)
{
  init_stack();
  init(args.size(), const_cast<char * *>(&args[0]));
}

Rice::VM::
~VM()
{
  init_stack();
}

void Rice::VM::
init_stack()
{
  RUBY_INIT_STACK;
}

void Rice::VM::
run()
{
  ruby_run();
}

void Rice::VM::
init(int argc, char * argv[])
{
  if(ruby_scope)
  {
    throw std::runtime_error("Only one VM allowed per application");
  }

  ruby_init();
  ruby_options(argc, argv);
}

