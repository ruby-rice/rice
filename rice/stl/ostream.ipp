#include <sstream>
#include <fstream>
#include <iostream>

namespace Rice::stl
{
  class OStreamHelper
  {
  public:
    OStreamHelper(Data_Type<std::ostream> klass) : klass_(klass)
    {
      this->define_write_methods();
      this->define_state_methods();
    }

  private:
    void define_write_methods()
    {
      klass_.define_method("write", [](std::ostream& stream, VALUE value) -> std::ostream&
        {
          stream << Object(value).to_s().c_str();
          return stream;
        }, Arg("value").setValue())
        .define_method<std::ostream& (std::ostream::*)()>("flush", &std::ostream::flush)
        .define_method("clear", [](std::ostream& stream, int state)
          {
            stream.clear(static_cast<std::ios_base::iostate>(state));
          }, Arg("state") = static_cast<int>(std::ios_base::goodbit));

      rb_define_alias(klass_, "<<", "write");
    }

    void define_state_methods()
    {
      klass_.define_method("good?", &std::ostream::good)
            .define_method("bad?", &std::ostream::bad)
            .define_method("fail?", &std::ostream::fail)
            .define_method("eof?", &std::ostream::eof);
    }

    Data_Type<std::ostream> klass_;
  };

  class OStringStreamHelper
  {
  public:
    OStringStreamHelper(Data_Type<std::ostringstream> klass) : klass_(klass)
    {
      this->define_constructors();
      this->define_methods();
    }

  private:
    void define_constructors()
    {
      klass_.define_constructor(Constructor<std::ostringstream>())
            .define_constructor(Constructor<std::ostringstream, const std::string&>(), Arg("str"));
    }

    void define_methods()
    {
      klass_.define_method<std::string(std::ostringstream::*)() const>("str", &std::ostringstream::str)
            .define_method("str=", [](std::ostringstream& stream, const std::string& s) { stream.str(s); }, Arg("str"));

      rb_define_alias(klass_, "to_s", "str");
    }

    Data_Type<std::ostringstream> klass_;
  };

  class OFStreamHelper
  {
  public:
    OFStreamHelper(Data_Type<std::ofstream> klass) : klass_(klass)
    {
      this->define_constructors();
      this->define_methods();
    }

  private:
    void define_constructors()
    {
      klass_.define_constructor(Constructor<std::ofstream>())
            .define_constructor(Constructor<std::ofstream, const std::string&>(), Arg("filename"));
    }

    void define_methods()
    {
      klass_.define_method("open", [](std::ofstream& stream, const std::string& filename) { stream.open(filename); }, Arg("filename"))
            .define_method("close", &std::ofstream::close)
            .define_method<bool(std::ofstream::*)() const>("open?", &std::ofstream::is_open);
    }

    Data_Type<std::ofstream> klass_;
  };
}

namespace Rice
{
  inline Data_Type<std::ostream> define_ostream()
  {
    define_ios_base();

    Module rb_mStd = define_module("Std");
    if (Data_Type<std::ostream>::check_defined("OStream", rb_mStd))
    {
      return Data_Type<std::ostream>();
    }

    Data_Type<std::ostream> result = define_class_under<std::ostream>(rb_mStd, "OStream");
    stl::OStreamHelper helper(result);

    rb_mStd.const_set("COUT", Data_Object<std::ostream>(&std::cout).value());
    rb_mStd.const_set("CERR", Data_Object<std::ostream>(&std::cerr).value());

    return result;
  }

  inline Data_Type<std::ostringstream> define_ostringstream(std::string klassName)
  {
    define_ostream();

    if (klassName.empty())
    {
      klassName = "OStringStream";
    }

    Module rb_mStd = define_module("Std");
    if (Data_Type<std::ostringstream>::check_defined(klassName, rb_mStd))
    {
      return Data_Type<std::ostringstream>();
    }

    Data_Type<std::ostringstream> result = define_class_under<std::ostringstream, std::ostream>(rb_mStd, klassName.c_str());
    stl::OStringStreamHelper helper(result);
    return result;
  }

  inline Data_Type<std::ofstream> define_ofstream(std::string klassName)
  {
    define_ostream();

    if (klassName.empty())
    {
      klassName = "OFStream";
    }

    Module rb_mStd = define_module("Std");
    if (Data_Type<std::ofstream>::check_defined(klassName, rb_mStd))
    {
      return Data_Type<std::ofstream>();
    }

    Data_Type<std::ofstream> result = define_class_under<std::ofstream, std::ostream>(rb_mStd, klassName.c_str());
    stl::OFStreamHelper helper(result);
    return result;
  }
}

namespace Rice::detail
{
  template<>
  struct Type<std::ostream>
  {
    static bool verify()
    {
      if (!Data_Type<std::ostream>::is_defined())
      {
        define_ostream();
      }
      return true;
    }
  };

  template<>
  struct Type<std::ostringstream>
  {
    static bool verify()
    {
      if (!Data_Type<std::ostringstream>::is_defined())
      {
        define_ostringstream();
      }
      return true;
    }
  };

  template<>
  struct Type<std::ofstream>
  {
    static bool verify()
    {
      if (!Data_Type<std::ofstream>::is_defined())
      {
        define_ofstream();
      }
      return true;
    }
  };
}
