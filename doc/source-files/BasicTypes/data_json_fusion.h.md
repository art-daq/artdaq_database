# data_json_fusion.h

## File Overview

**Location**: `/home/user/artdaq-database/artdaq-database/BasicTypes/data_json_fusion.h`

This header file adapts the `JsonData` struct for use with Boost.Fusion, a library that provides reflection-like capabilities for C++ structures. This adaptation enables the JsonData type to work with Boost serialization, iteration, and other metaprogramming facilities.

**Purpose**: Enables Boost.Fusion introspection and manipulation of JsonData structures.

## Dependencies

- `artdaq-database/BasicTypes/data_json.h` - JsonData class definition
- `<boost/fusion/adapted/struct/adapt_struct.hpp>` - Boost.Fusion adaptation macros
- `<boost/fusion/include/adapt_struct.hpp>` - Boost.Fusion include helpers

## What is Boost.Fusion?

Boost.Fusion bridges compile-time and runtime programming by:
- Treating structures as compile-time sequences
- Enabling iteration over struct members
- Providing algorithms that work on struct members
- Supporting serialization and deserialization

### Use Cases
- Generic serialization/deserialization
- Automatic JSON/XML/binary encoding
- Reflection-like capabilities in C++
- Generic algorithms on struct members

## Namespace Alias

```cpp
namespace cfg = artdaq::database::basictypes;
```

**Purpose**: Creates a short alias `cfg` for the verbose `artdaq::database::basictypes` namespace.

**Usage**: Makes the BOOST_FUSION_ADAPT_STRUCT macro more readable.

## Key Macros

### BOOST_FUSION_ADAPT_STRUCT

```cpp
BOOST_FUSION_ADAPT_STRUCT(cfg::JsonData, (std::string, json_buffer))
```

**Purpose**: Adapts the `JsonData` struct to be a Boost.Fusion sequence.

**Parameters**:
1. `cfg::JsonData` - The struct type being adapted
2. `(std::string, json_buffer)` - Member specification: (type, name)

**Effect**: After this macro, `JsonData` can be used with:
- `boost::fusion::at_c<N>()` - Access Nth member
- `boost::fusion::for_each()` - Iterate over members
- `boost::fusion::size()` - Get member count
- Serialization libraries (Boost.Serialization, etc.)

## What This Enables

### 1. Member Access by Index

```cpp
#include <boost/fusion/include/at_c.hpp>

JsonData json(R"({"key": "value"})");

// Access first member (json_buffer) by index
std::string& buffer = boost::fusion::at_c<0>(json);
std::cout << "Buffer: " << buffer << "\n";
```

### 2. Iteration Over Members

```cpp
#include <boost/fusion/include/for_each.hpp>

struct PrintMember {
    template<typename T>
    void operator()(T& member) const {
        std::cout << "Member: " << member << "\n";
    }
};

JsonData json(R"({"key": "value"})");
boost::fusion::for_each(json, PrintMember());
// Output: Member: {"key": "value"}
```

### 3. Serialization Support

```cpp
// Boost.Serialization can automatically serialize JsonData
template<class Archive>
void serialize(Archive& ar, JsonData& data, const unsigned int version) {
    // Automatically handles all adapted members
    boost::fusion::for_each(data, serialize_member<Archive>(ar));
}
```

### 4. Compile-Time Information

```cpp
#include <boost/fusion/include/size.hpp>

// Get number of members at compile time
constexpr auto member_count = boost::fusion::result_of::size<JsonData>::value;
static_assert(member_count == 1, "JsonData should have 1 member");
```

## Usage Context

### Where This Is Used

The Fusion adaptation is used in:

1. **Serialization Systems**: Database storage and retrieval
2. **Generic Algorithms**: Operating on configuration types uniformly
3. **Metaprogramming**: Template code that works with multiple BasicTypes
4. **Testing**: Generic test utilities that work with all data types

### Integration with artdaq-database

The artdaq-database likely uses this for:

```cpp
// Generic serialization function that works with all BasicTypes
template<typename DataType>
void serialize_to_database(const DataType& data) {
    // Boost.Fusion allows generic traversal of members
    boost::fusion::for_each(data, db_serializer());
}

// Works with JsonData, FhiclData, XmlData
serialize_to_database(JsonData(...));
serialize_to_database(FhiclData(...));
serialize_to_database(XmlData(...));
```

## Related Adaptations

Similar Fusion adaptations exist for:
- `FhiclData` - See `data_fhicl_fusion.h`
- `XmlData` - See `data_xml_fusion.h`

All three are adapted in the same way, enabling uniform treatment:

```cpp
// Generic function works with all three types
template<typename T>
void process_config(const T& config) {
    // Access first member (buffer) generically
    auto& buffer = boost::fusion::at_c<0>(config);
    std::cout << "Config: " << buffer << "\n";
}

process_config(JsonData(...));   // Works
process_config(FhiclData(...));  // Works
process_config(XmlData(...));    // Works
```

## Header Guards

```cpp
#ifndef _ARTDAQ_DATABASE_BASICTYPES_JSON_FUSION_H_
#define _ARTDAQ_DATABASE_BASICTYPES_JSON_FUSION_H_
```

Standard include guard prevents multiple inclusion.

## Notes for Developers

### When to Include This Header

Include this header when you need:
- Boost.Fusion operations on JsonData
- Generic serialization
- Metaprogramming with BasicTypes
- Iteration over struct members

**Don't include** if you just need basic JsonData functionality - use `data_json.h` instead.

### Adding New Members

If you add a new member to `JsonData`:

```cpp
struct JsonData {
    std::string json_buffer;
    int version;  // New member
};
```

Update the adaptation:

```cpp
BOOST_FUSION_ADAPT_STRUCT(cfg::JsonData,
    (std::string, json_buffer)
    (int, version)
)
```

### Compilation Impact

Including this header pulls in Boost.Fusion templates:
- Increased compilation time
- Template instantiation overhead
- Only include where needed

### Type Safety

Boost.Fusion is type-safe:
- Member access is compile-time checked
- Type mismatches cause compilation errors
- No runtime overhead for type checking

## Example Usage Scenarios

### Scenario 1: Generic JSON Extraction

```cpp
#include "data_json_fusion.h"
#include <boost/fusion/include/at_c.hpp>

template<typename DataType>
std::string extract_json(const DataType& data) {
    // Works with JsonData, FhiclData, XmlData
    // because all have a string buffer as first member
    return boost::fusion::at_c<0>(data);
}
```

### Scenario 2: Database Serialization

```cpp
#include "data_json_fusion.h"
#include <boost/fusion/include/for_each.hpp>

struct DatabaseWriter {
    template<typename T>
    void operator()(const T& field) const {
        // Write each field to database
        db_write(field);
    }
};

void save_to_db(const JsonData& json) {
    boost::fusion::for_each(json, DatabaseWriter());
}
```

### Scenario 3: Validation

```cpp
#include "data_json_fusion.h"
#include <boost/fusion/include/for_each.hpp>

struct Validator {
    bool& valid;

    template<typename T>
    void operator()(const T& field) const {
        valid = valid && !field.empty();
    }
};

bool validate(const JsonData& json) {
    bool valid = true;
    boost::fusion::for_each(json, Validator{valid});
    return valid;
}
```

## Best Practices

1. **Selective Inclusion**: Only include fusion headers where needed
2. **Namespace Aliases**: Use short aliases for readability
3. **Consistency**: Keep all BasicTypes adaptations in sync
4. **Documentation**: Document which members are adapted and in what order

## Limitations

1. **Public Members Only**: Only public members can be adapted
2. **POD-like Structs**: Works best with simple data structures
3. **No Inheritance**: Doesn't adapt inherited members automatically
4. **Order Matters**: Member order in adaptation must match declaration order

## Related Documentation

- `data_json.h.md` - JsonData class documentation
- `data_fhicl_fusion.h.md` - FHICL Fusion adaptation
- `data_xml_fusion.h.md` - XML Fusion adaptation
- Boost.Fusion documentation: https://www.boost.org/doc/libs/release/libs/fusion/

## Advanced Topics

### Custom Fusion Algorithms

You can write custom algorithms that work with adapted types:

```cpp
template<typename Sequence>
void print_all_members(const Sequence& seq) {
    boost::fusion::for_each(seq, [](const auto& member) {
        std::cout << member << "\n";
    });
}

JsonData json(...);
print_all_members(json);  // Works because of Fusion adaptation
```

### Type Introspection

```cpp
#include <boost/fusion/include/value_at.hpp>

// Get type of first member at compile time
using FirstMemberType = typename boost::fusion::result_of::value_at_c<JsonData, 0>::type;
static_assert(std::is_same<FirstMemberType, std::string>::value);
```
