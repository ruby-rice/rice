#ifndef Rice__Ruby_Api_hpp_
#define Rice__Ruby_Api_hpp_


// =========   NativeRegistry.hpp   =========
#ifndef Rice_Ruby_Api_Native_Registry_hpp
#define Rice_Ruby_Api_Native_Registry_hpp

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
#ifndef Rice_Ruby_Api_Type_Registry_hpp
#define Rice_Ruby_Api_Type_Registry_hpp

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
#ifndef Rice_Ruby_Api_Registries_hpp
#define Rice_Ruby_Api_Registries_hpp

void Init_Registries();

#endif

// =========   Registries.ipp   =========
using namespace Rice;

inline void Init_Registries()
{
  Module rb_mRice = define_module("Rice");

  define_class_under<detail::Registries>(rb_mRice, "Registries").
    define_singleton_attr("instance", &detail::Registries::instance, AttrAccess::Read).
    define_attr("instances", &detail::Registries::instances, AttrAccess::Read).
    define_attr("natives", &detail::Registries::natives, AttrAccess::Read).
    define_attr("types", &detail::Registries::types, AttrAccess::Read);
}

// =========   Arg.hpp   =========
#ifndef Rice_Ruby_Api_Arg_hpp
#define Rice_Ruby_Api_Arg_hpp

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
#ifndef Rice_Ruby_Api_Parameter_hpp
#define Rice_Ruby_Api_Parameter_hpp

extern "C"
void Init_Parameter();

#endif

// =========   Parameter.ipp   =========
using namespace Rice;

inline void Init_Parameter()
{
  Module rb_mRice = define_module("Rice");

  define_class_under<detail::ParameterAbstract>(rb_mRice, "Parameter").
    define_attr("arg", &detail::ParameterAbstract::arg).
    define_method("klass", &detail::ParameterAbstract::rubyTypeName);
}

// =========   Native.hpp   =========
#ifndef Rice_Ruby_Api_Native_Function_hpp
#define Rice_Ruby_Api_Native_Function_hpp

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
    .define_value("Proc", detail::NativeKind::Proc);

  define_class_under<detail::Native>(rb_mRice, "Native").
    define_method("name", &detail::Native::name).
    define_method("kind", &detail::Native::kind).
    define_method("return_type", &detail::Native::rubyReturnType).
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
inline void Init_Ruby_Api()
{
  Init_Registries();
  Init_Native_Registry();
  Init_Type_Registry();
  Init_Arg();
  Init_Parameter();
  Init_Native();
}

#endif
