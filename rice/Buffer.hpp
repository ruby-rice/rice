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

    T* get();
    void release();

    size_t size() const;

    bool isOwner() const;
    void setOwner(bool value);

    VALUE read(size_t offset, size_t count) const;
    Array toArray(size_t offset, size_t count) const;

    VALUE read() const;
    Array toArray() const;

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

    T** get();
    void release();

    size_t size() const;

    void setOwner(bool value);
    bool isOwner() const;

    VALUE read(size_t offset, size_t count) const;
    Array toArray(size_t offset, size_t count) const;

    VALUE read() const;
    Array toArray() const;

  private:
    bool m_owner = false;
    size_t m_size = 0;
    T** m_outer = nullptr;
    std::vector<Buffer<T>> m_inner;
  };

  template<typename T>
  Data_Type<Buffer<T>> define_buffer(std::string klassName = "");

  void define_fundamental_buffer_types();
}
#endif // Rice__Buffer__hpp_