#ifndef VM__hpp
#define VM__hpp

#include <vector>

namespace Rice
{

class VM
{
public:
  VM(char * app_name);
  VM(int argc, char * argv[]);
  VM(std::vector<char *> const & args);
  ~VM();

  void init_stack();
  void run();

private:
  void init(int argc, char * argv[]);
};

}

#endif // VM__hpp
