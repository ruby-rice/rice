#include <iostream>
#include <map>
#include "unittest.hpp"

size_t assertions;

namespace
{

typedef std::map<std::string, Test_Suite> Test_Suites;
Test_Suite * last_test_suite;

Test_Suites & test_suites()
{
  static Test_Suites test_suites;
  return test_suites;
}

} // namespace

Test_Suite & test_suite()
{
  return *last_test_suite;
}

void new_test_suite(std::string const & name)
{
  test_suites()[name] = Test_Suite(name);
  last_test_suite = &test_suites()[name];
}

std::ostream & operator<<(std::ostream & out, Failure const & failure)
{
  out << "" << failure.test_suite_name_ << ":" << failure.test_case_name_ << ": " << failure.what_;
  return out;
}

Test_Suite::
Test_Suite(std::string const & name)
  : name_(name)
  , setup_(0)
  , teardown_(0)
{
}

void Test_Suite::
run(Test_Result & result)
{
  for(Test_Cases::iterator it = test_cases_.begin(),
      end = test_cases_.end();
      it != end;
      ++it)
  {
    try
    {
      std::cout << "" << name() << ":" << it->name() << " ";
      std::cout.flush();
      if(setup_)
      {
        setup_();
      }
      it->run();
      std::cout << ".";
    }
    catch(Assertion_Failed const & ex)
    {
      std::cout << "F";
      result.add_failure(Failure(name(), it->name(), ex.what()));
    }
    catch(std::exception const & ex)
    {
      std::cout << "E";
      result.add_error(Failure(name(), it->name(), ex.what()));
    }
    catch(...)
    {
      std::cout << "E";
      result.add_error(Failure(name(), it->name(), "Unknown exception"));
    }
    if(teardown_)
    {
      teardown_();
    }
    std::cout << std::endl;
  }
}

int main()
{
  Test_Result result;
  size_t num_tests = 0;

  for(Test_Suites::iterator it = test_suites().begin(),
      end = test_suites().end();
      it != end;
      ++it)
  {
    it->second.run(result);
    num_tests += it->second.size();
  }

  std::cout << std::endl;

  std::cout << num_tests << " test(s), "
            << assertions << " assertion(s), "
            << result.errors().size() << " error(s), "
            << result.failures().size() << " failure(s)"
            << std::endl;

  if(result.errors().size() > 0)
  {
    std::cout << std::endl << "Errors:" << std::endl;
    for(std::vector<Failure>::const_iterator it = result.errors().begin(),
        end = result.errors().end();
        it != end;
        ++it)
    {
      std::cout << *it << std::endl;
    }
  }

  if(result.failures().size() > 0)
  {
    std::cout << std::endl << "Failures:" << std::endl;
    for(std::vector<Failure>::const_iterator it = result.failures().begin(),
        end = result.failures().end();
        it != end;
        ++it)
    {
      std::cout << *it << std::endl;
    }
  }

  return result.errors().size() + result.failures().size();
}

