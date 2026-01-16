# fhicl_types.cpp

**Path:** `artdaq-database/DataFormats/Fhicl/fhicl_types.cpp`

**Implements:** [fhicl_types.h](./fhicl_types.h.md)

**Purpose:** Provides explicit template specializations for the `unwrapper` utility class when working with FHiCL `value_t` types. These specializations enable type-safe access to the underlying values stored in FHiCL variant types.

## Implementation Overview

This file contains minimal but essential template specializations that bridge the generic `unwrapper` utility (defined in `shared_types.h`) with the FHiCL-specific type system. The specializations allow the `unwrapper` class to work with both mutable and const references to FHiCL values.

## Key Algorithms

### Template Specialization for value_t

The `unwrapper` class provides a uniform interface for accessing values stored in variant types. This file provides the required explicit instantiations for FHiCL's `value_t` type.

**Why this approach:** The `unwrapper` template requires explicit specialization for each variant type it supports. This separation into a .cpp file:
1. Reduces compilation time by avoiding template instantiation in every translation unit
2. Provides a single point of definition for FHiCL-specific unwrapper behavior
3. Maintains consistency with the pattern used for JSON types in the codebase

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/DataFormats/Fhicl/fhicl_types.h` | FHiCL type definitions including `value_t`, `table_t`, `sequence_t` |
| `artdaq-database/DataFormats/common.h` | Common definitions and TRACE logging macros |

## Internal Functions

### `unwrapper<value_t>::unwrapper(value_t& a)`

**Brief:** Constructor specialization that initializes an unwrapper for a mutable FHiCL value reference.

**Called by:** Generic code using `sharedtypes::unwrap()` with FHiCL values

**Purpose:** Enables the unwrapper utility to work with mutable `value_t` references, allowing modification of the underlying variant value.

### `unwrapper<const value_t>::unwrapper(const value_t& a)`

**Brief:** Constructor specialization that initializes an unwrapper for a const FHiCL value reference.

**Called by:** Generic code using `sharedtypes::unwrap()` with const FHiCL values

**Purpose:** Enables the unwrapper utility to work with const `value_t` references for read-only access to variant values.

## TRACE Configuration

```cpp
#define TRACE_NAME "fhicl_types.cpp"
```

The TRACE_NAME macro identifies this file in debug trace output. Use TRACE_CNTL to enable tracing for this component.

## Performance Considerations

- **Minimal overhead:** The specializations are simple constructor initializations with no additional logic
- **No runtime cost:** Template specializations are resolved at compile time

## Error Handling Strategy

This file does not perform error handling directly. Error handling for type mismatches in the underlying `boost::variant` occurs when calling methods on the unwrapper (such as `value_as<T>()`), which will throw `boost::bad_get` if the requested type does not match the stored type.

## Testing Notes

- **Unit tests:** Covered by tests in `test/DataFormats/` that exercise FHiCL parsing and manipulation
- **Key test cases:** Round-trip conversion tests that parse FHiCL, modify values, and regenerate FHiCL text

## Maintenance Notes

When adding new FHiCL-specific types that need to work with the unwrapper utility:
1. Add the new type definition to `fhicl_types.h`
2. Add corresponding explicit specializations in this file
3. Ensure BOOST_FUSION_ADAPT_STRUCT is called if the type needs Spirit/Karma integration

## See Also

- [fhicl_types.h](./fhicl_types.h.md) - Type definitions this file implements
- [shared_types.h](../../shared_types.h.md) - Base `unwrapper` template definition
- [data_fhicl.cpp](../../BasicTypes/data_fhicl.cpp.md) - Higher-level FHiCL data wrapper
