# fhicl_types.h

## File Overview

Defines the type system for FHiCL (Fermilab Hierarchical Configuration Language) documents. Unlike JSON/XML/CONF, FHiCL has its own rich type system supporting comments, annotations, tables, and sequences.

**Location**: `/home/user/artdaq-database/artdaq-database/DataFormats/Fhicl/fhicl_types.h`

## Dependencies

### Third-Party Libraries
- `<boost/fusion/adapted/struct/adapt_struct.hpp>` - Boost.Fusion adaptation
- `<boost/fusion/include/adapt_struct.hpp>` - Boost.Fusion includes

### Project Headers
- `"artdaq-database/DataFormats/common.h"` - Common infrastructure
- `"artdaq-database/DataFormats/shared_types.h"` - Base templates

## Namespace

```cpp
artdaq::database::fhicl
```

**Alias**: `namespace fcl = artdaq::database::fhicl;`

## Type Definitions

### Forward Declarations

```cpp
struct table_t;
struct sequence_t;
```

### Variant Value Type

```cpp
using variant_value_t = sharedtypes::variant_value_of<table_t, sequence_t>;
```

Holds: table, sequence, string, decimal, integer, or bool.

### Basic Types

```cpp
using basic_key_t = sharedtypes::basic_key_t;                    // std::string
using optional_comment_t = sharedtypes::optional_comment_t;      // boost::optional<std::string>
using optional_annotation_t = sharedtypes::optional_annotation_t; // boost::optional<std::string>
```

### FHiCL-Specific Types

```cpp
using key_t = sharedtypes::key_of<basic_key_t, optional_comment_t>;
using value_t = sharedtypes::value_of<variant_value_t, optional_annotation_t>;
using atom_t = sharedtypes::kv_pair_of<key_t, value_t>;
```

| Type | Description |
|------|-------------|
| `key_t` | Key with optional comment |
| `value_t` | Value with optional annotation |
| `atom_t` | Key-value pair (FHiCL parameter) |

### Composite Types

```cpp
struct table_t : sharedtypes::table_of<atom_t> {};
struct sequence_t : sharedtypes::vector_of<value_t> {};
```

**table_t**: FHiCL table (like JSON object but with metadata)
**sequence_t**: FHiCL sequence (like JSON array but with metadata)

## Boost.Fusion Adaptation

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

**Purpose**: Enables Boost.Spirit parsers/generators to work with these structures.

## Design Features

**Metadata Support**:
- Keys can have comments
- Values can have annotations
- Preserves FHiCL documentation in data structure

**Hierarchical**:
- Tables contain atoms (parameters)
- Sequences contain values
- Supports arbitrary nesting

**FHiCL Compatibility**:
- Duplicate keys allowed
- Order preserved
- Comments/annotations maintained

## Usage Example

```cpp
using namespace artdaq::database::fhicl;

// Create table with commented key
table_t config;
key_t key("timeout", "Connection timeout in seconds");
value_t value(static_cast<integer>(30), "default value");
config.push_back(atom_t{key, value});

// Create sequence
sequence_t items;
items.push_back(value_t{std::string("item1")});
items.push_back(value_t{std::string("item2")});
```

## Related Files

- **fhicl_types.cpp** - Type implementations
- **fhicl_reader.h** - FHiCL parser
- **fhicl_writer.h** - FHiCL generator
- **shared_types.h** - Base templates
- **fhiclcpplib_includes.h** - FHiCL-CPP library integration
