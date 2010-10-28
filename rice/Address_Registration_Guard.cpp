#include "Address_Registration_Guard.hpp"

bool Rice::Address_Registration_Guard::enabled = true;
bool Rice::Address_Registration_Guard::exit_handler_registered = false;

static void disable_all_guards(VALUE)
{
    Rice::Address_Registration_Guard::disable();
}

void Rice::Address_Registration_Guard::registerExitHandler()
{
    if (exit_handler_registered) return;
    rb_set_end_proc(&disable_all_guards, Qnil);
    exit_handler_registered = true;
}

void Rice::Address_Registration_Guard::disable()
{
    enabled = false;
}

