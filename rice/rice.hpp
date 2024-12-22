#ifndef Rice__hpp_
#define Rice__hpp_

// Traits
#include "detail/ruby.hpp"
#include "traits/rice_traits.hpp"
#include "traits/function_traits.hpp"
#include "traits/method_traits.hpp"
#include "traits/attribute_traits.hpp"

// Helpers
#include "detail/TupleIterator.hpp"

// Code for C++ to call Ruby
#include "Exception.hpp"
#include "detail/Jump_Tag.hpp"
#include "detail/RubyFunction.hpp"
#include "detail/RubyFunction.ipp"

// C++ API declarations
#include "Identifier.hpp"
#include "Identifier.ipp"
#include "cpp_api/Object.hpp"
#include "cpp_api/Builtin_Object.hpp"
#include "cpp_api/String.hpp"
#include "cpp_api/Array.hpp"
#include "cpp_api/Hash.hpp"
#include "cpp_api/Symbol.hpp"

// Type Conversion declarations
#include "detail/Type.hpp"
#include "detail/RubyType.hpp"
#include "detail/RubyType.ipp"
#include "detail/to_ruby.hpp"
#include "detail/from_ruby.hpp"

#include "Arg.hpp"
#include "Arg.ipp"
#include "Return.hpp"
#include "Return.ipp"
#include "detail/to_ruby.ipp"
#include "detail/from_ruby.ipp"

// Registries
#include "detail/TypeRegistry.hpp"
#include "detail/TypeRegistry.ipp"
#include "detail/InstanceRegistry.hpp"
#include "detail/InstanceRegistry.ipp"

#include "detail/ExceptionHandler.hpp"
#include "detail/HandlerRegistry.hpp"

#include "detail/ExceptionHandler.ipp"

#include "detail/Native.hpp"
#include "detail/NativeRegistry.hpp"
#include "detail/NativeRegistry.ipp"
#include "detail/Registries.hpp"
#include "detail/Registries.ipp"

#include "detail/Type.ipp"
#include "detail/HandlerRegistry.ipp"
#include "HandlerRegistration.hpp"

// Code for Ruby to call C++
#include "Exception.ipp"
#include "detail/cpp_protect.hpp"
#include "detail/Wrapper.hpp"
#include "detail/Wrapper.ipp"
#include "detail/MethodInfo.hpp"
#include "detail/MethodInfo.ipp"
#include "detail/Native.ipp"
#include "detail/NativeAttributeGet.hpp"
#include "detail/NativeAttributeGet.ipp"
#include "detail/NativeAttributeSet.hpp"
#include "detail/NativeAttributeSet.ipp"
#include "detail/NativeFunction.hpp"
#include "detail/NativeFunction.ipp"
#include "detail/NativeIterator.hpp"
#include "detail/NativeIterator.ipp"

// C++ API definitions
#include "cpp_api/Object.ipp"
#include "cpp_api/Builtin_Object.ipp"
#include "cpp_api/String.ipp"
#include "cpp_api/Array.ipp"
#include "cpp_api/Hash.ipp"
#include "cpp_api/Symbol.ipp"

#include "cpp_api/Module.hpp"
#include "cpp_api/Module.ipp"
#include "cpp_api/Class.hpp"
#include "cpp_api/Class.ipp"
#include "cpp_api/Struct.hpp"
#include "cpp_api/Struct.ipp"

#include "Address_Registration_Guard.hpp"
#include "Address_Registration_Guard.ipp"
#include "global_function.hpp"
#include "global_function.ipp"

// Code involved in creating custom DataTypes (ie, Ruby classes that wrap C++ classes)
#include "ruby_mark.hpp"
#include "detail/default_allocation_func.hpp"
#include "Director.hpp"
#include "Data_Type.hpp"
#include "Data_Type.ipp"
#include "detail/default_allocation_func.ipp"
#include "Constructor.hpp"
#include "Constructor.ipp"
#include "Data_Object.hpp"
#include "Data_Object.ipp"
#include "Enum.hpp"
#include "Enum.ipp"
#include "MemoryView.hpp"
#include "MemoryView.ipp"

// Dependent on Module, Class, Array and String
#include "forward_declares.ipp"

#endif // Rice__hpp_
