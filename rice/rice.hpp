#ifndef Rice__hpp_
#define Rice__hpp_

// Ruby
#include "detail/ruby.hpp"

// C++ headers -h has to come after Ruby on MacOS for reasons I do not understand
#include <cstdio>
#include <string>
#include <typeinfo>
#include <typeindex>

#ifdef _MSC_VER
  // Prevent _strdup deprecated message on MSVC
  #define strdup _strdup 
#endif

// Traits
#include "traits/rice_traits.hpp"
#include "traits/function_traits.hpp"
#include "traits/method_traits.hpp"
#include "traits/attribute_traits.hpp"

// Code for C++ to call Ruby
#include "Exception.hpp"
#include "JumpException.hpp"
#include "JumpException.ipp"
#include "detail/RubyFunction.hpp"
#include "detail/RubyFunction.ipp"
#include "detail/to_ruby.hpp"

// Code for Ruby to call C++
#include "Arg.hpp"
#include "Return.hpp"
#include "detail/MethodInfo.hpp"
#include "detail/from_ruby.hpp"

// C++ API declarations
#include "detail/Type.hpp"
#include "cpp_api/Encoding.hpp"
#include "cpp_api/Identifier.hpp"
#include "cpp_api/Identifier.ipp"
#include "cpp_api/Object.hpp"
#include "cpp_api/Builtin_Object.hpp"
#include "cpp_api/String.hpp"
#include "cpp_api/Symbol.hpp"
#include "cpp_api/Array.hpp"
#include "cpp_api/Hash.hpp"

#include "cpp_api/Module.hpp"
#include "cpp_api/Class.hpp"

#include "detail/Native.hpp"
#include "detail/NativeAttributeGet.hpp"
#include "detail/NativeAttributeSet.hpp"
#include "Data_Type.hpp"
#include "Data_Object.hpp"

// Type Conversion declarations
#include "detail/RubyType.hpp"
#include "detail/RubyType.ipp"
#include "detail/Wrapper.hpp"

// Registries
#include "detail/TypeRegistry.hpp"
#include "detail/InstanceRegistry.hpp"
#include "detail/DefaultHandler.hpp"
#include "detail/HandlerRegistry.hpp"
#include "detail/NativeRegistry.hpp"
#include "detail/Registries.hpp"

// To / From Ruby
#include "Arg.ipp"
#include "Return.ipp"
#include "Constructor.hpp"
#include "Buffer.hpp"
#include "Buffer.ipp"
#include "detail/to_ruby.ipp"
#include "detail/from_ruby.ipp"
#include "detail/Proc.hpp"

// Registries
#include "detail/TypeRegistry.ipp"
#include "detail/InstanceRegistry.ipp"
#include "detail/DefaultHandler.ipp"
#include "detail/HandlerRegistry.ipp"
#include "detail/NativeRegistry.ipp"
#include "detail/Registries.ipp"

#include "detail/Type.ipp"

// Code for Ruby to call C++
#include "Exception.ipp"
#include "detail/cpp_protect.hpp"
#include "detail/Wrapper.ipp"
#include "detail/MethodInfo.ipp"
#include "detail/Native.ipp"
#include "detail/NativeAttributeGet.ipp"
#include "detail/NativeAttributeSet.ipp"
#include "detail/NativeFunction.hpp"
#include "detail/NativeFunction.ipp"
#include "detail/NativeIterator.hpp"
#include "detail/NativeIterator.ipp"
#include "detail/NativeCallbackFFI.hpp"
#include "detail/NativeCallbackFFI.ipp"
#include "detail/NativeCallbackSimple.hpp"
#include "detail/NativeCallbackSimple.ipp"
#include "detail/Proc.ipp"

// C++ API definitions
#include "cpp_api/Encoding.ipp"
#include "cpp_api/Object.ipp"
#include "cpp_api/Builtin_Object.ipp"
#include "cpp_api/String.ipp"
#include "cpp_api/Array.ipp"
#include "cpp_api/Hash.ipp"
#include "cpp_api/Symbol.ipp"

#include "cpp_api/Module.ipp"
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
#include "Data_Type.ipp"
#include "detail/default_allocation_func.ipp"
#include "Constructor.ipp"
#include "Callback.hpp"
#include "Callback.ipp"
#include "Data_Object.ipp"
#include "Enum.hpp"
#include "Enum.ipp"
#include "MemoryView.hpp"
#include "MemoryView.ipp"

// Dependent on Module, Class, Array and String
#include "forward_declares.ipp"

// For now include libc support - maybe should be separate header file someday
#include "libc/file.hpp"

// Initialize Rice
#include "Init.hpp"
#include "Init.ipp"

#endif // Rice__hpp_
