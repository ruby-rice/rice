// Minimal Rice extension for testing Ractor safety.
// Compiled with -DRICE_RACTOR_SAFE to enable mutex-protected InstanceRegistry.

#include <rice/rice.hpp>
#include <rice/stl.hpp>

#include <vector>

namespace {
  class Counter {
  public:
    Counter(int start) : value_(start) {}
    int value() const { return value_; }
    void increment() { value_++; }
  private:
    int value_;
  };

  // Object with an expensive constructor — spends significant time inside
  // Rice's InstanceRegistry add/lookup cycle, widening the window for
  // concurrent access from multiple Ractors.
  class HeavyObject {
  public:
    HeavyObject(int size) {
      data_.resize(size);
      for (int i = 0; i < size; i++) {
        data_[i] = i * i;
      }
    }

    int sum() const {
      int s = 0;
      for (auto v : data_) { s += v; }
      return s;
    }

  private:
    std::vector<int> data_;
  };
}

extern "C"
void Init_ractor_test_ext() {
  rb_ext_ractor_safe(true);

  Rice::define_class<Counter>("Counter")
    .define_constructor(Rice::Constructor<Counter, int>())
    .define_method("value", &Counter::value)
    .define_method("increment", &Counter::increment);

  Rice::define_class<HeavyObject>("HeavyObject")
    .define_constructor(Rice::Constructor<HeavyObject, int>())
    .define_method("sum", &HeavyObject::sum);
}
