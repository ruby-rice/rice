#include <cstdio>
#include <filesystem>

#include "unittest.hpp"
#include "embed_ruby.hpp"
#include <rice/rice.hpp>
#include <rice/stl.hpp>

RICE_USE_NAMESPACE

TESTSUITE(FILE);

SETUP(FILE)
{
  embed_ruby();
}

TEARDOWN(FILE)
{
  rb_gc_start();
}

namespace
{
  FILE* openFile()
  {
    std::filesystem::path path = __FILE__;
    FILE* fptr = fopen(path.string().c_str(), "rb");
    if (!fptr)
    {
      int err = errno;
      throw std::runtime_error("Failed to open file '" + path.string() + "': " +
        std::strerror(err) + " (errno=" + std::to_string(err) + ")");
    }
    return fptr;
  }

  std::string readFile(FILE* fptr)
  {
    std::ostringstream result;

    char buffer[255]; 
    while (fgets(buffer, sizeof(buffer), fptr) != NULL)
    {
      result << buffer;
    }
    return result.str();
  }

  bool closeFile(FILE* fptr)
  {
    // crashes on windows due to Ruby overriding and mixing UCRTs
    //fclose(fptr);
    return true;
  }
}

TESTCASE(File)
{
  Module m = define_module("TestingModule");
  m.define_module_function("open_file", openFile).
    define_module_function("read_file", readFile).
    define_module_function("close_file", closeFile);

  Data_Object<FILE> file = m.call("open_file");
  ASSERT((file.value() != Qnil));
  ASSERT((file.get() != nullptr));

  String string = m.call("read_file", file);
  ASSERT((string.length() > 1300));

  Object result = m.call("close_file", file);
  ASSERT_EQUAL(Qtrue, result.value());
}