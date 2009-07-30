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

#include "mininode.hpp"
#include <cstring>

extern "C"
char const * ruby_node_name(int node);

namespace
{

int node_value(char const * name)
{
  /* TODO: any way to end the block? */
  int j;
  for(j = 0; ; ++j)
  {
    if(!std::strcmp(name, ruby_node_name(j)))
    {
      return j;
    }
  }
}

} // namespace

static int NODE_METHOD_ = -1;

int Rice::detail::Mininode::get_NODE_METHOD()
{
  if(NODE_METHOD_ == -1)
  {
    NODE_METHOD_ = node_value("NODE_METHOD");
  }

  return NODE_METHOD_;
}
static int NODE_FBODY_ = -1;

int Rice::detail::Mininode::get_NODE_FBODY()
{
  if(NODE_FBODY_ == -1)
  {
    NODE_FBODY_ = node_value("NODE_FBODY");
  }

  return NODE_FBODY_;
}
static int NODE_CFUNC_ = -1;

int Rice::detail::Mininode::get_NODE_CFUNC()
{
  if(NODE_CFUNC_ == -1)
  {
    NODE_CFUNC_ = node_value("NODE_CFUNC");
  }

  return NODE_CFUNC_;
}
static int NODE_SCOPE_ = -1;

int Rice::detail::Mininode::get_NODE_SCOPE()
{
  if(NODE_SCOPE_ == -1)
  {
    NODE_SCOPE_ = node_value("NODE_SCOPE");
  }

  return NODE_SCOPE_;
}
static int NODE_BLOCK_ = -1;

int Rice::detail::Mininode::get_NODE_BLOCK()
{
  if(NODE_BLOCK_ == -1)
  {
    NODE_BLOCK_ = node_value("NODE_BLOCK");
  }

  return NODE_BLOCK_;
}
static int NODE_IF_ = -1;

int Rice::detail::Mininode::get_NODE_IF()
{
  if(NODE_IF_ == -1)
  {
    NODE_IF_ = node_value("NODE_IF");
  }

  return NODE_IF_;
}
static int NODE_CASE_ = -1;

int Rice::detail::Mininode::get_NODE_CASE()
{
  if(NODE_CASE_ == -1)
  {
    NODE_CASE_ = node_value("NODE_CASE");
  }

  return NODE_CASE_;
}
static int NODE_WHEN_ = -1;

int Rice::detail::Mininode::get_NODE_WHEN()
{
  if(NODE_WHEN_ == -1)
  {
    NODE_WHEN_ = node_value("NODE_WHEN");
  }

  return NODE_WHEN_;
}
static int NODE_OPT_N_ = -1;

int Rice::detail::Mininode::get_NODE_OPT_N()
{
  if(NODE_OPT_N_ == -1)
  {
    NODE_OPT_N_ = node_value("NODE_OPT_N");
  }

  return NODE_OPT_N_;
}
static int NODE_WHILE_ = -1;

int Rice::detail::Mininode::get_NODE_WHILE()
{
  if(NODE_WHILE_ == -1)
  {
    NODE_WHILE_ = node_value("NODE_WHILE");
  }

  return NODE_WHILE_;
}
static int NODE_UNTIL_ = -1;

int Rice::detail::Mininode::get_NODE_UNTIL()
{
  if(NODE_UNTIL_ == -1)
  {
    NODE_UNTIL_ = node_value("NODE_UNTIL");
  }

  return NODE_UNTIL_;
}
static int NODE_ITER_ = -1;

int Rice::detail::Mininode::get_NODE_ITER()
{
  if(NODE_ITER_ == -1)
  {
    NODE_ITER_ = node_value("NODE_ITER");
  }

  return NODE_ITER_;
}
static int NODE_FOR_ = -1;

int Rice::detail::Mininode::get_NODE_FOR()
{
  if(NODE_FOR_ == -1)
  {
    NODE_FOR_ = node_value("NODE_FOR");
  }

  return NODE_FOR_;
}
static int NODE_BREAK_ = -1;

int Rice::detail::Mininode::get_NODE_BREAK()
{
  if(NODE_BREAK_ == -1)
  {
    NODE_BREAK_ = node_value("NODE_BREAK");
  }

  return NODE_BREAK_;
}
static int NODE_NEXT_ = -1;

int Rice::detail::Mininode::get_NODE_NEXT()
{
  if(NODE_NEXT_ == -1)
  {
    NODE_NEXT_ = node_value("NODE_NEXT");
  }

  return NODE_NEXT_;
}
static int NODE_REDO_ = -1;

int Rice::detail::Mininode::get_NODE_REDO()
{
  if(NODE_REDO_ == -1)
  {
    NODE_REDO_ = node_value("NODE_REDO");
  }

  return NODE_REDO_;
}
static int NODE_RETRY_ = -1;

int Rice::detail::Mininode::get_NODE_RETRY()
{
  if(NODE_RETRY_ == -1)
  {
    NODE_RETRY_ = node_value("NODE_RETRY");
  }

  return NODE_RETRY_;
}
static int NODE_BEGIN_ = -1;

int Rice::detail::Mininode::get_NODE_BEGIN()
{
  if(NODE_BEGIN_ == -1)
  {
    NODE_BEGIN_ = node_value("NODE_BEGIN");
  }

  return NODE_BEGIN_;
}
static int NODE_RESCUE_ = -1;

int Rice::detail::Mininode::get_NODE_RESCUE()
{
  if(NODE_RESCUE_ == -1)
  {
    NODE_RESCUE_ = node_value("NODE_RESCUE");
  }

  return NODE_RESCUE_;
}
static int NODE_RESBODY_ = -1;

int Rice::detail::Mininode::get_NODE_RESBODY()
{
  if(NODE_RESBODY_ == -1)
  {
    NODE_RESBODY_ = node_value("NODE_RESBODY");
  }

  return NODE_RESBODY_;
}
static int NODE_ENSURE_ = -1;

int Rice::detail::Mininode::get_NODE_ENSURE()
{
  if(NODE_ENSURE_ == -1)
  {
    NODE_ENSURE_ = node_value("NODE_ENSURE");
  }

  return NODE_ENSURE_;
}
static int NODE_AND_ = -1;

int Rice::detail::Mininode::get_NODE_AND()
{
  if(NODE_AND_ == -1)
  {
    NODE_AND_ = node_value("NODE_AND");
  }

  return NODE_AND_;
}
static int NODE_OR_ = -1;

int Rice::detail::Mininode::get_NODE_OR()
{
  if(NODE_OR_ == -1)
  {
    NODE_OR_ = node_value("NODE_OR");
  }

  return NODE_OR_;
}
static int NODE_MASGN_ = -1;

int Rice::detail::Mininode::get_NODE_MASGN()
{
  if(NODE_MASGN_ == -1)
  {
    NODE_MASGN_ = node_value("NODE_MASGN");
  }

  return NODE_MASGN_;
}
static int NODE_LASGN_ = -1;

int Rice::detail::Mininode::get_NODE_LASGN()
{
  if(NODE_LASGN_ == -1)
  {
    NODE_LASGN_ = node_value("NODE_LASGN");
  }

  return NODE_LASGN_;
}
static int NODE_DASGN_ = -1;

int Rice::detail::Mininode::get_NODE_DASGN()
{
  if(NODE_DASGN_ == -1)
  {
    NODE_DASGN_ = node_value("NODE_DASGN");
  }

  return NODE_DASGN_;
}
static int NODE_DASGN_CURR_ = -1;

int Rice::detail::Mininode::get_NODE_DASGN_CURR()
{
  if(NODE_DASGN_CURR_ == -1)
  {
    NODE_DASGN_CURR_ = node_value("NODE_DASGN_CURR");
  }

  return NODE_DASGN_CURR_;
}
static int NODE_GASGN_ = -1;

int Rice::detail::Mininode::get_NODE_GASGN()
{
  if(NODE_GASGN_ == -1)
  {
    NODE_GASGN_ = node_value("NODE_GASGN");
  }

  return NODE_GASGN_;
}
static int NODE_IASGN_ = -1;

int Rice::detail::Mininode::get_NODE_IASGN()
{
  if(NODE_IASGN_ == -1)
  {
    NODE_IASGN_ = node_value("NODE_IASGN");
  }

  return NODE_IASGN_;
}
static int NODE_IASGN2_ = -1;

int Rice::detail::Mininode::get_NODE_IASGN2()
{
  if(NODE_IASGN2_ == -1)
  {
    NODE_IASGN2_ = node_value("NODE_IASGN2");
  }

  return NODE_IASGN2_;
}
static int NODE_CDECL_ = -1;

int Rice::detail::Mininode::get_NODE_CDECL()
{
  if(NODE_CDECL_ == -1)
  {
    NODE_CDECL_ = node_value("NODE_CDECL");
  }

  return NODE_CDECL_;
}
static int NODE_CVDECL_ = -1;

int Rice::detail::Mininode::get_NODE_CVDECL()
{
  if(NODE_CVDECL_ == -1)
  {
    NODE_CVDECL_ = node_value("NODE_CVDECL");
  }

  return NODE_CVDECL_;
}
static int NODE_OP_ASGN1_ = -1;

int Rice::detail::Mininode::get_NODE_OP_ASGN1()
{
  if(NODE_OP_ASGN1_ == -1)
  {
    NODE_OP_ASGN1_ = node_value("NODE_OP_ASGN1");
  }

  return NODE_OP_ASGN1_;
}
static int NODE_OP_ASGN2_ = -1;

int Rice::detail::Mininode::get_NODE_OP_ASGN2()
{
  if(NODE_OP_ASGN2_ == -1)
  {
    NODE_OP_ASGN2_ = node_value("NODE_OP_ASGN2");
  }

  return NODE_OP_ASGN2_;
}
static int NODE_OP_ASGN_AND_ = -1;

int Rice::detail::Mininode::get_NODE_OP_ASGN_AND()
{
  if(NODE_OP_ASGN_AND_ == -1)
  {
    NODE_OP_ASGN_AND_ = node_value("NODE_OP_ASGN_AND");
  }

  return NODE_OP_ASGN_AND_;
}
static int NODE_OP_ASGN_OR_ = -1;

int Rice::detail::Mininode::get_NODE_OP_ASGN_OR()
{
  if(NODE_OP_ASGN_OR_ == -1)
  {
    NODE_OP_ASGN_OR_ = node_value("NODE_OP_ASGN_OR");
  }

  return NODE_OP_ASGN_OR_;
}
static int NODE_CALL_ = -1;

int Rice::detail::Mininode::get_NODE_CALL()
{
  if(NODE_CALL_ == -1)
  {
    NODE_CALL_ = node_value("NODE_CALL");
  }

  return NODE_CALL_;
}
static int NODE_FCALL_ = -1;

int Rice::detail::Mininode::get_NODE_FCALL()
{
  if(NODE_FCALL_ == -1)
  {
    NODE_FCALL_ = node_value("NODE_FCALL");
  }

  return NODE_FCALL_;
}
static int NODE_VCALL_ = -1;

int Rice::detail::Mininode::get_NODE_VCALL()
{
  if(NODE_VCALL_ == -1)
  {
    NODE_VCALL_ = node_value("NODE_VCALL");
  }

  return NODE_VCALL_;
}
static int NODE_SUPER_ = -1;

int Rice::detail::Mininode::get_NODE_SUPER()
{
  if(NODE_SUPER_ == -1)
  {
    NODE_SUPER_ = node_value("NODE_SUPER");
  }

  return NODE_SUPER_;
}
static int NODE_ZSUPER_ = -1;

int Rice::detail::Mininode::get_NODE_ZSUPER()
{
  if(NODE_ZSUPER_ == -1)
  {
    NODE_ZSUPER_ = node_value("NODE_ZSUPER");
  }

  return NODE_ZSUPER_;
}
static int NODE_ARRAY_ = -1;

int Rice::detail::Mininode::get_NODE_ARRAY()
{
  if(NODE_ARRAY_ == -1)
  {
    NODE_ARRAY_ = node_value("NODE_ARRAY");
  }

  return NODE_ARRAY_;
}
static int NODE_ZARRAY_ = -1;

int Rice::detail::Mininode::get_NODE_ZARRAY()
{
  if(NODE_ZARRAY_ == -1)
  {
    NODE_ZARRAY_ = node_value("NODE_ZARRAY");
  }

  return NODE_ZARRAY_;
}
static int NODE_VALUES_ = -1;

int Rice::detail::Mininode::get_NODE_VALUES()
{
  if(NODE_VALUES_ == -1)
  {
    NODE_VALUES_ = node_value("NODE_VALUES");
  }

  return NODE_VALUES_;
}
static int NODE_HASH_ = -1;

int Rice::detail::Mininode::get_NODE_HASH()
{
  if(NODE_HASH_ == -1)
  {
    NODE_HASH_ = node_value("NODE_HASH");
  }

  return NODE_HASH_;
}
static int NODE_RETURN_ = -1;

int Rice::detail::Mininode::get_NODE_RETURN()
{
  if(NODE_RETURN_ == -1)
  {
    NODE_RETURN_ = node_value("NODE_RETURN");
  }

  return NODE_RETURN_;
}
static int NODE_YIELD_ = -1;

int Rice::detail::Mininode::get_NODE_YIELD()
{
  if(NODE_YIELD_ == -1)
  {
    NODE_YIELD_ = node_value("NODE_YIELD");
  }

  return NODE_YIELD_;
}
static int NODE_LVAR_ = -1;

int Rice::detail::Mininode::get_NODE_LVAR()
{
  if(NODE_LVAR_ == -1)
  {
    NODE_LVAR_ = node_value("NODE_LVAR");
  }

  return NODE_LVAR_;
}
static int NODE_DVAR_ = -1;

int Rice::detail::Mininode::get_NODE_DVAR()
{
  if(NODE_DVAR_ == -1)
  {
    NODE_DVAR_ = node_value("NODE_DVAR");
  }

  return NODE_DVAR_;
}
static int NODE_GVAR_ = -1;

int Rice::detail::Mininode::get_NODE_GVAR()
{
  if(NODE_GVAR_ == -1)
  {
    NODE_GVAR_ = node_value("NODE_GVAR");
  }

  return NODE_GVAR_;
}
static int NODE_IVAR_ = -1;

int Rice::detail::Mininode::get_NODE_IVAR()
{
  if(NODE_IVAR_ == -1)
  {
    NODE_IVAR_ = node_value("NODE_IVAR");
  }

  return NODE_IVAR_;
}
static int NODE_CONST_ = -1;

int Rice::detail::Mininode::get_NODE_CONST()
{
  if(NODE_CONST_ == -1)
  {
    NODE_CONST_ = node_value("NODE_CONST");
  }

  return NODE_CONST_;
}
static int NODE_CVAR_ = -1;

int Rice::detail::Mininode::get_NODE_CVAR()
{
  if(NODE_CVAR_ == -1)
  {
    NODE_CVAR_ = node_value("NODE_CVAR");
  }

  return NODE_CVAR_;
}
static int NODE_NTH_REF_ = -1;

int Rice::detail::Mininode::get_NODE_NTH_REF()
{
  if(NODE_NTH_REF_ == -1)
  {
    NODE_NTH_REF_ = node_value("NODE_NTH_REF");
  }

  return NODE_NTH_REF_;
}
static int NODE_BACK_REF_ = -1;

int Rice::detail::Mininode::get_NODE_BACK_REF()
{
  if(NODE_BACK_REF_ == -1)
  {
    NODE_BACK_REF_ = node_value("NODE_BACK_REF");
  }

  return NODE_BACK_REF_;
}
static int NODE_MATCH_ = -1;

int Rice::detail::Mininode::get_NODE_MATCH()
{
  if(NODE_MATCH_ == -1)
  {
    NODE_MATCH_ = node_value("NODE_MATCH");
  }

  return NODE_MATCH_;
}
static int NODE_MATCH2_ = -1;

int Rice::detail::Mininode::get_NODE_MATCH2()
{
  if(NODE_MATCH2_ == -1)
  {
    NODE_MATCH2_ = node_value("NODE_MATCH2");
  }

  return NODE_MATCH2_;
}
static int NODE_MATCH3_ = -1;

int Rice::detail::Mininode::get_NODE_MATCH3()
{
  if(NODE_MATCH3_ == -1)
  {
    NODE_MATCH3_ = node_value("NODE_MATCH3");
  }

  return NODE_MATCH3_;
}
static int NODE_LIT_ = -1;

int Rice::detail::Mininode::get_NODE_LIT()
{
  if(NODE_LIT_ == -1)
  {
    NODE_LIT_ = node_value("NODE_LIT");
  }

  return NODE_LIT_;
}
static int NODE_STR_ = -1;

int Rice::detail::Mininode::get_NODE_STR()
{
  if(NODE_STR_ == -1)
  {
    NODE_STR_ = node_value("NODE_STR");
  }

  return NODE_STR_;
}
static int NODE_DSTR_ = -1;

int Rice::detail::Mininode::get_NODE_DSTR()
{
  if(NODE_DSTR_ == -1)
  {
    NODE_DSTR_ = node_value("NODE_DSTR");
  }

  return NODE_DSTR_;
}
static int NODE_XSTR_ = -1;

int Rice::detail::Mininode::get_NODE_XSTR()
{
  if(NODE_XSTR_ == -1)
  {
    NODE_XSTR_ = node_value("NODE_XSTR");
  }

  return NODE_XSTR_;
}
static int NODE_DXSTR_ = -1;

int Rice::detail::Mininode::get_NODE_DXSTR()
{
  if(NODE_DXSTR_ == -1)
  {
    NODE_DXSTR_ = node_value("NODE_DXSTR");
  }

  return NODE_DXSTR_;
}
static int NODE_EVSTR_ = -1;

int Rice::detail::Mininode::get_NODE_EVSTR()
{
  if(NODE_EVSTR_ == -1)
  {
    NODE_EVSTR_ = node_value("NODE_EVSTR");
  }

  return NODE_EVSTR_;
}
static int NODE_DREGX_ = -1;

int Rice::detail::Mininode::get_NODE_DREGX()
{
  if(NODE_DREGX_ == -1)
  {
    NODE_DREGX_ = node_value("NODE_DREGX");
  }

  return NODE_DREGX_;
}
static int NODE_DREGX_ONCE_ = -1;

int Rice::detail::Mininode::get_NODE_DREGX_ONCE()
{
  if(NODE_DREGX_ONCE_ == -1)
  {
    NODE_DREGX_ONCE_ = node_value("NODE_DREGX_ONCE");
  }

  return NODE_DREGX_ONCE_;
}
static int NODE_ARGS_ = -1;

int Rice::detail::Mininode::get_NODE_ARGS()
{
  if(NODE_ARGS_ == -1)
  {
    NODE_ARGS_ = node_value("NODE_ARGS");
  }

  return NODE_ARGS_;
}
static int NODE_ARGS_AUX_ = -1;

int Rice::detail::Mininode::get_NODE_ARGS_AUX()
{
  if(NODE_ARGS_AUX_ == -1)
  {
    NODE_ARGS_AUX_ = node_value("NODE_ARGS_AUX");
  }

  return NODE_ARGS_AUX_;
}
static int NODE_OPT_ARG_ = -1;

int Rice::detail::Mininode::get_NODE_OPT_ARG()
{
  if(NODE_OPT_ARG_ == -1)
  {
    NODE_OPT_ARG_ = node_value("NODE_OPT_ARG");
  }

  return NODE_OPT_ARG_;
}
static int NODE_POSTARG_ = -1;

int Rice::detail::Mininode::get_NODE_POSTARG()
{
  if(NODE_POSTARG_ == -1)
  {
    NODE_POSTARG_ = node_value("NODE_POSTARG");
  }

  return NODE_POSTARG_;
}
static int NODE_ARGSCAT_ = -1;

int Rice::detail::Mininode::get_NODE_ARGSCAT()
{
  if(NODE_ARGSCAT_ == -1)
  {
    NODE_ARGSCAT_ = node_value("NODE_ARGSCAT");
  }

  return NODE_ARGSCAT_;
}
static int NODE_ARGSPUSH_ = -1;

int Rice::detail::Mininode::get_NODE_ARGSPUSH()
{
  if(NODE_ARGSPUSH_ == -1)
  {
    NODE_ARGSPUSH_ = node_value("NODE_ARGSPUSH");
  }

  return NODE_ARGSPUSH_;
}
static int NODE_SPLAT_ = -1;

int Rice::detail::Mininode::get_NODE_SPLAT()
{
  if(NODE_SPLAT_ == -1)
  {
    NODE_SPLAT_ = node_value("NODE_SPLAT");
  }

  return NODE_SPLAT_;
}
static int NODE_TO_ARY_ = -1;

int Rice::detail::Mininode::get_NODE_TO_ARY()
{
  if(NODE_TO_ARY_ == -1)
  {
    NODE_TO_ARY_ = node_value("NODE_TO_ARY");
  }

  return NODE_TO_ARY_;
}
static int NODE_BLOCK_ARG_ = -1;

int Rice::detail::Mininode::get_NODE_BLOCK_ARG()
{
  if(NODE_BLOCK_ARG_ == -1)
  {
    NODE_BLOCK_ARG_ = node_value("NODE_BLOCK_ARG");
  }

  return NODE_BLOCK_ARG_;
}
static int NODE_BLOCK_PASS_ = -1;

int Rice::detail::Mininode::get_NODE_BLOCK_PASS()
{
  if(NODE_BLOCK_PASS_ == -1)
  {
    NODE_BLOCK_PASS_ = node_value("NODE_BLOCK_PASS");
  }

  return NODE_BLOCK_PASS_;
}
static int NODE_DEFN_ = -1;

int Rice::detail::Mininode::get_NODE_DEFN()
{
  if(NODE_DEFN_ == -1)
  {
    NODE_DEFN_ = node_value("NODE_DEFN");
  }

  return NODE_DEFN_;
}
static int NODE_ALIAS_ = -1;

int Rice::detail::Mininode::get_NODE_ALIAS()
{
  if(NODE_ALIAS_ == -1)
  {
    NODE_ALIAS_ = node_value("NODE_ALIAS");
  }

  return NODE_ALIAS_;
}
static int NODE_VALIAS_ = -1;

int Rice::detail::Mininode::get_NODE_VALIAS()
{
  if(NODE_VALIAS_ == -1)
  {
    NODE_VALIAS_ = node_value("NODE_VALIAS");
  }

  return NODE_VALIAS_;
}
static int NODE_UNDEF_ = -1;

int Rice::detail::Mininode::get_NODE_UNDEF()
{
  if(NODE_UNDEF_ == -1)
  {
    NODE_UNDEF_ = node_value("NODE_UNDEF");
  }

  return NODE_UNDEF_;
}
static int NODE_CLASS_ = -1;

int Rice::detail::Mininode::get_NODE_CLASS()
{
  if(NODE_CLASS_ == -1)
  {
    NODE_CLASS_ = node_value("NODE_CLASS");
  }

  return NODE_CLASS_;
}
static int NODE_MODULE_ = -1;

int Rice::detail::Mininode::get_NODE_MODULE()
{
  if(NODE_MODULE_ == -1)
  {
    NODE_MODULE_ = node_value("NODE_MODULE");
  }

  return NODE_MODULE_;
}
static int NODE_SCLASS_ = -1;

int Rice::detail::Mininode::get_NODE_SCLASS()
{
  if(NODE_SCLASS_ == -1)
  {
    NODE_SCLASS_ = node_value("NODE_SCLASS");
  }

  return NODE_SCLASS_;
}
static int NODE_COLON2_ = -1;

int Rice::detail::Mininode::get_NODE_COLON2()
{
  if(NODE_COLON2_ == -1)
  {
    NODE_COLON2_ = node_value("NODE_COLON2");
  }

  return NODE_COLON2_;
}
static int NODE_COLON3_ = -1;

int Rice::detail::Mininode::get_NODE_COLON3()
{
  if(NODE_COLON3_ == -1)
  {
    NODE_COLON3_ = node_value("NODE_COLON3");
  }

  return NODE_COLON3_;
}
static int NODE_DOT2_ = -1;

int Rice::detail::Mininode::get_NODE_DOT2()
{
  if(NODE_DOT2_ == -1)
  {
    NODE_DOT2_ = node_value("NODE_DOT2");
  }

  return NODE_DOT2_;
}
static int NODE_DOT3_ = -1;

int Rice::detail::Mininode::get_NODE_DOT3()
{
  if(NODE_DOT3_ == -1)
  {
    NODE_DOT3_ = node_value("NODE_DOT3");
  }

  return NODE_DOT3_;
}
static int NODE_FLIP2_ = -1;

int Rice::detail::Mininode::get_NODE_FLIP2()
{
  if(NODE_FLIP2_ == -1)
  {
    NODE_FLIP2_ = node_value("NODE_FLIP2");
  }

  return NODE_FLIP2_;
}
static int NODE_FLIP3_ = -1;

int Rice::detail::Mininode::get_NODE_FLIP3()
{
  if(NODE_FLIP3_ == -1)
  {
    NODE_FLIP3_ = node_value("NODE_FLIP3");
  }

  return NODE_FLIP3_;
}
static int NODE_ATTRSET_ = -1;

int Rice::detail::Mininode::get_NODE_ATTRSET()
{
  if(NODE_ATTRSET_ == -1)
  {
    NODE_ATTRSET_ = node_value("NODE_ATTRSET");
  }

  return NODE_ATTRSET_;
}
static int NODE_SELF_ = -1;

int Rice::detail::Mininode::get_NODE_SELF()
{
  if(NODE_SELF_ == -1)
  {
    NODE_SELF_ = node_value("NODE_SELF");
  }

  return NODE_SELF_;
}
static int NODE_NIL_ = -1;

int Rice::detail::Mininode::get_NODE_NIL()
{
  if(NODE_NIL_ == -1)
  {
    NODE_NIL_ = node_value("NODE_NIL");
  }

  return NODE_NIL_;
}
static int NODE_TRUE_ = -1;

int Rice::detail::Mininode::get_NODE_TRUE()
{
  if(NODE_TRUE_ == -1)
  {
    NODE_TRUE_ = node_value("NODE_TRUE");
  }

  return NODE_TRUE_;
}
static int NODE_FALSE_ = -1;

int Rice::detail::Mininode::get_NODE_FALSE()
{
  if(NODE_FALSE_ == -1)
  {
    NODE_FALSE_ = node_value("NODE_FALSE");
  }

  return NODE_FALSE_;
}
static int NODE_ERRINFO_ = -1;

int Rice::detail::Mininode::get_NODE_ERRINFO()
{
  if(NODE_ERRINFO_ == -1)
  {
    NODE_ERRINFO_ = node_value("NODE_ERRINFO");
  }

  return NODE_ERRINFO_;
}
static int NODE_DEFINED_ = -1;

int Rice::detail::Mininode::get_NODE_DEFINED()
{
  if(NODE_DEFINED_ == -1)
  {
    NODE_DEFINED_ = node_value("NODE_DEFINED");
  }

  return NODE_DEFINED_;
}
static int NODE_POSTEXE_ = -1;

int Rice::detail::Mininode::get_NODE_POSTEXE()
{
  if(NODE_POSTEXE_ == -1)
  {
    NODE_POSTEXE_ = node_value("NODE_POSTEXE");
  }

  return NODE_POSTEXE_;
}
static int NODE_ALLOCA_ = -1;

int Rice::detail::Mininode::get_NODE_ALLOCA()
{
  if(NODE_ALLOCA_ == -1)
  {
    NODE_ALLOCA_ = node_value("NODE_ALLOCA");
  }

  return NODE_ALLOCA_;
}
static int NODE_BMETHOD_ = -1;

int Rice::detail::Mininode::get_NODE_BMETHOD()
{
  if(NODE_BMETHOD_ == -1)
  {
    NODE_BMETHOD_ = node_value("NODE_BMETHOD");
  }

  return NODE_BMETHOD_;
}
static int NODE_MEMO_ = -1;

int Rice::detail::Mininode::get_NODE_MEMO()
{
  if(NODE_MEMO_ == -1)
  {
    NODE_MEMO_ = node_value("NODE_MEMO");
  }

  return NODE_MEMO_;
}
static int NODE_IFUNC_ = -1;

int Rice::detail::Mininode::get_NODE_IFUNC()
{
  if(NODE_IFUNC_ == -1)
  {
    NODE_IFUNC_ = node_value("NODE_IFUNC");
  }

  return NODE_IFUNC_;
}
static int NODE_DSYM_ = -1;

int Rice::detail::Mininode::get_NODE_DSYM()
{
  if(NODE_DSYM_ == -1)
  {
    NODE_DSYM_ = node_value("NODE_DSYM");
  }

  return NODE_DSYM_;
}
static int NODE_ATTRASGN_ = -1;

int Rice::detail::Mininode::get_NODE_ATTRASGN()
{
  if(NODE_ATTRASGN_ == -1)
  {
    NODE_ATTRASGN_ = node_value("NODE_ATTRASGN");
  }

  return NODE_ATTRASGN_;
}
static int NODE_PRELUDE_ = -1;

int Rice::detail::Mininode::get_NODE_PRELUDE()
{
  if(NODE_PRELUDE_ == -1)
  {
    NODE_PRELUDE_ = node_value("NODE_PRELUDE");
  }

  return NODE_PRELUDE_;
}
static int NODE_LAMBDA_ = -1;

int Rice::detail::Mininode::get_NODE_LAMBDA()
{
  if(NODE_LAMBDA_ == -1)
  {
    NODE_LAMBDA_ = node_value("NODE_LAMBDA");
  }

  return NODE_LAMBDA_;
}
static int NODE_OPTBLOCK_ = -1;

int Rice::detail::Mininode::get_NODE_OPTBLOCK()
{
  if(NODE_OPTBLOCK_ == -1)
  {
    NODE_OPTBLOCK_ = node_value("NODE_OPTBLOCK");
  }

  return NODE_OPTBLOCK_;
}
static int NODE_LAST_ = -1;

int Rice::detail::Mininode::get_NODE_LAST()
{
  if(NODE_LAST_ == -1)
  {
    NODE_LAST_ = node_value("NODE_LAST");
  }

  return NODE_LAST_;
}

#endif

