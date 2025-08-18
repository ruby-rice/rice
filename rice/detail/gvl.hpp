
#ifndef Rice__detail__gvl__hpp_
#define Rice__detail__gvl__hpp_

#include <functional>
#include <optional>
#include <ruby/thread.h>

namespace Rice
{
  namespace detail
  {
    //! A callback and its return value.
    template <typename F, typename R = std::invoke_result_t<F>>
    struct CallbackReturn {
      F f;
      std::optional<R> ret;

      void call() {
        this->ret.emplace(this->f());
      }

      R return_value() && {
        return *std::move(this->ret);
      }
    };

    template <typename F>
    struct CallbackReturn<F, void> {
      F f;

      void call() {
        this->f();
      }

      void return_value() && {}
    };

    //! Call a function without holding the Global VM Lock.
    template <typename F>
    auto without_gvl(rb_unblock_function_t *ubf, void *ubf_data, int flags, F&& f)
    {
      CallbackReturn<F> cr{std::forward<F>(f)};

      auto wrapper = [](void* ptr) -> void* {
        static_cast<CallbackReturn<F>*>(ptr)->call();
        return NULL;
      };

      rb_nogvl(wrapper, &cr, ubf, ubf_data, flags);

      return std::move(cr).return_value();
    }

    //! Call a function without holding the Global VM Lock.
    template <typename F>
    auto without_gvl(F&& f)
    {
      return without_gvl(nullptr, nullptr, 0, std::forward<F>(f));
    }

    //! Reacquire the Global VM Lock while calling a function.
    template <typename F>
    auto with_gvl(F&& f)
    {
      CallbackReturn<F> cr{std::forward<F>(f)};

      auto wrapper = [](void* ptr) -> void* {
        static_cast<CallbackReturn<F>*>(ptr)->call();
        return NULL;
      };

      rb_thread_call_with_gvl(wrapper, &cr);

      return std::move(cr).return_value();
    }
  } // detail
} // Rice

#endif // Rice__detail__gvl__hpp_
