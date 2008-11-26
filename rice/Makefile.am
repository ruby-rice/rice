lib_LIBRARIES = librice.a

librice_a_SOURCES = \
Class.cpp \
Data_Type.cpp \
Exception.cpp \
Identifier.cpp \
Module.cpp \
Object.cpp \
String.cpp \
Struct.cpp \
Symbol.cpp \
VM.cpp \
detail/check_ruby_type.cpp \
detail/demangle.cpp \
detail/method_data.cpp \
detail/protect.cpp \
detail/mininode.cpp

nobase_include_HEADERS = \
Address_Registration_Guard.hpp \
Address_Registration_Guard.ipp \
Address_Registration_Guard_defn.hpp \
Allocation_Strategies.hpp \
Array.hpp \
Array.ipp \
Builtin_Object.hpp \
Builtin_Object.ipp \
Builtin_Object_defn.hpp \
Class.hpp \
Class.ipp \
Class_defn.hpp \
Constructor.hpp \
Critical_Guard.hpp \
Critical_Guard.ipp \
Data_Object.hpp \
Data_Object.ipp \
Data_Object_defn.hpp \
Data_Type.hpp \
Data_Type.ipp \
Data_Type_defn.hpp \
Data_Type_fwd.hpp \
Enum.hpp \
Enum.ipp \
Exception.hpp \
Exception_Base.hpp \
Exception_Base.ipp \
Hash.hpp \
Hash.ipp \
Identifier.hpp \
Identifier.ipp \
Jump_Tag.hpp \
Makefile \
Module.hpp \
Module.ipp \
Module_defn.hpp \
Module_impl.hpp \
Module_impl.ipp \
Object.hpp \
Object.ipp \
Object_defn.hpp \
Require_Guard.hpp \
String.hpp \
Struct.hpp \
Struct.ipp \
Symbol.hpp \
Symbol.ipp \
VM.hpp \
generate_code.rb \
global_function.hpp \
global_function.ipp \
protect.hpp \
protect.ipp \
ruby_try_catch.hpp \
to_from_ruby.hpp \
to_from_ruby.ipp \
to_from_ruby_defn.hpp \
detail/Auto_Function_Wrapper.hpp \
detail/Auto_Function_Wrapper.ipp \
detail/Auto_Member_Function_Wrapper.hpp \
detail/Auto_Member_Function_Wrapper.ipp \
detail/Caster.hpp \
detail/Exception_Handler.hpp \
detail/Exception_Handler.ipp \
detail/Exception_Handler_defn.hpp \
detail/Iterator.hpp \
detail/Not_Copyable.hpp \
detail/Wrapped_Function.hpp \
detail/check_ruby_type.hpp \
detail/creation_funcs.hpp \
detail/creation_funcs.ipp \
detail/default_allocation_func.hpp \
detail/default_allocation_func.ipp \
detail/define_method_and_auto_wrap.hpp \
detail/define_method_and_auto_wrap.ipp \
detail/demangle.hpp \
detail/env.hpp \
detail/from_ruby.hpp \
detail/from_ruby.ipp \
detail/method_data.hpp \
detail/node.hpp \
detail/object_call.hpp \
detail/object_call.ipp \
detail/protect.hpp \
detail/remove_const.hpp \
detail/ruby.hpp \
detail/rubysig.hpp \
detail/st.hpp \
detail/to_ruby.hpp \
detail/to_ruby.ipp \
detail/win32.hpp \
detail/wrap_function.hpp \
detail/wrap_function.ipp

includedir = ${prefix}/include/rice

AM_CPPFLAGS = @RUBY_CPPFLAGS@

AM_CXXFLAGS = @RUBY_CXXFLAGS@

detail/method_data.cpp: detail/method_data.cpp.rpp detail/mininode.hpp
	ruby -C detail ../rubypp.rb method_data.cpp.rpp method_data.cpp

detail/mininode.cpp: detail/mininode.cpp.rpp
	ruby -C detail ../rubypp.rb mininode.cpp.rpp mininode.cpp

detail/mininode.hpp: detail/mininode.hpp.rpp
	ruby -C detail ../rubypp.rb mininode.hpp.rpp mininode.hpp

