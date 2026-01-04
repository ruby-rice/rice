# Rice Fuzzing Implementation Plan

This document outlines a plan to integrate fuzz testing into Rice using AFL++ or libFuzzer on Linux.

## Overview

Fuzzing Rice will help discover:
- Buffer overflows in string handling
- Integer overflow/underflow in numeric conversions
- Memory corruption in STL container conversions
- Exception handling edge cases
- Null byte and binary data handling issues

## Prerequisites

### System Requirements
- Linux (Ubuntu 22.04+ recommended)
- Clang 14+ with libFuzzer support
- Ruby development headers
- CMake 3.15+

### Installation (Ubuntu)
```bash
sudo apt update
sudo apt install clang llvm ruby-dev cmake build-essential

# Optional: AFL++ for alternative fuzzing
sudo apt install afl++
```

## Directory Structure

```
rice/
├── fuzz/
│   ├── CMakeLists.txt          # Fuzzing build configuration
│   ├── harnesses/
│   │   ├── fuzz_common.hpp     # Shared utilities (Ruby init, helpers)
│   │   ├── fuzz_string_conversion.cpp
│   │   ├── fuzz_numeric_conversion.cpp
│   │   ├── fuzz_array_conversion.cpp
│   │   ├── fuzz_hash_conversion.cpp
│   │   ├── fuzz_exception.cpp
│   │   └── fuzz_identifier.cpp
│   └── corpus/                  # Seed inputs
│       ├── string/
│       ├── numeric/
│       ├── array/
│       └── hash/
```

## Fuzz Targets

### 1. String Conversion (`fuzz_string_conversion.cpp`)
- `From_Ruby<std::string>` / `To_Ruby<std::string>`
- `From_Ruby<std::string_view>`
- `From_Ruby<const char*>`
- Tests: embedded nulls, binary data, empty strings, very long strings

### 2. Numeric Conversion (`fuzz_numeric_conversion.cpp`)
- All `From_Ruby<T>` for: int, long, long long, unsigned variants, float, double, bool, char
- Tests: boundary values, signedness, overflow, NaN/Inf

### 3. Array/Vector Conversion (`fuzz_array_conversion.cpp`)
- `From_Ruby<std::vector<T>>` for various T
- `To_Ruby<std::vector<T>>`
- Tests: empty arrays, large arrays, mixed types, nested arrays

### 4. Hash/Map Conversion (`fuzz_hash_conversion.cpp`)
- `From_Ruby<std::map<K,V>>` / `std::unordered_map`
- Tests: duplicate keys, empty hash, string keys with special chars

### 5. Exception Handling (`fuzz_exception.cpp`)
- `Rice::Exception` creation
- C++ to Ruby exception translation
- Ruby to C++ exception handling

### 6. Identifier/Symbol (`fuzz_identifier.cpp`)
- `Rice::Identifier`, `Rice::Symbol`, `Rice::String`
- Tests: special characters, empty strings, very long names

## CMake Configuration

Key options:
```cmake
option(USE_LIBFUZZER "Use libFuzzer (requires Clang)" ON)
option(USE_AFL "Use AFL++ instrumentation" OFF)
option(ENABLE_ASAN "Enable AddressSanitizer" ON)
option(ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer" ON)
```

Build commands:
```bash
# libFuzzer build
CC=clang CXX=clang++ cmake -B build-fuzz -S fuzz \
    -DCMAKE_BUILD_TYPE=Debug \
    -DUSE_LIBFUZZER=ON \
    -DENABLE_ASAN=ON

cmake --build build-fuzz

# AFL++ build
CC=afl-clang-fast CXX=afl-clang-fast++ cmake -B build-afl -S fuzz \
    -DCMAKE_BUILD_TYPE=Debug \
    -DUSE_AFL=ON

cmake --build build-afl
```

## Running the Fuzzers

### libFuzzer
```bash
# Run with corpus
./build-fuzz/fuzz_string_conversion corpus/string/

# Run with options
./build-fuzz/fuzz_string_conversion \
    -max_len=4096 \
    -timeout=10 \
    -jobs=4 \
    corpus/string/
```

### AFL++
```bash
afl-fuzz -i corpus/string -o findings -- ./build-afl/fuzz_string_conversion
```

## Seed Corpus

Create minimal valid inputs for each target:

```bash
# String corpus
echo -n "" > corpus/string/empty
echo -n "hello" > corpus/string/simple
echo -n "hello world" > corpus/string/spaces
printf '\x00\x01\x02\xff' > corpus/string/binary
printf 'test\x00null' > corpus/string/embedded_null

# Numeric corpus (binary representations)
printf '\x00\x00\x00\x00' > corpus/numeric/zero
printf '\xff\xff\xff\x7f' > corpus/numeric/max_int32
printf '\x00\x00\x00\x80' > corpus/numeric/min_int32

# Array corpus
printf '\x01\x00\x00\x00' > corpus/array/one_element
printf '\x01\x00\x00\x00\x02\x00\x00\x00\x03\x00\x00\x00' > corpus/array/three_elements
```

## Key Implementation Details

### Ruby GC Considerations

**This is critical for fuzzing Rice correctly.**

Ruby's GC can run at unpredictable times, which creates challenges:

1. **Objects may be collected mid-operation** - A VALUE created at the start of a fuzz iteration might be GC'd before the test completes
2. **Stack scanning** - Ruby's conservative GC scans the C stack, but fuzzer-generated data patterns might confuse it
3. **GC timing bugs** - Real bugs might only manifest when GC runs at specific moments

#### Mitigation Strategies

```cpp
// 1. Disable GC during fuzz iteration (simplest)
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    rb_gc_disable();

    // ... fuzzing logic ...

    rb_gc_enable();
    rb_gc_start();  // Force GC between iterations to catch dangling refs
    return 0;
}

// 2. Pin objects to prevent collection
VALUE rb_str = make_ruby_string(data, size);
rb_gc_register_address(&rb_str);  // Pin
// ... use rb_str ...
rb_gc_unregister_address(&rb_str);  // Unpin

// 3. Use volatile to prevent compiler from optimizing away
volatile VALUE rb_str = make_ruby_string(data, size);
RB_GC_GUARD(rb_str);  // Ruby's guard macro
```

#### Two Fuzzing Modes

Consider running fuzzers in two modes:

1. **GC Disabled Mode** - Find logic bugs without GC interference
   ```cpp
   rb_gc_disable();
   // ... fuzz ...
   rb_gc_enable();
   ```

2. **GC Stress Mode** - Find GC-related bugs
   ```cpp
   rb_gc_stress_set(Qtrue);  // Force GC on every allocation
   // ... fuzz ...
   rb_gc_stress_set(Qfalse);
   ```

#### Updated Harness Template

```cpp
#include <ruby.h>

// Options controlled by environment or compile flags
static bool gc_disabled = false;
static bool gc_stress = false;

extern "C" int LLVMFuzzerInitialize(int* argc, char*** argv) {
    ruby_init();
    ruby_init_loadpath();

    // Check environment for mode
    if (getenv("FUZZ_GC_DISABLED")) {
        gc_disabled = true;
    }
    if (getenv("FUZZ_GC_STRESS")) {
        gc_stress = true;
        rb_gc_stress_set(Qtrue);
    }

    return 0;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    if (gc_disabled) rb_gc_disable();

    int state = 0;
    rb_protect([](VALUE arg) -> VALUE {
        // Create objects and immediately pin them
        VALUE obj = /* ... */;
        volatile VALUE guard = obj;
        RB_GC_GUARD(guard);

        // ... test logic ...

        return Qnil;
    }, (VALUE)data, &state);

    if (state) rb_set_errinfo(Qnil);

    if (gc_disabled) {
        rb_gc_enable();
        rb_gc_start();  // Clean up between iterations
    }

    return 0;
}
```

#### What GC Bugs Look Like

When fuzzing finds GC-related issues, you might see:
- Use-after-free in ASan (object was collected)
- Segfaults accessing VALUE that's now invalid
- Corruption when GC moves objects (compacting GC in Ruby 3.x)
- Memory leaks (objects never freed due to incorrect prevent_gc)

#### Testing keepAlive Specifically

Since Rice has `keepAlive()` for preventing GC of objects stored in containers, consider a dedicated harness:

```cpp
// fuzz_keepalive.cpp - Test GC safety of container operations
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    rb_gc_stress_set(Qtrue);  // Aggressive GC

    // Create a vector, add Ruby objects, trigger GC, access objects
    VALUE vec = /* create std::vector wrapper */;
    VALUE obj = /* create object */;

    // Add to vector (should keepAlive)
    rb_funcall(vec, rb_intern("push"), 1, obj);

    // Force multiple GC cycles
    for (int i = 0; i < 10; i++) {
        rb_gc_start();
    }

    // Access the object - should still be valid
    VALUE retrieved = rb_funcall(vec, rb_intern("[]"), 1, INT2NUM(0));
    // Use retrieved...

    rb_gc_stress_set(Qfalse);
    return 0;
}
```

### Ruby VM Initialization
The harnesses need to initialize Ruby VM once at startup:

```cpp
class RubyVM {
    static RubyVM& instance() {
        static RubyVM vm;
        return vm;
    }
private:
    RubyVM() {
        ruby_init();
        ruby_init_loadpath();
    }
    ~RubyVM() { ruby_cleanup(0); }
};
```

### Exception Safety
All fuzzer code must catch exceptions to avoid false positives:

```cpp
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    int state = 0;
    rb_protect([](VALUE arg) -> VALUE {
        // Fuzzing logic here
        return Qnil;
    }, (VALUE)data, &state);

    if (state) rb_set_errinfo(Qnil);  // Clear error
    return 0;
}
```

### Data Provider Pattern
Use a helper class to consume fuzz input in a structured way:

```cpp
class FuzzDataProvider {
    const uint8_t* data_;
    size_t size_, offset_ = 0;
public:
    template<typename T> T consume();
    std::string consume_string(size_t max_len);
    bool empty() const;
};
```

## Triage and Analysis

When fuzzer finds crashes:
1. Crashes saved to `findings/crashes/` (AFL++) or current dir (libFuzzer)
2. Reproduce: `./fuzz_target crash_file`
3. Debug: `gdb ./fuzz_target` then `run crash_file`
4. Analyze with ASan output for memory issues

## CI Integration (Future)

Add to GitHub Actions:
```yaml
fuzz:
  runs-on: ubuntu-latest
  steps:
    - uses: actions/checkout@v4
    - name: Build fuzzers
      run: |
        CC=clang CXX=clang++ cmake -B build-fuzz -S fuzz
        cmake --build build-fuzz
    - name: Run fuzzers (short)
      run: |
        ./build-fuzz/fuzz_string_conversion -max_total_time=60 corpus/string/
```

## Next Steps

1. Boot to Linux
2. Create the `fuzz/` directory structure
3. Implement harnesses one at a time, testing each
4. Build seed corpus
5. Run initial fuzzing campaign
6. Triage any findings
7. Add CI integration
