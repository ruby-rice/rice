#include "rice/Data_Type.hpp"
#include "rice/Constructor.hpp"

#include <map>

using namespace Rice;

namespace
{

class Map
{
private:
  struct Ruby_Value_Compare
  {
    bool operator()(Object lhs, Object rhs) const
    {
      Object result = lhs.call("<", rhs);
      return result.test();
    }
  };

  typedef std::map<Object, Object, Ruby_Value_Compare> Value_Map;

public:
  Object bracket(Object k)
  {
    Value_Map::iterator it = map_.find(k);
    return it == map_.end() ? Object(Qnil) : it->second;
  }

  Object bracket_equals(Object k, Object v)
  {
    map_[k] = v;
    return Qnil;
  }

  Value_Map::iterator begin() { return map_.begin(); }
  Value_Map::iterator end() { return map_.end(); }

  typedef Value_Map::value_type value_type;
  typedef Value_Map::iterator iterator;

private:
  Value_Map map_;
};

Data_Type<Map> rb_cMap;

} // namespace

template<>
Object to_ruby<Map::value_type>(Map::value_type const & p)
{
  return protect(rb_assoc_new, p.first, p.second);
}

#include <iostream>
extern "C"
void Init_map(void)
{
  RUBY_TRY
  {
    Map::iterator (Map::*begin)() = &Map::begin;
    Map::iterator (Map::*end)() = &Map::end;
    Rice::Module rb_mStd = define_module("Std");

    // TODO: no delete method on the map, because I'm not sure how to
    // make delete work properly while iterating
    rb_cMap =
      define_class_under<Map>(rb_mStd, "Map")
      .define_constructor(Constructor<Map>())
      .define_method("[]", &Map::bracket)
      .define_method("[]=", &Map::bracket_equals)
      .define_iterator(begin, end)
      .include_module(rb_mEnumerable)
      ;
  }
  RUBY_CATCH
}

