#ifndef Rice__hpp_
#define Rice__hpp_

#include "detail/ruby.hpp"
#include "detail/rice_traits.hpp"
#include "detail/function_traits.hpp"
#include "detail/Type.hpp"
#include "detail/TypeRegistry.hpp"
#include "detail/Wrapper.hpp"
#include "detail/default_allocation_func.hpp"
#include "detail/Jump_Tag.hpp"
#include "Exception_defn.hpp"
#include "detail/RubyFunction.hpp"
#include "detail/from_ruby.hpp"
#include "detail/to_ruby.hpp"
#include "Return.hpp"
#include "Arg.hpp"
#include "detail/MethodInfo.hpp"
#include "detail/Exception_Handler.hpp"
#include "detail/Iterator.hpp"
#include "detail/NativeReturn.hpp"
#include "Exception.ipp"
#include "detail/method_data.hpp"
#include "ruby_try_catch.hpp"
#include "detail/NativeAttribute.hpp"
#include "detail/NativeFunction.hpp"

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

// Dependent on Module, Class, Array and String
#include "forward_declares.ipp"

#endif // Rice__hpp_
