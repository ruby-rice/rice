#ifndef Rice__configmacros__hpp_
#define Rice__configmacros__hpp_

#define STR_EXPAND(tok) #tok
#define STR(tok) STR_EXPAND(tok)

#ifndef RICE_NAMESPACE
  #define RICE_NAMESPACE Rice
  #define RICE_DEFINE_MODULE_RICE define_module("Rice");
  #define RICE_DEFINE_MODULE_RICE_LIBC define_module("Libc");
  #define RICE_DEFINE_MODULE_RICE_STL define_module("Std");
#else
  #define RICE_DEFINE_MODULE_RICE define_module( STR(RICE_NAMESPACE) );
  #define RICE_DEFINE_MODULE_RICE_LIBC define_module_under(define_module( STR(RICE_NAMESPACE) ), "Libc");
  #define RICE_DEFINE_MODULE_RICE_STL define_module_under(define_module( STR(RICE_NAMESPACE) ), "Std");
#endif

#define RICE_PREPEND_NAMESPACE(name) ::RICE_NAMESPACE::name
#define RICE_USE_NAMESPACE using namespace ::RICE_NAMESPACE;
#define RICE_BEGIN_NAMESPACE namespace RICE_NAMESPACE {
#define RICE_END_NAMESPACE }

#define RICE_DETAIL_NAMESPACE RICE_NAMESPACE::detail
// #define RICE_DETAIL_USE_NAMESPACE using namespace ::RICE_DETAIL_NAMESPACE;
#define RICE_DETAIL_BEGIN_NAMESPACE namespace RICE_DETAIL_NAMESPACE {
#define RICE_DETAIL_END_NAMESPACE }

#define RICE_STL_NAMESPACE RICE_NAMESPACE::stl
// #define RICE_STL_USE_NAMESPACE using namespace ::RICE_STL_NAMESPACE;
#define RICE_STL_BEGIN_NAMESPACE namespace RICE_STL_NAMESPACE {
#define RICE_STL_END_NAMESPACE }

#endif  // Rice__configmacros__hpp_
