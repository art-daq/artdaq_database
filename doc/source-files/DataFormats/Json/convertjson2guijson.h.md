# convertjson2guijson.h

**Path:** `artdaq-database/DataFormats/Json/convertjson2guijson.h`

**Purpose:** Defines classes and utilities for bidirectional conversion between database JSON format and GUI JSON format. The database format separates data and metadata into distinct sections, while the GUI format embeds metadata alongside data for easier rendering in user interfaces. This module enables seamless transformation between these two representations.


## Key Concepts

### Database JSON Format vs GUI JSON Format

The artdaq-database system uses two different JSON representations:

**Database Format:**
- Data and metadata are stored in separate sections
- Efficient for storage, querying, and programmatic access
- Structure: `{ "document": { "data": {...}, "metadata": {...} } }`

**GUI Format:**
- Metadata is embedded with each data element
- Self-documenting structure for UI rendering
- Structure: `{ "guidata": [ { "name": "key", "type": "string", "value": "...", "comment": "..." } ] }`

### JSON Node Types

The conversion system operates on these node types defined by `type_t`:

| Type | Description |
|------|-------------|
| `NOTSET` | Uninitialized node |
| `VALUE` | Primitive value (string, number, boolean) |
| `DATA` | Key-value pair |
| `OBJECT` | JSON object (collection of key-value pairs) |
| `ARRAY` | JSON array (ordered collection of values) |

### Boost.Variant Usage

The module uses `boost::variant` extensively to handle the polymorphic nature of JSON values:

- `json_any_cref_t`: Variant of const references to JSON types
- `json_any_ref_t`: Variant of mutable references to JSON types

## Thread Safety

- **Thread-safe:** No (conversion classes are not thread-safe)
- **Concurrent access:** Each thread must create its own converter instances
- **Locking:** No internal locking; callers must synchronize if sharing instances

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/DataFormats/Json/json_types.h` | JSON AST type definitions (`object_t`, `array_t`, `value_t`, etc.) |
| `artdaq-database/DataFormats/common.h` | Common includes, TRACE logging, `confirm()` assertions |
| `artdaq-database/DataFormats/shared_literals.h` | String constants for JSON field names |

## Classes/Structures

### `value_crt`

A wrapper for const reference to `value_t`, used for type disambiguation in variant access.

**Thread Safety:** Thread-safe (immutable wrapper)

```cpp
struct value_crt {
  value_crt(value_t const& val) : value{val} {}
  value_t const& value;
};
```

### `value_rt`

A wrapper for mutable reference to `value_t`, used for type disambiguation in variant access.

**Thread Safety:** Not thread-safe (mutable wrapper)

```cpp
struct value_rt {
  value_rt(value_t& val) : value{val} {}
  value_t& value;
};
```

### `json_node_t`

A unified wrapper class providing a consistent interface for accessing different JSON node types. Supports both const and non-const access patterns through variant types.

**Thread Safety:** Not thread-safe

```cpp
class json_node_t final {
public:
  template <typename T> json_node_t(T& t);
  template <typename T> json_node_t(T const& t);
  json_node_t(bool b);  // For "not set" nodes

  template <typename T> T const& value_as() const;
  template <typename T> T& value_as();

  template <typename T, typename O> T& makeChild(O const& o);
  template <typename T, typename O> T& makeChildOfChildren(O const& o);

  type_t type() const;

  template <typename T> static json_node_t make_json_node(T const&);
  template <typename T> static json_node_t make_json_node(T&);
};
```

#### Methods

##### `value_as<T>() const -> T const&`

**Brief:** Extracts a const reference to the underlying value of the specified type from the JSON node.

**Template Parameters:**
- `T` - The expected type of the underlying value

**Preconditions:**
- The node must not be empty (enforced by `confirm()`)

**Returns:** Const reference to the value of type `T`

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_error` | If the value was not set |
| `boost::bad_get` | If the actual type does not match `T` |

**Thread Safety:** Not thread-safe

##### `value_as<T>() -> T&`

**Brief:** Extracts a mutable reference to the underlying value of the specified type from the JSON node.

**Template Parameters:**
- `T` - The expected type of the underlying value

**Preconditions:**
- The node must not be empty (enforced by `confirm()`)
- The node must hold a mutable reference (variant index 0)

**Returns:** Mutable reference to the value of type `T`

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `boost::bad_get` | If the actual type does not match `T` or node is const |

**Thread Safety:** Not thread-safe

##### `makeChild<T, O>(o) -> T&`

**Brief:** Creates a new child node of the specified type within the current node (which must be an object or array).

**Template Parameters:**
- `T` - The type of child to create (`object_t`, `array_t`, or value type)
- `O` - The type of the descriptor object containing name and type info

**Parameters:**
- `o` - Descriptor object with `name` and `type` fields

**Preconditions:**
- Current node must be of type `OBJECT` or `ARRAY`

**Returns:** Mutable reference to the newly created child

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_error` | If the value could not be created |

**Thread Safety:** Not thread-safe

##### `makeChildOfChildren<T, O>(o) -> T&`

**Brief:** Creates a new child node within the "children" section of the current node.

**Template Parameters:**
- `T` - The type of child to create
- `O` - The type of the descriptor object

**Parameters:**
- `o` - Descriptor object with `name` and `type` fields

**Preconditions:**
- Current node must be of type `OBJECT` or `ARRAY`

**Returns:** Mutable reference to the newly created child

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_error` | If the value could not be created |

**Thread Safety:** Not thread-safe

##### `type() -> type_t`

**Brief:** Returns the type enumeration for the JSON node.

**Returns:** The `type_t` value indicating the node's type (`VALUE`, `DATA`, `OBJECT`, `ARRAY`, or `NOTSET`)

**Thread Safety:** Not thread-safe

### `db2gui`

Functor class for converting database JSON format to GUI JSON format.

**Thread Safety:** Not thread-safe

```cpp
class db2gui final {
public:
  explicit db2gui(json_node_t data_node, json_node_t metadata_node);
  void operator()(json_node_t&) const;
};
```

#### Constructor

##### `db2gui(data_node, metadata_node)`

**Brief:** Constructs a converter with references to the source data and metadata nodes.

**Parameters:**
- `data_node` - JSON node containing the data to convert
- `metadata_node` - JSON node containing the corresponding metadata

#### Methods

##### `operator()(gui_node) -> void`

**Brief:** Performs the conversion, populating the GUI node with merged data and metadata.

**Parameters:**
- `gui_node` - Output node to receive the GUI-formatted JSON

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_error` | If conversion fails |
| `boost::bad_get` | If type mismatch occurs during conversion |

**Thread Safety:** Not thread-safe

### `gui2db`

Functor class for converting GUI JSON format to database JSON format.

**Thread Safety:** Not thread-safe

```cpp
class gui2db final {
public:
  explicit gui2db(json_node_t gui_node);
  void operator()(json_node_t&, json_node_t&) const;
};
```

#### Constructor

##### `gui2db(gui_node)`

**Brief:** Constructs a converter with a reference to the source GUI node.

**Parameters:**
- `gui_node` - JSON node containing the GUI-formatted data

#### Methods

##### `operator()(data_node, metadata_node) -> void`

**Brief:** Performs the conversion, splitting GUI data into separate data and metadata nodes.

**Parameters:**
- `data_node` - Output node to receive the extracted data
- `metadata_node` - Output node to receive the extracted metadata

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_error` | If conversion fails |
| `boost::bad_get` | If type mismatch occurs during conversion |

**Thread Safety:** Not thread-safe

### `data_node_t`

A read-only wrapper for data nodes, providing type-safe access to data values.

**Thread Safety:** Not thread-safe

```cpp
class data_node_t final {
public:
  explicit data_node_t(json_node_t const& node);
  template <typename T> T const& value() const;
  type_t type() const;
};
```

#### Methods

##### `value<T>() -> T const&`

**Brief:** Returns the node's value cast to the specified type.

##### `type() -> type_t`

**Brief:** Returns the type of the wrapped node.

### `metadata_node_t`

A read-only wrapper for metadata nodes with specific accessors for common metadata fields.

**Thread Safety:** Not thread-safe

```cpp
class metadata_node_t final {
public:
  explicit metadata_node_t(json_node_t const& node);
  std::string typeName() const;
  std::string comment() const;
  std::string annotation() const;
  bool hasMetadata() const;
  json_node_t child(std::string const&) const;
};
```

#### Methods

##### `typeName() -> std::string`

**Brief:** Returns the "type" field from the metadata (e.g., "string", "table", "sequence").

##### `comment() -> std::string`

**Brief:** Returns the "comment" field from the metadata.

##### `annotation() -> std::string`

**Brief:** Returns the "annotation" field from the metadata.

##### `hasMetadata() -> bool`

**Brief:** Returns `true` if this node contains valid metadata, `false` if unset.

##### `child(name) -> json_node_t`

**Brief:** Returns the child metadata node with the specified name, searching both direct children and the "children" section.

**Parameters:**
- `name` - Name of the child metadata to retrieve

**Returns:** The child node, or a "false" node if not found

### `gui_node_t`

A read-only wrapper for GUI nodes.

**Thread Safety:** Not thread-safe

```cpp
class gui_node_t final {
public:
  explicit gui_node_t(json_node_t const& node);
  template <typename T> T const& value() const;
  type_t type() const;
};
```

### `jcunwrapper<A>` and `jcunwrap()`

Helper template for unwrapping const references from variants.

```cpp
template <typename A>
struct jcunwrapper {
  jcunwrapper(A& a) : any{a} {}
  operator object_t const&();
  operator array_t const&();
  operator data_t const&();
  operator value_t const&();
};

template <typename A>
jcunwrapper<A> jcunwrap(A& any);
```

## Functions

### `json_db_to_gui(db_json, gui_json) -> bool`

**Brief:** Converts a database-format JSON string to GUI-format JSON string.

**Parameters:**
- `db_json` - Input JSON string in database format. Must not be empty.
- `gui_json` - Output JSON string in GUI format. Must be empty before calling.

**Preconditions:**
- `db_json` must not be empty
- `gui_json` must be empty
- `db_json` must contain a valid document structure with "document.data" and "document.metadata"

**Returns:** `true` if conversion succeeded, `false` otherwise

**Postconditions:**
- On success: `gui_json` contains the converted GUI-format JSON
- The original document structure is preserved with a new "converted.guidata" section

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_error` | If JSON parsing fails |
| `runtime_error` | If required document structure is missing |

**Thread Safety:** Safe to call from multiple threads (creates internal instances)

**Example:**
```cpp
#include "artdaq-database/DataFormats/Json/convertjson2guijson.h"
#include <iostream>

void convertForUI() {
  std::string db_json = R"({
    "document": {
      "data": {
        "threshold": 100,
        "enabled": true
      },
      "metadata": {
        "threshold": { "type": "number", "comment": "Detection threshold" },
        "enabled": { "type": "bool", "comment": "Enable detector" }
      }
    }
  })";

  std::string gui_json;
  try {
    if (artdaq::database::json_db_to_gui(db_json, gui_json)) {
      std::cout << "Converted for UI: " << gui_json << "\n";
    } else {
      std::cerr << "Conversion failed\n";
    }
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << "\n";
  }
}
```

### `json_gui_to_db(gui_json, db_json) -> bool`

**Brief:** Converts a GUI-format JSON string back to database-format JSON string.

**Parameters:**
- `gui_json` - Input JSON string in GUI format. Must not be empty.
- `db_json` - Output JSON string in database format. Must be empty before calling.

**Preconditions:**
- `gui_json` must not be empty
- `db_json` must be empty
- `gui_json` must contain a valid GUI document structure with "document.converted.guidata"

**Returns:** `true` if conversion succeeded, `false` otherwise

**Postconditions:**
- On success: `db_json` contains the converted database-format JSON
- Data and metadata are split into separate sections

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_error` | If JSON parsing fails |
| `runtime_error` | If required GUI structure is missing |

**Thread Safety:** Safe to call from multiple threads (creates internal instances)

**Example:**
```cpp
#include "artdaq-database/DataFormats/Json/convertjson2guijson.h"
#include <iostream>

void saveFromUI(const std::string& gui_json) {
  std::string db_json;
  try {
    if (artdaq::database::json_gui_to_db(gui_json, db_json)) {
      std::cout << "Ready for storage: " << db_json << "\n";
      // Store db_json in database
    } else {
      std::cerr << "Conversion failed\n";
    }
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << "\n";
  }
}
```

### `debug::JSON2GUIJSON() -> void`

**Brief:** Enables TRACE debugging for the JSON2GUIJSON conversion module.

**Side Effects:**
- Configures TRACE logging for this module

## Relationship to Other Components

This file is part of the DataFormats/Json module:

```
convertjson2guijson.h (this file)
    |
    +-- json_types.h (AST type definitions)
    +-- json_reader.h (JSON parsing)
    +-- json_writer.h (JSON generation)
    |
    +-- Used by: Web UI, Configuration GUI tools
```

## See Also

- [convertjson2guijson.cpp](./convertjson2guijson.cpp.md) - Implementation details
- [json_reader.h](./json_reader.h.md) - JSON parsing
- [json_writer.h](./json_writer.h.md) - JSON generation
- [json_types.h](./json_types.h.md) - AST type definitions
- [shared_literals.h](../shared_literals.h.md) - String constants used in conversion

## Notes for Developers

### Common Pitfalls

- **Pitfall 1:** Forgetting to pass empty output strings. Both conversion functions require empty output parameters.

- **Pitfall 2:** Missing document structure. The input must contain the expected structure (`document.data`, `document.metadata` for db2gui; `document.converted.guidata` for gui2db).

- **Pitfall 3:** Type mismatches during variant access. Always verify node types before calling `value_as<T>()`.

### Anti-patterns

```cpp
// DON'T do this - output must be empty:
std::string output = "existing";
json_db_to_gui(input, output);  // May fail

// DO this instead:
std::string output;
json_db_to_gui(input, output);

// DON'T do this - no error handling:
json_db_to_gui(input, output);
process(output);  // May be empty if conversion failed

// DO this instead:
if (json_db_to_gui(input, output)) {
  process(output);
} else {
  handleError();
}
```

### Format Differences

**Database Format Example:**
```json
{
  "document": {
    "data": { "threshold": 100 },
    "metadata": { "threshold": { "type": "number", "comment": "..." } }
  }
}
```

**GUI Format Example:**
```json
{
  "document": {
    "converted": {
      "guidata": [
        { "name": "threshold", "type": "number", "value": 100, "comment": "..." }
      ]
    }
  }
}
```
