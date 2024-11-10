// See https://www.cppstories.com/2022/tuple-iteration-apply/

template <typename Tuple_T, typename Function_T>
void for_each_tuple(Tuple_T&& tuple, Function_T&& callable)
{
  std::apply([&callable](auto&& ...args)
    {
      (callable(std::forward<decltype(args)>(args)), ...);
    }, std::forward<Tuple_T>(tuple));
}
