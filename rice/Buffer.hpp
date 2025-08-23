#ifndef Rice__Buffer__hpp_
#define Rice__Buffer__hpp_

namespace Rice
{
  template<typename T, typename = void>
  class Buffer;
    
  template<typename T>
  class Buffer<T, std::enable_if_t<!std::is_pointer_v<T> && !std::is_void_v<T>>>
  {
  public:
    using Element_T = T;

    Buffer(T* pointer);
    Buffer(T* pointer, size_t size);
    Buffer(VALUE value);

    ~Buffer();

    Buffer(const Buffer& other) = delete;
    Buffer(Buffer&& other);

    Buffer& operator=(const Buffer& other) = delete;
    Buffer& operator=(Buffer&& other);
    T& operator[](size_t index);

    T* ptr();
    T& reference();
    void release();

    size_t size() const;
    void setSize(size_t value);

    // Ruby API
    VALUE toString() const;

    VALUE bytes() const;
    VALUE bytes(size_t count) const;

    Array toArray() const;
    Array toArray(size_t count) const;

    bool isOwner() const;
    void setOwner(bool value);

  private:
    void fromBuiltinType(VALUE value);
    void fromWrappedType(VALUE value);

    bool m_owner = false;
    size_t m_size = 0;
    // std::unique_ptr would be great but std::unique_ptr<void> isn't allowed. Mutable is needed to
    // support const T* buffers
    mutable T* m_buffer = nullptr;
  };

  template<typename T>
  class Buffer<T*, std::enable_if_t<!detail::is_wrapped_v<T>>>
  {
  public:
    using Element_T = Buffer<T>;

    Buffer(T** pointer);
    Buffer(T** pointer, size_t size);
    Buffer(VALUE value);

    ~Buffer();

    Buffer(const Buffer& other) = delete;
    Buffer(Buffer&& other);

    Buffer& operator=(const Buffer& other) = delete;
    Buffer& operator=(Buffer&& other);

    Element_T& operator[](size_t index);

    T** ptr();
    void release();

    size_t size() const;
    void setSize(size_t value);

    // Ruby API
    VALUE toString() const;

    VALUE bytes() const;
    VALUE bytes(size_t count) const;

    Array toArray() const;
    Array toArray(size_t count) const;

    void setOwner(bool value);
    bool isOwner() const;

  private:
    bool m_owner = false;
    size_t m_size = 0;
    T** m_outer = nullptr;
    std::vector<Buffer<T>> m_inner;
  };

  template<typename T>
  class Buffer<T*, std::enable_if_t<detail::is_wrapped_v<T>>>
  {
  public:
    using Element_T = T*;

    Buffer(T** pointer);
    Buffer(T** pointer, size_t size);
    Buffer(VALUE value);

    ~Buffer();

    Buffer(const Buffer& other) = delete;
    Buffer(Buffer&& other);

    Buffer& operator=(const Buffer& other) = delete;
    Buffer& operator=(Buffer&& other);

    Element_T& operator[](size_t index);

    T** ptr();
    void release();

    size_t size() const;
    void setSize(size_t value);

    // Ruby API
    VALUE toString() const;

    VALUE bytes() const;
    VALUE bytes(size_t count) const;

    Array toArray() const;
    Array toArray(size_t count) const;

    void setOwner(bool value);
    bool isOwner() const;

  private:
    bool m_owner = false;
    size_t m_size = 0;
    T** m_buffer = nullptr;
  };

  template<typename T>
  class Buffer<T, std::enable_if_t<std::is_void_v<T>>>
  {
  public:
    Buffer(T* pointer);
    Buffer(VALUE value);
    Buffer(const Buffer& other) = delete;
    Buffer(Buffer&& other);
    ~Buffer();

    Buffer& operator=(const Buffer& other) = delete;
    Buffer& operator=(Buffer&& other);

    size_t size() const;
    void setSize(size_t value);
      
    VALUE bytes(size_t count) const;
    VALUE bytes() const;

    T* ptr();

  private:
    bool m_owner = false;
    size_t m_size = 0;
    T* m_buffer = nullptr;
  };

  template<typename T>
  Data_Type<Buffer<T>> define_buffer(std::string klassName = "");
}
#endif // Rice__Buffer__hpp_