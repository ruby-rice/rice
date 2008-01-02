#include "rice/Data_Type.hpp"
#include "rice/Constructor.hpp"

using namespace Rice;

class Organism
{
public:
  virtual ~Organism() = 0;
  virtual char const * name() = 0;
};

Organism::
~Organism()
{
}

class Animal
  : public Organism
{
public:
  virtual ~Animal() = 0;
  virtual char const * speak() = 0;
};

Animal::
~Animal()
{
}

class Bear
  : public Animal
{
public:
  virtual char const * name()
  {
    return "Bear";
  }

  virtual char const * speak()
  {
    return "I'm smarter than the average bear";
  }
};

class Dog
  : public Animal
{
public:
  virtual char const * name()
  {
    return "Dog";
  }

  virtual char const * speak()
  {
    return "Woof woof";
  }
};

class Rabbit
  : public Animal
{
public:
  virtual char const * name()
  {
    return "Rabbit";
  }

  virtual char const * speak()
  {
    return "What's up, doc?";
  }
};

extern "C"
void Init_animals(void)
{
  RUBY_TRY
  {
    define_class<Organism>("Organism")
      .define_method("name", &Organism::name);

    define_class<Animal, Organism>("Animal")
      .define_method("speak", &Animal::speak);

    define_class<Bear, Animal>("Bear")
      .define_constructor(Constructor<Bear>());
      
    define_class<Dog, Animal>("Dog")
      .define_constructor(Constructor<Dog>());

    define_class<Rabbit, Animal>("Rabbit")
      .define_constructor(Constructor<Rabbit>());
  }
  RUBY_CATCH
}

