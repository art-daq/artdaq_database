# ovlOrigin.h

**Path:** `artdaq-database/Overlay/ovlOrigin.h`

**Purpose:** Defines the `ovlOrigin` overlay class for tracking document provenance - where the configuration came from, in what format, when it was created, and what the original source data looked like. This class maintains a complete audit trail for configuration data, including a raw data list for preserving original source formats during format conversions (such as FHiCL to JSON).


## Key Concepts

### Provenance Tracking

The origin overlay provides complete tracking of document provenance with five key pieces of information:

| Field | Description | Default Value |
|-------|-------------|---------------|
| `format` | The data format (json, fcl, xml, etc.) | `"json"` |
| `name` | Source filename or identifier | `"not-provided"` |
| `source` | Origin type (file, template, database, API, etc.) | `"template"` |
| `created` | Timestamp when the document was first created | Current time |
| `rawdatalist` | List preserving original source data in various formats | Empty array |

This information enables:
- Tracing configuration issues back to source files
- Understanding the format conversion history
- Auditing who/what/when created or modified configurations
- Debugging format conversion problems

### Format Conversion History

The raw data list (`ovlRawDataList`) preserves original source data, enabling:
- Format round-tripping (FHiCL -> JSON -> FHiCL) with minimal data loss
- Validation against original source content
- Multiple format versions of the same data stored together
- Recovery of original format if needed

### Auto-Initialization with Defaults

When fields are missing during construction, the `init()` method creates them with sensible default values. This ensures every origin has complete provenance information even when created programmatically without explicit field initialization.

## Thread Safety

- **Thread-safe:** No
- **Concurrent access:** Not supported; external synchronization required
- **Locking:** None

All methods access mutable JSON data through references without synchronization. The comparison mask is stored in static storage, affecting all threads.

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/Overlay/common.h` | Module foundation types, constants, result type |
| `artdaq-database/Overlay/ovlKeyValue.h` | Base class providing JSON key-value access |
| `artdaq-database/Overlay/ovlTimeStamp.h` | Creation timestamp overlay |
| `artdaq-database/Overlay/ovlStringKeyValue.h` | Template for raw data entries |
| `artdaq-database/Overlay/ovlFixedList.h` | Container template for raw data list |

## Type Aliases

```cpp
using ovlRawData = ovlStringKeyValue<DOCUMENT_COMPARE_MUTE_RAWDATA>;
using ovlRawDataUPtr_t = std::unique_ptr<ovlRawData>;

using ovlRawDataList = ovlFixedList<ovlRawData, DOCUMENT_COMPARE_MUTE_SEARCH>;
using ovlRawDataListUPtr_t = std::unique_ptr<ovlRawDataList>;
```

These aliases define the types used for storing original source data in the raw data list. The `DOCUMENT_COMPARE_MUTE_RAWDATA` mask allows ignoring raw data during comparisons.

## Classes/Structures

### `ovlOrigin`

A type-safe overlay for document origin information, inheriting from `ovlKeyValue`. Provides accessors for provenance fields, manages creation timestamp and raw data list, and supports maskable comparison. All missing fields are auto-initialized with defaults during construction.

**Thread Safety:** Not thread-safe; references mutable JSON data.

#### Constructor

##### `ovlOrigin(object_t::key_type const& key, value_t& origin)`

**Brief:** Constructs an overlay for origin information, auto-initializing missing fields with default values to ensure complete provenance tracking.

**Parameters:**
- `key` - The JSON key under which this origin is stored (typically "origin")
- `origin` - Reference to the JSON object containing origin data

**Preconditions:**
- `origin` must be a valid JSON object or convertible to one

**Postconditions:**
- All origin fields exist (either from JSON or auto-initialized)
- `_created` timestamp overlay is mapped and ready
- `_rawdatalist` overlay is initialized (empty array if not present)
- All accessor methods will work without exceptions

**Throws:**
| Exception | Condition |
|-----------|-----------|
| Assertion failure | If `origin` is not an OBJECT type |

**Thread Safety:** Not applicable (construction)

**Default Values Created:**

| Field | Default Value | When Used |
|-------|---------------|-----------|
| `format` | `"json"` | Common default for programmatic document creation |
| `name` | `"not-provided"` | Indicates filename wasn't specified |
| `source` | `"template"` | Indicates created from template, not loaded from file |
| `created` | Current timestamp | Set to creation time |
| `rawdatalist` | Empty array `[]` | No raw data preserved initially |

**Example:**
```cpp
#include "artdaq-database/Overlay/ovlOrigin.h"
#include <iostream>

using namespace artdaq::database::overlay;

void createOrigin() {
  try {
    // Create with partial data - missing fields auto-initialized
    value_t originJson = object_t{};
    originJson["format"] = "fcl";
    originJson["name"] = "daq_config.fcl";

    ovlOrigin origin("origin", originJson);

    // All fields accessible - missing ones have defaults
    std::cout << "Format: " << origin.format() << std::endl;  // "fcl"
    std::cout << "Name: " << origin.name() << std::endl;      // "daq_config.fcl"
    std::cout << "Source: " << origin.source() << std::endl;  // "template" (default)
  } catch (const std::exception& e) {
    std::cerr << "Error creating origin: " << e.what() << std::endl;
  }
}
```

#### Special Member Functions

##### `ovlOrigin(ovlOrigin&&) = default`

**Brief:** Default move constructor allowing transfer of overlay ownership. The moved-from object should not be used after the move.

**Thread Safety:** Not applicable (construction)

##### `~ovlOrigin() = default`

**Brief:** Default destructor; no special cleanup required as the overlay does not own the underlying JSON data.

**Thread Safety:** Not applicable (destruction)

#### Methods

##### `format() -> std::string&`

**Brief:** Returns a mutable reference to the document format string, allowing modification of the format identifier.

**Preconditions:**
- The overlay must be properly initialized (automatic via constructor)

**Returns:** Reference to format string (e.g., "json", "fcl", "xml").

**Postconditions:**
- Modifications through the returned reference affect the underlying JSON

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `std::bad_cast` | If the `format` field is not a string type (unexpected after init) |

**Thread Safety:** Unsafe - returns mutable reference

##### `format() const -> std::string const&`

**Brief:** Returns a const reference to the document format string for read-only access.

**Returns:** Const reference to the format string.

**Thread Safety:** Unsafe (reads mutable JSON)

##### `name() -> std::string&`

**Brief:** Returns a mutable reference to the source filename or identifier, allowing modification of the name field.

**Preconditions:**
- The overlay must be properly initialized

**Returns:** Reference to the name string.

**Postconditions:**
- Modifications through the returned reference affect the underlying JSON

**Thread Safety:** Unsafe - returns mutable reference

##### `name() const -> std::string const&`

**Brief:** Returns a const reference to the source filename or identifier for read-only access.

**Returns:** Const reference to the name string.

**Thread Safety:** Unsafe (reads mutable JSON)

##### `source() -> std::string&`

**Brief:** Returns a mutable reference to the origin source type, allowing modification of how the document originated.

**Preconditions:**
- The overlay must be properly initialized

**Returns:** Reference to source string (e.g., "file", "template", "database", "gui", "api").

**Postconditions:**
- Modifications through the returned reference affect the underlying JSON

**Thread Safety:** Unsafe - returns mutable reference

##### `source() const -> std::string const&`

**Brief:** Returns a const reference to the origin source type for read-only access.

**Returns:** Const reference to the source string.

**Thread Safety:** Unsafe (reads mutable JSON)

##### `to_string() const -> std::string` [override]

**Brief:** Serializes the complete origin information to a JSON-formatted string for debugging and logging.

**Preconditions:**
- All origin components must be accessible

**Returns:** JSON representation including format, name, source, created timestamp, and raw data list.

**Postconditions:**
- Returns a valid JSON string representation
- Does not modify the underlying JSON

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `std::exception` | If any component access fails |

**Thread Safety:** Unsafe (reads mutable JSON)

**Example:**
```cpp
#include "artdaq-database/Overlay/ovlOrigin.h"

using namespace artdaq::database::overlay;

void debugOrigin(const ovlOrigin& origin) {
  try {
    std::cout << "Origin details:\n" << origin.to_string() << std::endl;
    // Output example:
    // {"origin":{
    // "format":"fcl",
    // "name":"config.fcl",
    // "source":"file",
    // "created":"2025-01-15T10:00:00Z",
    // "rawdatalist":[]
    // }}
  } catch (const std::exception& e) {
    std::cerr << "Error serializing origin: " << e.what() << std::endl;
  }
}
```

##### `operator==(ovlOrigin const& other) const -> result_t`

**Brief:** Compares this origin with another, respecting the `DOCUMENT_COMPARE_MUTE_ORIGIN` mask. When not masked, provides detailed difference reporting for each field.

**Parameters:**
- `other` - The origin to compare against

**Preconditions:**
- Both origins must have valid underlying JSON data

**Returns:** `result_t` where `first` is `true` if origins match (or masking is enabled), `false` otherwise with detailed difference description in `second`.

**Postconditions:**
- Neither origin is modified

**Throws:**
| Exception | Condition |
|-----------|-----------|
| None | Comparison does not throw |

**Thread Safety:** Unsafe (reads mutable JSON and static comparison mask)

**Example:**
```cpp
#include "artdaq-database/Overlay/ovlOrigin.h"
#include <iostream>

using namespace artdaq::database::overlay;
using namespace artdaq::database::result;

void compareOrigins() {
  try {
    value_t json1 = object_t{};
    json1["format"] = "json";
    json1["name"] = "config1.json";
    ovlOrigin origin1("origin", json1);

    value_t json2 = object_t{};
    json2["format"] = "fcl";
    json2["name"] = "config2.fcl";
    ovlOrigin origin2("origin", json2);

    // Full comparison - will show differences
    useCompareMask(0);
    auto result = origin1 == origin2;
    if (!result.first) {
      std::cout << "Origins differ:" << result.second << std::endl;
      // Output shows which fields differ
    }

    // Masked comparison - ignores origins
    useCompareMask(DOCUMENT_COMPARE_MUTE_ORIGIN);
    result = origin1 == origin2;
    if (result.first) {
      std::cout << "Origins match with masking" << std::endl;
    }
  } catch (const std::exception& e) {
    std::cerr << "Error comparing origins: " << e.what() << std::endl;
  }
}
```

## Type Alias

```cpp
using ovlOriginUPtr_t = std::unique_ptr<ovlOrigin>;
```

Unique pointer type for managing `ovlOrigin` instances.

## Private Members

### `_initOK`

**Type:** `bool`

**Brief:** Tracks whether initialization completed successfully during construction.

### `_created`

**Type:** `ovlTimeStamp`

**Brief:** Overlay for the creation timestamp field. Provides type-safe access to when the document was created.

### `_rawdatalist`

**Type:** `ovlRawDataListUPtr_t`

**Brief:** Unique pointer to the raw data list overlay containing original source data in various formats.

### `map_created(value_t& value) -> ovlTimeStamp`

**Brief:** Creates an `ovlTimeStamp` overlay for the creation timestamp field. Called after `init()` ensures the field exists.

**Called by:** Constructor

### `init(value_t& parent) -> bool`

**Brief:** Initializes missing fields with default values, ensuring complete provenance information.

**Called by:** Constructor

## Relationship to Other Components

### In the Overlay Hierarchy

```
ovlKeyValue (base)
     ^
     |
ovlOrigin (this class)
     |
     +-- ovlTimeStamp _created
     +-- ovlRawDataList _rawdatalist
```

### Used By

- **ovlDatabaseRecord** - Contains `_origin` as one of its 14 components for provenance tracking.

### JSON Structure

```json
{
  "origin": {
    "format": "fcl",
    "name": "daq_config_v1.fcl",
    "source": "file",
    "created": "2025-01-15T10:00:00Z",
    "rawdatalist": [
      "# Original FHiCL content...",
      "{\"converted\": \"JSON content\"}"
    ]
  }
}
```

## Notes for Developers

### Common Field Values

**Format:**

| Value | Description |
|-------|-------------|
| `"json"` | JSON format (default) |
| `"fcl"` | FHiCL configuration language |
| `"xml"` | XML format |
| `"txt"` | Plain text |
| `"conf"` | Generic configuration format |

**Source:**

| Value | Description |
|-------|-------------|
| `"file"` | Loaded from file system |
| `"template"` | Created from template (default) |
| `"database"` | Retrieved from database |
| `"gui"` | Created via graphical interface |
| `"api"` | Created via programmatic API |
| `"import"` | Imported from external system |

### Usage Example

```cpp
#include "artdaq-database/Overlay/ovlOrigin.h"
#include "artdaq-database/Overlay/ovlDatabaseRecord.h"
#include <iostream>

using namespace artdaq::database::overlay;

void workWithOrigin() {
  try {
    // Create via database record (preferred approach)
    value_t recordJson = object_t{};
    ovlDatabaseRecord record{recordJson};

    // Access origin component
    auto& origin = record.origin();

    // Set origin information for a file import
    origin.format() = "fcl";
    origin.name() = "production_config.fcl";
    origin.source() = "file";

    // Read values
    std::cout << "Document origin:" << std::endl;
    std::cout << "  From: " << origin.name() << std::endl;
    std::cout << "  Format: " << origin.format() << std::endl;
    std::cout << "  Source: " << origin.source() << std::endl;

    // Serialize for logging
    std::cout << "\nFull origin JSON:\n" << origin.to_string() << std::endl;
  } catch (const std::exception& e) {
    std::cerr << "Error working with origin: " << e.what() << std::endl;
  }
}
```

### Common Pitfalls

- **Free-form Fields:** Format, name, and source are free-form strings; they are not validated against a fixed set of values. Use consistent values across your application.
- **Default Values:** Auto-initialization provides defaults; check if these are appropriate for your use case. A document created programmatically will have `source="template"` unless explicitly set.
- **Raw Data List:** Typically empty unless format conversion occurred; don't expect it to be populated for simple imports.
- **Comparison Masking:** The `DOCUMENT_COMPARE_MUTE_ORIGIN` mask ignores all origin fields. Use more specific masks if you need partial comparison.

### Design Rationale

- **Provenance:** Know exactly where configuration came from for debugging
- **Debugging:** Trace issues back to source files with filename and format
- **Conversion Tracking:** Track format conversions (FHiCL to JSON) in raw data list
- **Audit Trail:** Record who/what/when created the configuration
- **Reproducibility:** Enable recreation of original format from stored data

### Anti-patterns

```cpp
// DON'T: Assume raw data list is always populated
auto& rawdata = origin.rawdatalist();  // May be empty
if (rawdata.size() > 0) { ... }  // Check first

// DO: Check before accessing
// (raw data list is only populated during format conversions)

// DON'T: Hardcode format strings inconsistently
origin.format() = "FHICL";  // Inconsistent casing
origin.format() = "fhicl";  // Different from standard
origin.format() = "fcl";    // Standard value - use this

// DO: Use consistent format identifiers
// Standard formats: "json", "fcl", "xml", "txt", "conf"
```

## See Also

- [ovlOrigin.cpp](./ovlOrigin.cpp.md) - Implementation file with method details
- [ovlKeyValue.h](./ovlKeyValue.h.md) - Base class providing field access
- [ovlTimeStamp.h](./ovlTimeStamp.h.md) - Timestamp overlay used for creation time
- [ovlFixedList.h](./ovlFixedList.h.md) - Container template used for raw data list
- [ovlDatabaseRecord.h](./ovlDatabaseRecord.h.md) - Uses `ovlOrigin` as a component
- [common.h](./common.h.md) - Comparison flags and result types
