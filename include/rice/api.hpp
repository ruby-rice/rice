// This file is part of [rice](https://github.com/ruby-rice/rice).
//
// Copyright (C) 2025 Jason Roelofs <jasongroelofs@gmail.com>
//                    Paul Brannan <curlypaul924@gmail.com>,
//                    Charlie Savage
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
// NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef Rice__Api_hpp_
#define Rice__Api_hpp_


// =========   ModuleRegistry.hpp   =========
#ifndef Rice_Api_Module_Registry_hpp
#define Rice_Api_Module_Registry_hpp

extern "C"
void Init_Module_Registry();

#endif

// =========   ModuleRegistry.ipp   =========
using namespace Rice;

extern "C"
void Init_Module_Registry()
{
  Module rb_mRice = define_module("Rice");

  Rice::define_class_under<detail::ModuleRegistry>(rb_mRice, "ModuleRegistry").
    define_method("modules", &detail::ModuleRegistry::modules, Return().setValue());
}

// =========   NativeRegistry.hpp   =========
#ifndef Rice_Api_Native_Registry_hpp
#define Rice_Api_Native_Registry_hpp

extern "C"
void Init_Native_Registry();

#endif

// =========   NativeRegistry.ipp   =========
using namespace Rice;

extern "C"
inline void Init_Native_Registry()
{
  Module rb_mRice = define_module("Rice");

  Rice::define_class_under<detail::NativeRegistry>(rb_mRice, "NativeRegistry").
    define_method("lookup", [](detail::NativeRegistry& self, VALUE klass) -> Array
    {
      Array result;

      const std::vector<detail::Native*> natives = self.lookup(klass);
      for (detail::Native* native : natives)
      {
        result.push(native, false);
      }

      return result;
    }, Arg("klass").setValue());
}

// =========   TypeRegistry.hpp   =========
#ifndef Rice_Api_Type_Registry_hpp
#define Rice_Api_Type_Registry_hpp

extern "C"
void Init_Type_Registry();

#endif

// =========   TypeRegistry.ipp   =========
using namespace Rice;

extern "C"
void Init_Type_Registry()
{
  Module rb_mRice = define_module("Rice");

  Rice::define_class_under<detail::TypeRegistry>(rb_mRice, "TypeRegistry").
    define_method("klasses", &detail::TypeRegistry::klasses, Return().setValue());
}

// =========   Registries.hpp   =========
#ifndef Rice_Api_Registries_hpp
#define Rice_Api_Registries_hpp

void Init_Registries();

#endif

// =========   Registries.ipp   =========
using namespace Rice;

inline void Init_Registries()
{
  Module rb_mRice = define_module("Rice");

  define_class_under<detail::Registries>(rb_mRice, "Registries").
    define_singleton_attr("instance", &detail::Registries::instance, AttrAccess::Read).
    define_attr("modules", &detail::Registries::modules, AttrAccess::Read).
    define_attr("natives", &detail::Registries::natives, AttrAccess::Read).
    define_attr("types", &detail::Registries::types, AttrAccess::Read);
}

// =========   Arg.hpp   =========
#ifndef Rice_Api_Arg_hpp
#define Rice_Api_Arg_hpp

extern "C"
void Init_Arg();

#endif

// =========   Arg.ipp   =========
using namespace Rice;

inline void Init_Arg()
{
  Module rb_mRice = define_module("Rice");

  define_class_under<Arg>(rb_mRice, "Arg").
    define_attr("name", &Arg::name, AttrAccess::Read);
}

// =========   Parameter.hpp   =========
#ifndef Rice_Api_Parameter_hpp
#define Rice_Api_Parameter_hpp

extern "C"
void Init_Parameter();

#endif

// =========   Parameter.ipp   =========
using namespace Rice;

inline void Init_Parameter()
{
  Module rb_mRice = define_module("Rice");

  define_class_under<detail::ParameterAbstract>(rb_mRice, "Parameter").
    define_method("arg", &detail::ParameterAbstract::arg).
    define_method("klass", &detail::ParameterAbstract::klass, Return().setValue()).
    define_method("cpp_klass", &detail::ParameterAbstract::cppTypeName);
}

// =========   Native.hpp   =========
#ifndef Rice_Api_Native_Function_hpp
#define Rice_Api_Native_Function_hpp

extern "C"
void Init_Native();

#endif

// =========   Native.ipp   =========
using namespace Rice;

#include <ostream>

extern "C"
inline void Init_Native()
{
  Module rb_mRice = define_module("Rice");

  define_enum_under<detail::NativeKind>("NativeKind", rb_mRice)
    .define_value("Function", detail::NativeKind::Function)
    .define_value("Method", detail::NativeKind::Method)
    .define_value("AttributeReader", detail::NativeKind::AttributeReader)
    .define_value("AttributeWriter", detail::NativeKind::AttributeWriter)
    .define_value("Proc", detail::NativeKind::Proc)
    .define_value("Callback", detail::NativeKind::Callback);

  define_class_under<detail::Native>(rb_mRice, "Native").
    define_method("name", &detail::Native::name).
    define_method("kind", &detail::Native::kind).
    define_method("return_klass", &detail::Native::returnKlass, Return().setValue()).
    define_method("parameters", &detail::Native::parameters).
    define_method("to_s", [](detail::Native& self) -> std::string
      {
        std::ostringstream stream;
        stream << "Native";

        switch (self.kind())
        {
          case detail::NativeKind::Function:
            stream << "Function";
            break;
          case detail::NativeKind::Method:
            stream << "Method";
            break;
          case detail::NativeKind::Iterator:
            stream << "Iterator";
            break;
          case detail::NativeKind::AttributeReader:
            stream << "AttributeReader";
            break;
          case detail::NativeKind::AttributeWriter:
            stream << "AttributeWriter";
            break;
          case detail::NativeKind::Proc:
            stream << "Proc";
            break;
        }

        stream << "<" << self.name() << "> ";
        return stream.str();
      });
}

extern "C"
inline void Init_Rice_Api()
{
  Init_Registries();
  Init_Module_Registry();
  Init_Native_Registry();
  Init_Type_Registry();
  Init_Arg();
  Init_Parameter();
  Init_Native();
}
#endif
