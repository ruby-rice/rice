#include "rice/Data_Type.hpp"
#include "rice/Constructor.hpp"
#include "rice/String.hpp"

using namespace Rice;

namespace
{

  class CallbackHolder {
    public:

    void registerCallback(Rice::Object cb) {
      callback = cb;
    }

    Rice::Object fireCallback(Rice::String param) {
      return callback.call("call", param);
    }

    Rice::Object callback;
  };

} // namespace

extern "C"
void Init_sample_callbacks()
{
  RUBY_TRY
  {
    define_class<CallbackHolder>("CallbackHolder")
      .define_constructor(Constructor<CallbackHolder>())
      .define_method("register_callback", &CallbackHolder::registerCallback)
      .define_method("fire_callback", &CallbackHolder::fireCallback);
  }
  RUBY_CATCH
}

