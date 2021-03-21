#ifndef Rice__hpp_
#define Rice__hpp_

#include "detail/ruby.hpp"
#include "detail/rice_traits.hpp"
#include "detail/Type.hpp"
#include "detail/TypeRegistry.hpp"

#include "Return.hpp"
#include "Arg.hpp"
#include "detail/Arguments.hpp"

#include "detail/Wrapper.hpp"
#include "detail/default_allocation_func.hpp"
#include "detail/Jump_Tag.hpp"
#include "Exception_defn.hpp"
#include "detail/Ruby_Function.hpp"
#include "detail/from_ruby.hpp"
#include "detail/Exception_Handler.hpp"
#include "detail/Iterator.hpp"
#include "detail/NativeArg.hpp"
#include "detail/to_ruby.hpp"
#include "Exception.ipp"
#include "detail/method_data.hpp"
#include "ruby_try_catch.hpp"
#include "detail/Native_Attribute.hpp"
#include "detail/Native_Function.hpp"

// Deprecated - Arg_operators is deprecated and should be removed in the future
#include "Arg_operators.hpp"

#include "ruby_mark.hpp"

#include "Identifier.hpp"

#include "Object.hpp"

#include "Builtin_Object.hpp"
#include "String.hpp"
#include "Array.hpp"
#include "Hash.hpp"
#include "Symbol.hpp"

#include "Address_Registration_Guard.hpp"
#include "Module.hpp"
#include "global_function.hpp"
#include "detail/Caster.hpp"

#include "Class.hpp"
#include "Struct.hpp"

#include "Director.hpp"
#include "Data_Type.hpp"
#include "Constructor.hpp"
#include "Data_Object.hpp"
#include "detail/Iterator.ipp"

// Dependent on Data_Object due to the way method metadata is stored in the Ruby class
#include "detail/default_allocation_func.ipp"

#include "Enum.hpp"

#include "detail/Type.ipp"

// Dependent on Module, Class, Array and String
#include "forward_declares.ipp"

#endif // Rice__hpp_
