#include "rice/Enum.hpp"
#include "rice/ruby_try_catch.hpp"

using namespace Rice;

namespace
{

enum Sample_Enum
{
  SE_FOO = 1,
  SE_BAR = 42,
  SE_BAZ = 100,
};

Rice::Enum<Sample_Enum> sample_enum_type;

char const * description(Sample_Enum e)
{
  switch(e)
  {
    case SE_FOO: return "Fairly Ordinary Object";
    case SE_BAR: return "Beginner's All-purpose Ratchet";
    case SE_BAZ: return "Better than A Zebra";
  }
  return "???";
}

} // namespace

template<>
Sample_Enum from_ruby<Sample_Enum>(Object x)
{
  Data_Object<Sample_Enum> d(x, sample_enum_type);
  return *d;
}

extern "C"
void Init_sample_enum()
{
  RUBY_TRY
  {
    sample_enum_type =
      define_enum<Sample_Enum>("Sample_Enum")
      .define_value("FOO", SE_FOO)
      .define_value("BAR", SE_BAR)
      .define_value("BAZ", SE_BAZ);
     
    sample_enum_type
      .define_method("description", description);
  }
  RUBY_CATCH
}

