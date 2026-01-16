# data_fhicl_fusion.h

**Path:** `artdaq-database/BasicTypes/data_fhicl_fusion.h`

**Purpose:** Adapts the FhiclData structure for use with Boost.Fusion, enabling compile-time reflection and generic programming capabilities. This allows FhiclData to participate in generic algorithms, serialization frameworks, and metaprogramming operations alongside JsonData and XmlData.


## Key Concepts

### Boost.Fusion

Boost.Fusion is a library that bridges compile-time and runtime programming by treating C++ structures as sequences of elements. This enables:

- **Generic iteration:** Loop over struct members without knowing their names
- **Compile-time introspection:** Query member count and types at compile time
- **Automatic serialization:** Generate serialization code automatically
- **Type-safe algorithms:** Apply algorithms to struct members with full type checking

### Uniform Adaptation Pattern

All three BasicTypes (JsonData, FhiclData, XmlData) are adapted identically with exactly one string buffer member. This deliberate design choice enables truly generic code that works interchangeably with any BasicType:

| Type | Adapted Member | Type |
|------|----------------|------|
| JsonData | `json_buffer` | `std::string` |
| FhiclData | `fhicl_buffer` | `std::string` |
| XmlData | `xml_buffer` | `std::string` |

### Selective Member Adaptation

Only `fhicl_buffer` is adapted to the Fusion sequence, not `fhicl_file_name`. This is intentional:

- **`fhicl_buffer`**: Essential configuration data, needed for serialization
- **`fhicl_file_name`**: Metadata only, not essential for generic operations
- **Consistency**: Matches JsonData and XmlData which have single adapted members

## Thread Safety

- **Thread-safe:** N/A (header-only, no runtime state)
- **Concurrent access:** Boost.Fusion operations on const objects are thread-safe; concurrent modification of FhiclData objects requires external synchronization
- **Locking:** None required for Fusion operations themselves

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/BasicTypes/data_fhicl.h` | FhiclData class definition |
| `<boost/fusion/adapted/struct/adapt_struct.hpp>` | `BOOST_FUSION_ADAPT_STRUCT` macro |
| `<boost/fusion/include/adapt_struct.hpp>` | Boost.Fusion include helpers |

## Macros

### `BOOST_FUSION_ADAPT_STRUCT`

```cpp
namespace cfg = artdaq::database::basictypes;

BOOST_FUSION_ADAPT_STRUCT(cfg::FhiclData, (std::string, fhicl_buffer))
```

**Brief:** Adapts FhiclData to be a Boost.Fusion sequence with one member, enabling compile-time iteration and generic algorithms.

**Parameters:**
- First: Fully-qualified struct type (`cfg::FhiclData`)
- Subsequent: Member specifications as `(type, name)` pairs

**Preconditions:**
- FhiclData must be defined before this macro is used
- Must be at namespace scope (not inside a function)

**Postconditions:**
- FhiclData can be used with all Boost.Fusion algorithms
- FhiclData becomes a valid Fusion sequence

**Thread Safety:** N/A (compile-time macro)

**Effect:** After this macro, FhiclData can be used with all Boost.Fusion algorithms:
- `boost::fusion::at_c<N>()` - Access Nth member (N=0 only for FhiclData)
- `boost::fusion::for_each()` - Iterate over members
- `boost::fusion::size<T>::value` - Get member count at compile time
- `boost::fusion::result_of::value_at_c<T, N>::type` - Get member type

## Usage Examples

### Member Access by Index

```cpp
#include "artdaq-database/BasicTypes/data_fhicl_fusion.h"
#include <boost/fusion/include/at_c.hpp>
#include <iostream>

using namespace artdaq::database::basictypes;

void accessByIndex() {
  try {
    FhiclData fhicl("parameter: value\nthreshold: 100");

    // Access the first (and only) adapted member
    std::string& buffer = boost::fusion::at_c<0>(fhicl);

    std::cout << "Buffer content:\n" << buffer << "\n";

    // Modify via Fusion access (be careful - no validation)
    buffer = "new_parameter: new_value";
    std::cout << "Modified content:\n" << fhicl.fhicl_buffer << "\n";

  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << "\n";
  }
}
```

### Generic Iteration

```cpp
#include "artdaq-database/BasicTypes/data_fhicl_fusion.h"
#include <boost/fusion/include/for_each.hpp>
#include <iostream>

using namespace artdaq::database::basictypes;

struct PrintMember {
  template<typename T>
  void operator()(const T& member) const {
    std::cout << "Member value:\n" << member << "\n";
  }
};

void iterateMembers() {
  try {
    FhiclData fhicl("param: value\ncount: 42");

    // Prints each adapted member (just fhicl_buffer in this case)
    boost::fusion::for_each(fhicl, PrintMember());

    // Note: fhicl_file_name is NOT printed - it is not adapted

  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << "\n";
  }
}
```

### Compile-Time Type Information

```cpp
#include "artdaq-database/BasicTypes/data_fhicl_fusion.h"
#include <boost/fusion/include/size.hpp>
#include <boost/fusion/include/value_at.hpp>
#include <type_traits>

using namespace artdaq::database::basictypes;

void compileTimeInfo() {
  // Get member count at compile time
  constexpr auto member_count = boost::fusion::result_of::size<FhiclData>::value;
  static_assert(member_count == 1, "FhiclData has 1 adapted member");

  // Get member type at compile time
  using BufferType = typename boost::fusion::result_of::value_at_c<FhiclData, 0>::type;
  static_assert(std::is_same<BufferType, std::string>::value,
                "First member is std::string");

  // This information is available at compile time - no runtime cost
}
```

### Generic Code for All BasicTypes

```cpp
#include "artdaq-database/BasicTypes/data_fhicl_fusion.h"
#include "artdaq-database/BasicTypes/data_json_fusion.h"
#include "artdaq-database/BasicTypes/data_xml_fusion.h"
#include <boost/fusion/include/at_c.hpp>
#include <iostream>

using namespace artdaq::database::basictypes;

// Generic function that works with FhiclData, JsonData, or XmlData
template<typename ConfigType>
std::string extractBuffer(const ConfigType& config) {
  return boost::fusion::at_c<0>(config);
}

// Generic validation for any BasicType
template<typename ConfigType>
bool isBufferEmpty(const ConfigType& config) {
  return boost::fusion::at_c<0>(config).empty();
}

// Generic size calculation
template<typename ConfigType>
size_t getBufferSize(const ConfigType& config) {
  return boost::fusion::at_c<0>(config).size();
}

void genericOperations() {
  try {
    FhiclData fhicl("param: value");
    JsonData json(R"({"param": "value"})");
    XmlData xml("<config><param>value</param></config>");

    // Same function works with all three types
    std::cout << "FHiCL buffer size: " << getBufferSize(fhicl) << "\n";
    std::cout << "JSON buffer size: " << getBufferSize(json) << "\n";
    std::cout << "XML buffer size: " << getBufferSize(xml) << "\n";

    // Generic validation
    std::cout << "FHiCL empty: " << (isBufferEmpty(fhicl) ? "yes" : "no") << "\n";
    std::cout << "JSON empty: " << (isBufferEmpty(json) ? "yes" : "no") << "\n";
    std::cout << "XML empty: " << (isBufferEmpty(xml) ? "yes" : "no") << "\n";

  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << "\n";
  }
}
```

### Generic Serialization

```cpp
#include "artdaq-database/BasicTypes/data_fhicl_fusion.h"
#include <boost/fusion/include/for_each.hpp>
#include <sstream>
#include <iostream>

using namespace artdaq::database::basictypes;

// Generic serializer that works with any Fusion-adapted BasicType
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
    FhiclData fhicl("detector: { threshold: 100 }");

    std::string serialized = serialize(fhicl);
    std::cout << "Serialized length: " << serialized.size() << " bytes\n";

    // Works identically with other BasicTypes
    JsonData json(R"({"key": "value"})");
    std::string json_serialized = serialize(json);
    std::cout << "JSON serialized length: " << json_serialized.size() << " bytes\n";

  } catch (const std::exception& e) {
    std::cerr << "Serialization error: " << e.what() << "\n";
  }
}
```

## Relationship to Other Components

### Consistency Across BasicTypes

All three BasicTypes follow the identical adaptation pattern:

```cpp
// All adapted identically
BOOST_FUSION_ADAPT_STRUCT(cfg::JsonData, (std::string, json_buffer))
BOOST_FUSION_ADAPT_STRUCT(cfg::FhiclData, (std::string, fhicl_buffer))
BOOST_FUSION_ADAPT_STRUCT(cfg::XmlData, (std::string, xml_buffer))
```

This enables generic code that operates uniformly on any BasicType.

### Files in Fusion Family

| File | Purpose |
|------|---------|
| `data_json_fusion.h` | Boost.Fusion adaptation for JsonData |
| `data_fhicl_fusion.h` | Boost.Fusion adaptation for FhiclData (this file) |
| `data_xml_fusion.h` | Boost.Fusion adaptation for XmlData |

## See Also

- [data_fhicl.h](./data_fhicl.h.md) - FhiclData class definition
- [data_json_fusion.h](./data_json_fusion.h.md) - JsonData Fusion adaptation
- [data_xml_fusion.h](./data_xml_fusion.h.md) - XmlData Fusion adaptation
- [External: Boost.Fusion](https://www.boost.org/doc/libs/release/libs/fusion/) - Boost.Fusion documentation

## Notes for Developers

### When to Include This Header

**Include `data_fhicl_fusion.h` when:**
- Using Boost.Fusion algorithms with FhiclData
- Writing generic code that operates on any BasicType
- Implementing serialization that uses Fusion
- Need compile-time iteration over members

**For basic FhiclData usage, include `data_fhicl.h` instead** - it is lighter weight and faster to compile.

### Accessing Non-Adapted Members

The `fhicl_file_name` member is NOT adapted and cannot be accessed via Fusion:

```cpp
FhiclData fhicl("param: value");

// This works - direct access
fhicl.fhicl_file_name = "config.fcl";
std::cout << "File: " << fhicl.fhicl_file_name << "\n";

// This FAILS to compile - only 1 member adapted
// auto& name = boost::fusion::at_c<1>(fhicl);  // Error: index out of range

// To access file_name in generic code, use direct member access
std::string filename = fhicl.fhicl_file_name;  // OK
```

### Common Pitfalls

- **Pitfall 1:** Expecting `fhicl_file_name` to be accessible via Fusion. It is intentionally not adapted to maintain consistency with JsonData and XmlData.
- **Pitfall 2:** Including Fusion headers when not needed. They increase compilation time significantly due to heavy template instantiation.
- **Pitfall 3:** Modifying the adaptation without updating JsonData and XmlData to match. All three must be kept consistent for generic code to work correctly.
- **Pitfall 4:** Using Fusion access without understanding it returns references - modifications affect the original object.

### Compilation Overhead

Boost.Fusion is template-heavy and can increase compilation time:

| Factor | Impact |
|--------|--------|
| Include this header | Moderate increase in compile time |
| Use Fusion algorithms | Further increase per algorithm used |
| Template instantiation | Generates code for each type used |

**Best practices:**
- Only include Fusion headers in files that need them
- Consider using the base `data_fhicl.h` header when Fusion is not needed
- Consider precompiled headers for heavy Fusion usage

### Adding Members to Adaptation

If you need to adapt additional FhiclData members:

```cpp
// Current (single member)
BOOST_FUSION_ADAPT_STRUCT(cfg::FhiclData, (std::string, fhicl_buffer))

// Extended (multiple members) - would break consistency with other BasicTypes!
BOOST_FUSION_ADAPT_STRUCT(cfg::FhiclData,
    (std::string, fhicl_buffer)
    (std::string, fhicl_file_name)
)
```

**Warning:** Adding members would break the uniform interface with JsonData and XmlData. Generic code assumes all BasicTypes have exactly one adapted member.

### Anti-patterns

```cpp
// DON'T do this - including Fusion header when not using Fusion features:
#include "artdaq-database/BasicTypes/data_fhicl_fusion.h"
// ...only using fhicl.fhicl_buffer directly...

// DO this instead - use the lighter header:
#include "artdaq-database/BasicTypes/data_fhicl.h"

// DON'T do this - modifying via Fusion without understanding it's a reference:
void badModification(FhiclData fhicl) {  // Note: passed by value
  boost::fusion::at_c<0>(fhicl) = "modified";  // Modifies local copy!
}

// DO this instead - be explicit about intentions:
void goodModification(FhiclData& fhicl) {  // Pass by reference
  boost::fusion::at_c<0>(fhicl) = "modified";  // Modifies original
}

// Or use const for read-only access:
void readOnly(const FhiclData& fhicl) {
  const std::string& buffer = boost::fusion::at_c<0>(fhicl);
  // Cannot modify through const reference
}

// DON'T do this - assuming fhicl_file_name is accessible via Fusion:
void wrongAssumption(const FhiclData& fhicl) {
  // auto& filename = boost::fusion::at_c<1>(fhicl);  // Compile error!
}

// DO this instead - access non-adapted members directly:
void correctAccess(const FhiclData& fhicl) {
  const std::string& filename = fhicl.fhicl_file_name;  // Direct access
}
```

### Type Safety

Boost.Fusion provides compile-time type safety:

```cpp
FhiclData fhicl("param: value");

// Correct - std::string& returned
std::string& buffer = boost::fusion::at_c<0>(fhicl);

// Compile error - index out of range (FhiclData has only 1 adapted member)
// auto& x = boost::fusion::at_c<1>(fhicl);  // Error!

// Compile error - wrong type assignment
// int& x = boost::fusion::at_c<0>(fhicl);  // Error: cannot bind int& to std::string
```
