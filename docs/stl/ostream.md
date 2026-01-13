# std::ostream

Rice provides support for exposing C++ output streams to Ruby. This allows Ruby code to interact with C++ streams, including writing to them and passing them to C++ functions that expect stream arguments.

## Supported Classes

Rice supports three output stream types:

* `std::ostream` - The base output stream class
* `std::ostringstream` - String-based output stream
* `std::ofstream` - File-based output stream

## Ruby Classes

The stream classes are defined in the `Std` module:

* `Std::OStream` - Base class for output streams
* `Std::OStringStream` - Inherits from `Std::OStream`
* `Std::OFStream` - Inherits from `Std::OStream`

## Defining Stream Classes

To explicitly define the stream classes:

```cpp
#include <rice/rice.hpp>
#include <rice/stl.hpp>

Rice::define_ostream();        // Defines Std::OStream
Rice::define_ostringstream();  // Defines Std::OStringStream
Rice::define_ofstream();       // Defines Std::OFStream
```

Stream classes are also automatically registered when C++ functions use them as parameters or return types.

## Standard Stream Constants

When `define_ostream()` is called, Rice also defines constants for the standard C++ streams:

* `Std::COUT` - References `std::cout`
* `Std::CERR` - References `std::cerr`

## Ruby API

### OStream Methods

All output stream classes inherit these methods from `Std::OStream`:

| Method | Description |
|:-------|:------------|
| `write(value)` | Writes a value to the stream |
| `<<` | Alias for `write` |
| `flush` | Flushes the stream buffer |
| `clear` | Clears error state flags |
| `good?` | Returns true if no error flags are set |
| `bad?` | Returns true if a non-recoverable error occurred |
| `fail?` | Returns true if an operation failed |
| `eof?` | Returns true if end-of-file was reached |

### OStringStream Methods

In addition to inherited methods:

| Method | Description |
|:-------|:------------|
| `new` | Creates an empty string stream |
| `new(str)` | Creates a stream initialized with the given string |
| `str` | Returns the stream contents as a string |
| `str=` | Sets the stream contents |
| `to_s` | Alias for `str` |

### OFStream Methods

In addition to inherited methods:

| Method | Description |
|:-------|:------------|
| `new` | Creates an unopened file stream |
| `new(filename)` | Creates and opens a file stream |
| `open(filename)` | Opens a file for writing |
| `close` | Closes the file |
| `open?` | Returns true if a file is open |

## Examples

### Writing to a String Stream

```ruby
stream = Std::OStringStream.new
stream.write("Hello")
stream << " " << "World" << 123
puts stream.str  # => "Hello World123"
```

### Writing to a File

```ruby
stream = Std::OFStream.new("output.txt")
stream << "Line 1\n"
stream << "Line 2\n"
stream.flush
stream.close
```

### Passing Streams to C++ Functions

```cpp
// C++ code
void logMessage(std::ostream& os, const std::string& msg) {
    os << "[LOG] " << msg << std::endl;
}

// Register with Rice
m.define_module_function("log_message", &logMessage);
```

```ruby
# Ruby code
stream = Std::OStringStream.new
log_message(stream, "Application started")
puts stream.str  # => "[LOG] Application started\n"

# Or write directly to stdout
log_message(Std::COUT, "Hello from Ruby!")
```
