# Overload Resolution

This document explains how Rice resolves C++ method overloads when called from Ruby.

## Overview

When a C++ class has multiple methods with the same name but different parameter types (overloads), Rice must determine which one to call based on the Ruby arguments provided. Rice uses a numeric scoring system where each overload receives a score from 0.0 to 1.0, and the highest-scoring overload is selected.

## Scoring Constants

The scoring system is defined in `rice/detail/from_ruby.hpp`:

```cpp
struct Convertible
{
  static constexpr double Exact = 1.0;           // Perfect type match
  static constexpr double None = 0.0;            // Cannot convert
  static constexpr double IntToFloat = 0.5;      // Penalty for int to float conversion
  static constexpr double SignedToUnsigned = 0.5;// Penalty for signed to unsigned (can't represent negatives)
  static constexpr double FloatToInt = 0.5;      // Penalty for float to int conversion
  static constexpr double ConstMismatch = 0.99;  // Penalty for const mismatch
};
```

## Precision Bits

Ruby numeric types have precision defined in terms of bits:

| Ruby Type               | Precision Bits  | Notes                       |
|-------------------------|-----------------|-----------------------------|
| Integer (Fixnum/Bignum) | 63              | Same as C++ long long       |
| Float                   | 53              | Same as C++ double mantissa |

C++ types use `std::numeric_limits<T>::digits`:

| C++ Type           | Precision Bits |
|--------------------|----------------|
| char               | 7              |
| signed char        | 7              |
| unsigned char      | 8              |
| short              | 15             |
| unsigned short     | 16             |
| int                | 31             |
| unsigned int       | 32             |
| long               | 31 or 63*      |
| unsigned long      | 32 or 64*      |
| long long          | 63             |
| unsigned long long | 64             |
| float              | 24 (mantissa)  |
| double             | 53 (mantissa)  |

\* Platform dependent

## Same-Domain Conversions

When converting within the same numeric domain (integer-to-integer or float-to-float):

**Widening conversion (target >= source bits):** Score = 1.0

**Narrowing conversion (target < source bits):** Score = targetBits / sourceBits

Example: Ruby Integer (63 bits) to C++ short (15 bits)
```
Score = 15 / 63 = 0.238
```

### Signed to Unsigned

When converting a Ruby Integer (which is signed) to an unsigned C++ type, a penalty is applied because unsigned types cannot represent negative values:

```
Score = precisionScore * SignedToUnsigned
      = precisionScore * 0.5
```

Example: Ruby Integer (63 bits) to C++ unsigned int (32 bits)
```
precisionScore = 32 / 63 = 0.508
Score = 0.508 * 0.5 = 0.254
```

This ensures signed types are preferred over unsigned types. For example, given overloads `foo(int)` and `foo(unsigned int)`:

| Overload          | Score  | Calculation |
|-------------------|--------|-------------|
| foo(int)          | 0.49   | 31/63       |
| foo(unsigned int) | 0.25   | 32/63 × 0.5 |

Result: `foo(int)` is selected.

## Cross-Domain Conversions

### Integer to Float

When converting a Ruby Integer to a C++ float type, the score combines precision loss with a domain-change penalty:

```
Score = precisionScore * IntToFloat
      = precisionScore * 0.5
```

Example: Ruby Integer (63 bits) to C++ double (53 bits)
```
precisionScore = min(63, 53) / 63 = 53/63 = 0.841
Score = 0.841 * 0.5 = 0.42
```

Example: Ruby Integer (63 bits) to C++ float (24 bits)
```
precisionScore = min(63, 24) / 63 = 24/63 = 0.381
Score = 0.381 * 0.5 = 0.19
```

### Float to Integer

When converting a Ruby Float to a C++ integer type, a penalty is applied because the fractional part is lost:

```
Score = precisionScore * FloatToInt
      = precisionScore * 0.5
```

Example: Ruby Float (53 bits) to C++ int (31 bits)
```
precisionScore = 31 / 53 = 0.585
Score = 0.585 * 0.5 = 0.292
```

Example: Ruby Float (53 bits) to C++ long long (63 bits)
```
precisionScore = 53 / 53 = 1.0 (capped, since 63 >= 53)
Score = 1.0 * 0.5 = 0.5
```

## Type Mapping Reference

The following table shows conversion scores for all Ruby-to-C++ type combinations, with the underlying calculations.

| C++ Type           | Bits | True | False | Nil | String |     Integer      |      Float       |
|--------------------|:----:|:----:|:-----:|:---:|:------:|:----------------:|:----------------:|
| bool               |  -   | 1.0  |  1.0  | 1.0 |        |                  |                  |
| char               |  7   |      |       |     |  1.0   |   0.11 = 7/63    | 0.07 = 7/53×0.5  |
| signed char        |  7   |      |       |     |  1.0   |   0.11 = 7/63    | 0.07 = 7/53×0.5  |
| unsigned char      |  8   |      |       |     |  1.0   | 0.06 = 8/63×0.5  | 0.08 = 8/53×0.5  |
| short              | 15   |      |       |     |        |   0.24 = 15/63   | 0.14 = 15/53×0.5 |
| unsigned short     | 16   |      |       |     |        | 0.13 = 16/63×0.5 | 0.15 = 16/53×0.5 |
| int                | 31   |      |       |     |        |   0.49 = 31/63   | 0.29 = 31/53×0.5 |
| unsigned int       | 32   |      |       |     |        | 0.25 = 32/63×0.5 | 0.30 = 32/53×0.5 |
| long*              | 31   |      |       |     |        |   0.49 = 31/63   | 0.29 = 31/53×0.5 |
| unsigned long*     | 32   |      |       |     |        | 0.25 = 32/63×0.5 | 0.30 = 32/53×0.5 |
| long long          | 63   |      |       |     |        |   1.0 = 63/63    |  0.50 = 1.0×0.5  |
| unsigned long long | 64   |      |       |     |        |  0.50 = 1.0×0.5  |  0.50 = 1.0×0.5  |
| float              | 24   |      |       |     |        | 0.19 = 24/63×0.5 |   0.45 = 24/53   |
| double             | 53   |      |       |     |        | 0.42 = 53/63×0.5 |   1.0 = 53/53    |

\* `long` is platform-dependent. On 64-bit systems: `long` = 63 bits, `unsigned long` = 64 bits.

**Score formulas:**
- **Integer → signed integer**: `targetBits / 63` (narrowing) or `1.0` (widening)
- **Integer → unsigned integer**: `(targetBits / 63) × 0.5` (SignedToUnsigned penalty)
- **Integer → float**: `min(targetBits, 63) / 63 × 0.5` (IntToFloat penalty)
- **Float → integer**: `min(targetBits, 53) / 53 × 0.5` (FloatToInt penalty)
- **Float → float**: `targetBits / 53` (narrowing) or `1.0` (widening)

## Default Parameters

When an overload has default parameters and the caller does not provide all arguments, a small penalty is applied for each default used:

```
parameterMatch = 0.99 ^ defaultCount
```

This ensures that overloads that exactly match the argument count are preferred over those that rely on defaults.

## Final Score Calculation

The final score for an overload is:

```
finalScore = minParameterScore * parameterMatch
```

Where:
- `minParameterScore` is the minimum score across all passed arguments
- `parameterMatch` is the penalty for default parameters (0.99 per default)

Using the minimum ensures that one bad match cannot be hidden by good matches elsewhere.

## Const Correctness

When a Ruby-wrapped C++ object is passed as an argument:

- Passing a const object to a non-const parameter: Score = 0.0 (not allowed)
- Passing a non-const object to a const parameter: Score *= 0.99 (small penalty)
- Matching constness: No penalty

## Resolution Process

The resolution happens in `rice/detail/Native.ipp`:

1. `Native::resolve()` is called when Ruby invokes a method
2. For each registered overload, `Native::matches()` computes a score
3. Overloads are sorted by score (highest first)
4. The highest-scoring overload is selected
5. If no overload scores above 0.0, an error is raised

## Examples

### Example 1: Exact Match vs Type Conversion

Given these C++ overloads:
```cpp
void foo(int x);
void foo(double x);
```

Called with Ruby Integer `foo(42)`:

| Overload | Parameter Score | Final Score |
|----------|-----------------|-------------|
| foo(int) | 1.0 (exact) | 1.0 |
| foo(double) | 0.42 (int to float) | 0.42 |

Result: `foo(int)` is selected.

Called with Ruby Float `foo(3.14)`:

| Overload | Parameter Score | Final Score |
|----------|-----------------|-------------|
| foo(int) | 0.5 (float to int) | 0.5 |
| foo(double) | 1.0 (exact) | 1.0 |

Result: `foo(double)` is selected.

### Example 2: Default Parameters

Given these C++ overloads:
```cpp
void bar(int x);
void bar(int x, int y = 0);
```

Called with `bar(1)`:

| Overload | Min Score | Param Match | Final Score |
|----------|-----------|-------------|-------------|
| bar(int) | 1.0 | 1.0 | 1.0 |
| bar(int, int=0) | 1.0 | 0.99 | 0.99 |

Result: `bar(int)` is selected because it has no defaults.

Called with `bar(1, 2)`:

| Overload | Min Score | Param Match | Final Score |
|----------|-----------|-------------|-------------|
| bar(int) | N/A | N/A | 0.0 (too many args) |
| bar(int, int=0) | 1.0 | 1.0 | 1.0 |

Result: `bar(int, int=0)` is selected.

### Example 3: Precision-Based Selection

Given these C++ overloads:
```cpp
void baz(short x);
void baz(long long x);
```

Called with Ruby Integer `baz(1)`:

| Overload | Parameter Score | Final Score |
|----------|-----------------|-------------|
| baz(short) | 15/63 = 0.238 | 0.238 |
| baz(long long) | 63/63 = 1.0 | 1.0 |

Result: `baz(long long)` is selected because it can hold the full precision.

### Example 4: Mixed Arguments

Given this C++ overload:
```cpp
void qux(int a, double b);
void qux(double a, int b);
```

Called with `qux(1, 2.0)` (Integer, Float):

| Overload | Scores | Min Score | Final |
|----------|--------|-----------|-------|
| qux(int, double) | 1.0, 1.0 | 1.0 | 1.0 |
| qux(double, int) | 0.42, 0.5 | 0.42 | 0.42 |

Result: `qux(int, double)` is selected.

Called with `qux(1.0, 2)` (Float, Integer):

| Overload | Scores | Min Score | Final |
|----------|--------|-----------|-------|
| qux(int, double) | 0.5, 0.42 | 0.42 | 0.42 |
| qux(double, int) | 1.0, 1.0 | 1.0 | 1.0 |

Result: `qux(double, int)` is selected.

### Example 5: Const Correctness

Given these C++ overloads:
```cpp
void process(MyClass& obj);
void process(const MyClass& obj);
```

Called with a non-const Ruby-wrapped MyClass:

| Overload | Score |
|----------|-------|
| process(MyClass&) | 1.0 |
| process(const MyClass&) | 0.99 |

Result: `process(MyClass&)` is selected.

Called with a const Ruby-wrapped MyClass:

| Overload | Score |
|----------|-------|
| process(MyClass&) | 0.0 (not allowed) |
| process(const MyClass&) | 1.0 |

Result: `process(const MyClass&)` is selected.

## Key Files

- `rice/detail/from_ruby.hpp` - Defines Convertible constants
- `rice/detail/from_ruby.ipp` - Implements rubyPrecisionBits(), precisionScore(), and FromRubyFundamental
- `rice/detail/Native.hpp` - Defines Resolved struct and Native class
- `rice/detail/Native.ipp` - Implements resolve(), matches(), and matchParameters()
- `rice/detail/Parameter.hpp` - Defines Parameter class
- `rice/detail/Parameter.ipp` - Implements Parameter::matches()

## See Also

- `test/test_Overloads.cpp` - Test cases for overload resolution
