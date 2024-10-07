#include "unittest.hpp"
#include "embed_ruby.hpp"

#include <rice/rice.hpp>
#include <rice/stl.hpp>

using namespace Rice;

TESTSUITE(NativeRegistry);

SETUP(NativeRegistry)
{
  embed_ruby();
}

TESTCASE(collisions)
{
  std::array<Class, 100> classes;
  int scale = 1000;

  for (int i = 0; i < std::size(classes); i++)
  {
    Class cls(anonymous_class());

    for (int j = 0; j < scale; j++)
    {
      cls.define_function("int" + std::to_string(j), []() { return 1; });
      cls.define_function("long" + std::to_string(j), []() { return 1L; });
      cls.define_function("double" + std::to_string(j), []() { return 1.0; });
      cls.define_function("float" + std::to_string(j), []() { return 1.0f; });
      cls.define_function("bool" + std::to_string(j), []() { return true; });
    }

    classes[i] = cls;
  }

  for (auto& cls : classes)
  {
    auto obj = cls.call("new");

    for (int j = 0; j < scale; j++)
    {
      obj.call("int" + std::to_string(j));
      obj.call("long" + std::to_string(j));
      obj.call("double" + std::to_string(j));
      obj.call("float" + std::to_string(j));
      obj.call("bool" + std::to_string(j));
    }
  }
}
