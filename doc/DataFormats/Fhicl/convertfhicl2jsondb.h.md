# convertfhicl2jsondb.h

**Path:** `artdaq-database/DataFormats/Fhicl/convertfhicl2jsondb.h`

**Purpose:** Provides bidirectional conversion between FHiCL (Fermilab Hierarchical Configuration Language) documents and the database JSON format. This module preserves all metadata including comments, annotations, and protection attributes during conversion, enabling round-trip transformations without data loss.


## Key Concepts

### FHiCL to JSON Database Conversion

FHiCL is the native configuration language used in Fermilab experiments. When storing FHiCL configurations in the database, they must be converted to JSON format while preserving:

- **Comments**: Lines starting with `#` placed before configuration values
- **Annotations**: Inline comments on the same line as values
- **Protection attributes**: FHiCL-specific protection markers like `@protect_ignore` and `@protect_error`
- **Type information**: The original FHiCL type (NIL, BOOL, NUMBER, STRING, SEQUENCE, TABLE, etc.)
- **Prolog sections**: Special FHiCL sections that define reusable configuration fragments

### Data Pair Structure

The conversion produces two parallel structures:
1. **data**: The actual configuration values
2. **metadata**: Type information, comments, annotations, and protection attributes

## Thread Safety

- **Thread-safe:** No
- **Concurrent access:** Each conversion operation should use its own `fcl2jsondb` or `json2fcldb` instance
- **Locking:** No internal locking; callers must synchronize access to shared FHiCL/JSON data

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/DataFormats/Fhicl/fhiclcpplib_includes.h` | FHiCL library types (`::fhicl::extended_value`, `::fhicl::value_tag`) |
| `artdaq-database/DataFormats/common.h` | Common DataFormats infrastructure |
| `artdaq-database/DataFormats/shared_literals.h` | String literals for JSON field names |
| `artdaq-database/DataFormats/Fhicl/fhicl_reader.h` | FHiCL parsing utilities |
| `artdaq-database/DataFormats/Fhicl/fhicl_types.h` | FHiCL AST types |
| `artdaq-database/DataFormats/Json/json_types.h` | JSON AST types |

## Type Definitions

### `datapair_t`

```cpp
using datapair_t = std::pair<jsn::data_t, jsn::data_t>;
```

**Brief:** Holds a pair of JSON data structures: the first for actual values, the second for metadata.

### `valuetuple_t`

```cpp
using valuetuple_t = std::tuple<jsn::key_t const&, jsn::value_t const&, jsn::value_t const&>;
```

**Brief:** A tuple containing key, data value, and metadata value references for JSON to FHiCL conversion.

## Classes/Structures

### `return_pair`

**Brief:** A helper structure that provides convenient typed access to the data and metadata components of a `datapair_t`.

**Thread Safety:** Not applicable (value type)

#### Constructor

##### `return_pair(datapair_t& pair)`

**Brief:** Constructs a return_pair wrapper around a datapair_t reference.

**Parameters:**
- `pair` - Reference to the datapair_t to wrap

#### Methods

##### `data_<T>() -> T&`

**Brief:** Returns a typed reference to the data value component.

**Returns:** Reference to the data value cast to type T

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `boost::bad_get` | When the variant does not contain type T |

##### `metadata_<T>() -> T&`

**Brief:** Returns a typed reference to the metadata value component.

**Returns:** Reference to the metadata value cast to type T

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `boost::bad_get` | When the variant does not contain type T |

---

### `extra_opts`

**Brief:** Configuration options controlling which sections of a FHiCL document to process during conversion.

**Thread Safety:** Not applicable (value type)

#### Members

| Member | Type | Default | Description |
|--------|------|---------|-------------|
| `readProlog` | `bool` | `false` | Whether to process FHiCL prolog sections |
| `readMain` | `bool` | `true` | Whether to process the main FHiCL body |

#### Methods

##### `enablePrologMode() -> void`

**Brief:** Configures the converter to process only prolog sections, ignoring the main body.

**Postconditions:**
- `readProlog` is `true`
- `readMain` is `false`

##### `enableDefaultMode() -> void`

**Brief:** Configures the converter to process only the main body, ignoring prolog sections.

**Postconditions:**
- `readProlog` is `false`
- `readMain` is `true`

##### `enableCombinedMode() -> void`

**Brief:** Configures the converter to process both prolog and main body sections.

**Postconditions:**
- `readProlog` is `true`
- `readMain` is `true`

---

### `fcl2jsondb`

**Brief:** Functor class that converts FHiCL key-value pairs to database JSON format with metadata preservation.

**Thread Safety:** Instances are not thread-safe; use separate instances for concurrent conversions.

#### Type Definitions

```cpp
using fcl_value = ::fhicl::extended_value;
using fhicl_key_value_pair_t = std::pair<const std::string, fcl_value> const;
using args_tuple_t = std::tuple<fhicl_key_value_pair_t const&, fhicl_key_value_pair_t const&, comments_t const&, extra_opts const&>;
```

#### Constructor

##### `fcl2jsondb(args_tuple_t)`

**Brief:** Constructs a converter from a tuple of FHiCL parsing context.

**Parameters:**
- `args` - Tuple containing:
  - `self`: The current FHiCL key-value pair to convert
  - `parent`: The parent FHiCL key-value pair (for context)
  - `comments`: Map of line numbers to comment strings
  - `opts`: Conversion options

#### Conversion Operators

##### `operator datapair_t()`

**Brief:** Performs the FHiCL to JSON conversion, returning both data and metadata.

**Returns:** A `datapair_t` containing the converted data value and metadata object

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `::fhicl::exception` | When parsing fails or invalid FHiCL structure is encountered |
| `boost::bad_lexical_cast` | When numeric conversion fails |
| `boost::bad_numeric_cast` | When numeric range overflow occurs |

**Example:**
```cpp
#include "artdaq-database/DataFormats/Fhicl/convertfhicl2jsondb.h"

void convertFhiclNode() {
  using namespace artdaq::database::fhicljson;

  // Assume fhicl_kvp is a parsed FHiCL key-value pair
  // and parent_kvp is its parent context
  comments_t comments;  // Line number to comment mapping
  extra_opts opts;
  opts.enableDefaultMode();

  try {
    auto args = std::forward_as_tuple(fhicl_kvp, parent_kvp, comments, opts);
    datapair_t result = fcl2jsondb(args);

    // result.first contains the data
    // result.second contains the metadata
  } catch (const ::fhicl::exception& e) {
    std::cerr << "FHiCL conversion error: " << e.what() << "\n";
  }
}
```

---

### `json2fcldb`

**Brief:** Functor class that converts database JSON format back to FHiCL AST structures.

**Thread Safety:** Instances are not thread-safe; use separate instances for concurrent conversions.

#### Type Definitions

```cpp
using args_tuple_t = std::tuple<valuetuple_t const&, valuetuple_t const&, extra_opts const&>;
```

#### Constructor

##### `json2fcldb(args_tuple_t)`

**Brief:** Constructs a converter from a tuple of JSON conversion context.

**Parameters:**
- `args` - Tuple containing:
  - `self`: The current JSON value tuple (key, data, metadata)
  - `parent`: The parent JSON value tuple (for context)
  - `opts`: Conversion options

#### Conversion Operators

##### `operator fcl::value_t()`

**Brief:** Converts the JSON data to a FHiCL value structure.

**Returns:** A `fcl::value_t` containing the converted FHiCL value

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `std::exception` | When type conversion fails |

##### `operator fcl::atom_t()`

**Brief:** Converts the JSON data to a FHiCL atom (key-value pair) structure.

**Returns:** A `fcl::atom_t` containing the converted key and value with annotations

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `std::exception` | When type conversion fails |
| `std::out_of_range` | When required metadata fields are missing |

**Example:**
```cpp
#include "artdaq-database/DataFormats/Fhicl/convertfhicl2jsondb.h"

void convertJsonNode() {
  using namespace artdaq::database::fhicljson;

  // Assume we have JSON data and metadata for a configuration value
  jsn::key_t key = "threshold";
  jsn::value_t data = static_cast<integer>(100);
  jsn::value_t metadata;  // Populated with type info, comments, etc.

  valuetuple_t self_tuple = std::forward_as_tuple(key, data, metadata);
  valuetuple_t parent_tuple = /* parent context */;
  extra_opts opts;

  try {
    auto args = std::forward_as_tuple(self_tuple, parent_tuple, opts);
    fcl::atom_t result = json2fcldb(args);

    // result contains the FHiCL key and value
  } catch (const std::exception& e) {
    std::cerr << "JSON to FHiCL conversion error: " << e.what() << "\n";
  }
}
```

## Functions

### `fhicl_to_ast(fcl, filename, json) -> bool`

**Brief:** Parses a FHiCL string and converts it to a JSON AST (Abstract Syntax Tree) representation suitable for database storage.

**Parameters:**
- `fcl` - The FHiCL document content as a string
- `filename` - The source filename (used for error messages and source tracking)
- `json` - Output parameter for the resulting JSON object

**Preconditions:**
- `fcl` should contain valid FHiCL syntax

**Returns:** `true` if conversion succeeded, `false` otherwise

**Postconditions:**
- On success, `json` contains the converted JSON AST with data and metadata

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `::fhicl::exception` | When FHiCL parsing fails |

**Thread Safety:** Safe if called with distinct output objects

**Side Effects:**
- None

**Example:**
```cpp
#include "artdaq-database/DataFormats/Fhicl/convertfhicl2jsondb.h"
#include <iostream>

void parseFhiclDocument() {
  using namespace artdaq::database::fhicljson;

  std::string fhicl_content = R"(
    threshold: 100
    enabled: true
    detector: {
      name: "TPC"
      channels: 256
    }
  )";

  jsn::object_t json_ast;

  try {
    bool success = fhicl_to_ast(fhicl_content, "config.fcl", json_ast);
    if (success) {
      std::cout << "Successfully converted FHiCL to JSON AST\n";
      // json_ast now contains the structured representation
    } else {
      std::cerr << "Conversion failed\n";
    }
  } catch (const ::fhicl::exception& e) {
    std::cerr << "FHiCL parse error: " << e.what() << "\n";
  }
}
```

## Debug Functions

### `debug::FCL2JSON() -> void`

**Brief:** Enables verbose TRACE logging for the FHiCL to JSON conversion module.

**Side Effects:**
- Modifies global TRACE logging configuration
- Enables all log levels for the `convertfhicl2jsondb.cpp` trace name

**Thread Safety:** Not thread-safe; call during initialization only

## Relationship to Other Components

This header is part of the FHiCL DataFormats module and bridges between FHiCL parsing and the JSON document model used throughout artdaq-database:

- Uses `fhicl_reader.h` and `fhicl_types.h` for FHiCL AST representation
- Produces `json_types.h` structures for database storage
- Works with `helper_functions.h` for string escaping and type conversion
- Consumed by `ConfigurationDB` module for configuration import/export

## See Also

- [convertfhicl2jsondb.cpp](./convertfhicl2jsondb.cpp.md) - Implementation details
- [fhicl_types.h](./fhicl_types.h.md) - FHiCL AST type definitions
- [fhicl_reader.h](./fhicl_reader.h.md) - FHiCL parsing functionality
- [helper_functions.h](./helper_functions.h.md) - String conversion utilities
- [json_types.h](../Json/json_types.h.md) - JSON AST type definitions

## Notes for Developers

### Common Pitfalls

- **Missing comments map:** The `comments` parameter must contain line number mappings for comment preservation to work correctly
- **Prolog handling:** By default, prolog sections are skipped; use `enablePrologMode()` or `enableCombinedMode()` if prolog data is needed
- **Type preservation:** String quoting style (single, double, unquoted) is preserved in metadata and must be restored during JSON to FHiCL conversion

### Anti-patterns

```cpp
// DON'T: Ignore conversion options
fcl2jsondb converter(args);  // Uses default opts which may skip prolog

// DO: Explicitly configure conversion behavior
extra_opts opts;
opts.enableCombinedMode();  // Process both prolog and main body
fcl2jsondb converter(std::forward_as_tuple(kvp, parent, comments, opts));
```
