# data_json_fusion.h

**Path:** `artdaq-database/BasicTypes/data_json_fusion.h`

**Purpose:** Adapts the JsonData structure for use with Boost.Fusion, enabling compile-time reflection and generic programming capabilities. This allows JsonData to participate in generic algorithms, serialization frameworks, and metaprogramming operations alongside FhiclData and XmlData.


## Key Concepts

### Boost.Fusion Overview

Boost.Fusion bridges compile-time and runtime programming by treating C++ structures as sequences that can be traversed at compile time. Key capabilities:

- **Generic iteration:** Use `boost::fusion::for_each()` to iterate over struct members
- **Indexed access:** Use `boost::fusion::at_c<N>()` to access the Nth member
- **Type introspection:** Query member count and types at compile time
- **Serialization integration:** Many serialization libraries support Fusion-adapted types

### Why Fusion Adaptation?

The Fusion adaptation enables:
1. **Generic algorithms** that work uniformly on JsonData, FhiclData, and XmlData
2. **Compile-time type safety** when accessing struct members
3. **Integration** with serialization frameworks and template metaprogramming
4. **Uniform interface** across all BasicTypes despite different internal buffer names

### Uniform Adaptation Pattern

All BasicTypes (JsonData, FhiclData, XmlData) are adapted identically with a single string buffer member:

| Type | Adapted Member | Index |
|------|----------------|-------|
| JsonData | `json_buffer` | 0 |
| FhiclData | `fhicl_buffer` | 0 |
| XmlData | `xml_buffer` | 0 |

This uniformity enables truly generic code that operates interchangeably on any BasicType.

### Fusion Sequence Properties

After adaptation, JsonData becomes a Boost.Fusion sequence with these properties:
- **Size:** 1 (single adapted member)
- **Member 0:** `json_buffer` of type `std::string`
- **Iteration:** Visits `json_buffer` only

**Note:** FhiclData has a second member (`fhicl_file_name`) that is NOT adapted, keeping the uniform single-member interface.

## Thread Safety

- **Thread-safe:** N/A (header-only, no runtime state)
- **Concurrent access:** Boost.Fusion operations on const objects are thread-safe; concurrent modification of JsonData objects requires external synchronization
- **Locking:** None required for Fusion operations themselves

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/BasicTypes/data_json.h` | JsonData class definition |
| `<boost/fusion/adapted/struct/adapt_struct.hpp>` | `BOOST_FUSION_ADAPT_STRUCT` macro for adapting structs |
| `<boost/fusion/include/adapt_struct.hpp>` | Boost.Fusion include helpers |

## Macros

### `BOOST_FUSION_ADAPT_STRUCT`

```cpp
namespace cfg = artdaq::database::basictypes;

BOOST_FUSION_ADAPT_STRUCT(cfg::JsonData, (std::string, json_buffer))
```

**Brief:** Adapts JsonData to be a Boost.Fusion sequence with one element, enabling compile-time reflection and generic algorithms.

**Parameters:**
1. `cfg::JsonData` - The fully-qualified struct type being adapted
2. `(std::string, json_buffer)` - Member specification: (type, name)

**Preconditions:**
- JsonData must be defined before this macro is used
- Must be at namespace scope (not inside a function)

**Postconditions:**
- JsonData can be used with all Boost.Fusion algorithms
- JsonData becomes a valid Fusion sequence

**Thread Safety:** N/A (compile-time macro)

**Effect:** After this macro, JsonData can be used with all Boost.Fusion algorithms:
- `boost::fusion::at_c<0>(json)` - Access `json_buffer`
- `boost::fusion::for_each(json, func)` - Iterate over members (just one)
- `boost::fusion::size<JsonData>::value` - Returns 1
- `boost::fusion::result_of::value_at_c<JsonData, 0>::type` - Returns `std::string`

## Usage Examples

### Member Access by Index

```cpp
#include "artdaq-database/BasicTypes/data_json_fusion.h"
#include <boost/fusion/include/at_c.hpp>
#include <iostream>

using namespace artdaq::database::basictypes;

void accessByIndex() {
  try {
    JsonData json(R"({"key": "value"})");

    // Access the buffer via Fusion (returns reference)
    std::string& buffer = boost::fusion::at_c<0>(json);
    std::cout << "Buffer content: " << buffer << "\n";

    // Modify via Fusion access (be careful - no validation)
    buffer = R"({"modified": true})";
    std::cout << "Modified content: " << json.json_buffer << "\n";

  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << "\n";
  }
}
```

### Iteration Over Members

```cpp
#include "artdaq-database/BasicTypes/data_json_fusion.h"
#include <boost/fusion/include/for_each.hpp>
#include <iostream>

using namespace artdaq::database::basictypes;

struct PrintMember {
  template<typename T>
  void operator()(const T& member) const {
    std::cout << "Member value: " << member << "\n";
  }
};

void iterateMembers() {
  try {
    JsonData json(R"({"key": "value"})");

    // Prints the single adapted member (json_buffer)
    boost::fusion::for_each(json, PrintMember());

  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << "\n";
  }
}
```

### Compile-Time Information

```cpp
#include "artdaq-database/BasicTypes/data_json_fusion.h"
#include <boost/fusion/include/size.hpp>
#include <boost/fusion/include/value_at.hpp>
#include <type_traits>

using namespace artdaq::database::basictypes;

void compileTimeInfo() {
  // Get member count at compile time
  constexpr auto count = boost::fusion::result_of::size<JsonData>::value;
  static_assert(count == 1, "JsonData has 1 adapted member");

  // Get member type at compile time
  using BufferType = typename boost::fusion::result_of::value_at_c<JsonData, 0>::type;
  static_assert(std::is_same<BufferType, std::string>::value,
                "First member is std::string");

  // This information is available at compile time - no runtime cost
}
```

### Generic Code for All BasicTypes

```cpp
#include "artdaq-database/BasicTypes/data_json_fusion.h"
#include "artdaq-database/BasicTypes/data_fhicl_fusion.h"
#include "artdaq-database/BasicTypes/data_xml_fusion.h"
#include <boost/fusion/include/at_c.hpp>
#include <iostream>

using namespace artdaq::database::basictypes;

// Generic function that works with any Fusion-adapted BasicType
template<typename T>
void processConfig(const T& config) {
  const auto& buffer = boost::fusion::at_c<0>(config);
  std::cout << "Configuration size: " << buffer.length() << " bytes\n";
}

// Generic validation for any BasicType
template<typename T>
bool isConfigEmpty(const T& config) {
  return boost::fusion::at_c<0>(config).empty();
}

// Generic buffer extraction
template<typename T>
std::string extractBuffer(const T& config) {
  return boost::fusion::at_c<0>(config);
}

void useGenericFunctions() {
  try {
    JsonData json(R"({"key": "value"})");
    FhiclData fhicl("param: value");
    XmlData xml("<config/>");

    // Same function works with all types!
    std::cout << "Processing JSON:\n";
    processConfig(json);

    std::cout << "Processing FHiCL:\n";
    processConfig(fhicl);

    std::cout << "Processing XML:\n";
    processConfig(xml);

    // Generic validation
    std::cout << "\nEmpty checks:\n";
    std::cout << "JSON empty: " << (isConfigEmpty(json) ? "yes" : "no") << "\n";
    std::cout << "FHiCL empty: " << (isConfigEmpty(fhicl) ? "yes" : "no") << "\n";
    std::cout << "XML empty: " << (isConfigEmpty(xml) ? "yes" : "no") << "\n";

  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << "\n";
  }
}
```

### Generic Serialization Example

```cpp
#include "artdaq-database/BasicTypes/data_json_fusion.h"
#include <boost/fusion/include/for_each.hpp>
#include <sstream>
#include <iostream>

using namespace artdaq::database::basictypes;

// Generic serializer that works with any Fusion-adapted type
template<typename DataType>
std::string serialize(const DataType& data) {
  std::ostringstream ss;
  boost::fusion::for_each(data, [&ss](const auto& member) {
    ss << member;
  });
  return ss.str();
}

void demonstrateSerialization() {
  try {
    JsonData json(R"({"serialized": true})");

    std::string serialized = serialize(json);
    std::cout << "Serialized length: " << serialized.size() << " bytes\n";

    // Works identically with other BasicTypes
    FhiclData fhicl("param: value");
    std::string fhicl_serialized = serialize(fhicl);
    std::cout << "FHiCL serialized length: " << fhicl_serialized.size() << " bytes\n";

  } catch (const std::exception& e) {
    std::cerr << "Serialization error: " << e.what() << "\n";
  }
}
```

## Relationship to Other Components

### All BasicTypes Have Identical Fusion Interfaces

```cpp
// All adapted the same way - single member at index 0
BOOST_FUSION_ADAPT_STRUCT(cfg::JsonData, (std::string, json_buffer))
BOOST_FUSION_ADAPT_STRUCT(cfg::FhiclData, (std::string, fhicl_buffer))
BOOST_FUSION_ADAPT_STRUCT(cfg::XmlData, (std::string, xml_buffer))
```

This enables generic code that processes configuration regardless of format.

### Files in Fusion Family

| File | Purpose |
|------|---------|
| `data_json_fusion.h` | Boost.Fusion adaptation for JsonData (this file) |
| `data_fhicl_fusion.h` | Boost.Fusion adaptation for FhiclData |
| `data_xml_fusion.h` | Boost.Fusion adaptation for XmlData |

## See Also

- [data_json.h](./data_json.h.md) - JsonData class definition
- [data_fhicl_fusion.h](./data_fhicl_fusion.h.md) - FhiclData Fusion adaptation
- [data_xml_fusion.h](./data_xml_fusion.h.md) - XmlData Fusion adaptation
- [External: Boost.Fusion](https://www.boost.org/doc/libs/release/libs/fusion/) - Boost.Fusion documentation

## Notes for Developers

### When to Include This Header

**Include `data_json_fusion.h` when:**
- Using Boost.Fusion algorithms with JsonData
- Writing generic code that operates on any BasicType
- Implementing serialization that uses Fusion
- Need compile-time member introspection

**For basic JsonData usage, include `data_json.h` instead** - it is lighter weight and does not bring in Boost.Fusion dependencies.

### Adding New Members to JsonData

If you add a new member to JsonData:

```cpp
// If adding a new member:
struct JsonData {
    std::string json_buffer;
    int version;  // New member
};

// Update adaptation:
BOOST_FUSION_ADAPT_STRUCT(cfg::JsonData,
    (std::string, json_buffer)
    (int, version)
)
```

**Warning:** This would break the uniform interface with FhiclData and XmlData. Generic code assumes all BasicTypes have exactly one adapted member. Consider the impact before adding.

### Common Pitfalls

- **Pitfall 1:** Including Fusion headers when not needed. They significantly increase compilation time due to heavy template instantiation.
- **Pitfall 2:** Assuming all members are adapted. FhiclData's `fhicl_file_name` is NOT adapted intentionally.
- **Pitfall 3:** Modifying one type's adaptation without updating the others. Keep all three BasicTypes consistent for generic code to work correctly.
- **Pitfall 4:** Using Fusion access without understanding it returns references - modifications affect the original object.

### Compilation Impact

Boost.Fusion is heavily template-based:

| Factor | Impact |
|--------|--------|
| Include this header | Moderate increase in compile time |
| Use Fusion algorithms | Further increase per algorithm used |
| Template instantiation | Generates code for each type used |

**Best practices:**
- Only include in files that need Fusion capabilities
- Forward declare when possible
- Consider precompiled headers for heavy Fusion usage

### Type Safety

Boost.Fusion provides compile-time type safety:

```cpp
JsonData json(R"({"key": "value"})");

// Correct - std::string& returned
std::string& buffer = boost::fusion::at_c<0>(json);

// Compile error - index out of range
// auto& x = boost::fusion::at_c<1>(json);  // Error: JsonData has only 1 member

// Compile error - wrong type assignment
// int& x = boost::fusion::at_c<0>(json);  // Error: cannot bind int& to std::string
```

### Anti-patterns

```cpp
// DON'T do this - including Fusion header when not using Fusion features:
#include "artdaq-database/BasicTypes/data_json_fusion.h"
// ...only using json.json_buffer directly...

// DO this instead - use the lighter header:
#include "artdaq-database/BasicTypes/data_json.h"

// DON'T do this - modifying via Fusion without understanding it's a reference:
void badModification(JsonData json) {  // Note: passed by value
  boost::fusion::at_c<0>(json) = "modified";  // Modifies local copy!
}

// DO this instead - be explicit about intentions:
void goodModification(JsonData& json) {  // Pass by reference
  boost::fusion::at_c<0>(json) = "modified";  // Modifies original
}

// Or use const for read-only access:
void readOnly(const JsonData& json) {
  const std::string& buffer = boost::fusion::at_c<0>(json);
  // Cannot modify through const reference
}
```
