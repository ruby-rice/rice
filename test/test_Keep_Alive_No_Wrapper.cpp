#include "unittest.hpp"
#include "embed_ruby.hpp"
#include <rice/rice.hpp>
#include <rice/stl.hpp>

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

    Object getPets(void)    {
      Array pets;
      for(auto p: pets_) {
        pets.push(p);
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
    ASSERT_EQUAL("Could not find wrapper for 'Array' return type. Did you use keepAlive() on a method that returns a builtin type?",
                 ex.what())
  );
}
