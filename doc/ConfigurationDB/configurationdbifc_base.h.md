# configurationdbifc_base.h

**Path:** `artdaq-database/ConfigurationDB/configurationdbifc_base.h`

**Purpose:** Defines the base infrastructure for configuration serialization in artdaq-database. This header provides template classes for wrapping user configuration objects and making them serializable to/from JSON and FHiCL formats, along with helper structures for version management. It serves as the foundation upon which `ConfigurationInterface` builds its database operations.


## Key Concepts

### Configuration Serialization Pattern

This header implements a two-layer serialization pattern:

1. **MakeSerializable<CONF>**: Low-level template that users must specialize to define how their configuration types serialize to/from data formats. This requires implementing `writeDocumentImpl`, `readDocumentImpl`, and `configurationNameImpl` methods.

2. **ConfigurationSerializer<CONF, SERIALIZABLE>**: High-level wrapper that provides noexcept guarantees and `result_t` return values instead of exceptions. This is the preferred interface for database operations.

### Pointer/Reference Requirement

Both template classes enforce that the configuration type parameter must be a pointer or reference:

```cpp
static_assert(std::is_pointer<CONF>::value || std::is_reference<CONF>::value,
              "Template parameter must be either a pointer or reference type");
```

This ensures:
- Configuration objects can be mutated during deserialization
- No unnecessary copies of potentially large configuration structures
- Clear ownership semantics

### SFINAE for Format Selection

The `ConfigurationSerializer` uses SFINAE (Substitution Failure Is Not An Error) to provide type-safe overloads for different data formats (JSON vs FHiCL). This enables compile-time format selection with no runtime overhead.

## Thread Safety

- **Thread-safe:** No
- **Concurrent access:** Not supported - create separate serializer instances per thread
- **Locking:** None - classes are not designed for concurrent access

**Recommendation:** Each thread that needs to serialize/deserialize configurations should create its own serializer instance. The serializers hold references to configuration objects, so sharing instances across threads could lead to data races.

## Dependencies

| Include | Purpose |
|---------|---------|
| `<utility>` | std::pair for result types, std::swap |
| `artdaq-database/BasicTypes/basictypes.h` | FhiclData, JsonData type wrappers |
| `artdaq-database/ConfigurationDB/configurationdb.h` | Configuration database types and result_t |
| `artdaq-database/DataFormats/Json/json_reader.h` | JSON parsing utilities |
| `artdaq-database/DataFormats/Json/json_writer.h` | JSON serialization utilities |
| `artdaq-database/ConfigurationDB/options_operation_managedocument.h` | Document operation options |

## Functions

### `make_error_msg` (Lambda)

```cpp
inline auto make_error_msg = [](const char* msg) -> std::string;
```

**Brief:** Formats an error message as a JSON object string for consistent error reporting.

**Parameters:**
- `msg` - The error message text to wrap

**Returns:** JSON-formatted error string in the format `{error:"<message>"}`

**Thread Safety:** Safe - stateless lambda

**Example:**
```cpp
#include "artdaq-database/ConfigurationDB/configurationdbifc_base.h"

void handleError() {
  auto error = make_error_msg("Connection failed");
  // Returns: "{error:\"Connection failed\"}"
  std::cerr << error << std::endl;
}
```

## Classes/Structures

### `MakeSerializable<CONF>`

A template class that wraps user configuration objects and provides serialization/deserialization methods. Users must specialize this template for their custom configuration types by implementing the private `*Impl` methods.

**Thread Safety:** Not thread-safe - do not share instances across threads

#### Constructor

##### `MakeSerializable(CONF conf)`

**Brief:** Constructs a serializer wrapper for the given configuration object.

**Parameters:**
- `conf` - Configuration object (must be pointer or reference type)

**Preconditions:**
- `CONF` must be a pointer or reference type (enforced at compile time)

**Example:**
```cpp
struct MyConfig {
  std::string name;
  int value;
};

MyConfig config;
config.name = "test";
config.value = 42;

// Using pointer type
MakeSerializable<MyConfig*> serializer(&config);
```

#### Methods

##### `writeDocument(TYPE& data) const -> bool`

**Brief:** Serializes the wrapped configuration object to the specified data format (JSON or FHiCL).

**Parameters:**
- `data` - Output parameter to receive the serialized data

**Preconditions:**
- User must have specialized `writeDocumentImpl<TYPE>` for the configuration type

**Returns:** `true` on successful serialization, `false` on failure

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `artdaq::database::exception` | Serialization fails or implementation throws |

**Thread Safety:** Not thread-safe

**Example:**
```cpp
MakeSerializable<MyConfig*> serializer(&config);
JsonData json_output;

try {
  if (serializer.writeDocument(json_output)) {
    std::cout << "Serialized: " << std::string(json_output) << std::endl;
  } else {
    std::cerr << "Serialization returned false" << std::endl;
  }
} catch (const artdaq::database::exception& e) {
  std::cerr << "Serialization error: " << e.what() << std::endl;
}
```

---

##### `readDocument(TYPE const& data) -> bool`

**Brief:** Deserializes data from the specified format into the wrapped configuration object.

**Parameters:**
- `data` - Input data in JSON or FHiCL format

**Preconditions:**
- User must have specialized `readDocumentImpl<TYPE>` for the configuration type

**Returns:** `true` on successful deserialization, `false` on failure

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `artdaq::database::exception` | Deserialization fails or implementation throws |

**Thread Safety:** Not thread-safe

**Example:**
```cpp
JsonData json_input{R"({"name": "test", "value": 42})"};
MakeSerializable<MyConfig*> serializer(&config);

try {
  if (serializer.readDocument(json_input)) {
    std::cout << "Loaded: " << config.name << std::endl;
  }
} catch (const artdaq::database::exception& e) {
  std::cerr << "Deserialization error: " << e.what() << std::endl;
}
```

---

##### `configurationName() const -> std::string`

**Brief:** Returns the collection name for this configuration type, used to determine which database collection stores these configurations.

**Preconditions:**
- User must have specialized `configurationNameImpl()` for the configuration type

**Returns:** String identifying the configuration collection (e.g., "DetectorConfigs")

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `artdaq::database::exception` | Implementation throws or fails |

**Thread Safety:** Not thread-safe

---

#### Deleted Operations

```cpp
MakeSerializable() = delete;
MakeSerializable(MakeSerializable&&) = delete;
MakeSerializable(MakeSerializable const&) = delete;
MakeSerializable& operator=(MakeSerializable const&) = delete;
MakeSerializable& operator=(MakeSerializable&&) = delete;
```

The serializer is non-copyable and non-movable to maintain a stable association with the wrapped configuration object and prevent accidental aliasing.

---

### `ConfigurationSerializer<CONF, SERIALIZABLE>`

A type-safe wrapper around `MakeSerializable` that provides noexcept guarantees and `result_t` return values instead of exceptions. This is the recommended interface for database operations.

**Thread Safety:** Not thread-safe - create separate instances per thread

#### Type Aliases

```cpp
using Serializable_t = SERIALIZABLE<CONF>;
```

#### Static Factory Method

##### `wrap(CONF conf) -> ConfigurationSerializer<CONF, SERIALIZABLE>`

**Brief:** Factory method to create a serializer instance wrapping the given configuration object.

**Parameters:**
- `conf` - Configuration object (pointer or reference)

**Returns:** A new ConfigurationSerializer instance

**Thread Safety:** Safe (creates new instance)

**Example:**
```cpp
auto serializer = ConfigurationSerializer<MyConfig*, MakeSerializable>::wrap(&config);
```

#### Methods

##### `writeDocument<TYPE>(TYPE& data) const noexcept -> cf::result_t`

**Brief:** Serializes the configuration to the specified format with result-based error handling. Uses SFINAE to select the appropriate implementation for JsonData or FhiclData.

**Parameters:**
- `data` - Output parameter for serialized data

**Returns:** `result_t` pair containing:
- `first`: `true` on success, `false` on failure
- `second`: JSON-formatted message string

**Postconditions:**
- On success, `data` contains the serialized configuration
- On failure, `data` is unchanged

**Thread Safety:** Not thread-safe

**Example:**
```cpp
#include "artdaq-database/ConfigurationDB/configurationdbifc_base.h"

using namespace artdaq::database::configuration;

void serializeConfig(MyConfig* config) {
  auto serializer = ConfigurationSerializer<MyConfig*, MakeSerializable>::wrap(config);

  JsonData json;
  auto result = serializer.writeDocument(json);

  if (result.first) {
    std::cout << "Success: " << std::string(json) << std::endl;
  } else {
    std::cerr << "Error: " << result.second << std::endl;
  }
}
```

---

##### `readDocument<TYPE>(TYPE const& data) noexcept -> cf::result_t`

**Brief:** Deserializes data into the configuration with result-based error handling.

**Parameters:**
- `data` - Input data in JSON or FHiCL format

**Returns:** `result_t` pair with success status and message

**Thread Safety:** Not thread-safe

---

##### `configurationName() const noexcept -> std::string`

**Brief:** Returns the collection name for this configuration type. If the specialized implementation throws, falls back to using the demangled type name.

**Returns:** Configuration collection name string

**Postconditions:**
- Always returns a non-empty string
- Falls back to demangled type name on error

**Thread Safety:** Not thread-safe

---

### `VersionInfo`

A simple data structure representing a specific version of a configuration for a particular entity. Used primarily for global configuration composition operations.

**Thread Safety:** Plain data struct - safe to copy and use across threads

#### Fields

| Field | Type | Description |
|-------|------|-------------|
| `configuration` | `std::string` | Collection/configuration type name |
| `version` | `std::string` | Version identifier (e.g., "v1.0") |
| `entity` | `std::string` | Entity/component name |

#### Methods

##### `validate() const -> void`

**Brief:** Validates that all required fields are non-empty, throwing if validation fails.

**Preconditions:**
- None (validation is the purpose of this method)

**Postconditions:**
- If no exception thrown, all fields are guaranteed non-empty

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `artdaq::database::invalid_option_exception` | `configuration` is empty |
| `artdaq::database::invalid_option_exception` | `version` is empty |
| `artdaq::database::invalid_option_exception` | `entity` is empty |

**Thread Safety:** Safe (read-only operation)

**Example:**
```cpp
#include "artdaq-database/ConfigurationDB/configurationdbifc_base.h"

using namespace artdaq::database::configuration;

void useVersionInfo() {
  VersionInfo info;
  info.configuration = "DetectorConfigs";
  info.version = "v1.0";
  info.entity = "Detector_001";

  try {
    info.validate();
    // Safe to use info
  } catch (const artdaq::database::invalid_option_exception& e) {
    std::cerr << "Invalid version info: " << e.what() << std::endl;
  }
}
```

## Relationship to Other Components

This file serves as the foundation layer for the ConfigurationDB serialization system:

```
configurationdbifc.h (ConfigurationInterface)
        |
        v
configurationdbifc_base.h (MakeSerializable, ConfigurationSerializer)
        |
        v
BasicTypes/basictypes.h (JsonData, FhiclData)
```

- **configurationdbifc.h**: The main `ConfigurationInterface` class uses `ConfigurationSerializer` internally to serialize/deserialize user configuration types
- **options_operation_managedocument.h**: Provides operation options that work with the serialized data
- **basictypes.h**: Defines the `JsonData` and `FhiclData` wrapper types used for serialization targets

## Common Pitfalls

### Forgetting to Specialize Implementation Methods

```cpp
// DON'T: Try to use MakeSerializable without specialization
MakeSerializable<MyConfig*> ser(&config);
JsonData json;
ser.writeDocument(json);  // LINKER ERROR: undefined reference to writeDocumentImpl

// DO: Specialize the implementation methods first
template <>
template <>
bool MakeSerializable<MyConfig*>::writeDocumentImpl<JsonData>(JsonData& data) const {
  // Your serialization logic here
  return true;
}
```

### Using Value Types Instead of Pointers/References

```cpp
// DON'T: Use value types
MakeSerializable<MyConfig> ser(config);  // COMPILE ERROR: static_assert fails

// DO: Use pointer or reference types
MakeSerializable<MyConfig*> ser(&config);  // OK
MakeSerializable<MyConfig&> ser(config);   // OK
```

### Sharing Serializers Across Threads

```cpp
// DON'T: Share serializer across threads
MakeSerializable<MyConfig*> shared_ser(&config);
std::thread t1([&shared_ser]() { shared_ser.writeDocument(json1); });
std::thread t2([&shared_ser]() { shared_ser.writeDocument(json2); });
// DATA RACE!

// DO: Create per-thread serializers
std::thread t1([&config]() {
  MakeSerializable<MyConfig*> ser(&config);
  ser.writeDocument(json1);
});
```

## See Also

- [configurationdbifc.h](./configurationdbifc.h.md) - Main ConfigurationInterface that uses these base classes
- [basictypes.h](../BasicTypes/basictypes.h.md) - JsonData and FhiclData type definitions
- [configurationdb.h](./configurationdb.h.md) - Configuration database type definitions
- [options_operation_managedocument.h](./options_operation_managedocument.h.md) - Document operation options

---

**Documentation generated for artdaq-database ConfigurationDB module**
