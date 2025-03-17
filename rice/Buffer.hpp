#ifndef Rice__Buffer__hpp_
#define Rice__Buffer__hpp_

namespace Rice
{
  template<typename T>
  class Buffer
  {
  public:
    using type = T;

    Buffer(T* pointer);
    Buffer(T* pointer, size_t size);
    Buffer(const Buffer& other);

    Buffer<std::remove_pointer_t<T>> operator*();
    VALUE read(size_t offset, size_t count);
    Array toArray(size_t offset, size_t count);

    VALUE read();
    Array toArray();

    T* pointer = nullptr;
    size_t size = 0;
  };
}

namespace Rice::detail
{
  template<typename T>
  Data_Type<T> define_buffer();
}
#endif // Rice__Buffer__hpp_