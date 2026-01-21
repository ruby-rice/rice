#ifndef Rice__hpp_
#define Rice__hpp_

// Ruby
#include "detail/ruby.hpp"

// C++ headers have to come after Ruby on MacOS for reasons I do not understand
#include <cstdio>
#include <cstring> // For std::memset
#include <map>
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

// Wrap C++ objects as Ruby objects
#include "detail/Wrapper.hpp"
#include "detail/Type.hpp"
#include "detail/TypeIndexParser.hpp"

// Code for C++ to call Ruby
#include "Exception.hpp"
#include "JumpException.hpp"
#include "JumpException.ipp"
#include "detail/NativeInvoker.hpp"
#include "detail/NativeInvoker.ipp"
#include "detail/to_ruby.hpp"

// Code for Ruby to call C++
#include "Arg.hpp"
#include "Return.hpp"
#include "detail/from_ruby.hpp"
#include "detail/RubyType.hpp"
#include "detail/Parameter.hpp"

// C++ API declarations
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
#include "detail/RubyType.ipp"

// Registries
#include "detail/TypeRegistry.hpp"
#include "detail/InstanceRegistry.hpp"
#include "detail/DefaultHandler.hpp"
#include "detail/HandlerRegistry.hpp"
#include "detail/ModuleRegistry.hpp"
#include "detail/NativeRegistry.hpp"
#include "detail/Registries.hpp"

// To / From Ruby
#include "Arg.ipp"
#include "detail/Parameter.ipp"
#include "NoGVL.hpp"
#include "Return.ipp"
#include "Constructor.hpp"
#include "Buffer.hpp"
#include "Pointer.hpp"
#include "Reference.hpp"
#include "Buffer.ipp"
#include "Pointer.ipp"
#include "detail/Types.ipp"
#include "detail/to_ruby.ipp"
#include "detail/from_ruby.ipp"
#include "Reference.ipp"
#include "detail/Proc.hpp"

// Registries
#include "detail/TypeRegistry.ipp"
#include "detail/InstanceRegistry.ipp"
#include "detail/DefaultHandler.ipp"
#include "detail/HandlerRegistry.ipp"
#include "detail/ModuleRegistry.ipp"
#include "detail/NativeRegistry.ipp"
#include "detail/Registries.ipp"

#include "detail/Type.ipp"
#include "detail/TypeIndexParser.ipp"

// Code for Ruby to call C++
#include "Exception.ipp"
#include "detail/cpp_protect.hpp"
#include "detail/Wrapper.ipp"
#include "detail/Native.ipp"
#include "detail/NativeAttributeGet.ipp"
#include "detail/NativeAttributeSet.ipp"
#include "detail/NativeFunction.hpp"
#include "detail/NativeFunction.ipp"
#include "detail/NativeIterator.hpp"
#include "detail/NativeIterator.ipp"
#include "detail/NativeMethod.hpp"
#include "detail/NativeMethod.ipp"
#include "detail/NativeProc.hpp"
#include "detail/NativeProc.ipp"
#include "detail/Anchor.hpp"
#include "detail/Anchor.ipp"
#include "Pin.hpp"
#include "Pin.ipp"
#include "detail/NativeCallback.hpp"
#include "detail/NativeCallback.ipp"
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

// Dependent on Module, Array, Symbol - used by stl smart pointers
#include "detail/Forwards.hpp"

// For now include libc support - maybe should be separate header file someday
#include "libc/file.hpp"

#endif // Rice__hpp_
