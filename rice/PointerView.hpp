#ifndef Rice__PointerView__hpp_
#define Rice__PointerView__hpp_

namespace Rice
{
  template<typename T>
  class PointerView
  {
  public:
    using type = T;

    PointerView(T* pointer);
    PointerView(T* pointer, size_t size);
    PointerView(const PointerView& other);

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
  Data_Type<T> define_pointer_view();
}
#endif // Rice__PointerView__hpp_