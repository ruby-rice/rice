#ifndef Rice__detail__cpp_protect__hpp_
#define Rice__detail__cpp_protect__hpp_

#include <regex>
#include <stdexcept>

#if __has_include(<filesystem>)
  #include <filesystem>
  namespace fs = std::filesystem;
#elif __has_include(<experimental/filesystem>)
  #include <experimental/filesystem>
  namespace fs = std::experimental::filesystem;
#else
  #error "no filesystem include found :'("
#endif


namespace Rice::detail
{
  template <typename Callable_T>
  auto cpp_protect(Callable_T&& func)
  {
    try
    {
      return func();
    }
    catch (...)
    {
      try
      {
        std::function<void()> handler = detail::Registries::instance.handlers.handler();
        handler();
      }
      catch (::Rice::Exception const& ex)
      {
        rb_exc_raise(ex.value());
      }
      catch (::Rice::JumpException const& ex)
      {
        rb_jump_tag(ex.tag);
      }
      catch (std::bad_alloc const& ex)
      {
        /* This won't work quite right if the rb_exc_new2 fails; not
           much we can do about that, since Ruby doesn't give us access
           to a pre-allocated NoMemoryError object */
        rb_exc_raise(rb_exc_new2(rb_eNoMemError, ex.what()));
      }
      catch (std::domain_error const& ex)
      {
        rb_exc_raise(rb_exc_new2(rb_eFloatDomainError, ex.what()));
      }
      catch (std::invalid_argument const& ex)
      {
        rb_exc_raise(rb_exc_new2(rb_eArgError, ex.what()));
      }
      catch (fs::filesystem_error const& ex)
      {
        rb_exc_raise(rb_exc_new2(rb_eIOError, ex.what()));
      }
      catch (std::length_error const& ex)
      {
        rb_exc_raise(rb_exc_new2(rb_eIndexError, ex.what()));
      }
      catch (std::out_of_range const& ex)
      {
        rb_exc_raise(rb_exc_new2(rb_eIndexError, ex.what()));
      }
      catch (std::overflow_error const& ex)
      {
        rb_exc_raise(rb_exc_new2(rb_eRangeError, ex.what()));
      }
      catch (std::range_error const& ex)
      {
        rb_exc_raise(rb_exc_new2(rb_eRangeError, ex.what()));
      }
      catch (std::regex_error const& ex)
      {
        rb_exc_raise(rb_exc_new2(rb_eRegexpError, ex.what()));
      }
      catch (std::system_error const& ex)
      {
        rb_exc_raise(rb_exc_new2(rb_eSystemCallError, ex.what()));
      }
      catch (std::underflow_error const& ex)
      {
        rb_exc_raise(rb_exc_new2(rb_eRangeError, ex.what()));
      }
      catch (std::exception const& ex)
      {
        rb_exc_raise(rb_exc_new2(rb_eRuntimeError, ex.what()));
      }
      catch (...)
      {
        rb_exc_raise(rb_exc_new2(rb_eRuntimeError, "Unknown C++ exception thrown"));
      }
      throw std::runtime_error("Should never get here - just making compilers happy");
    }
  }
}
#endif // Rice__detail__cpp_protect__hpp_
