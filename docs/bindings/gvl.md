# Global Virtual Machine Lock (GVL)

Ruby's Global Virtual Machine Lock, commonly known as the GVL, prevents multiple native threads from executing Ruby code simultaneously. This ensures thread safety within the Ruby VM by serializing access to Ruby's internal data structures and preventing race conditions.

While the GVL simplifies Ruby's internal implementation and makes extension development safer, it also means that multi-threaded Ruby programs cannot achieve true parallelism when executing Ruby code, even on multi-core systems.

However, C++ code called from Ruby often does not need to hold the GVL. If the native code is going to take a significant amount of time to execute, then holding the GVL unnecessarily prevents other Ruby threads from making progress.

## Function/Method Annotation

Rice lets you mark functions and methods that can be run without the GVL.

Assume you are wrapping a function that will take a long time to complete:

```cpp
void slowFunction()
{
  std::this_thread::sleep_for(std::chrono::milliseconds(100000));
}
```

You can tell Rice to run this code without holding the GVL by using the `NoGVL` object to mark it as GVL safe:

```cpp
Module m = define_module("Testing");
m.define_module_function("slow_function", &slowFunction, NoGvl());
```

When `slow_function` is called from Ruby, Rice will first ask Ruby to release the GVL before executing the C++ code.

## Helper Method

Sometimes you may wish to manually release the GVL. Ruby provides two APIs to do this, `rb_thread_call_without_gvl` and `rb_thread_call_without_gvl2`. However, calling them is inconvenient because they require passing in a callback function and figuring out how to pass state to the callback and then return results.

To make this easier, Rice provides a helper method called `detail:no_gvl`. It is simple to use:

```cpp
int result = detail::no_gvl(&slowFunction);
```

Rice will ask Ruby to release the GVL, call `slowFunction` and then reacquire the GVL before returning.

This also works for functions and member functions that take multiple arguments. For example, assume this member function:

```cpp
class Task
{
  public:
    int run(bool arg1)
    {
    }
};
```

To call the `run` method without the GVL, pass the object instance as the first parameter and any additional method parameters next.

```cpp
Task task(84);
int result = detail::no_gvl(&Task::run, task, false);
```

## Acquire the GVL

If a thread releases the GVL, it may not interact with any Ruby objects or the Ruby VM. If you need to call Ruby, then you have to reacquire the GVL. This can be done using Ruby's CAPI `rb_thread_call_with_gvl()`. Rice currently does not provide a helper method to call this function.
