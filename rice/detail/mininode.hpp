#ifndef Rice__detail__mininode__hpp
#define Rice__detail__mininode__hpp

#include "ruby.hpp"

// TODO: This is silly, autoconf...
#undef PACKAGE_NAME
#undef PACKAGE_STRING
#undef PACKAGE_TARNAME
#undef PACKAGE_VERSION

#include "../config.hpp"

// TODO: This is silly, autoconf...
#undef PACKAGE_NAME
#undef PACKAGE_STRING
#undef PACKAGE_TARNAME
#undef PACKAGE_VERSION

#if !defined(HAVE_NODE_H) && !defined(REALLY_HAVE_RUBY_NODE_H)

namespace Rice
{

namespace detail
{

namespace Mininode
{

typedef struct RNode {
  unsigned long flags;
  void * reserved;
  union {
    struct RNode * node;
    VALUE (*cfunc)(ANYARGS);
  } u1;
  union {
    struct RNode * node;
    VALUE value;
  } u2;
  union {
    struct RNode * node;
  } u3;
} NODE;

#define nd_cfnc u1.cfunc
#define nd_rval u2.value

/* TODO: No way to know the correct size of node_type */
enum node_type {
  NODE_FOO,
};

extern "C"
void rb_add_method(VALUE, ID, NODE *, int);

extern "C"
NODE *rb_node_newnode(enum node_type, VALUE, VALUE, VALUE);

template<typename T, typename A0, typename A1, typename A2>
inline NODE * NEW_NODE(T t, A0 a0, A1 a1, A2 a2)
{
  return rb_node_newnode((node_type)t, (VALUE)a0, (VALUE)a1, (VALUE)a2);
}

#define NODE_METHOD Rice::detail::Mininode::get_NODE_METHOD()
#define NODE_FBODY Rice::detail::Mininode::get_NODE_FBODY()
#define NODE_CFUNC Rice::detail::Mininode::get_NODE_CFUNC()
#define NODE_SCOPE Rice::detail::Mininode::get_NODE_SCOPE()
#define NODE_BLOCK Rice::detail::Mininode::get_NODE_BLOCK()
#define NODE_IF Rice::detail::Mininode::get_NODE_IF()
#define NODE_CASE Rice::detail::Mininode::get_NODE_CASE()
#define NODE_WHEN Rice::detail::Mininode::get_NODE_WHEN()
#define NODE_OPT_N Rice::detail::Mininode::get_NODE_OPT_N()
#define NODE_WHILE Rice::detail::Mininode::get_NODE_WHILE()
#define NODE_UNTIL Rice::detail::Mininode::get_NODE_UNTIL()
#define NODE_ITER Rice::detail::Mininode::get_NODE_ITER()
#define NODE_FOR Rice::detail::Mininode::get_NODE_FOR()
#define NODE_BREAK Rice::detail::Mininode::get_NODE_BREAK()
#define NODE_NEXT Rice::detail::Mininode::get_NODE_NEXT()
#define NODE_REDO Rice::detail::Mininode::get_NODE_REDO()
#define NODE_RETRY Rice::detail::Mininode::get_NODE_RETRY()
#define NODE_BEGIN Rice::detail::Mininode::get_NODE_BEGIN()
#define NODE_RESCUE Rice::detail::Mininode::get_NODE_RESCUE()
#define NODE_RESBODY Rice::detail::Mininode::get_NODE_RESBODY()
#define NODE_ENSURE Rice::detail::Mininode::get_NODE_ENSURE()
#define NODE_AND Rice::detail::Mininode::get_NODE_AND()
#define NODE_OR Rice::detail::Mininode::get_NODE_OR()
#define NODE_MASGN Rice::detail::Mininode::get_NODE_MASGN()
#define NODE_LASGN Rice::detail::Mininode::get_NODE_LASGN()
#define NODE_DASGN Rice::detail::Mininode::get_NODE_DASGN()
#define NODE_DASGN_CURR Rice::detail::Mininode::get_NODE_DASGN_CURR()
#define NODE_GASGN Rice::detail::Mininode::get_NODE_GASGN()
#define NODE_IASGN Rice::detail::Mininode::get_NODE_IASGN()
#define NODE_IASGN2 Rice::detail::Mininode::get_NODE_IASGN2()
#define NODE_CDECL Rice::detail::Mininode::get_NODE_CDECL()
#define NODE_CVDECL Rice::detail::Mininode::get_NODE_CVDECL()
#define NODE_OP_ASGN1 Rice::detail::Mininode::get_NODE_OP_ASGN1()
#define NODE_OP_ASGN2 Rice::detail::Mininode::get_NODE_OP_ASGN2()
#define NODE_OP_ASGN_AND Rice::detail::Mininode::get_NODE_OP_ASGN_AND()
#define NODE_OP_ASGN_OR Rice::detail::Mininode::get_NODE_OP_ASGN_OR()
#define NODE_CALL Rice::detail::Mininode::get_NODE_CALL()
#define NODE_FCALL Rice::detail::Mininode::get_NODE_FCALL()
#define NODE_VCALL Rice::detail::Mininode::get_NODE_VCALL()
#define NODE_SUPER Rice::detail::Mininode::get_NODE_SUPER()
#define NODE_ZSUPER Rice::detail::Mininode::get_NODE_ZSUPER()
#define NODE_ARRAY Rice::detail::Mininode::get_NODE_ARRAY()
#define NODE_ZARRAY Rice::detail::Mininode::get_NODE_ZARRAY()
#define NODE_VALUES Rice::detail::Mininode::get_NODE_VALUES()
#define NODE_HASH Rice::detail::Mininode::get_NODE_HASH()
#define NODE_RETURN Rice::detail::Mininode::get_NODE_RETURN()
#define NODE_YIELD Rice::detail::Mininode::get_NODE_YIELD()
#define NODE_LVAR Rice::detail::Mininode::get_NODE_LVAR()
#define NODE_DVAR Rice::detail::Mininode::get_NODE_DVAR()
#define NODE_GVAR Rice::detail::Mininode::get_NODE_GVAR()
#define NODE_IVAR Rice::detail::Mininode::get_NODE_IVAR()
#define NODE_CONST Rice::detail::Mininode::get_NODE_CONST()
#define NODE_CVAR Rice::detail::Mininode::get_NODE_CVAR()
#define NODE_NTH_REF Rice::detail::Mininode::get_NODE_NTH_REF()
#define NODE_BACK_REF Rice::detail::Mininode::get_NODE_BACK_REF()
#define NODE_MATCH Rice::detail::Mininode::get_NODE_MATCH()
#define NODE_MATCH2 Rice::detail::Mininode::get_NODE_MATCH2()
#define NODE_MATCH3 Rice::detail::Mininode::get_NODE_MATCH3()
#define NODE_LIT Rice::detail::Mininode::get_NODE_LIT()
#define NODE_STR Rice::detail::Mininode::get_NODE_STR()
#define NODE_DSTR Rice::detail::Mininode::get_NODE_DSTR()
#define NODE_XSTR Rice::detail::Mininode::get_NODE_XSTR()
#define NODE_DXSTR Rice::detail::Mininode::get_NODE_DXSTR()
#define NODE_EVSTR Rice::detail::Mininode::get_NODE_EVSTR()
#define NODE_DREGX Rice::detail::Mininode::get_NODE_DREGX()
#define NODE_DREGX_ONCE Rice::detail::Mininode::get_NODE_DREGX_ONCE()
#define NODE_ARGS Rice::detail::Mininode::get_NODE_ARGS()
#define NODE_ARGS_AUX Rice::detail::Mininode::get_NODE_ARGS_AUX()
#define NODE_OPT_ARG Rice::detail::Mininode::get_NODE_OPT_ARG()
#define NODE_POSTARG Rice::detail::Mininode::get_NODE_POSTARG()
#define NODE_ARGSCAT Rice::detail::Mininode::get_NODE_ARGSCAT()
#define NODE_ARGSPUSH Rice::detail::Mininode::get_NODE_ARGSPUSH()
#define NODE_SPLAT Rice::detail::Mininode::get_NODE_SPLAT()
#define NODE_TO_ARY Rice::detail::Mininode::get_NODE_TO_ARY()
#define NODE_BLOCK_ARG Rice::detail::Mininode::get_NODE_BLOCK_ARG()
#define NODE_BLOCK_PASS Rice::detail::Mininode::get_NODE_BLOCK_PASS()
#define NODE_DEFN Rice::detail::Mininode::get_NODE_DEFN()
#define NODE_ALIAS Rice::detail::Mininode::get_NODE_ALIAS()
#define NODE_VALIAS Rice::detail::Mininode::get_NODE_VALIAS()
#define NODE_UNDEF Rice::detail::Mininode::get_NODE_UNDEF()
#define NODE_CLASS Rice::detail::Mininode::get_NODE_CLASS()
#define NODE_MODULE Rice::detail::Mininode::get_NODE_MODULE()
#define NODE_SCLASS Rice::detail::Mininode::get_NODE_SCLASS()
#define NODE_COLON2 Rice::detail::Mininode::get_NODE_COLON2()
#define NODE_COLON3 Rice::detail::Mininode::get_NODE_COLON3()
#define NODE_DOT2 Rice::detail::Mininode::get_NODE_DOT2()
#define NODE_DOT3 Rice::detail::Mininode::get_NODE_DOT3()
#define NODE_FLIP2 Rice::detail::Mininode::get_NODE_FLIP2()
#define NODE_FLIP3 Rice::detail::Mininode::get_NODE_FLIP3()
#define NODE_ATTRSET Rice::detail::Mininode::get_NODE_ATTRSET()
#define NODE_SELF Rice::detail::Mininode::get_NODE_SELF()
#define NODE_NIL Rice::detail::Mininode::get_NODE_NIL()
#define NODE_TRUE Rice::detail::Mininode::get_NODE_TRUE()
#define NODE_FALSE Rice::detail::Mininode::get_NODE_FALSE()
#define NODE_ERRINFO Rice::detail::Mininode::get_NODE_ERRINFO()
#define NODE_DEFINED Rice::detail::Mininode::get_NODE_DEFINED()
#define NODE_POSTEXE Rice::detail::Mininode::get_NODE_POSTEXE()
#define NODE_ALLOCA Rice::detail::Mininode::get_NODE_ALLOCA()
#define NODE_BMETHOD Rice::detail::Mininode::get_NODE_BMETHOD()
#define NODE_MEMO Rice::detail::Mininode::get_NODE_MEMO()
#define NODE_IFUNC Rice::detail::Mininode::get_NODE_IFUNC()
#define NODE_DSYM Rice::detail::Mininode::get_NODE_DSYM()
#define NODE_ATTRASGN Rice::detail::Mininode::get_NODE_ATTRASGN()
#define NODE_PRELUDE Rice::detail::Mininode::get_NODE_PRELUDE()
#define NODE_LAMBDA Rice::detail::Mininode::get_NODE_LAMBDA()
#define NODE_OPTBLOCK Rice::detail::Mininode::get_NODE_OPTBLOCK()
#define NODE_LAST Rice::detail::Mininode::get_NODE_LAST()

int get_NODE_METHOD();
int get_NODE_FBODY();
int get_NODE_CFUNC();
int get_NODE_SCOPE();
int get_NODE_BLOCK();
int get_NODE_IF();
int get_NODE_CASE();
int get_NODE_WHEN();
int get_NODE_OPT_N();
int get_NODE_WHILE();
int get_NODE_UNTIL();
int get_NODE_ITER();
int get_NODE_FOR();
int get_NODE_BREAK();
int get_NODE_NEXT();
int get_NODE_REDO();
int get_NODE_RETRY();
int get_NODE_BEGIN();
int get_NODE_RESCUE();
int get_NODE_RESBODY();
int get_NODE_ENSURE();
int get_NODE_AND();
int get_NODE_OR();
int get_NODE_MASGN();
int get_NODE_LASGN();
int get_NODE_DASGN();
int get_NODE_DASGN_CURR();
int get_NODE_GASGN();
int get_NODE_IASGN();
int get_NODE_IASGN2();
int get_NODE_CDECL();
int get_NODE_CVDECL();
int get_NODE_OP_ASGN1();
int get_NODE_OP_ASGN2();
int get_NODE_OP_ASGN_AND();
int get_NODE_OP_ASGN_OR();
int get_NODE_CALL();
int get_NODE_FCALL();
int get_NODE_VCALL();
int get_NODE_SUPER();
int get_NODE_ZSUPER();
int get_NODE_ARRAY();
int get_NODE_ZARRAY();
int get_NODE_VALUES();
int get_NODE_HASH();
int get_NODE_RETURN();
int get_NODE_YIELD();
int get_NODE_LVAR();
int get_NODE_DVAR();
int get_NODE_GVAR();
int get_NODE_IVAR();
int get_NODE_CONST();
int get_NODE_CVAR();
int get_NODE_NTH_REF();
int get_NODE_BACK_REF();
int get_NODE_MATCH();
int get_NODE_MATCH2();
int get_NODE_MATCH3();
int get_NODE_LIT();
int get_NODE_STR();
int get_NODE_DSTR();
int get_NODE_XSTR();
int get_NODE_DXSTR();
int get_NODE_EVSTR();
int get_NODE_DREGX();
int get_NODE_DREGX_ONCE();
int get_NODE_ARGS();
int get_NODE_ARGS_AUX();
int get_NODE_OPT_ARG();
int get_NODE_POSTARG();
int get_NODE_ARGSCAT();
int get_NODE_ARGSPUSH();
int get_NODE_SPLAT();
int get_NODE_TO_ARY();
int get_NODE_BLOCK_ARG();
int get_NODE_BLOCK_PASS();
int get_NODE_DEFN();
int get_NODE_ALIAS();
int get_NODE_VALIAS();
int get_NODE_UNDEF();
int get_NODE_CLASS();
int get_NODE_MODULE();
int get_NODE_SCLASS();
int get_NODE_COLON2();
int get_NODE_COLON3();
int get_NODE_DOT2();
int get_NODE_DOT3();
int get_NODE_FLIP2();
int get_NODE_FLIP3();
int get_NODE_ATTRSET();
int get_NODE_SELF();
int get_NODE_NIL();
int get_NODE_TRUE();
int get_NODE_FALSE();
int get_NODE_ERRINFO();
int get_NODE_DEFINED();
int get_NODE_POSTEXE();
int get_NODE_ALLOCA();
int get_NODE_BMETHOD();
int get_NODE_MEMO();
int get_NODE_IFUNC();
int get_NODE_DSYM();
int get_NODE_ATTRASGN();
int get_NODE_PRELUDE();
int get_NODE_LAMBDA();
int get_NODE_OPTBLOCK();
int get_NODE_LAST();

static const int NOEX_PUBLIC = 0x0;

inline NODE * NEW_METHOD(NODE * body, VALUE origin, int noex)
{
  return NEW_NODE(NODE_METHOD, origin, body, noex);
}

#ifdef RUBY_VM

inline NODE * NEW_FBODY(NODE * body, ID id)
{
  return NEW_NODE(NODE_FBODY, id, body, 0);
}

#else

inline NODE * NEW_FBODY(NODE * body, ID id, VALUE origin)
{
  return NEW_NODE(NODE_FBODY, body, id, origin);
}

#endif // RUBY_VM

inline NODE * NEW_CFUNC(RUBY_METHOD_FUNC cfunc, int arity)
{
  return NEW_NODE(NODE_CFUNC, cfunc, arity, 0);
}

} // namespace Mininode

} // namespace detail

} // namespace Rice

#endif

#endif // Rice__detail__mininode__hpp

