#include "VM.hpp"
#include "detail/ruby.hpp"

#ifndef RUBY_VM
extern C
{
#include "env.h"
}
#endif

#include <stdexcept>

Rice::VM::
VM(char * app_name)
{
  Object o;
  init_stack(&o);
  init(1, &app_name);
}

Rice::VM::
VM(int argc, char * argv[])
{
  Object o;
  init_stack(&o);
  init(argc, argv);
}

Rice::VM::
VM(std::vector<char *> const & args)
{
  check_not_initialized();
  Object o;
  init_stack(&o);
  init(args.size(), const_cast<char * *>(&args[0]));
}

Rice::VM::
~VM()
{
  Object o;
  init_stack(&o);
}

#if RUBY_VERSION_CODE < 186
  extern "C"
  void Init_stack(VALUE *);
#endif

void Rice::VM::
init_stack(Object * location_on_stack)
{
  VALUE * v = const_cast<VALUE *>(&location_on_stack->value());
#if RUBY_VERSION_CODE >= 186
  ruby_init_stack(v);
#else
  Init_stack(v);
#endif
}

void Rice::VM::
run()
{
  Object o;
  init_stack(&o);
#ifdef RUBY_VM
#else
  ruby_run();
#endif
}

extern "C"
{

RUBY_EXTERN VALUE * rb_gc_stack_start;

}

void Rice::VM::
check_not_initialized() const
{
  if(rb_gc_stack_start)
  {
    throw std::runtime_error("Only one VM allowed per application");
  }
}

void Rice::VM::
init(int argc, char * argv[])
{
  ruby_init();
  ruby_options(argc, argv);
}

