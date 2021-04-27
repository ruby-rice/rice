#ifndef Rice__Hash__hpp_
#define Rice__Hash__hpp_

#include "Builtin_Object_defn.hpp"
#include "Array.hpp"
#include "detail/ruby.hpp"
#include <iterator>
#include <type_traits>

namespace Rice
{
  //! A wrapper for the ruby Hash class.
  //! This class provides a C++-style interface to ruby's Hash class and
  //! its associated rb_hash_* functions.
  //! Example:
  //! \code
  //!   Hash h;
  //!   h[42] = String("foo");
  //!   h[10] = String("bar");
  //!   std::cout << String(h[42]) << std::endl;
  //! \endcode
  class Hash: public Builtin_Object<T_HASH>
  {
  public:
    //! Construct a new hash.
    Hash();

    //! Wrap an existing hash.
    /*! \param v the hash to wrap.
     */
    Hash(Object v);

    //! Return the number of elements in the hash.
    size_t size() const;

  private:
    //! A helper class so hash[key]=value can work.
    class Proxy;

  public:
    //! Get the value for the given key.
    /*! \param key the key whose value should be retrieved from the hash.
     *  \return the value associated with the given key.
     */
    template<typename Key_T>
    Proxy const operator[](Key_T const& key) const;

    //! Get the value for the given key.
    /*! \param key the key whose value should be retrieved from the hash.
     *  \return the value associated with the given key.
     */
    template<typename Key_T>
    Proxy operator[](Key_T const& key);

    //! Get the value for the given key
    /*! The returned value is converted to the type given by Value_T.
     *  \param key the key whose value should be retrieved from the hash.
     *  \return the value associated with the given key, converted to C++
     *  type Value_T.
     */
    template<typename Value_T, typename Key_T>
    Value_T get(Key_T const& key);

    //! A helper class for dereferencing iterators
    class Entry;

    //! A helper class for implementing iterators for a Hash.
    template<typename Hash_Ptr_T, typename Value_T>
    class Iterator;

  public:
    //! An iterator.
    typedef Iterator<Hash*, Entry> iterator;

    //! A const iterator.
    typedef Iterator<Hash const*, Entry const> const_iterator;

  public:
    //! Return an iterator to the beginning of the hash.
    iterator begin();

    //! Return a const iterator to the beginning of the hash.
    const_iterator begin() const;

    //! Return an iterator to the end of the hash.
    iterator end();

    //! Return a const to the end of the hash.
    const_iterator end() const;
  };

  //! A helper class so hash[key]=value can work.
  class Hash::Proxy
  {
  public:
    //! Construct a new Proxy.
    Proxy(Hash* hash, Object key);

    //! Implicit conversion to Object.
    operator Object() const;

    //! Explicit conversion to VALUE.
    VALUE value() const;

    //! Assignment operator.
    template<typename T>
    Object operator=(T const& value);

  private:
    Hash* hash_;
    Object key_;
  };

  //! A helper class for dereferencing iterators
  /*! This class is intended to look similar to an std::pair.
   */
  class Hash::Entry
  {
  public:
    //! Construct a new Entry.
    Entry(Hash* hash, Object key);

    //! Copy constructor.
    Entry(Entry const& entry);

    Object const key;          //!< The key
    Object const& first;      //!< An alias for the key

    Proxy value;              //!< The value
    Proxy& second;           //!< An alias for the value

    Entry& operator=(Entry const& rhs);

    friend bool operator<(Entry const& lhs, Entry const& rhs);
  };

  bool operator<(Hash::Entry const& lhs, Hash::Entry const& rhs);

  //! A helper class for implementing iterators for a Hash.
  template<typename Hash_Ptr_T, typename Value_T>
  class Hash::Iterator
  {
  public:
    using iterator_category = std::input_iterator_tag;
    using value_type = Value_T;
    using difference_type = long;
    using pointer = Object*;
    using reference = Value_T&;

    //! Construct a new Iterator.
    Iterator(Hash_Ptr_T hash);

    //! Construct a new Iterator with a given start-at index point
    Iterator(Hash_Ptr_T hash, int start_at);

    //! Construct an Iterator from another Iterator of a different const
    //! qualification.
    template<typename Iterator_T>
    Iterator(Iterator_T const& iterator);

    //! Preincrement operator.
    Iterator& operator++();

    //! Postincrement operator.
    Iterator operator++(int);

    //! Dereference operator.
    Value_T operator*();

    //! Dereference operator.
    Value_T* operator->();

    //! Equality operator.
    bool operator==(Iterator const& rhs) const;

    //! Inequality operator.
    bool operator!=(Iterator const& rhs) const;

    template<typename Hash_Ptr_T_, typename Value_T_>
    friend class Hash::Iterator;

  protected:
    Object current_key();

    Array hash_keys();

  private:
    Hash_Ptr_T hash_;
    long current_index_;
    VALUE keys_;

    mutable typename std::remove_const<Value_T>::type tmp_;
  };
} // namespace Rice

#include "Hash.ipp"

#endif // Rice__Hash__hpp_

