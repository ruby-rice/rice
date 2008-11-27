#ifndef Rice__Hash__ipp_
#define Rice__Hash__ipp_

#include "protect.hpp"
#include "to_from_ruby.hpp"
#include "Builtin_Object.hpp"
#include "Exception.hpp"
#include "Builtin_Object.hpp"
#include <algorithm>

// TODO: Evil hack
struct st_table_entry {
  unsigned int hash;
  st_data_t key;
  st_data_t record;
  st_table_entry *next;
};

inline Rice::Hash::
Hash()
  : Builtin_Object<RHash, T_HASH>(protect(rb_hash_new))
{
}

inline Rice::Hash::
Hash(Object v)
  : Builtin_Object<RHash, T_HASH>(v)
{
}

inline size_t Rice::Hash::
size() const
{
  return RHASH_TBL(this->value())->num_entries;
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
  return protect(rb_hash_aset, hash_, key_, to_ruby(value));
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
  return Proxy(*this, to_ruby(key));
}

template<typename Key_T>
inline Rice::Hash::Proxy Rice::Hash::
operator[](Key_T const & key)
{
  return Proxy(*this, to_ruby(key));
}

template<typename Value_T, typename Key_T>
inline Value_T Rice::Hash::
get(Key_T const & key)
{
  Object ruby_key(to_ruby(key));
  Object value = operator[](ruby_key);
  try
  {
    return from_ruby<Value_T>(value);
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
Iterator(Hash_Ref_T hash, st_data_t bin, st_table_entry * ptr)
  : hash_(hash)
  , tbl_(RHASH_TBL(hash.value()))
  , bin_(bin)
  , ptr_(ptr)
  , tmp_(hash, Qnil)
{
  // If we aren't already at the end, then use the increment operator to
  // point to the first element
  if(!ptr_ && bin_ < tbl_->num_bins)
  {
    operator++();
  }
}

template<typename Hash_Ref_T, typename Value_T>
inline Rice::Hash::Iterator<Hash_Ref_T, Value_T>::
Iterator(Iterator const & iterator)
  : hash_(iterator.hash_.value())
  , tbl_(iterator.tbl_)
  , bin_(iterator.bin_)
  , ptr_(iterator.ptr_)
  , tmp_(iterator.hash_, Qnil)
{
}

template<typename Hash_Ref_T, typename Value_T>
template<typename Iterator_T>
inline Rice::Hash::Iterator<Hash_Ref_T, Value_T>::
Iterator(Iterator_T const & iterator)
  : hash_(iterator.hash_.value())
  , tbl_(iterator.tbl_)
  , bin_(iterator.bin_)
  , ptr_(iterator.ptr_)
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
  // Go to the next element in the bin; this will be a no-op if we were
  // called from the constructor, because ptr_ will be 0 (and if its
  // not, this function won't get called).
  if(ptr_)
  {
    ptr_ = ptr_->next;
  }

  // If we've reached the end of the bin, then try the next bin until
  // we have run out of bins
  while(ptr_ == 0)
  {
    ++bin_;
    if(bin_ == tbl_->num_bins)
    {
      // At the end..
      return *this;
    }
    ptr_ = tbl_->bins[bin_];
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
  return Value_T(hash_, ptr_->key);
}

template<typename Hash_Ref_T, typename Value_T>
inline Value_T *
Rice::Hash::Iterator<Hash_Ref_T, Value_T>::
operator->()
{
  Entry tmp(hash_, ptr_->key);
  this->tmp_.swap(tmp);
  return &tmp_;
}

template<typename Hash_Ref_T, typename Value_T>
inline bool Rice::Hash::Iterator<Hash_Ref_T, Value_T>::
operator==(Iterator const & rhs) const
{
  return hash_.value() == rhs.hash_.value()
    && tbl_ == rhs.tbl_
    && bin_ == rhs.bin_
    && ptr_ == rhs.ptr_;
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
  swap(tbl_, iterator.tbl_);
  swap(bin_, iterator.bin_);
  swap(ptr_, iterator.ptr_);
}

inline Rice::Hash::iterator Rice::Hash::
begin()
{
  st_table * tbl(RHASH_TBL(value()));
  return iterator(*this, 0, tbl->bins[0]);
}

inline Rice::Hash::const_iterator Rice::Hash::
begin() const
{
  st_table * tbl(RHASH_TBL(value()));
  return const_iterator(*this, 0, tbl->bins[0]);
}

inline Rice::Hash::iterator Rice::Hash::
end()
{
  st_table * tbl(RHASH_TBL(value()));
  return iterator(*this, tbl->num_bins, 0);
}

inline Rice::Hash::const_iterator Rice::Hash::
end() const
{
  st_table * tbl(RHASH_TBL(value()));
  return const_iterator(*this, tbl->num_bins, 0);
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

