namespace Rice
{
  template<typename T, typename...Arg_Ts>
  class Constructor
  {
  public:
    static constexpr std::size_t arity = sizeof...(Arg_Ts);

    static constexpr bool isCopyConstrutor()
    {
      if constexpr (arity == 1)
      {
        using Arg_Types = std::tuple<Arg_Ts...>;
        using First_Arg_T = std::tuple_element_t<0, Arg_Types>;
        return (arity == 1 &&
                std::is_lvalue_reference_v<First_Arg_T> &&
                std::is_same_v<T, detail::intrinsic_type<First_Arg_T>>);
      }
      else
      {
        return false;
      }
    }

    static constexpr bool isMoveConstrutor()
    {
      if constexpr (arity == 1)
      {
        using Arg_Types = std::tuple<Arg_Ts...>;
        using First_Arg_T = std::tuple_element_t<0, Arg_Types>;
        return (arity == 1 &&
          std::is_rvalue_reference_v<First_Arg_T> &&
          std::is_same_v<T, detail::intrinsic_type<First_Arg_T>>);
      }
      else
      {
        return false;
      }
    }

    static void initialize(VALUE self, Arg_Ts...args)
    {
      // Call C++ constructor
      T* data = new T(args...);
      detail::replace<T>(self, Data_Type<T>::ruby_data_type(), data, true);
    }

    static void initialize_copy(VALUE self, const T& other)
    {
      // Call C++ copy constructor
      T* data = new T(other);
      detail::replace<T>(self, Data_Type<T>::ruby_data_type(), data, true);
    }

  };

  //! Special-case Constructor used when defining Directors.
  template<typename T, typename...Arg_Ts>
  class Constructor<T, Object, Arg_Ts...>
  {
    public:
      static constexpr bool isCopyConstrutor()
      {
        return false;
      }

      static constexpr bool isMoveConstrutor()
      {
        return false;
      }

      static void initialize(Object self, Arg_Ts...args)
      {
        // Call C++ constructor
        T* data = new T(self, args...);
        detail::replace<T>(self.value(), Data_Type<T>::ruby_data_type(), data, true);
      }
  };
}