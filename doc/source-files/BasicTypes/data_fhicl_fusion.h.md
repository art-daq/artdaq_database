# data_fhicl_fusion.h

## File Overview

**Location**: `/home/user/artdaq-database/artdaq-database/BasicTypes/data_fhicl_fusion.h`

This header adapts the `FhiclData` struct for use with Boost.Fusion, enabling reflection-like capabilities for FHICL data structures. This allows FhiclData to participate in generic algorithms, serialization, and metaprogramming operations.

**Purpose**: Enables Boost.Fusion introspection and manipulation of FhiclData structures.

## Dependencies

- `artdaq-database/BasicTypes/data_fhicl.h` - FhiclData class definition
- `<boost/fusion/adapted/struct/adapt_struct.hpp>` - Boost.Fusion adaptation macros
- `<boost/fusion/include/adapt_struct.hpp>` - Boost.Fusion include helpers

## What is Boost.Fusion?

See `data_json_fusion.h.md` for a comprehensive explanation of Boost.Fusion.

**In Brief**: Boost.Fusion treats structs as compile-time sequences, enabling:
- Generic iteration over members
- Automatic serialization
- Reflection-like capabilities
- Compile-time algorithms

## Namespace Alias

```cpp
namespace cfg = artdaq::database::basictypes;
```

**Purpose**: Short alias for the verbose namespace, improving readability.

## Adaptation

### BOOST_FUSION_ADAPT_STRUCT

```cpp
BOOST_FUSION_ADAPT_STRUCT(cfg::FhiclData, (std::string, fhicl_buffer))
```

**Purpose**: Adapts FhiclData to be a Boost.Fusion sequence.

**Adapted Members**:
- `fhicl_buffer` (type: `std::string`) - The FHICL configuration buffer

**Note**: Only `fhicl_buffer` is adapted, not `fhicl_file_name`

### Why Only fhicl_buffer?

The adaptation includes only `fhicl_buffer` because:

1. **Primary Data**: `fhicl_buffer` is the actual configuration content
2. **Serialization**: Only the buffer needs to be serialized/deserialized
3. **Consistency**: Matches the pattern used in `JsonData` and `XmlData`
4. **Metadata**: `fhicl_file_name` is metadata, not essential data

**Implication**: When using Fusion algorithms, you'll only iterate over `fhicl_buffer`.

## What This Enables

### 1. Member Access by Index

```cpp
#include <boost/fusion/include/at_c.hpp>

FhiclData fhicl("parameter: value");

// Access fhicl_buffer by index
std::string& buffer = boost::fusion::at_c<0>(fhicl);
std::cout << "Buffer: " << buffer << "\n";
```

### 2. Generic Iteration

```cpp
#include <boost/fusion/include/for_each.hpp>

struct Printer {
    template<typename T>
    void operator()(T& member) const {
        std::cout << "Member: " << member << "\n";
    }
};

FhiclData fhicl("param: value");
boost::fusion::for_each(fhicl, Printer());
// Output: Member: param: value
```

### 3. Serialization

```cpp
// Generic serialization that works with all BasicTypes
template<typename DataType>
void serialize_to_db(const DataType& data) {
    boost::fusion::for_each(data, db_writer());
}

FhiclData fhicl(...);
serialize_to_db(fhicl);  // Works via Fusion adaptation
```

### 4. Type Information

```cpp
#include <boost/fusion/include/size.hpp>

// Number of adapted members (compile-time constant)
constexpr auto member_count = boost::fusion::result_of::size<FhiclData>::value;
static_assert(member_count == 1, "FhiclData has 1 adapted member");
```

## Usage Context

### Generic Functions with Multiple Types

The Fusion adaptation enables writing generic code that works with FhiclData, JsonData, and XmlData uniformly:

```cpp
// Works with all three BasicTypes
template<typename ConfigType>
std::string extract_buffer(const ConfigType& config) {
    return boost::fusion::at_c<0>(config);
}

// Usage
FhiclData fhicl("fhicl: config");
JsonData json("{\"json\": \"config\"}");
XmlData xml("<xml>config</xml>");

std::string fhicl_buf = extract_buffer(fhicl);  // Works
std::string json_buf = extract_buffer(json);    // Works
std::string xml_buf = extract_buffer(xml);      // Works
```

### Database Operations

```cpp
template<typename DataType>
class DatabaseSerializer {
public:
    void save(const DataType& data) {
        boost::fusion::for_each(data, [this](const auto& field) {
            db_.write(field);
        });
    }

private:
    Database& db_;
};

// Works with FhiclData
FhiclData config(...);
DatabaseSerializer<FhiclData> serializer(db);
serializer.save(config);
```

### Validation

```cpp
struct BufferValidator {
    bool& is_valid;

    template<typename T>
    void operator()(const T& buffer) {
        is_valid = is_valid && !buffer.empty();
    }
};

bool validate_config(const FhiclData& fhicl) {
    bool valid = true;
    boost::fusion::for_each(fhicl, BufferValidator{valid});
    return valid;
}
```

## Comparison with Other BasicTypes

All three BasicTypes follow the same adaptation pattern:

### FhiclData
```cpp
BOOST_FUSION_ADAPT_STRUCT(cfg::FhiclData, (std::string, fhicl_buffer))
```
- 1 adapted member
- `fhicl_file_name` is NOT adapted (metadata only)

### JsonData
```cpp
BOOST_FUSION_ADAPT_STRUCT(cfg::JsonData, (std::string, json_buffer))
```
- 1 adapted member
- Consistent with FhiclData

### XmlData
```cpp
BOOST_FUSION_ADAPT_STRUCT(cfg::XmlData, (std::string, xml_buffer))
```
- 1 adapted member
- Consistent with FhiclData and JsonData

**Result**: All three types can be used interchangeably in generic Fusion-based code.

## Header Guards

```cpp
#ifndef _ARTDAQ_DATABASE_BASICTYPES_FHICL_FUSION_H_
#define _ARTDAQ_DATABASE_BASICTYPES_FHICL_FUSION_H_
```

## Notes for Developers

### Adding Members to FhiclData

If you add new members to `FhiclData`:

```cpp
struct FhiclData {
    std::string fhicl_buffer;
    std::string fhicl_file_name;
    int version;  // New member
};
```

Decide whether to adapt it:

```cpp
// Option 1: Adapt all essential data
BOOST_FUSION_ADAPT_STRUCT(cfg::FhiclData,
    (std::string, fhicl_buffer)
    (int, version)
)
// Don't include metadata like fhicl_file_name

// Option 2: Keep adaptation unchanged if new member is metadata
BOOST_FUSION_ADAPT_STRUCT(cfg::FhiclData,
    (std::string, fhicl_buffer)
)
```

### Accessing Non-Adapted Members

You can still access `fhicl_file_name` directly:

```cpp
FhiclData fhicl;
fhicl.fhicl_file_name = "config.fcl";  // Direct access works
std::cout << fhicl.fhicl_file_name;    // Direct access works

// But NOT via Fusion:
// auto& name = boost::fusion::at_c<1>(fhicl);  // ERROR: only 1 member adapted
```

### When to Include This Header

Include `data_fhicl_fusion.h` when:
- Using Boost.Fusion algorithms with FhiclData
- Writing generic serialization code
- Implementing metaprogramming with BasicTypes
- Need compile-time iteration over members

**Don't include** for basic FhiclData usage - use `data_fhicl.h` instead.

### Compilation Overhead

Boost.Fusion is template-heavy:
- Increases compilation time
- Generates template instantiations
- Only include where needed

## Best Practices

1. **Consistent Adaptation**: Keep all BasicTypes adapted consistently
2. **Document Changes**: Update this file if adaptation changes
3. **Essential Data Only**: Only adapt members essential for serialization
4. **Test Generic Code**: Ensure generic algorithms work with all BasicTypes

## Example: Generic Configuration Processor

```cpp
#include "artdaq-database/BasicTypes/data_fhicl_fusion.h"
#include "artdaq-database/BasicTypes/data_json_fusion.h"
#include "artdaq-database/BasicTypes/data_xml_fusion.h"
#include <boost/fusion/include/for_each.hpp>

// Generic processor works with any BasicType
template<typename ConfigType>
class ConfigProcessor {
public:
    void process(const ConfigType& config) {
        std::cout << "Processing configuration:\n";

        boost::fusion::for_each(config, [](const auto& buffer) {
            std::cout << "  Buffer size: " << buffer.length() << " bytes\n";
            std::cout << "  Content preview: "
                      << buffer.substr(0, 50) << "...\n";
        });
    }
};

// Usage with all three types
FhiclData fhicl("fhicl_param: value");
JsonData json("{\"json_param\": \"value\"}");
XmlData xml("<xml><param>value</param></xml>");

ConfigProcessor<FhiclData> proc1;
proc1.process(fhicl);  // Works

ConfigProcessor<JsonData> proc2;
proc2.process(json);   // Works

ConfigProcessor<XmlData> proc3;
proc3.process(xml);    // Works
```

## Related Documentation

- `data_fhicl.h.md` - FhiclData class interface
- `data_fhicl.cpp.md` - Implementation details
- `data_json_fusion.h.md` - JSON Fusion adaptation (detailed Fusion explanation)
- `data_xml_fusion.h.md` - XML Fusion adaptation
- Boost.Fusion: https://www.boost.org/doc/libs/release/libs/fusion/
