# fhicl_types.h

**Path:** `artdaq-database/DataFormats/Fhicl/fhicl_types.h`

**Purpose:** Defines the type system for FHiCL (Fermilab Hierarchical Configuration Language) documents. Unlike JSON/XML/CONF, FHiCL has its own rich type system supporting comments, annotations, tables, and sequences with full metadata preservation.

## Key Concepts

### Why FHiCL Has Its Own Types

FHiCL configurations in high-energy physics experiments often contain extensive documentation:
- **Comments**: Describe the purpose of parameters
- **Annotations**: Specify units, constraints, or default values

Preserving this metadata requires types that go beyond simple JSON structures.

### Type Hierarchy

```
table_t (root container)
    |
    +-- atom_t (key-value pair)
            |
            +-- key_t (with optional comment)
            |       |
            |       +-- basic_key_t (std::string)
            |       +-- optional_comment_t
            |
            +-- value_t (with optional annotation)
                    |
                    +-- variant_value_t (table, sequence, string, number, bool)
                    +-- optional_annotation_t
```

## Thread Safety

- **Thread-safe:** Yes (header-only, types are value types)
- **Concurrent access:** Types can be used in multiple threads
- **Locking:** None (users must protect shared instances)

## Dependencies

| Include | Purpose |
|---------|---------|
| `<boost/fusion/adapted/struct/adapt_struct.hpp>` | Boost.Fusion struct adaptation |
| `<boost/fusion/include/adapt_struct.hpp>` | Boost.Fusion includes |
| `artdaq-database/DataFormats/common.h` | Common infrastructure |
| `artdaq-database/DataFormats/shared_types.h` | Base template types |

## Type Aliases

### Basic Types

| Type | Definition | Description |
|------|------------|-------------|
| `basic_key_t` | `std::string` | Simple string key |
| `optional_comment_t` | `boost::optional<std::string>` | Optional comment text |
| `optional_annotation_t` | `boost::optional<std::string>` | Optional annotation text |

### FHiCL-Specific Types

| Type | Definition | Description |
|------|------------|-------------|
| `key_t` | `key_of<basic_key_t, optional_comment_t>` | Key with optional comment |
| `value_t` | `value_of<variant_value_t, optional_annotation_t>` | Value with optional annotation |
| `atom_t` | `kv_pair_of<key_t, value_t>` | Complete key-value pair |
| `variant_value_t` | `variant_value_of<table_t, sequence_t>` | Union of all value types |

## Classes/Structures

### `table_t`

**Brief:** FHiCL table structure (analogous to JSON object but with metadata).

```cpp
struct table_t : sharedtypes::table_of<atom_t> {};
```

**Inherits from:** `sharedtypes::table_of<atom_t>`

**Features:**
- Maintains insertion order
- Allows duplicate keys (FHiCL requirement)
- Contains `atom_t` elements (key-value pairs with metadata)

### `sequence_t`

**Brief:** FHiCL sequence structure (analogous to JSON array but with metadata).

```cpp
struct sequence_t : sharedtypes::vector_of<value_t> {};
```

**Inherits from:** `sharedtypes::vector_of<value_t>`

**Features:**
- Maintains insertion order
- Contains `value_t` elements (values with annotations)

## Boost.Fusion Adaptation

The types are adapted for Boost.Spirit parsing/generation:

```cpp
BOOST_FUSION_ADAPT_STRUCT(fcl::atom_t,
    (fcl::key_t, key)
    (fcl::value_t, value))

BOOST_FUSION_ADAPT_STRUCT(fcl::key_t,
    (fcl::optional_comment_t, comment)
    (fcl::basic_key_t, key))

BOOST_FUSION_ADAPT_STRUCT(fcl::value_t,
    (fcl::variant_value_t, value)
    (fcl::optional_annotation_t, annotation))
```

**Purpose:** Enables Boost.Spirit parsers and generators to directly populate and read these structures.

## Usage Example

```cpp
#include "artdaq-database/DataFormats/Fhicl/fhicl_types.h"

using namespace artdaq::database::fhicl;

void buildFhiclAST() {
  // Create table with commented key
  table_t config;

  // Create a key with comment
  key_t key;
  key.key = "timeout";
  key.comment = "Connection timeout in seconds";

  // Create a value with annotation
  value_t value;
  value.value = static_cast<integer>(30);
  value.annotation = "default value";

  // Add to table
  atom_t atom;
  atom.key = key;
  atom.value = value;
  config.push_back(atom);

  // Create a sequence
  sequence_t items;
  value_t item1;
  item1.value = std::string("item1");
  items.push_back(item1);
}
```

## Relationship to Other Components

```
fhicl_types.h
    |
    +-- shared_types.h (base templates)
    |       +-- key_of, value_of, kv_pair_of
    |       +-- table_of, vector_of
    |       +-- variant_value_of
    |
    +-- Used by:
            +-- fhicl_reader.h (parser output)
            +-- fhicl_writer.h (generator input)
            +-- convertfhicl2jsondb.h (conversion)
```

### Comparison with Other Format Types

| Feature | FHiCL | JSON | CONF/XML |
|---------|-------|------|----------|
| Own type system | Yes (`table_t`) | Yes (`object_t`) | No (uses JSON) |
| Comment support | Yes | No | No |
| Annotation support | Yes | No | No |
| Duplicate keys | Yes | Yes | Yes |

## Notes for Developers

- All types are header-only with no separate implementation file except `fhicl_types.cpp` for utility functions
- The namespace alias `namespace fcl = artdaq::database::fhicl;` is provided for convenience
- Boost.Fusion adaptation enables direct use with Boost.Spirit parsers
- The `variant_value_t` uses `boost::recursive_wrapper` for nested structures

### Common Pitfalls

- **Wrong namespace:** Use `fhicl::` or `fcl::`, not `json::`
- **Missing Fusion adaptation:** Custom types won't work with Spirit parsers without adaptation
- **Forgetting metadata:** Comments/annotations are optional but should be preserved when present

## See Also

- [fhicl_types.cpp](./fhicl_types.cpp.md) - Type implementations
- [fhicl_reader.h](./fhicl_reader.h.md) - FHiCL parser using these types
- [fhicl_writer.h](./fhicl_writer.h.md) - FHiCL generator using these types
- [../shared_types.h](../shared_types.h.md) - Base template definitions
