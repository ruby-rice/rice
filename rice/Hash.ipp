#ifndef Rice__Hash__ipp_
#define Rice__Hash__ipp_

#include "protect.hpp"
#include "Exception.hpp"
#include "Builtin_Object.hpp"
#include "detail/st.hpp"
#include <algorithm>

inline Rice::Hash::
Hash()
  : Builtin_Object<T_HASH>(protect(rb_hash_new))
{
}

inline Rice::Hash::
Hash(Object v)
  : Builtin_Object<T_HASH>(v)
{
}

inline size_t Rice::Hash::
size() const
{
  return RHASH_SIZE(this->value());
}

inline Rice::Hash::Proxy::
Proxy(Hash hash, Object key)
  : hash_(hash)
  , key_(key)
{
}

/*
inline Rice::Hash::Proxy::
operator VALUE() const
{
  return value();
}
*/

inline Rice::Hash::Proxy::
operator Rice::Object() const
{
  return value();
}

inline VALUE Rice::Hash::Proxy::
value() const
{
  return protect(rb_hash_aref, hash_, key_);
}

template<typename T>
inline Rice::Object Rice::Hash::Proxy::
operator=(T const & value)
{
  return protect(rb_hash_aset, hash_, key_, detail::To_Ruby<T>::convert(value));
}

inline void Rice::Hash::Proxy::
swap(Proxy & proxy)
{
  hash_.swap(proxy.hash_);
  key_.swap(proxy.key_);
}

template<typename Key_T>
inline Rice::Hash::Proxy const Rice::Hash::
operator[](Key_T const & key) const
{
  return Proxy(*this, detail::To_Ruby<Key_T>::convert(key));
}

template<typename Key_T>
inline Rice::Hash::Proxy Rice::Hash::
operator[](Key_T const & key)
{
  return Proxy(*this, detail::To_Ruby<Key_T>::convert(key));
}

template<typename Value_T, typename Key_T>
inline Value_T Rice::Hash::
get(Key_T const & key)
{
  Object ruby_key(detail::To_Ruby<Key_T>::convert(key));
  Object value = operator[](ruby_key);
  try
  {
    return Rice::detail::From_Ruby<Value_T>::convert(value);
  }
  catch(Exception const & ex)
  {
    String s_key(ruby_key.to_s());
    throw Exception(
        ex,
        "%s while converting value for key %s",
        ex.what(),
        s_key.c_str());
  }
}

inline Rice::Hash::Entry::
Entry(Hash hash, Object key)
  : key(key)
  , first(Hash::Entry::key)
  , value(hash, key)
  , second(Hash::Entry::value)
{
}

inline Rice::Hash::Entry::
Entry(Entry const & entry)
  : key(entry.key)
  , first(Hash::Entry::key)
  , value(entry.value)
  , second(Hash::Entry::value)
{
}

inline Rice::Hash::Entry & Rice::Hash::Entry::
operator=(Rice::Hash::Entry const & rhs)
{
  Entry tmp(rhs);
  swap(tmp);
  return *this;
}

inline void Rice::Hash::Entry::
swap(Rice::Hash::Entry & entry)
{
  const_cast<Object &>(key).swap(const_cast<Object &>(entry.key));
  value.swap(entry.value);
}

template<typename Hash_Ref_T, typename Value_T>
inline Rice::Hash::Iterator<Hash_Ref_T, Value_T>::
Iterator(Hash_Ref_T hash)
  : hash_(hash)
  , current_index_(0)
  , keys_(Qnil)
  , tmp_(hash, Qnil)
{
}

template<typename Hash_Ref_T, typename Value_T>
inline Rice::Hash::Iterator<Hash_Ref_T, Value_T>::
Iterator(Hash_Ref_T hash, int start_at)
  : hash_(hash)
  , current_index_(start_at)
  , keys_(Qnil)
  , tmp_(hash, Qnil)
{
}

template<typename Hash_Ref_T, typename Value_T>
inline Rice::Hash::Iterator<Hash_Ref_T, Value_T>::
Iterator(Iterator const & iterator)
  : hash_(iterator.hash_.value())
  , current_index_(iterator.current_index_)
  , keys_(Qnil)
  , tmp_(iterator.hash_, Qnil)
{
}

template<typename Hash_Ref_T, typename Value_T>
template<typename Iterator_T>
inline Rice::Hash::Iterator<Hash_Ref_T, Value_T>::
Iterator(Iterator_T const & iterator)
  : hash_(iterator.hash_.value())
  , current_index_(iterator.current_index_)
  , keys_(Qnil)
  , tmp_(iterator.hash_, Qnil)
{
}

template<typename Hash_Ref_T, typename Value_T>
inline Rice::Hash::Iterator<Hash_Ref_T, Value_T> &
Rice::Hash::Iterator<Hash_Ref_T, Value_T>::
operator=(Iterator const & iterator)
{
  Iterator tmp(iterator);

  this->swap(tmp);

  return *this;
}

template<typename Hash_Ref_T, typename Value_T>
inline Rice::Hash::Iterator<Hash_Ref_T, Value_T> &
Rice::Hash::Iterator<Hash_Ref_T, Value_T>::
operator++()
{
  // Ensure we're within the range
  if(current_index_ < hash_keys().size()) {
    current_index_++;
  }

  return *this;
}

template<typename Hash_Ref_T, typename Value_T>
inline Rice::Hash::Iterator<Hash_Ref_T, Value_T>
Rice::Hash::Iterator<Hash_Ref_T, Value_T>::
operator++(int)
{
  Iterator copy(*this);
  ++(*this);
  return copy;
}

template<typename Hash_Ref_T, typename Value_T>
inline Value_T
Rice::Hash::Iterator<Hash_Ref_T, Value_T>::
operator*()
{
  return Value_T(hash_, current_key());
}

template<typename Hash_Ref_T, typename Value_T>
inline Value_T *
Rice::Hash::Iterator<Hash_Ref_T, Value_T>::
operator->()
{
  Entry tmp(hash_, current_key());
  this->tmp_.swap(tmp);
  return &tmp_;
}

template<typename Hash_Ref_T, typename Value_T>
inline bool Rice::Hash::Iterator<Hash_Ref_T, Value_T>::
operator==(Iterator const & rhs) const
{
  return hash_.value() == rhs.hash_.value()
    && current_index_ == rhs.current_index_;
}

template<typename Hash_Ref_T, typename Value_T>
inline bool Rice::Hash::Iterator<Hash_Ref_T, Value_T>::
operator!=(Iterator const & rhs) const
{
  return !(*this == rhs);
}

template<typename Hash_Ref_T, typename Value_T>
inline void
Rice::Hash::Iterator<Hash_Ref_T, Value_T>::
swap(Iterator& iterator)
{
  using namespace std;

  hash_.swap(iterator.hash_);
  swap(keys_, iterator.keys_);
  swap(current_index_, iterator.current_index_);
}

template<typename Hash_Ref_T, typename Value_T>
inline Rice::Object
Rice::Hash::Iterator<Hash_Ref_T, Value_T>::
current_key()
{
  return hash_keys()[current_index_];
}


template<typename Hash_Ref_T, typename Value_T>
inline Rice::Array
Rice::Hash::Iterator<Hash_Ref_T, Value_T>::
hash_keys()
{
  if(NIL_P(keys_)) {
    keys_ = rb_funcall(hash_, rb_intern("keys"), 0, 0);
  }

  return Rice::Array(keys_);
}

inline Rice::Hash::iterator Rice::Hash::
begin()
{
  return iterator(*this);
}

inline Rice::Hash::const_iterator Rice::Hash::
begin() const
{
  return const_iterator(*this);
}

inline Rice::Hash::iterator Rice::Hash::
end()
{
  return iterator(*this, (int)size());
}

inline Rice::Hash::const_iterator Rice::Hash::
end() const
{
  return const_iterator(*this, (int)size());
}

inline bool Rice::
operator<(
    Hash::Entry const & lhs, Hash::Entry const & rhs)
{
  Object lhs_key(lhs.key);
  Object rhs_key(rhs.key);
  if(lhs_key < rhs_key)
  {
    return true;
  }
  else if(lhs_key > rhs_key)
  {
    return false;
  }
  else if(Object(lhs.value.value()) < Object(rhs.value.value()))
  {
    return true;
  }
  else
  {
    return false;
  }
}

#endif // Rice__Hash__ipp_

