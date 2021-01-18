#ifndef Rice__hpp_
#define Rice__hpp_

#include "detail/ruby_version_code.hpp"
#include "detail/ruby.hpp"
#include "detail/rice_traits.hpp"

#include "detail/default_allocation_func.hpp"
#include "detail/demangle.hpp"
#include "detail/env.hpp"
#include "detail/from_ruby.hpp"
#include "detail/Exception_Handler.hpp"
#include "detail/Jump_Tag.hpp"
#include "detail/method_data.hpp"
#include "detail/NativeArg.hpp"
#include "detail/protect.hpp"
#include "detail/to_ruby.hpp"
#include "detail/Ruby_Function.hpp"
#include "detail/Wrapped_Function.hpp"

#include "Arg_impl.hpp"
#include "detail/Arguments.hpp"
#include "Arg_operators.hpp"
#include "global_function.hpp"
#include "Exception.hpp"
#include "protect.hpp"

#include "Identifier.hpp"

#include "detail/Iterator.hpp"

#include "Object.hpp"
#include "Builtin_Object.hpp"
#include "String.hpp"
#include "Array.hpp"
#include "Hash.hpp"
#include "String.hpp"
#include "Symbol.hpp"

#include "Address_Registration_Guard.hpp"
#include "Module.hpp"

#include "detail/Caster.hpp"

#include "Class.hpp"

#include "detail/creation_funcs.hpp"

#include "Constructor.hpp"
#include "Director.hpp"
#include "Data_Type.hpp"
#include "Data_Object.hpp"
#include "Enum.hpp"
#include "Struct.hpp"

#include "forward_declares.ipp"

#endif // Rice__hpp_
