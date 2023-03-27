#include <variant>

namespace Rice::detail
{
  template<typename...Types>
  struct Type<std::variant<Types...>>
  {
    using Tuple_T = std::tuple<Types...>;

    template<std::size_t... I>
    constexpr static bool verifyTypes(std::index_sequence<I...>& indices)
    {
      return (Type<std::tuple_element_t<I, Tuple_T>>::verify() && ...);
    }

    template<std::size_t... I>
    constexpr static bool verify()
    {
      auto indices = std::make_index_sequence<std::variant_size_v<std::variant<Types...>>>{};
      return verifyTypes(indices);
    }
  };

  template<typename...Types>
  class To_Ruby<std::variant<Types...>>
  {
  public:

    template<typename T>
    static VALUE convertElement(std::variant<Types...>& data, bool takeOwnership)
    {
      return To_Ruby<T>().convert(std::get<T>(data));
    }

    template<std::size_t... I>
    static VALUE convertIterator(std::variant<Types...>& data, bool takeOwnership, std::index_sequence<I...>& indices)
    {
      // Create a tuple of the variant types so we can look over the tuple's types
      using Tuple_T = std::tuple<Types...>;
      
      /* This is a fold expression. In pseudo code:
       
        for (type in variant.types)
        {
          if (variant.has_value<type>())
            return ToRuby<type>().convert(variant.getValue<type>)
        }

        The list of variant types is stored in Tuple_T. The number of types is stored in I.
        Starting with index 0, get the variant type using td::tuple_element_t<I, Tuple_T>>.
        Next check if the variant has a value for that type using std::holds_alternative<T>. 
        If yes, then call convertElement and save the return value to result. Then use the 
        comma operator to return true to the fold expression. If the variant does not have
        a value for the type then return false. 
        
        The fold operator is or (||). If an index returns false, then the next index is evaulated
        up until I. 
        
        Code inspired by https://www.foonathan.net/2020/05/fold-tricks/ */

      VALUE result = Qnil;
      ((std::holds_alternative<std::tuple_element_t<I, Tuple_T>>(data) ?
               (result = convertElement<std::tuple_element_t<I, Tuple_T>>(data, takeOwnership), true) : false) || ...);

      return result;
    }

    static VALUE convert(std::variant<Types...>& data, bool takeOwnership = false)
    {
      auto indices = std::make_index_sequence<std::variant_size_v<std::variant<Types...>>>{};
      return convertIterator(data, takeOwnership, indices);
    }
  };

  template<typename...Types>
  class To_Ruby<std::variant<Types...>&>
  {
  public:
    template<typename T>
    static VALUE convertElement(std::variant<Types...>& data, bool takeOwnership)
    {
      return To_Ruby<T>().convert(std::get<T>(data));
    }

    template<std::size_t... I>
    static VALUE convertIterator(std::variant<Types...>& data, bool takeOwnership, std::index_sequence<I...>& indices)
    {
      // Create a tuple of the variant types so we can look over the tuple's types
      using Tuple_T = std::tuple<Types...>;

      // See comments above for explanation of this code
      VALUE result = Qnil;
      ((std::holds_alternative<std::tuple_element_t<I, Tuple_T>>(data) ?
        (result = convertElement<std::tuple_element_t<I, Tuple_T>>(data, takeOwnership), true) : false) || ...);

      return result;
    }

    static VALUE convert(std::variant<Types...>& data, bool takeOwnership = false)
    {
      auto indices = std::make_index_sequence<std::variant_size_v<std::variant<Types...>>>{};
      return convertIterator(data, takeOwnership, indices);
    }
  };

  template<typename...Types>
  class From_Ruby<std::variant<Types...>>
  {
  public:
    template<typename T>
    std::variant<Types...> convertElement(VALUE value)
    {
      return From_Ruby<T>().convert(value);
    }

    template<std::size_t... I>
    std::variant<Types...> convertIterator(VALUE value, std::index_sequence<I...>& indices)
    {
      // Create a tuple of the variant types so we can look over the tuple's types
      using Tuple_T = std::tuple<Types...>;

      std::variant<Types...> result;
      ((From_Ruby<std::tuple_element_t<I, Tuple_T>>().is_convertible(value) ?
        (result = convertElement<std::tuple_element_t<I, Tuple_T>>(value), true) : false) || ...);

      return result;
    }

    std::variant<Types...> convert(VALUE value)
    {
      auto indices = std::make_index_sequence<std::variant_size_v<std::variant<Types...>>>{};
      return convertIterator(value, indices);
    }
  };

  template<typename...Types>
  class From_Ruby<std::variant<Types...>&>
  {
  public:
    template<typename T>
    std::variant<Types...> convertElement(VALUE value)
    {
      return From_Ruby<T>().convert(value);
    }

    template<std::size_t... I>
    std::variant<Types...> convertIterator(VALUE value, std::index_sequence<I...>& indices)
    {
      // Create a tuple of the variant types so we can look over the tuple's types
      using Tuple_T = std::tuple<Types...>;

      std::variant<Types...> result;
      ((From_Ruby<std::tuple_element_t<I, Tuple_T>>().is_convertible(value) ?
        (result = convertElement<std::tuple_element_t<I, Tuple_T>>(value), true) : false) || ...);

      return result;
    }

    std::variant<Types...> convert(VALUE value)
    {
      auto indices = std::make_index_sequence<std::variant_size_v<std::variant<Types...>>>{};
      return convertIterator(value, indices);
    }

  private:
    std::variant<Types...> converted_;
  };
}