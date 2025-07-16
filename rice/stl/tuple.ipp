#include <tuple>

namespace Rice::detail
{
  template<typename...Types>
  struct Type<std::tuple<Types...>>
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
      auto indices = std::make_index_sequence<std::tuple_size_v<std::tuple<Types...>>>{};
      return verifyTypes(indices);
    }
  };

  template<typename...Types>
  class To_Ruby<std::tuple<Types...>>
  {
  public:
    To_Ruby() = default;

    explicit To_Ruby(Return* returnInfo) : returnInfo_(returnInfo)
    {
    }

    VALUE convert(const std::tuple<Types...>& data)
    {
      Array result;

      for_each_tuple(data, [&](auto element)
      {
        result.push(element, true);
      });

      return result.value();
    }

  private:
    Return* returnInfo_ = nullptr;
  };

  template<typename...Types>
  class To_Ruby<std::tuple<Types...>&>
  {
  public:
    To_Ruby() = default;

    explicit To_Ruby(Return* returnInfo) : returnInfo_(returnInfo)
    {
    }

    VALUE convert(const std::tuple<Types...>& data)
    {
      Array result;

      bool isOwner = (this->returnInfo_ && this->returnInfo_->isOwner());

      for_each_tuple(data, [&](auto& element)
      {
        result.push(element, isOwner);
      });

      return result.value();
    }

  private:
    Return* returnInfo_ = nullptr;
  };

  template<typename...Types>
  class From_Ruby<std::tuple<Types...>>
  {
  public:
    using Tuple_T = std::tuple<Types...>;

    template<std::size_t... I>
    constexpr static bool verifyTypes(Array& array, std::index_sequence<I...>& indices)
    {
      return (Type<std::tuple_element_t<I, Tuple_T>>::verify() && ...);
    }

    From_Ruby() = default;

    explicit From_Ruby(Arg* arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      Convertible result = Convertible::None;

      // The ruby value must be an array of the correct size
      if (rb_type(value) != RUBY_T_ARRAY || Array(value).size() != std::tuple_size_v<Tuple_T>)
      {
        return result;
      }
      
      // Now check that each tuple type is convertible
      Array array(value);
      int i = 0;
      for_each_tuple(this->fromRubys_,
        [&](auto& fromRuby)
        {
          result = result | fromRuby.is_convertible(array[i].value());
          i++;
        });

      return result;
    }

    template <std::size_t... I>
    std::tuple<Types...> convertInternal(Array array, std::index_sequence<I...>& indices)
    {
      return std::forward_as_tuple(std::get<I>(this->fromRubys_).convert(array[I].value())...);
    }

    std::tuple<Types...> convert(VALUE value)
    {
      Array array(value);
      auto indices = std::make_index_sequence<std::tuple_size_v<Tuple_T>>{};
      return convertInternal(array, indices);
    }

  private:
    // Possible converters we could use for this variant
    using From_Ruby_Ts = std::tuple<From_Ruby<Types>...>;
    From_Ruby_Ts fromRubys_;
  };

/*  template<typename...Types>
  class From_Ruby<std::tuple<Types...>&> : public From_Ruby<std::tuple<Types...>>
  {
  public:
    std::tuple<Types...>& convert(VALUE value)
    {
      int index = this->figureIndex(value);
      this->converted_ = this->convertInternal(value, index);
      return this->converted_;
    }

  private:
    std::tuple<Types...> converted_;
  };*/
}
