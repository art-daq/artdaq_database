# convertjson2guijson.h

## File Overview

This header defines classes and utilities for bidirectional conversion between database JSON format and GUI JSON format. The database format separates data and metadata, while the GUI format embeds metadata alongside data for easier rendering in user interfaces.

**Location**: `/home/user/artdaq-database/artdaq-database/DataFormats/Json/convertjson2guijson.h`

## Dependencies

### Project Headers
- `"artdaq-database/DataFormats/Json/json_types.h"` - JSON AST types
- `"artdaq-database/DataFormats/common.h"` - Common includes
- `"artdaq-database/DataFormats/shared_literals.h"` - String constants

## Core Classes

### json_node_t

Wrapper class providing unified interface for accessing different JSON node types (value, data, object, array).

**Features**:
- Type-safe variant wrapper
- Support for both const and non-const access
- Child creation methods
- Type introspection

**Key Methods**:
- `value_as<T>()` - Extract typed value
- `makeChild<T, O>(o)` - Create child node
- `makeChildOfChildren<T, O>(o)` - Create child in "children" section
- `type()` - Get node type

### db2gui

Functor for converting database JSON to GUI JSON format.

```cpp
class db2gui final {
public:
    explicit db2gui(json_node_t data_node, json_node_t metadata_node);
    void operator()(json_node_t&) const;
};
```

**Conversion**: Merges separate data and metadata into single GUI-friendly structure.

### gui2db

Functor for converting GUI JSON to database JSON format.

```cpp
class gui2db final {
public:
    explicit gui2db(json_node_t gui_node);
    void operator()(json_node_t&, json_node_t&) const;
};
```

**Conversion**: Splits combined GUI format into separate data and metadata sections.

### data_node_t

Read-only wrapper for data nodes.

```cpp
class data_node_t final {
public:
    explicit data_node_t(json_node_t const& node);
    template <typename T>
    T const& value() const;
    type_t type() const;
};
```

### metadata_node_t

Read-only wrapper for metadata nodes with specific accessors.

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

### gui_node_t

Read-only wrapper for GUI nodes.

```cpp
class gui_node_t final {
public:
    explicit gui_node_t(json_node_t const& node);
    template <typename T>
    T const& value() const;
    type_t type() const;
};
```

## Conversion Functions

```cpp
bool json_db_to_gui(std::string const& db_json, std::string& gui_json);
bool json_gui_to_db(std::string const& gui_json, std::string& db_json);
```

**Purpose**: High-level functions for string-to-string conversion.

## Usage Example

### Database to GUI Conversion

```cpp
std::string db_json = R"({
    "document": {
        "data": {"key": "value"},
        "metadata": {"key": {"type": "string", "comment": "A key"}}
    }
})";

std::string gui_json;
if (json_db_to_gui(db_json, gui_json)) {
    // gui_json contains GUI-friendly format with embedded metadata
}
```

### GUI to Database Conversion

```cpp
std::string gui_json = "...";  // From UI
std::string db_json;

if (json_gui_to_db(gui_json, db_json)) {
    // db_json has separated data and metadata
}
```

## Design Rationale

**Database Format**:
- Separates data from metadata
- Efficient for storage and querying
- Clean data model

**GUI Format**:
- Embeds metadata with data
- Easier for UI rendering
- Self-documenting structure

## Related Files

- **convertjson2guijson.cpp** - Implementation
- **json_types.h** - JSON AST types
- **json_reader.h/json_writer.h** - JSON parsing/generation
