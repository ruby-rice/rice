#ifndef Rice__detail__cfp__hpp_
#define Rice__detail__cfp__hpp_

#include "ruby.hpp"

namespace Rice
{

namespace detail
{

VALUE * cfp();

VALUE & cfp_data_memo_node_and_pc(VALUE * cfp);
VALUE & cfp_self(VALUE * cfp);
VALUE & cfp_method_class(VALUE * cfp);

} // detail

} // Rice

#include "cfp.ipp"

#endif // Rice__detail__cfp__hpp_
