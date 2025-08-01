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
