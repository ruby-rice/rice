#ifndef Rice__Buffer__hpp_
#define Rice__Buffer__hpp_

namespace Rice
{
  template<typename T, typename = void>
  class Buffer;
    
  template<typename T>
  class Buffer<T, std::enable_if_t<!std::is_pointer_v<T>>>
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
    // std::unique_ptr would be greate but std::unique_ptr<void> isn't allowed
    T* m_buffer = nullptr;
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

  template<>
  class Buffer<void>
  {
  public:
    Buffer(void* pointer);
    Buffer(const Buffer& other) = delete;
    Buffer(Buffer&& other);

    Buffer& operator=(const Buffer& other) = delete;
    Buffer& operator=(Buffer&& other);

    void* ptr();

  private:
    void* m_buffer = nullptr;
  };

  template<typename T>
  Data_Type<Buffer<T>> define_buffer(std::string klassName = "");

  void define_fundamental_buffer_types();
}
#endif // Rice__Buffer__hpp_