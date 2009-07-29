$spec = Gem::Specification.new do |s|
  s.name = 'rice'
  s.version = '1.1.0'
  s.summary = 'Ruby Interface for C++ Extensions'
  s.homepage = 'http://rice.rubyforge.org/'
  s.rubyforge_project = 'rice'
  s.author = 'Paul Brannan'
  s.email = 'curlypaul924@gmail.com'

  s.description = <<-END
Rice is a C++ interface to Ruby's C API.  It provides a type-safe and
exception-safe interface in order to make embedding Ruby and writing
Ruby extensions with C++ easier.  It is similar to Boost.Python in many
ways, but also attempts to provide an object-oriented interface to all
of the Ruby C API.
  END

  s.extensions = ['extconf.rb']

  s.test_files = [
    'test/test_rice.rb',
  ]

  s.extra_rdoc_files = [ 'README' ]

  s.require_paths = [ 'ruby/lib' ]

  s.files = %w(
    COPYING
    README
    README.mingw
    Doxyfile
    doxygen.ac
    doxygen.am
    extconf.rb
    bootstrap
    configure.ac
    configure
    config.guess
    config.sub
    depcomp
    doxygen.ac
    doxygen.am
    install-sh
    missing
    post-autoconf.rb
    post-automake.rb
    ruby.ac
    aclocal.m4
    Rakefile
    Makefile.am
    Makefile.in
    rice/Makefile.am
    rice/Makefile.in
    ruby/Makefile.am
    ruby/Makefile.in
    ruby/lib/Makefile.am
    ruby/lib/Makefile.in
    sample/Makefile.am
    sample/Makefile.in
    test/Makefile.am
    test/Makefile.in
    rice/Arg.hpp
    rice/Exception_Base.hpp
    rice/Constructor.hpp
    rice/Object_defn.hpp
    rice/VM.cpp
    rice/VM.hpp
    rice/Module.hpp
    rice/Critical_Guard.ipp
    rice/Identifier.ipp
    rice/Exception_defn.hpp
    rice/Data_Type_fwd.hpp
    rice/config.hpp
    rice/Builtin_Object.hpp
    rice/Exception_Base.ipp
    rice/Symbol.cpp
    rice/Class.cpp
    rice/Array.hpp
    rice/Module_defn.hpp
    rice/Hash.ipp
    rice/Exception.hpp
    rice/Struct.ipp
    rice/protect.hpp
    rice/Data_Object.hpp
    rice/Class.ipp
    rice/Module.cpp
    rice/Arg_impl.hpp
    rice/Hash.hpp
    rice/Symbol.ipp
    rice/Class_defn.hpp
    rice/Arg_operators.cpp
    rice/Object.hpp
    rice/Allocation_Strategies.hpp
    rice/Enum.ipp
    rice/Director.cpp
    rice/Object.cpp
    rice/Address_Registration_Guard_defn.hpp
    rice/Builtin_Object.ipp
    rice/String.hpp
    rice/protect.ipp
    rice/Exception_Base_defn.hpp
    rice/Array.ipp
    rice/Require_Guard.hpp
    rice/Module_impl.ipp
    rice/Identifier.cpp
    rice/Data_Object.ipp
    rice/Exception.cpp
    rice/Data_Type.ipp
    rice/Symbol.hpp
    rice/Builtin_Object_defn.hpp
    rice/Struct.cpp
    rice/Data_Type.cpp
    rice/ruby_try_catch.hpp
    rice/Data_Type.hpp
    rice/String.cpp
    rice/Object.ipp
    rice/Data_Type_defn.hpp
    rice/Director.hpp
    rice/Critical_Guard.hpp
    rice/Jump_Tag.hpp
    rice/to_from_ruby.ipp
    rice/to_from_ruby.hpp
    rice/Module_impl.hpp
    rice/Arg_operators.hpp
    rice/Struct.hpp
    rice/Identifier.hpp
    rice/global_function.hpp
    rice/ruby_mark.hpp
    rice/Data_Object_defn.hpp
    rice/Address_Registration_Guard.hpp
    rice/Class.hpp
    rice/global_function.ipp
    rice/to_from_ruby_defn.hpp
    rice/Address_Registration_Guard.ipp
    rice/Enum.hpp
    rice/Module.ipp
    rice/generate_code.rb
    rice/rubypp.rb
    rice/config.hpp.in
    rice/detail/creation_funcs.hpp
    rice/detail/Arguments.hpp
    rice/detail/wrap_function.ipp
    rice/detail/check_ruby_type.cpp
    rice/detail/demangle.cpp
    rice/detail/check_ruby_type.hpp
    rice/detail/Auto_Member_Function_Wrapper.ipp
    rice/detail/mininode.cpp
    rice/detail/Auto_Member_Function_Wrapper.hpp
    rice/detail/protect.cpp
    rice/detail/node.hpp
    rice/detail/Wrapped_Function.hpp
    rice/detail/protect.hpp
    rice/detail/mininode.cpp.rpp
    rice/detail/demangle.hpp
    rice/detail/define_method_and_auto_wrap.hpp
    rice/detail/default_allocation_func.ipp
    rice/detail/wrap_function.hpp
    rice/detail/Caster.hpp
    rice/detail/object_call.hpp
    rice/detail/from_ruby.ipp
    rice/detail/ruby.hpp
    rice/detail/st.hpp
    rice/detail/from_ruby.hpp
    rice/detail/win32.hpp
    rice/detail/mininode.hpp.rpp
    rice/detail/creation_funcs.ipp
    rice/detail/to_ruby.hpp
    rice/detail/remove_const.hpp
    rice/detail/Auto_Function_Wrapper.hpp
    rice/detail/env.hpp
    rice/detail/method_data.cpp.rpp
    rice/detail/to_ruby.ipp
    rice/detail/method_data.hpp
    rice/detail/Exception_Handler.hpp
    rice/detail/object_call.ipp
    rice/detail/mininode.hpp
    rice/detail/rubysig.hpp
    rice/detail/ruby_version_code.hpp
    rice/detail/method_data.cpp
    rice/detail/Iterator.hpp
    rice/detail/Not_Copyable.hpp
    rice/detail/Exception_Handler_defn.hpp
    rice/detail/define_method_and_auto_wrap.ipp
    rice/detail/Auto_Function_Wrapper.ipp
    rice/detail/Exception_Handler.ipp
    rice/detail/default_allocation_func.hpp
    rice/detail/ruby_version_code.hpp.in
    ruby/lib/mkmf-rice.rb.in
    ruby/lib/version.rb
    sample/enum/extconf.rb
    sample/enum/sample_enum.cpp
    sample/enum/test.rb
    sample/enum/extconf.rb
    sample/map/extconf.rb
    sample/map/map.cpp
    sample/map/test.rb
    sample/map/extconf.rb
    sample/inheritance/extconf.rb
    sample/inheritance/animals.cpp
    sample/inheritance/test.rb
    sample/inheritance/extconf.rb
    test/test_Data_Type.cpp
    test/test_Exception.cpp
    test/test_To_From_Ruby.cpp
    test/test_String.cpp
    test/test_Data_Object.cpp
    test/test_Critical_Guard.cpp
    test/test_VM.cpp
    test/test_Array.cpp
    test/test_global_functions.cpp
    test/test_Struct.cpp
    test/test_Builtin_Object.cpp
    test/test_Module.cpp
    test/test_Allocation_Strategies.cpp
    test/test_Identifier.cpp
    test/unittest.hpp
    test/test_Constructor.cpp
    test/test_Jump_Tag.cpp
    test/test_Class.cpp
    test/test_Object.cpp
    test/test_Hash.cpp
    test/test_Symbol.cpp
    test/unittest.cpp
    test/test_Enum.cpp
    test/test_Address_Registration_Guard.cpp
    test/test_Director.cpp
  )
end
