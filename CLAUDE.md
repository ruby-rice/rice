# Rice Project Context

## Overview
Rice is a C++ header-only library for creating Ruby bindings. It provides:
- Simple C++ syntax for wrapping and defining Ruby classes
- Automatic type conversions between C++ and Ruby
- Automatic exception handling
- Smart pointer support for garbage collection

## Build & Test

### Windows (MSVC)
```bash
build_rice.bat                    # Build the project
build/msvc-debug/test/unittest.exe  # Run tests
```

### CMake
```bash
cmake --preset msvc-debug         # Configure
cmake --build build/msvc-debug    # Build
```

## Project Structure
- `rice/` - Main header-only library source
- `rice/rice.hpp` - Combined header (generated)
- `rice/stl.hpp` - STL bindings header (generated)
- `test/` - Test suite
- `docs/` - Documentation (mkdocs)
- `examples/` - Usage examples

## Development Notes
- Requires C++17 or later
- Requires Ruby 2.7+
- Run `rake` to regenerate combined headers and run tests

## Conventions
-

## Current Work
- Ruby Kaigi 2026 talk submission (deadline: Jan 11, 2026 JST)
- Documents in `rubykaigi/`: abstract.md, pitch.md, review_committee.md
- Talk title: "Rice: A Journey from Pybind11 Envy to Wrapping OpenCV"

## Decisions Made
-
