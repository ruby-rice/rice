#include "unittest.hpp"
#include "rice/Allocation_Strategies.hpp"

using namespace Rice;

namespace
{

bool constructor_called = false;
bool destructor_called = false;

class Foo
{
public:
  Foo()
  {
    constructor_called = true;
  }

  ~Foo()
  {
    destructor_called = true;
  }
};

} // namespace

TESTSUITE(Allocation_Strategies);

SETUP(Allocation_Strategies)
{
  ruby_init();
  constructor_called = false;
  destructor_called = false;
}

TESTCASE(default_allocation_strategy_allocate)
{
  Foo * t = Default_Allocation_Strategy<Foo>::allocate();
  ASSERT(constructor_called);
  ASSERT(!destructor_called);
  delete t;
  ASSERT(constructor_called);
  ASSERT(destructor_called);
}

TESTCASE(default_allocation_strategy_free)
{
  Foo * t = new Foo;
  ASSERT(constructor_called);
  ASSERT(!destructor_called);
  Default_Allocation_Strategy<Foo>::free(t);
  ASSERT(constructor_called);
  ASSERT(destructor_called);
}

TESTCASE(xmalloc_allocation_strategy_allocate)
{
  Foo * t = Xmalloc_Allocation_Strategy<Foo>::allocate();
  ASSERT(constructor_called);
  ASSERT(!destructor_called);
  t->~Foo();
  xfree(t);
  ASSERT(constructor_called);
  ASSERT(destructor_called);
}

TESTCASE(xmalloc_allocation_strategy_free)
{
  Foo * t = Xmalloc_Allocation_Strategy<Foo>::allocate();
  ASSERT(constructor_called);
  ASSERT(!destructor_called);
  Xmalloc_Allocation_Strategy<Foo>::free(t);
  ASSERT(constructor_called);
  ASSERT(destructor_called);
}

