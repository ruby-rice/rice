# Enumerators

Although not used that frequently, Ruby supports enumerators that enable both internal and external iteration. The easiest way to create an enumerator is to not pass a block to an enumerable method. For example:

```ruby
a = [1, 2, 3, 4, 5]

# Common way to iterate
a.each do |i|
  puts i
end

# Get an enumerator instead
enumerator = a.each

# Now use it
enumerator.map |i|
  i * 2
end
```

Rice has built-in support to returning enumerators for STL containers such as [std::vector](../stl/vector.md), [std::map](../stl/map.md) and [std::unordered_map](../stl/unordered_map.md).

Implementing enumerators is tricky - and in fact requires a number of Rice features. So let's take a look at how enumerator support is implemented for std::vector.

## Implementation

Let's start with looking at the code:

```cpp
define_method("each", [](T& vector) -> const std::variant<std::reference_wrapper<T>, Object>
{
  if (!rb_block_given_p())
  {
    auto rb_size_function = [](VALUE recv, VALUE argv, VALUE eobj) -> VALUE
    {
      T* receiver = Data_Object<T>::from_ruby(recv);
      return detail::To_Ruby<size_t>().convert(receiver->size());
    };

    return rb_enumeratorize_with_size(detail::selfThread, Identifier("each").to_sym(), 0, nullptr, rb_size_function);
  }

  for (Value_T& item : vector)
  {
    VALUE element = detail::To_Ruby<Value_T>().convert(item);
    detail::protect(rb_yield, element);
  }

  return std::ref(vector);
});
```

## Method Signature

Rice defines an each method to support Ruby's enumerable module. The vector is passed by reference to avoid a copy. The return type is a std::variant because the method can either return a Ruby enumerator or the vector.

## Creating an Enumerator

If a block is not provided, the method returns an enumerator using rb_enumeratorize_with_size.

## Yielding to a Block

The code iterates over each item by reference, wraps it in a Ruby object, and yields it to the block.

## Returning Self

Returns a reference to the vector wrapped in std::reference_wrapper to enable method chaining.
