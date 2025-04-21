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
    Buffer(VALUE value);

    ~Buffer();

    Buffer(const Buffer& other) = delete;
    Buffer(Buffer&& other);

    Buffer& operator=(const Buffer& other) = delete;
    Buffer& operator=(Buffer&& other);

    T* ptr();
    T& reference();
    void release();

    size_t size() const;
    void setSize(size_t value);

    // Ruby API
    VALUE toString() const;
    VALUE toString(size_t count) const;

    Array toArray() const;
    Array toArray(size_t count) const;

    T get(size_t index) const;
    void set(size_t index, T value);

    bool isOwner() const;
    void setOwner(bool value);

  private:
    void fromRubyType(VALUE value);
    void fromDataType(VALUE value);

    bool m_owner = false;
    size_t m_size = 0;
    // std::unique_ptr would be greate but std::unique_ptr<void> isn't allowed
    T* m_buffer = nullptr;
  };

  template<typename T>
  class Buffer<T*>
  {
  public:
    using type = T*;

    Buffer(T** pointer);
    Buffer(T** pointer, size_t size);
    Buffer(VALUE value);

    ~Buffer();

    Buffer(const Buffer& other) = delete;
    Buffer(Buffer&& other);

    Buffer& operator=(const Buffer& other) = delete;
    Buffer& operator=(Buffer&& other);

    const Buffer<T>& operator[](size_t index);

    T** ptr();
    void release();

    size_t size() const;
    void setSize(size_t value);

    // Ruby API
    VALUE toString() const;
    VALUE toString(size_t count) const;

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