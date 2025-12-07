#ifndef Rice__ModuleRegistry__hpp_
#define Rice__ModuleRegistry__hpp_

RICE_DETAIL_BEGIN_NAMESPACE
  class ModuleRegistry
  {
  public:
    void add(VALUE module);
    // API for access from Ruby
    VALUE modules();

  private:
    std::set<VALUE> modules_{};
  };
RICE_DETAIL_END_NAMESPACE

#endif // Rice__ModuleRegistry__hpp_
