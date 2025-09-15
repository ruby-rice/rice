#ifndef Rice__ModuleRegistry__hpp_
#define Rice__ModuleRegistry__hpp_

namespace Rice::detail
{
  class ModuleRegistry
  {
  public:
    void add(VALUE module);
    // API for access from Ruby
    VALUE modules();

  private:
    std::set<VALUE> modules_{};
  };
}

#endif // Rice__ModuleRegistry__hpp_
