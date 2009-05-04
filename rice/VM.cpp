#include "VM.hpp"
#include "detail/ruby.hpp"
#include "detail/env.hpp" 
#include "detail/ruby_version_code.hpp"

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
VM(std::vector<const char *> const & args)
{
  check_not_initialized();
  init_stack();
  init(args.size(), const_cast<char * *>(&args[0]));
}

Rice::VM::
~VM()
{
  init_stack();
}

#if RICE__RUBY_VERSION_CODE < 186
  extern "C"
  void Init_stack(VALUE *);
#endif

void Rice::VM::
init_stack()
{
#if RICE__RUBY_VERSION_CODE >= 186
  RUBY_INIT_STACK;
#else
  VALUE v;
  Init_stack(&v);
#endif
}

void Rice::VM::
run()
{
#if RICE__RUBY_VERSION_CODE >= 190
  ruby_run_node(node_);
#else
  ruby_run();
#endif
}

extern "C"
{

#if RICE__RUBY_VERSION_CODE < 190
RUBY_EXTERN VALUE * rb_gc_stack_start;
#endif

}

void Rice::VM::
check_not_initialized() const
{
#if RICE__RUBY_VERSION_CODE < 190
  if(rb_gc_stack_start)
  {
    throw std::runtime_error("Only one VM allowed per application");
  }
#endif
  // TODO: how to do this check on 1.9?
}

void Rice::VM::
init(int argc, char * argv[])
{
  ruby_init();
#if RICE__RUBY_VERSION_CODE >= 190
  node_ =
#endif
  ruby_options(argc, argv);
}

