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
    To_Ruby() = default;

    explicit To_Ruby(Return* returnInfo)
    {
    }

    template<typename U, typename V>
    VALUE convertElement(U& data, bool takeOwnership)
    {
      return To_Ruby<V>().convert(std::forward<V>(std::get<V>(data)));
    }

    template<typename U, std::size_t... I>
    VALUE convertIterator(U& data, bool takeOwnership, std::index_sequence<I...>& indices)
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
        
        The fold operator is or (||). If an index returns false, then the next index is evaluated
        up until I. 
        
        Code inspired by https://www.foonathan.net/2020/05/fold-tricks/ */

      VALUE result = Qnil;

      #if defined(__GNUC__) || defined(__clang__)
      #pragma GCC diagnostic push
      #pragma GCC diagnostic ignored "-Wunused-value"
      #endif

      ((std::holds_alternative<std::tuple_element_t<I, Tuple_T>>(data) ?
               (result = convertElement<U, std::tuple_element_t<I, Tuple_T>>(data, takeOwnership), true) : false) || ...);
      
      #if defined(__GNUC__) || defined(__clang__)
      #pragma GCC diagnostic pop
      #endif

      return result;
    }

    template<typename U>
    VALUE convert(U& data, bool takeOwnership = false)
    {
      auto indices = std::make_index_sequence<std::variant_size_v<std::variant<Types...>>>{};
      return convertIterator(data, takeOwnership, indices);
    }

    template<typename U>
    VALUE convert(U&& data, bool takeOwnership = false)
    {
      auto indices = std::make_index_sequence<std::variant_size_v<std::variant<Types...>>>{};
      return convertIterator(data, takeOwnership, indices);
    }
  };

  template<typename...Types>
  class To_Ruby<std::variant<Types...>&>
  {
  public:
    To_Ruby() = default;

    explicit To_Ruby(Return* returnInfo)
    {
    }

    template<typename U, typename V>
    VALUE convertElement(U& data, bool takeOwnership)
    {
      if constexpr (std::is_const_v<U>)
      {
        return To_Ruby<V>().convert(std::get<V>(data));
      }
      else
      {
        return To_Ruby<V>().convert(std::forward<V>(std::get<V>(data)));
      }
    }

    template<typename U, std::size_t... I>
    VALUE convertIterator(U& data, bool takeOwnership, std::index_sequence<I...>& indices)
    {
      // Create a tuple of the variant types so we can look over the tuple's types
      using Tuple_T = std::tuple<Types...>;

      // See comments above for explanation of this code
      VALUE result = Qnil;

      #if defined(__GNUC__) || defined(__clang__)
      #pragma GCC diagnostic push
      #pragma GCC diagnostic ignored "-Wunused-value"
      #endif

      ((std::holds_alternative<std::tuple_element_t<I, Tuple_T>>(data) ?
        (result = convertElement<U, std::tuple_element_t<I, Tuple_T>>(data, takeOwnership), true) : false) || ...);

      #if defined(__GNUC__) || defined(__clang__)
      #pragma GCC diagnostic pop
      #endif

      return result;
    }

    template<typename U>
    VALUE convert(U& data, bool takeOwnership = false)
    {
      auto indices = std::make_index_sequence<std::variant_size_v<std::variant<Types...>>>{};
      return convertIterator(data, takeOwnership, indices);
    }
  };

  template<typename...Types>
  class From_Ruby<std::variant<Types...>>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      Convertible result = Convertible::None;

      for_each_tuple(this->fromRubys_,
        [&](auto& fromRuby)
        {
          result = result | fromRuby.is_convertible(value);
        });

      return result;
    }

    // This method search through a variant's types to figure out which one the
    // currently Ruby value best matches. It then returns the index of the type.
    int figureIndex(VALUE value)
    {
      int i = 0;
      int index = -1;
      Convertible foundConversion = Convertible::None;

      for_each_tuple(this->fromRubys_,
        [&](auto& fromRuby)
        {
          Convertible isConvertible = fromRuby.is_convertible(value);

          if (isConvertible > foundConversion)
          {
            index = i;
            foundConversion = isConvertible;
          }
          i++;
        });

      if (index == -1)
      {
        rb_raise(rb_eArgError, "Could not find converter for variant");
      }

      return index;
    }

    /* This method loops over each type in the variant, creates a From_Ruby converter,
       and then check if the converter can work with the provided Rby value (it checks
       the type of the Ruby object to see if it matches the variant type).
       If yes, then the converter runs. If no, then the method recursively calls itself
       increasing the index.

       We use recursion, with a constexpr, to avoid having to instantiate an instance
       of the variant to store results from a fold expression like the To_Ruby code
       does above. That allows us to process variants with non default constructible
       arguments like std::reference_wrapper. */
    template <std::size_t I = 0>
    std::variant<Types...> convertInternal(VALUE value, int index)
    {
      if constexpr (I < std::variant_size_v<std::variant<Types...>>)
      {
        if (I == index)
        {
          auto fromRuby = std::get<I>(this->fromRubys_);
          return fromRuby.convert(value);
        }
        else
        {
          return convertInternal<I + 1>(value, index);
        }
      }
      rb_raise(rb_eArgError, "Could not find converter for variant");
    }

    std::variant<Types...> convert(VALUE value)
    {
      int index = this->figureIndex(value);
      return this->convertInternal(value, index);
    }

  private:
    // Possible converters we could use for this variant
    using From_Ruby_Ts = std::tuple<From_Ruby<Types>...>;
    From_Ruby_Ts fromRubys_;
  };

  template<typename...Types>
  class From_Ruby<std::variant<Types...>&> : public From_Ruby<std::variant<Types...>>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg)
    {
    }

    std::variant<Types...>& convert(VALUE value)
    {
      int index = this->figureIndex(value);
      this->converted_ = this->convertInternal(value, index);
      return this->converted_;
    }

  private:
    std::variant<Types...> converted_;
  };
}
