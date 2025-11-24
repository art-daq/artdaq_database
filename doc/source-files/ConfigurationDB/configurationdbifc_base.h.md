# configurationdbifc_base.h

## File Overview

This header file defines the base infrastructure for configuration serialization in artdaq-database. It provides template classes for wrapping user configuration objects and making them serializable to/from JSON and FHiCL formats, along with helper structures for version management.

**Location**: `/home/user/artdaq-database/artdaq-database/ConfigurationDB/configurationdbifc_base.h`

**Lines of Code**: 203

**Purpose**: Base classes and templates for configuration serialization and version management

## Dependencies

### Standard Library
- `<utility>` - std::pair for results

### Project Headers
- `"artdaq-database/BasicTypes/basictypes.h"` - FhiclData, JsonData types
- `"artdaq-database/ConfigurationDB/configurationdb.h"` - Configuration database types
- `"artdaq-database/DataFormats/Json/json_reader.h"` - JSON parsing
- `"artdaq-database/DataFormats/Json/json_writer.h"` - JSON serialization
- `"artdaq-database/ConfigurationDB/options_operation_managedocument.h"` - Document operations

## Namespace: artdaq::database::configuration

All types and functions in this file are declared within the `artdaq::database::configuration` namespace.

## Helper Functions

### make_error_msg
```cpp
inline auto make_error_msg = [](const char* msg) {
    return std::string("{error:\"").append(msg).append("\"}");
};
```

**Purpose**: Lambda function to format error messages as JSON objects.

**Parameters**:
- `msg` - Error message string

**Returns**: JSON-formatted error string

**Usage Example**:
```cpp
auto error = make_error_msg("Connection failed");
// Returns: "{error:"Connection failed"}"
```

---

## Template Class: MakeSerializable<CONF>

### Purpose

`MakeSerializable` is a template class that wraps user configuration objects and provides serialization/deserialization methods. Users must specialize this template for their custom configuration types.

### Template Parameters

- **CONF**: User configuration type (must be pointer or reference type)

### Static Assertions

```cpp
static_assert(std::is_pointer<CONF>::value || std::is_reference<CONF>::value,
              "Template parameter must be either a pointer or reference type");
```

**Purpose**: Enforces that configuration types must be passed by pointer or reference.

**Rationale**: Ensures efficient handling and allows mutation of configuration objects.

---

### Public Methods

#### writeDocument
```cpp
template <class TYPE>
bool writeDocument(TYPE& data) const
```

**Purpose**: Serialize the configuration to JSON or FHiCL format.

**Template Parameters**:
- `TYPE` - Output format type (`JsonData` or `FhiclData`)

**Parameters**:
- `data` - Output parameter to receive serialized data

**Returns**: `true` on success, `false` on failure

**Throws**: `artdaq::database::exception` with detailed error message

**Usage**:
```cpp
MakeSerializable<MyConfig*> serializer(config_ptr);
JsonData json_output;

if (serializer.writeDocument(json_output)) {
    // Use json_output
} else {
    // Handle error
}
```

**Implementation**: Delegates to `writeDocumentImpl` which must be specialized by user.

---

#### readDocument
```cpp
template <class TYPE>
bool readDocument(TYPE const& data)
```

**Purpose**: Deserialize JSON or FHiCL data into the configuration object.

**Template Parameters**:
- `TYPE` - Input format type (`JsonData` or `FhiclData`)

**Parameters**:
- `data` - Input data to deserialize

**Returns**: `true` on success, `false` on failure

**Throws**: `artdaq::database::exception` with detailed error message

**Usage**:
```cpp
MakeSerializable<MyConfig*> serializer(config_ptr);
JsonData json_input = loadFromFile("config.json");

if (serializer.readDocument(json_input)) {
    // config_ptr is now populated
} else {
    // Handle error
}
```

**Implementation**: Delegates to `readDocumentImpl` which must be specialized by user.

---

#### configurationName
```cpp
std::string configurationName() const
```

**Purpose**: Get the collection name for this configuration type.

**Returns**: String identifying the configuration collection

**Throws**: `artdaq::database::exception` on errors

**Usage**:
```cpp
auto name = serializer.configurationName();
// Returns: "MyConfigurations"
```

**Implementation**: Delegates to `configurationNameImpl` which must be specialized by user.

---

### Constructor

```cpp
MakeSerializable(CONF conf)
```

**Purpose**: Construct serializer wrapper for a configuration object.

**Parameters**:
- `conf` - Configuration object (pointer or reference)

**Usage**:
```cpp
MyConfig* config = new MyConfig();
MakeSerializable<MyConfig*> serializer(config);
```

---

### Deleted Operations

```cpp
MakeSerializable() = delete;
MakeSerializable(MakeSerializable&&) = delete;
MakeSerializable(MakeSerializable const&) = delete;
MakeSerializable& operator=(MakeSerializable const&) = delete;
MakeSerializable& operator=(MakeSerializable&&) = delete;
```

**Design**: Serializer is non-copyable and non-movable to maintain association with wrapped configuration.

---

### Private Methods (Must be Specialized)

#### writeDocumentImpl
```cpp
template <class TYPE>
bool writeDocumentImpl(TYPE&) const;
```

**Purpose**: Implementation hook for serialization - must be specialized by user.

**Specialization Example**:
```cpp
template <>
template <>
bool MakeSerializable<MyConfig*>::writeDocumentImpl<JsonData>(JsonData& data) const {
    jsn::object_t json;
    json["name"] = _conf->name;
    json["value"] = _conf->value;

    std::ostringstream oss;
    oss << json;
    data = JsonData{oss.str()};

    return true;
}
```

---

#### readDocumentImpl
```cpp
template <class TYPE>
bool readDocumentImpl(TYPE const&);
```

**Purpose**: Implementation hook for deserialization - must be specialized by user.

**Specialization Example**:
```cpp
template <>
template <>
bool MakeSerializable<MyConfig*>::readDocumentImpl<JsonData>(JsonData const& data) const {
    jsn::object_t json;
    if (!jsn::JsonReader().read(std::string(data), json)) {
        return false;
    }

    _conf->name = unwrap(json).value_as<std::string>("name");
    _conf->value = unwrap(json).value_as<int>("value");

    return true;
}
```

---

#### configurationNameImpl
```cpp
std::string configurationNameImpl() const;
```

**Purpose**: Implementation hook for collection name - must be specialized by user.

**Specialization Example**:
```cpp
template <>
std::string MakeSerializable<MyConfig*>::configurationNameImpl() const {
    return "MyConfigurations";
}
```

---

### Private Members

```cpp
CONF _conf;
```

**Purpose**: Stores the wrapped configuration object (pointer or reference).

---

## Template Class: ConfigurationSerializer<CONF, SERIALIZABLE>

### Purpose

`ConfigurationSerializer` provides a type-safe wrapper around `MakeSerializable` with enhanced error handling and noexcept guarantees. It returns `result_t` instead of throwing exceptions.

### Template Parameters

- **CONF**: Configuration type (pointer or reference)
- **SERIALIZABLE**: Serialization strategy template (typically `MakeSerializable`)

### Static Assertions

```cpp
static_assert(std::is_pointer<CONF>::value || std::is_reference<CONF>::value,
              "Template parameter must be either a pointer or reference type");
```

---

### Type Aliases

```cpp
using Serializable_t = SERIALIZABLE<CONF>;
```

**Purpose**: Convenience alias for the instantiated serialization strategy.

---

### Factory Method

#### wrap
```cpp
static ConfigurationSerializer<CONF, SERIALIZABLE> wrap(CONF conf)
```

**Purpose**: Factory method to create a serializer instance.

**Parameters**:
- `conf` - Configuration object to wrap

**Returns**: ConfigurationSerializer instance

**Usage**:
```cpp
auto serializer = ConfigurationSerializer<MyConfig*, MakeSerializable>::wrap(config_ptr);
```

**Design**: Preferred way to create instances - provides clear intent.

---

### Public Methods

#### writeDocument (FhiclData)
```cpp
template <class TYPE>
typename std::enable_if<std::is_same<TYPE, FhiclData>::value, cf::result_t>::type
writeDocument(TYPE& data) const noexcept
```

**Purpose**: Serialize configuration to FHiCL format with result handling.

**SFINAE**: Only enabled when `TYPE` is `FhiclData`

**Parameters**:
- `data` - Output parameter for FHiCL data

**Returns**: `result_t` - (success boolean, message string)

**Exception Safety**: noexcept - never throws

**Implementation**:
1. Creates temporary FhiclData
2. Calls underlying writeDocument
3. Swaps on success
4. Returns result with success/failure message

---

#### writeDocument (JsonData)
```cpp
template <class TYPE>
typename std::enable_if<std::is_same<TYPE, JsonData>::value, cf::result_t>::type
writeDocument(TYPE& data) const noexcept
```

**Purpose**: Serialize configuration to JSON format with result handling.

**SFINAE**: Only enabled when `TYPE` is `JsonData`

**Parameters**:
- `data` - Output parameter for JSON data

**Returns**: `result_t` - (success boolean, message string)

**Exception Safety**: noexcept - never throws

**Usage Example**:
```cpp
auto serializer = ConfigurationSerializer<Config*, MakeSerializable>::wrap(&config);

JsonData json;
auto result = serializer.writeDocument(json);

if (result.first) {
    std::cout << "Serialized to JSON successfully\n";
    // Use json
} else {
    std::cerr << "Error: " << result.second << "\n";
}
```

---

#### readDocument (FhiclData)
```cpp
template <class TYPE>
typename std::enable_if<std::is_same<TYPE, FhiclData>::value, cf::result_t>::type
readDocument(TYPE const& data) noexcept
```

**Purpose**: Deserialize FHiCL data into configuration.

**SFINAE**: Only enabled when `TYPE` is `FhiclData`

**Parameters**:
- `data` - Input FHiCL data

**Returns**: `result_t` - (success boolean, message string)

**Exception Safety**: noexcept

---

#### readDocument (JsonData)
```cpp
template <class TYPE>
typename std::enable_if<std::is_same<TYPE, JsonData>::value, cf::result_t>::type
readDocument(TYPE const& data) noexcept
```

**Purpose**: Deserialize JSON data into configuration.

**SFINAE**: Only enabled when `TYPE` is `JsonData`

**Parameters**:
- `data` - Input JSON data

**Returns**: `result_t` - (success boolean, message string)

**Exception Safety**: noexcept

**Usage Example**:
```cpp
auto serializer = ConfigurationSerializer<Config*, MakeSerializable>::wrap(&config);

JsonData json = loadFromDatabase();
auto result = serializer.readDocument(json);

if (result.first) {
    // config is now populated
} else {
    std::cerr << "Failed to deserialize: " << result.second << "\n";
}
```

---

#### configurationName
```cpp
std::string configurationName() const noexcept
```

**Purpose**: Get collection name for this configuration type.

**Returns**: Configuration collection name

**Exception Safety**: noexcept - returns demangled type name on error

**Fallback Behavior**: If specialized implementation throws:
1. Demangles the configuration type name
2. Extracts class name from fully-qualified name
3. Returns extracted name

**Example**:
```cpp
auto name = serializer.configurationName();
// Returns: "MyConfigurations" (from specialization)
// Or: "MyConfig" (from type demangling fallback)
```

---

### Constructor (Private)

```cpp
ConfigurationSerializer(CONF conf)
```

**Purpose**: Private constructor - use `wrap()` factory method instead.

**Design**: Forces use of factory method for consistent creation pattern.

---

### Deleted Operations

```cpp
ConfigurationSerializer() = delete;
ConfigurationSerializer(ConfigurationSerializer const&) = delete;
ConfigurationSerializer& operator=(ConfigurationSerializer const&) = delete;
ConfigurationSerializer& operator=(ConfigurationSerializer&&) = delete;
```

**Design**: Non-copyable (but movable) to prevent accidental duplication.

---

### Private Members

```cpp
CONF _conf;
```

**Purpose**: Stores wrapped configuration object.

---

## Struct: VersionInfo

### Purpose

`VersionInfo` is a simple data structure that represents a specific version of a configuration for a particular entity. It's used primarily for global configuration composition.

### Fields

```cpp
std::string configuration;  // Collection/configuration type name
std::string version;        // Version identifier
std::string entity;         // Entity/component name
```

**Example**:
```cpp
VersionInfo info;
info.configuration = "ComponentConfigs";
info.version = "v1.0";
info.entity = "DAQComponent1";
```

---

### Methods

#### validate
```cpp
void validate() const
```

**Purpose**: Validate that all required fields are non-empty.

**Throws**: `invalid_option_exception` if any field is empty

**Validation Rules**:
- `configuration` must not be empty
- `version` must not be empty
- `entity` must not be empty

**Usage**:
```cpp
VersionInfo info{"ComponentConfigs", "v1.0", "DAQ1"};

try {
    info.validate();
    // All fields are valid
} catch (artdaq::database::invalid_option_exception const& e) {
    std::cerr << "Invalid version info: " << e.what() << "\n";
}
```

**Note**: Uses `confirm()` macro for debug-mode assertions before throwing.

---

## Design Patterns

### Template Specialization Pattern

Users extend functionality by specializing template methods:

```cpp
// User specializes for their configuration type
template <>
template <>
bool MakeSerializable<MyConfig*>::writeDocumentImpl<JsonData>(JsonData& data) const {
    // Custom serialization logic
    return true;
}
```

**Benefits**:
1. Type-safe compile-time dispatch
2. No runtime overhead
3. Extensible without modifying base code

---

### SFINAE for Type Selection

`ConfigurationSerializer` uses SFINAE (Substitution Failure Is Not An Error) to provide different implementations for JSON vs FHiCL:

```cpp
template <class TYPE>
typename std::enable_if<std::is_same<TYPE, JsonData>::value, cf::result_t>::type
writeDocument(TYPE& data) const noexcept
```

**Benefits**:
1. Single interface for multiple formats
2. Compile-time type checking
3. No runtime branching

---

### Result Type Pattern

Methods return `result_t` instead of throwing:

```cpp
using result_t = std::pair<bool, std::string>;
```

**Benefits**:
1. No exception overhead
2. Explicit error handling
3. C-compatible interfaces
4. Predictable performance

---

## Usage Example: Complete Implementation

### Step 1: Define Configuration Type

```cpp
struct MyConfiguration {
    std::string name;
    int run_number;
    std::vector<std::string> components;
};
```

### Step 2: Specialize MakeSerializable

```cpp
namespace artdaq {
namespace database {
namespace configuration {

template <>
class MakeSerializable<MyConfiguration*> {
 public:
  MakeSerializable(MyConfiguration* conf) : _conf(conf) {}

  // JSON serialization
  template <>
  bool writeDocumentImpl<JsonData>(JsonData& data) const {
    jsn::object_t json;
    json["name"] = _conf->name;
    json["run_number"] = _conf->run_number;

    jsn::array_t components;
    for (auto const& comp : _conf->components) {
      components.push_back(comp);
    }
    json["components"] = components;

    std::ostringstream oss;
    oss << json;
    data = JsonData{oss.str()};
    return true;
  }

  // JSON deserialization
  template <>
  bool readDocumentImpl<JsonData>(JsonData const& data) {
    jsn::object_t json;
    if (!jsn::JsonReader().read(std::string(data), json)) {
      return false;
    }

    _conf->name = unwrap(json).value_as<std::string>("name");
    _conf->run_number = unwrap(json).value_as<int>("run_number");

    auto const& comps = unwrap(json).value_as<jsn::array_t>("components");
    _conf->components.clear();
    for (auto const& comp : comps) {
      _conf->components.push_back(unwrap(comp).value_as<std::string>());
    }

    return true;
  }

  std::string configurationNameImpl() const {
    return "MyConfigurations";
  }

 private:
  MyConfiguration* _conf;
};

}  // namespace configuration
}  // namespace database
}  // namespace artdaq
```

### Step 3: Use with ConfigurationInterface

```cpp
#include "artdaq-database/ConfigurationDB/configurationdbifc.h"

void example() {
    MyConfiguration config;
    config.name = "TestConfig";
    config.run_number = 12345;
    config.components = {"DAQ1", "DAQ2"};

    ConfigurationInterface ifc;

    // Store version
    auto result = ifc.storeVersion<MyConfiguration*, JsonData>(
        &config, "v1.0", "TestEntity"
    );

    if (result.first) {
        std::cout << "Stored successfully\n";
    }

    // Load version
    MyConfiguration loaded;
    result = ifc.loadVersion<MyConfiguration*, JsonData>(
        &loaded, "v1.0", "TestEntity"
    );

    if (result.first) {
        std::cout << "Loaded: " << loaded.name << "\n";
    }
}
```

---

## Thread Safety

- **MakeSerializable**: Not thread-safe - do not share instances across threads
- **ConfigurationSerializer**: Not thread-safe - create separate instances per thread
- **VersionInfo**: Plain data struct - safe to copy and use across threads

---

## Best Practices

### Implement All Format Methods

```cpp
// Implement both JSON and FHiCL serialization
template <>
bool writeDocumentImpl<JsonData>(...) { /* ... */ }

template <>
bool writeDocumentImpl<FhiclData>(...) { /* ... */ }
```

### Use Collection Names Consistently

```cpp
// Collection names should be plural and descriptive
std::string configurationNameImpl() const {
    return "ComponentConfigs";  // GOOD
    // return "config";           // BAD - too generic
    // return "Component";        // BAD - singular
}
```

### Validate VersionInfo

```cpp
// Always validate before use
VersionInfo info{config, version, entity};
try {
    info.validate();
    // Safe to use
} catch (...) {
    // Handle invalid info
}
```

### Handle Serialization Errors

```cpp
auto serializer = ConfigurationSerializer<Config*, MakeSerializable>::wrap(&config);

JsonData json;
auto result = serializer.writeDocument(json);

if (!result.first) {
    TLOG(TLVL_ERROR) << "Serialization failed: " << result.second;
    // Take corrective action
}
```

---

## Related Files

- **configurationdbifc.h** - Main configuration interface using these base classes
- **basictypes.h** - JsonData and FhiclData type definitions
- **configurationdb.h** - Configuration database type definitions
- **options_operation_managedocument.h** - Document operation options

---

**Documentation generated for artdaq-database ConfigurationDB module**
