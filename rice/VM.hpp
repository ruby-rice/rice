#ifndef VM__hpp
#define VM__hpp

#include <vector>
#include "detail/ruby_version_code.hpp"

namespace Rice
{

class VM
{
public:
  VM(char * app_name);
  VM(int argc, char * argv[]);
  VM(std::vector<const char *> const & args);
  ~VM();

  void init_stack();
  void run();

private:
  void check_not_initialized() const;
  void init(int argc, char * argv[]);

#if RICE__RUBY_VERSION_CODE >= 190
  void * node_;
#endif
};

}

#endif // VM__hpp
