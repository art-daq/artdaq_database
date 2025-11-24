# data_xml_fusion.h

## File Overview

**Location**: `/home/user/artdaq-database/artdaq-database/BasicTypes/data_xml_fusion.h`

This header adapts the `XmlData` struct for use with Boost.Fusion, enabling reflection-like capabilities for XML data structures. This allows XmlData to participate in generic algorithms, serialization, and metaprogramming operations alongside FhiclData and JsonData.

**Purpose**: Enables Boost.Fusion introspection and manipulation of XmlData structures.

## Dependencies

- `artdaq-database/BasicTypes/data_xml.h` - XmlData class definition
- `<boost/fusion/adapted/struct/adapt_struct.hpp>` - Boost.Fusion adaptation macros
- `<boost/fusion/include/adapt_struct.hpp>` - Boost.Fusion include helpers

## What is Boost.Fusion?

See `data_json_fusion.h.md` for a comprehensive explanation of Boost.Fusion.

**In Brief**: Boost.Fusion provides compile-time reflection-like capabilities, enabling generic programming with struct types.

## Namespace Alias

```cpp
namespace cfg = artdaq::database::basictypes;
```

**Purpose**: Short alias for the verbose namespace, improving readability.

## Adaptation

### BOOST_FUSION_ADAPT_STRUCT

```cpp
BOOST_FUSION_ADAPT_STRUCT(cfg::XmlData, (std::string, xml_buffer))
```

**Purpose**: Adapts XmlData to be a Boost.Fusion sequence.

**Adapted Members**:
- `xml_buffer` (type: `std::string`) - The XML configuration buffer

**Member Count**: 1 (only `xml_buffer`)

## Consistency Across BasicTypes

All three BasicTypes are adapted identically:

### XmlData
```cpp
BOOST_FUSION_ADAPT_STRUCT(cfg::XmlData, (std::string, xml_buffer))
```
- 1 adapted member: `xml_buffer`

### JsonData
```cpp
BOOST_FUSION_ADAPT_STRUCT(cfg::JsonData, (std::string, json_buffer))
```
- 1 adapted member: `json_buffer`

### FhiclData
```cpp
BOOST_FUSION_ADAPT_STRUCT(cfg::FhiclData, (std::string, fhicl_buffer))
```
- 1 adapted member: `fhicl_buffer`
- Note: `fhicl_file_name` is NOT adapted

**Result**: All three types have identical Fusion interfaces, enabling truly generic code.

## What This Enables

### 1. Uniform Member Access

```cpp
#include <boost/fusion/include/at_c.hpp>

// Works identically for all three types
XmlData xml("<config/>");
JsonData json("{}");
FhiclData fhicl("param: value");

auto& xml_buf = boost::fusion::at_c<0>(xml);      // Access xml_buffer
auto& json_buf = boost::fusion::at_c<0>(json);    // Access json_buffer
auto& fhicl_buf = boost::fusion::at_c<0>(fhicl);  // Access fhicl_buffer
```

### 2. Generic Algorithms

```cpp
#include <boost/fusion/include/for_each.hpp>

// Single function works with all BasicTypes
template<typename ConfigType>
size_t get_buffer_size(const ConfigType& config) {
    size_t size = 0;
    boost::fusion::for_each(config, [&size](const auto& buffer) {
        size = buffer.length();
    });
    return size;
}

// Usage
XmlData xml("<config>...</config>");
JsonData json("{...}");
FhiclData fhicl("...");

auto xml_size = get_buffer_size(xml);      // Works
auto json_size = get_buffer_size(json);    // Works
auto fhicl_size = get_buffer_size(fhicl);  // Works
```

### 3. Generic Serialization

```cpp
// Generic database writer works with all BasicTypes
template<typename DataType>
class DatabaseWriter {
public:
    void save(const std::string& key, const DataType& data) {
        boost::fusion::for_each(data, [this, &key](const auto& buffer) {
            db_.write(key, buffer);
        });
    }

private:
    Database& db_;
};

// Works with XmlData, JsonData, FhiclData
DatabaseWriter<XmlData> writer(db);
writer.save("config", xml_data);
```

### 4. Type Uniformity

```cpp
// Compile-time verification that all BasicTypes are uniform
static_assert(
    boost::fusion::result_of::size<XmlData>::value == 1,
    "XmlData has 1 member"
);
static_assert(
    boost::fusion::result_of::size<JsonData>::value == 1,
    "JsonData has 1 member"
);
static_assert(
    boost::fusion::result_of::size<FhiclData>::value == 1,
    "FhiclData has 1 member"
);
```

## Usage Context

### Complete Generic Example

```cpp
#include "artdaq-database/BasicTypes/data_xml_fusion.h"
#include "artdaq-database/BasicTypes/data_json_fusion.h"
#include "artdaq-database/BasicTypes/data_fhicl_fusion.h"
#include <boost/fusion/include/for_each.hpp>
#include <boost/fusion/include/at_c.hpp>

// Generic config processor works with all types
template<typename ConfigType>
class ConfigProcessor {
public:
    void process(const ConfigType& config) {
        std::cout << "Processing configuration\n";

        // Access buffer generically
        const auto& buffer = boost::fusion::at_c<0>(config);

        std::cout << "  Size: " << buffer.length() << " bytes\n";
        std::cout << "  Preview: "
                  << buffer.substr(0, std::min(size_t(50), buffer.length()))
                  << "...\n";
    }

    bool validate(const ConfigType& config) {
        bool valid = true;
        boost::fusion::for_each(config, [&valid](const auto& buffer) {
            valid = valid && !buffer.empty();
        });
        return valid;
    }
};

// Usage
int main() {
    XmlData xml("<daq><param>value</param></daq>");
    JsonData json(R"({"daq": {"param": "value"}})");
    FhiclData fhicl("daq: { param: value }");

    ConfigProcessor<XmlData> xml_proc;
    ConfigProcessor<JsonData> json_proc;
    ConfigProcessor<FhiclData> fhicl_proc;

    xml_proc.process(xml);      // Works
    json_proc.process(json);    // Works
    fhicl_proc.process(fhicl);  // Works

    std::cout << "XML valid: " << xml_proc.validate(xml) << "\n";
    std::cout << "JSON valid: " << json_proc.validate(json) << "\n";
    std::cout << "FHICL valid: " << fhicl_proc.validate(fhicl) << "\n";
}
```

### Database Integration Example

```cpp
// Generic database interface for all config types
template<typename ConfigType>
class ConfigDatabase {
public:
    void store(const std::string& name, const ConfigType& config) {
        // Extract buffer using Fusion
        const auto& buffer = boost::fusion::at_c<0>(config);

        // Store in database
        db_.insert(name, buffer);
        TLOG(10) << "Stored " << name << " (" << buffer.length() << " bytes)";
    }

    ConfigType retrieve(const std::string& name) {
        std::string buffer = db_.get(name);

        // Create ConfigType with buffer
        // (Fusion doesn't help with construction, handle separately)
        ConfigType config(buffer);
        return config;
    }

private:
    Database db_;
};

// Works with all three types
ConfigDatabase<XmlData> xml_db;
ConfigDatabase<JsonData> json_db;
ConfigDatabase<FhiclData> fhicl_db;
```

## Header Guards

```cpp
#ifndef _ARTDAQ_DATABASE_BASICTYPES_XML_FUSION_H_
#define _ARTDAQ_DATABASE_BASICTYPES_XML_FUSION_H_
```

## Notes for Developers

### Perfect Symmetry

The Fusion adaptations create perfect symmetry across BasicTypes:
- All have exactly 1 adapted member
- All members are `std::string`
- All members are named `*_buffer`
- All types support the same Fusion operations

**Benefit**: Write once, use with all types.

### When to Include

Include this header when:
- Writing generic code that works with multiple BasicTypes
- Using Boost.Fusion algorithms with XmlData
- Implementing serialization/deserialization
- Need compile-time introspection

**Don't include** for basic XmlData usage - use `data_xml.h` instead.

### Adding Members

If you add members to XmlData:

```cpp
struct XmlData {
    std::string xml_buffer;
    std::string schema_version;  // New member
};
```

Decide whether to adapt it:

```cpp
// Option 1: Adapt all data members
BOOST_FUSION_ADAPT_STRUCT(cfg::XmlData,
    (std::string, xml_buffer)
    (std::string, schema_version)
)

// Option 2: Keep only xml_buffer adapted (recommended for consistency)
BOOST_FUSION_ADAPT_STRUCT(cfg::XmlData,
    (std::string, xml_buffer)
)
```

**Recommendation**: Maintain symmetry with JsonData and FhiclData.

### Compilation Impact

Including Fusion headers:
- Increases compilation time
- Generates template instantiations
- Worth it for generic code
- Only include where needed

## Best Practices

1. **Maintain Consistency**: Keep all BasicTypes adapted the same way
2. **Generic Programming**: Use Fusion when writing type-agnostic code
3. **Selective Inclusion**: Only include Fusion headers when needed
4. **Document Changes**: Update documentation if adaptation changes

## Example: Type-Agnostic Validator

```cpp
#include "artdaq-database/BasicTypes/data_xml_fusion.h"
#include "artdaq-database/BasicTypes/data_json_fusion.h"
#include "artdaq-database/BasicTypes/data_fhicl_fusion.h"
#include <boost/fusion/include/for_each.hpp>

// Validator that works with any BasicType
template<typename ConfigType>
class ConfigValidator {
public:
    struct ValidationResult {
        bool valid = true;
        std::string error;
    };

    ValidationResult validate(const ConfigType& config) {
        ValidationResult result;

        boost::fusion::for_each(config, [&result](const auto& buffer) {
            if (buffer.empty()) {
                result.valid = false;
                result.error = "Empty buffer";
            }
            if (buffer.length() > MAX_SIZE) {
                result.valid = false;
                result.error = "Buffer too large";
            }
        });

        return result;
    }

private:
    static constexpr size_t MAX_SIZE = 1024 * 1024;  // 1MB
};

// Usage
XmlData xml("<config/>");
JsonData json("{}");
FhiclData fhicl("");

ConfigValidator<XmlData> xml_validator;
ConfigValidator<JsonData> json_validator;
ConfigValidator<FhiclData> fhicl_validator;

auto xml_result = xml_validator.validate(xml);
auto json_result = json_validator.validate(json);
auto fhicl_result = fhicl_validator.validate(fhicl);

if (!fhicl_result.valid) {
    std::cerr << "FHICL validation failed: "
              << fhicl_result.error << "\n";
}
```

## Comparison: With and Without Fusion

### Without Fusion (Repetitive)

```cpp
// Separate implementation for each type
size_t get_xml_size(const XmlData& xml) {
    return xml.xml_buffer.length();
}

size_t get_json_size(const JsonData& json) {
    return json.json_buffer.length();
}

size_t get_fhicl_size(const FhiclData& fhicl) {
    return fhicl.fhicl_buffer.length();
}
```

### With Fusion (Generic)

```cpp
// Single implementation for all types
template<typename ConfigType>
size_t get_config_size(const ConfigType& config) {
    return boost::fusion::at_c<0>(config).length();
}

// Works with XmlData, JsonData, FhiclData
```

**Benefit**: Less code, easier maintenance, compile-time type safety.

## Related Documentation

- `data_xml.h.md` - XmlData class interface
- `data_xml.cpp.md` - Implementation details
- `data_json_fusion.h.md` - JSON Fusion adaptation (detailed Fusion explanation)
- `data_fhicl_fusion.h.md` - FHICL Fusion adaptation
- Boost.Fusion: https://www.boost.org/doc/libs/release/libs/fusion/

## Advanced: Compile-Time Type Introspection

```cpp
#include <boost/fusion/include/value_at.hpp>
#include <type_traits>

// Get type of first member at compile time
using XmlBufferType = typename boost::fusion::result_of::value_at_c<XmlData, 0>::type;
using JsonBufferType = typename boost::fusion::result_of::value_at_c<JsonData, 0>::type;
using FhiclBufferType = typename boost::fusion::result_of::value_at_c<FhiclData, 0>::type;

// All are std::string
static_assert(std::is_same<XmlBufferType, std::string>::value);
static_assert(std::is_same<JsonBufferType, std::string>::value);
static_assert(std::is_same<FhiclBufferType, std::string>::value);

// All three types have the same structure
static_assert(std::is_same<XmlBufferType, JsonBufferType>::value);
static_assert(std::is_same<JsonBufferType, FhiclBufferType>::value);
```

This compile-time verification ensures all BasicTypes maintain structural consistency.
