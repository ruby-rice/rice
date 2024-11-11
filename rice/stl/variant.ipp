#include <variant>
#include "../detail/TupleIterator.hpp"

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
    static VALUE convertElement(const std::variant<Types...>& data, bool takeOwnership)
    {
      return To_Ruby<T>().convert(std::get<T>(data));
    }

    template<std::size_t... I>
    static VALUE convertIterator(const std::variant<Types...>& data, bool takeOwnership, std::index_sequence<I...>& indices)
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

    static VALUE convert(const std::variant<Types...>& data, bool takeOwnership = false)
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
    static VALUE convertElement(const std::variant<Types...>& data, bool takeOwnership)
    {
      return To_Ruby<T>().convert(std::get<T>(data));
    }

    template<std::size_t... I>
    static VALUE convertIterator(const std::variant<Types...>& data, bool takeOwnership, std::index_sequence<I...>& indices)
    {
      // Create a tuple of the variant types so we can look over the tuple's types
      using Tuple_T = std::tuple<Types...>;

      // See comments above for explanation of this code
      VALUE result = Qnil;
      ((std::holds_alternative<std::tuple_element_t<I, Tuple_T>>(data) ?
        (result = convertElement<std::tuple_element_t<I, Tuple_T>>(data, takeOwnership), true) : false) || ...);

      return result;
    }

    static VALUE convert(const std::variant<Types...>& data, bool takeOwnership = false)
    {
      auto indices = std::make_index_sequence<std::variant_size_v<std::variant<Types...>>>{};
      return convertIterator(data, takeOwnership, indices);
    }
  };

  template<typename...Types>
  class From_Ruby<std::variant<Types...>>
  {
  public:
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
      int i = 0;
      int index = -1;

      for_each_tuple(this->fromRubys_,
        [&](auto& fromRuby)
        {
          Convertible isConvertible = fromRuby.is_convertible(value);
          if (isConvertible == Convertible::Exact)
          {
            index = i;
          }
          else if (isConvertible == Convertible::TypeCast && index == -1)
          {
            index = i;
          }
          i++;
        });

      if (index == -1)
      {
        rb_raise(rb_eArgError, "Could not find converter for variant");
      }

      return this->convertInternal(value, index);
    }

  private:
    // Possible converters we could use for this variant
    using From_Ruby_Ts = std::tuple<From_Ruby<Types>...>;
    From_Ruby_Ts fromRubys_;
  };

  template<typename...Types>
  class From_Ruby<std::variant<Types...>&>
  {
  public:
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
      // Loop over each possible type in the variant.
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
      int i = 0;
      int index = -1;

      for_each_tuple(this->fromRubys_,
        [&](auto& fromRuby)
        {
          Convertible isConvertible = fromRuby.is_convertible(value);
          if (isConvertible == Convertible::Exact)
          {
            index = i;
          }
          else if (isConvertible == Convertible::TypeCast && index == -1)
          {
            index = i;
          }
          i++;
        });

      if (index == -1)
      {
        rb_raise(rb_eArgError, "Could not find converter for variant");
      }

      return this->convertInternal(value, index);
    }

  private:
    // Possible converters we could use for this variant
    using From_Ruby_Ts = std::tuple<From_Ruby<Types>...>;
    From_Ruby_Ts fromRubys_;
  };
}
