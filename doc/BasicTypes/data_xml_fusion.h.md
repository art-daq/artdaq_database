# data_xml_fusion.h

**Path:** `artdaq-database/BasicTypes/data_xml_fusion.h`

**Purpose:** Adapts the XmlData structure for use with Boost.Fusion, enabling compile-time reflection and generic programming capabilities. This allows XmlData to participate in generic algorithms, serialization frameworks, and metaprogramming operations alongside JsonData and FhiclData.


## Key Concepts

### Boost.Fusion

Boost.Fusion provides compile-time reflection-like capabilities, enabling generic programming with struct types. For a comprehensive explanation, see [data_json_fusion.h.md](./data_json_fusion.h.md).

### Perfect Symmetry with Other BasicTypes

All three BasicTypes (JsonData, FhiclData, XmlData) are adapted identically:

| Type | Adapted Member | Type | Index |
|------|----------------|------|-------|
| JsonData | `json_buffer` | `std::string` | 0 |
| FhiclData | `fhicl_buffer` | `std::string` | 0 |
| XmlData | `xml_buffer` | `std::string` | 0 |

This deliberate design enables truly generic code that works interchangeably with any BasicType.

## Thread Safety

- **Thread-safe:** N/A (header-only, no runtime state)
- **Concurrent access:** Boost.Fusion operations on const objects are thread-safe; concurrent modification of XmlData objects requires external synchronization
- **Locking:** None required for Fusion operations themselves

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/BasicTypes/data_xml.h` | XmlData class definition |
| `<boost/fusion/adapted/struct/adapt_struct.hpp>` | `BOOST_FUSION_ADAPT_STRUCT` macro |
| `<boost/fusion/include/adapt_struct.hpp>` | Boost.Fusion include helpers |

## Namespace Alias

```cpp
namespace cfg = artdaq::database::basictypes;
```

**Brief:** Shorthand alias for the basictypes namespace, used to simplify the `BOOST_FUSION_ADAPT_STRUCT` macro invocation.

## Macros

### `BOOST_FUSION_ADAPT_STRUCT`

```cpp
BOOST_FUSION_ADAPT_STRUCT(cfg::XmlData, (std::string, xml_buffer))
```

**Brief:** Adapts XmlData to be a Boost.Fusion sequence with one element, enabling compile-time iteration and generic algorithm usage.

**Adapted Members:**
- `xml_buffer` (type: `std::string`, index: 0) - The XML configuration buffer

**Effect:** After this macro, XmlData can be used with all Boost.Fusion algorithms:
- `boost::fusion::at_c<0>(xml)` - Access `xml_buffer`
- `boost::fusion::for_each(xml, func)` - Iterate over members
- `boost::fusion::size<XmlData>::value` - Returns 1
- `boost::fusion::result_of::value_at_c<XmlData, 0>::type` - Returns `std::string`

## Usage Examples

### Uniform Member Access

```cpp
#include "artdaq-database/BasicTypes/data_xml_fusion.h"
#include "artdaq-database/BasicTypes/data_json_fusion.h"
#include "artdaq-database/BasicTypes/data_fhicl_fusion.h"
#include <boost/fusion/include/at_c.hpp>
#include <iostream>

using namespace artdaq::database::basictypes;

void uniformAccess() {
  XmlData xml("<config/>");
  JsonData json("{}");
  FhiclData fhicl("param: value");

  // Identical access pattern for all three types
  auto& xml_buf = boost::fusion::at_c<0>(xml);
  auto& json_buf = boost::fusion::at_c<0>(json);
  auto& fhicl_buf = boost::fusion::at_c<0>(fhicl);

  std::cout << "XML size: " << xml_buf.length() << "\n";
  std::cout << "JSON size: " << json_buf.length() << "\n";
  std::cout << "FHICL size: " << fhicl_buf.length() << "\n";
}
```

### Generic Algorithms

```cpp
#include "artdaq-database/BasicTypes/data_xml_fusion.h"
#include <boost/fusion/include/at_c.hpp>
#include <boost/fusion/include/for_each.hpp>

using namespace artdaq::database::basictypes;

// Generic function that works with any BasicType
template<typename ConfigType>
size_t getBufferSize(const ConfigType& config) {
  return boost::fusion::at_c<0>(config).length();
}

// Generic validation
template<typename ConfigType>
bool isBufferEmpty(const ConfigType& config) {
  return boost::fusion::at_c<0>(config).empty();
}

void useGenericFunctions() {
  XmlData xml("<config><param>value</param></config>");

  std::cout << "Buffer size: " << getBufferSize(xml) << "\n";
  std::cout << "Is empty: " << isBufferEmpty(xml) << "\n";
}
```

### Generic Configuration Validator

```cpp
#include "artdaq-database/BasicTypes/data_xml_fusion.h"
#include "artdaq-database/BasicTypes/data_json_fusion.h"
#include "artdaq-database/BasicTypes/data_fhicl_fusion.h"
#include <boost/fusion/include/for_each.hpp>

using namespace artdaq::database::basictypes;

template<typename ConfigType>
class ConfigValidator {
public:
  bool validate(const ConfigType& config) {
    bool valid = true;
    boost::fusion::for_each(config, [&valid](const auto& buffer) {
      if (buffer.empty()) {
        valid = false;
      }
    });
    return valid;
  }

  size_t totalSize(const ConfigType& config) {
    size_t total = 0;
    boost::fusion::for_each(config, [&total](const auto& buffer) {
      total += buffer.length();
    });
    return total;
  }
};

void validateConfigs() {
  XmlData xml("<config/>");
  ConfigValidator<XmlData> validator;

  std::cout << "Valid: " << validator.validate(xml) << "\n";
  std::cout << "Size: " << validator.totalSize(xml) << "\n";
}
```

### Compile-Time Type Verification

```cpp
#include "artdaq-database/BasicTypes/data_xml_fusion.h"
#include "artdaq-database/BasicTypes/data_json_fusion.h"
#include "artdaq-database/BasicTypes/data_fhicl_fusion.h"
#include <boost/fusion/include/value_at.hpp>
#include <type_traits>

using namespace artdaq::database::basictypes;

void verifyTypeConsistency() {
  // All BasicTypes have the same structure
  using XmlBufferType = typename boost::fusion::result_of::value_at_c<XmlData, 0>::type;
  using JsonBufferType = typename boost::fusion::result_of::value_at_c<JsonData, 0>::type;
  using FhiclBufferType = typename boost::fusion::result_of::value_at_c<FhiclData, 0>::type;

  // Compile-time assertions
  static_assert(std::is_same<XmlBufferType, std::string>::value, "XmlData member is std::string");
  static_assert(std::is_same<XmlBufferType, JsonBufferType>::value, "Same type as JsonData");
  static_assert(std::is_same<JsonBufferType, FhiclBufferType>::value, "Same type as FhiclData");
}
```

## Relationship to Other Components

### Consistency Across BasicTypes

All three BasicTypes follow identical adaptation:

```cpp
// All adapted the same way
BOOST_FUSION_ADAPT_STRUCT(cfg::JsonData, (std::string, json_buffer))
BOOST_FUSION_ADAPT_STRUCT(cfg::FhiclData, (std::string, fhicl_buffer))
BOOST_FUSION_ADAPT_STRUCT(cfg::XmlData, (std::string, xml_buffer))
```

### Files in Fusion Family

| File | Purpose |
|------|---------|
| `data_json_fusion.h` | Boost.Fusion adaptation for JsonData |
| `data_fhicl_fusion.h` | Boost.Fusion adaptation for FhiclData |
| `data_xml_fusion.h` | Boost.Fusion adaptation for XmlData (this file) |

## See Also

- [data_xml.h](./data_xml.h.md) - XmlData class definition
- [data_json_fusion.h](./data_json_fusion.h.md) - JsonData Fusion adaptation
- [data_fhicl_fusion.h](./data_fhicl_fusion.h.md) - FhiclData Fusion adaptation
- [External: Boost.Fusion](https://www.boost.org/doc/libs/release/libs/fusion/) - Boost.Fusion documentation

## Notes for Developers

### When to Include This Header

**Include `data_xml_fusion.h` when:**
- Using Boost.Fusion algorithms with XmlData
- Writing generic code that operates on any BasicType
- Implementing serialization that uses Fusion
- Need compile-time iteration over members

**For basic XmlData usage, include `data_xml.h` instead** - it is lighter weight and faster to compile.

### Common Pitfalls

- **Pitfall 1:** Including Fusion headers when not needed. They significantly increase compilation time due to heavy template instantiation.
- **Pitfall 2:** Expecting multiple members to be adapted. XmlData has only one adapted member (`xml_buffer`).
- **Pitfall 3:** Modifying the adaptation without updating JsonData and FhiclData to match. All three must be kept consistent for generic code to work correctly.

### Compilation Overhead

Boost.Fusion is template-heavy:

| Factor | Impact |
|--------|--------|
| Include this header | Moderate increase in compile time |
| Use Fusion algorithms | Further increase per algorithm used |
| Template instantiation | Generates code for each type used |

**Best practices:**
- Only include in files that need Fusion capabilities
- Consider precompiled headers for heavy Fusion usage
- Forward declare when possible

### Adding Members to XmlData

If you add new members to XmlData, consider whether to adapt them:

```cpp
// Current (single member)
BOOST_FUSION_ADAPT_STRUCT(cfg::XmlData, (std::string, xml_buffer))

// Extended (would break consistency!)
BOOST_FUSION_ADAPT_STRUCT(cfg::XmlData,
    (std::string, xml_buffer)
    (std::string, xml_schema)  // New member
)
```

**Warning:** Adding members would break the uniform interface. Generic code assumes all BasicTypes have exactly one adapted member. Consult with maintainers before changing.

### Type Safety

Boost.Fusion provides compile-time type safety:

```cpp
XmlData xml("<config/>");

// Correct - std::string& returned
std::string& buffer = boost::fusion::at_c<0>(xml);

// Compile error - index out of range
// auto& x = boost::fusion::at_c<1>(xml);  // Error!

// Compile error - wrong type
// int& x = boost::fusion::at_c<0>(xml);  // Error!
```
