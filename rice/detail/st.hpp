#ifndef Exc_Ruby___cpp__st__hpp_
#define Exc_Ruby___cpp__st__hpp_

/*! \file
 * \brief Hacks to allow functions in st.h to be called from C++
 * programs.
 */

extern "C"
{
#include "st.h"
}

namespace Exc_Ruby
{

namespace detail
{

extern "C" typedef int (*St_Insert_Signature)(
    register st_table *table,
    register char *key,
    char *value);
extern "C" typedef int (*St_Lookup_Signature)(
    st_table *table,
    register char *key,
    char **value); 
extern "C" typedef st_table* (*St_Init_Table_Signature)(
    struct st_hash_type *type);

#define st_insert(table, key, value) \
  ((::Exc_Ruby::detail::St_Insert_Signature)(st_insert))(table, key, value)
#define st_lookup(table, key, value) \
  ((::Exc_Ruby::detail::St_Lookup_Signature)(st_lookup))(table, key, value)
#define st_init_table(type) \
  ((::Exc_Ruby::detail::St_Init_Table_Signature)(st_init_table))(type)

} // namespace detail

} // namespace Exc_Ruby

#endif // Exc_Ruby___cpp__st__hpp_

