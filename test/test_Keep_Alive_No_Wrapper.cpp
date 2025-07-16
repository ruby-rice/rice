#include "unittest.hpp"
#include "embed_ruby.hpp"
#include <rice/rice.hpp>

using namespace Rice;

TESTSUITE(Keep_Alive_No_Wrapper);

namespace
{
  class Animal
  {
    public:
      Animal(char const * name) : name_(name) {}
      char const * getName() { return name_; }
      virtual ~Animal() = default;
    private:
      char const * name_;
  };

  class Zoo
  {
    public:
      Zoo(void)
      {
        pets_.push_back(new Animal("Bear"));
        pets_.push_back(new Animal("Tiger"));
        pets_.push_back(new Animal("Lion"));
      }

      ~Zoo()
      {
        for(auto pet : pets_)
        {
         delete pet;
        }
        pets_.clear();
      }

    Object getPets(void)
    {
      Array pets;
      for(auto p: pets_)
      {
        pets.push(p, false);
      }
      return pets;
    }

    private:
      std::vector<Animal*> pets_;
  };
}

SETUP(Keep_Alive_No_Wrapper)
{
  embed_ruby();
}

TEARDOWN(Keep_Alive_No_Wrapper)
{
  rb_gc_start();
}

TESTCASE(test_keep_alive_no_wrapper)
{
  define_class<Animal>("Animal")
    .define_constructor(Constructor<Animal, char const *>())
    .define_method("get_name", &Animal::getName);

  define_class<Zoo>("Zoo")
    .define_constructor(Constructor<Zoo>())
    .define_method("get_pets", &Zoo::getPets, Return().keepAlive());

  Module m = define_module("TestingModule");
  Object zoo = m.module_eval("@zoo = Zoo.new");

  // get_pets returns an Array (builtin type) so Return().keepAlive()
  // shall result in std::runtime_error
  ASSERT_EXCEPTION_CHECK(
    Exception,
    m.module_eval("@zoo.get_pets.each do |pet| puts pet.name; end"),
    ASSERT_EQUAL("When calling the method `get_pets' we could not find the wrapper for the 'Array' return type. You should not use keepAlive() on a Return or Arg that is a builtin Rice type.",
                 ex.what())
  );
}
