#include "unittest.hpp"
#include "rice/VM.hpp"
#include "rice/Object.hpp"
#include <memory>

using namespace Rice;

std::auto_ptr<VM> vm;

TESTSUITE(VM);

SETUP(VM)
{
  std::vector<const char *> args;
  args.push_back("test_VM");
  args.push_back("-e");
  args.push_back("puts \"HELLO\"");
  vm.reset(new VM(args));
}

TESTCASE(create_object)
{
  Object o;
  o.call("to_s");
}

