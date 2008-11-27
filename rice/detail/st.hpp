#ifndef Exc_Ruby___cpp__st__hpp_
#define Exc_Ruby___cpp__st__hpp_

/*! \file
 * \brief Hacks to allow functions in st.h to be called from C++
 * programs.
 */

#include "ruby.hpp"

// Ruby doesn't put extern "C" around st.h

extern "C"
{
#ifdef RUBY_VM
#include "ruby/st.h"
#else
#include "st.h"
#endif
}

// Older versions of Ruby don't have proper signatures for the st_
// functions

#if RICE__RUBY_VERSION_CODE < 180

typedef char * st_data_t;

namespace Exc_Ruby
{

namespace detail
{

extern "C" typedef int (*St_Insert_Signature)(
    st_table * table,
    st_data_t key,
    st_data_t * value);
extern "C" typedef int (*St_Lookup_Signature)(
    st_table * table,
    st_data_t key,
    st_data_t * value); 
extern "C" typedef st_table* (*St_Init_Table_Signature)(
    struct st_hash_type * type);

#define st_insert(table, key, value) \
  ((::Exc_Ruby::detail::St_Insert_Signature)(st_insert))(table, key, value)
#define st_lookup(table, key, value) \
  ((::Exc_Ruby::detail::St_Lookup_Signature)(st_lookup))(table, key, value)
#define st_init_table(type) \
  ((::Exc_Ruby::detail::St_Init_Table_Signature)(st_init_table))(type)

} // namespace detail

} // namespace Exc_Ruby

#endif // RICE__RUBY_VERSION_CODE < 180

#endif // Exc_Ruby___cpp__st__hpp_

