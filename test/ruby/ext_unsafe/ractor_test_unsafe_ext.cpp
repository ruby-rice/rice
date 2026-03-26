// Same extension as ractor_test_ext but compiled WITHOUT -DRICE_RACTOR_SAFE.
// Used to verify that concurrent Ractor access segfaults without the mutex.

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
void Init_ractor_test_unsafe_ext() {
  rb_ext_ractor_safe(true);

  Rice::define_class<Counter>("Counter")
    .define_constructor(Rice::Constructor<Counter, int>())
    .define_method("value", &Counter::value)
    .define_method("increment", &Counter::increment);

  Rice::define_class<HeavyObject>("HeavyObject")
    .define_constructor(Rice::Constructor<HeavyObject, int>())
    .define_method("sum", &HeavyObject::sum);
}
