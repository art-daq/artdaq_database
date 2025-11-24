# shared_literals.h

## File Overview

This header file defines a comprehensive set of string literals used throughout the DataFormats module. These literals represent JSON keys, data type identifiers, database field names, and other constant strings that are shared across different data format implementations (JSON, XML, FHiCL, CONF).

**Location**: `/home/user/artdaq-database/artdaq-database/DataFormats/shared_literals.h`

## Dependencies

### Standard Libraries
- `<string>` - For std::string type

### Using Directives

```cpp
using namespace std::string_literals;
```

This enables the `""s` string literal suffix for creating `std::string` objects directly from string literals.

## Header Guard

```cpp
#ifndef _ARTDAQ_DATABASE_SHAREDLITERALS_H_
#define _ARTDAQ_DATABASE_SHAREDLITERALS_H_
...
#endif
```

## Namespace

All literals are defined in the namespace:
```cpp
artdaq::database::dataformats::literal
```

## String Literals Reference

### General Purpose

| Literal | Value | Description |
|---------|-------|-------------|
| `dot` | `"."` | Dot character for path separators |
| `empty_json` | `"{}"` | Empty JSON object string |

### Database and Document Structure

| Literal | Value | Description |
|---------|-------|-------------|
| `database_record` | `"record"` | Database record field name |
| `main` | `"main"` | Main section identifier |
| `prolog` | `"prolog"` | Prolog section identifier |
| `entities` | `"entities"` | Entities collection name |
| `entity` | `"entity"` | Entity field name |
| `configurations` | `"configurations"` | Configurations collection name |
| `configuration` | `"configuration"` | Configuration field name |
| `collection` | `"collection"` | Collection field name |
| `document` | `"document"` | Document field name |

### MongoDB-Specific

| Literal | Value | Description |
|---------|-------|-------------|
| `id` | `"_id"` | MongoDB document ID field |
| `oid` | `"$oid"` | MongoDB ObjectId wrapper |
| `in` | `"$in"` | MongoDB $in operator |
| `ouid` | `"ouid"` | Object unique identifier (custom) |
| `filter` | `"filter"` | Query filter field |

### Metadata Fields

| Literal | Value | Description |
|---------|-------|-------------|
| `timestamp` | `"timestamp"` | Timestamp field |
| `source` | `"source"` | Source field |
| `format` | `"format"` | Format field |
| `file` | `"file"` | File field |
| `value` | `"value"` | Value field |
| `version` | `"version"` | Version field |
| `metadata` | `"metadata"` | Metadata section |
| `origin` | `"origin"` | Origin information |
| `data` | `"data"` | Data section |

### Data Type Identifiers

| Literal | Value | Description |
|---------|-------|-------------|
| `nil` | `"nil"` | Null/nil type |
| `string` | `"string"` | String type |
| `boolean` | `"bool"` | Boolean type |
| `complex` | `"complex"` | Complex type |
| `number` | `"number"` | Numeric type |
| `tableid` | `"tableid"` | Table identifier type |
| `type` | `"type"` | Type field |
| `object` | `"object"` | Object type |
| `table` | `"table"` | Table type |
| `array` | `"array"` | Array type |
| `sequence` | `"sequence"` | Sequence type |

### String Quoting Types

| Literal | Value | Description |
|---------|-------|-------------|
| `string_singlequoted` | `"string_singlequoted"` | Single-quoted string type |
| `string_doublequoted` | `"string_doublequoted"` | Double-quoted string type |
| `string_unquoted` | `"string_unquoted"` | Unquoted string type |

### Document Properties

| Literal | Value | Description |
|---------|-------|-------------|
| `empty` | `"empty"` | Empty property |
| `comment` | `"comment"` | Comment field |
| `annotation` | `"annotation"` | Annotation field |
| `protection` | `"protection"` | Protection field |
| `isdeleted` | `"isdeleted"` | Deletion flag |
| `isreadonly` | `"isreadonly"` | Read-only flag |
| `converted` | `"converted"` | Converted flag |
| `notprovided` | `"notprovided"` | Not provided indicator |

### Structural Elements

| Literal | Value | Description |
|---------|-------|-------------|
| `children` | `"children"` | Children nodes |
| `name` | `"name"` | Name field |
| `values` | `"values"` | Values field |
| `unknown` | `"unknown"` | Unknown type/value |
| `include` | `"#include"` | Include directive |
| `includes` | `"includes"` | Includes field |
| `linenum` | `"linenum"` | Line number field |

### Versioning and History

| Literal | Value | Description |
|---------|-------|-------------|
| `changelog` | `"changelog"` | Change log field |
| `bookkeeping` | `"bookkeeping"` | Bookkeeping section |
| `updates` | `"updates"` | Updates collection |
| `update` | `"update"` | Update field |
| `operation` | `"operation"` | Operation field |
| `history` | `"history"` | History section |
| `event` | `"event"` | Event field |

### Operations

| Literal | Value | Description |
|---------|-------|-------------|
| `assigned` | `"assigned"` | Assigned operation |
| `removed` | `"removed"` | Removed operation |
| `created` | `"created"` | Created operation |
| `operations` | `"operations"` | Operations field |
| `search` | `"search"` | Search operation |
| `query` | `"query"` | Query field |

### Collections and Data

| Literal | Value | Description |
|---------|-------|-------------|
| `runs` | `"runs"` | Runs collection |
| `attachments` | `"attachments"` | Attachments field |
| `comments` | `"comments"` | Comments field |
| `rawdatalist` | `"rawdata"` | Raw data list |
| `aliases` | `"aliases"` | Aliases collection |
| `alias` | `"alias"` | Alias field |
| `active` | `"active"` | Active flag |

### Configuration-Specific

| Literal | Value | Description |
|---------|-------|-------------|
| `configurationtype` | `"configtype"` | Configuration type field |
| `gui_data` | `"guidata"` | GUI data field |
| `schema` | `"schema"` | Schema field |

### Composite Path Literals

| Literal | Value | Description |
|---------|-------|-------------|
| `origin_format` | `"origin.format"` | Dot-notation path to origin format |
| `document_data` | `"document.data"` | Dot-notation path to document data |

### Regex Namespace

The `regex` nested namespace contains regex patterns:

```cpp
namespace regex {
    constexpr auto parse_decimal = "-?\\d*\\.?\\d+e[+-]?\\d+";
    constexpr auto parse_include = "(#include)";
}
```

| Literal | Pattern | Description |
|---------|---------|-------------|
| `parse_decimal` | `-?\\d*\\.?\\d+e[+-]?\\d+` | Matches scientific notation decimals |
| `parse_include` | `(#include)` | Matches include directive |

## Usage Examples

### Accessing Literals

```cpp
#include "artdaq-database/DataFormats/shared_literals.h"

namespace literal = artdaq::database::dataformats::literal;

// Using literals
auto key = literal::id;           // "_id"
auto oidKey = literal::oid;       // "$oid"
auto timestampKey = literal::timestamp;  // "timestamp"
```

### Building JSON Paths

```cpp
auto mainPath = literal::main;
auto timestampField = literal::timestamp;
auto fullPath = mainPath + "." + timestampField;  // "main.timestamp"

// Or use composite literals
auto originFormat = literal::origin_format;  // "origin.format"
```

### Type Checking

```cpp
if (typeField == literal::string) {
    // Handle string type
} else if (typeField == literal::number) {
    // Handle number type
} else if (typeField == literal::boolean) {
    // Handle boolean type
}
```

### MongoDB Queries

```cpp
// Build ObjectId query
std::ostringstream query;
query << "{" << literal::id << ":{"
      << literal::oid << ":\"" << objectId << "\"}}";
```

### Regex Usage

```cpp
std::regex decimal_regex(literal::regex::parse_decimal);
std::regex include_regex(literal::regex::parse_include);
```

## Design Rationale

**Benefits**:

1. **Centralization** - All string constants in one place reduces duplication and inconsistencies
2. **Type Safety** - Using `constexpr` provides compile-time constants with no runtime overhead
3. **Refactoring** - Changing a field name only requires updating one location
4. **Autocomplete** - IDEs can suggest available literals from the namespace
5. **Cross-Format Consistency** - JSON, XML, FHiCL, and CONF formats use the same field names

**Design Choices**:

1. **constexpr** - Provides compile-time evaluation and optimization
2. **Namespace organization** - Groups related literals under `artdaq::database::dataformats::literal`
3. **Descriptive names** - Literal names clearly indicate their purpose
4. **Regex namespace** - Separates regex patterns from simple string literals

## Common Patterns

### Document Structure Access

```cpp
// Accessing nested document fields
auto docData = document[literal::document][literal::data];
auto originFmt = document[literal::origin][literal::format];

// Or using composite literals
auto originFmt = document[literal::origin_format];  // Requires path parser
```

### Type Annotations

```cpp
struct FieldInfo {
    std::string type;
    std::string value;
};

FieldInfo field;
field.type = literal::string;
field.value = "example";
```

### MongoDB Extended JSON

```cpp
// Building MongoDB ObjectId in extended JSON format
auto buildOid = [](const std::string& oid) {
    return "{"s + literal::oid + ":\"" + oid + "\"}";
};
```

## Related Files

- **shared_types.h** - Defines types that use these literals for field names
- **Json/json_types.h** - JSON-specific type definitions using these literals
- **Xml/xml_types.h** - XML-specific type definitions using these literals
- **Fhicl/fhicl_types.h** - FHiCL-specific type definitions using these literals
- **Conf/conf_types.h** - CONF-specific type definitions using these literals

## Notes

- All literals are `constexpr`, meaning they're evaluated at compile time with no runtime overhead
- The literals support multiple data formats while maintaining consistent naming
- MongoDB-specific literals (`$oid`, `$in`, `_id`) follow MongoDB conventions
- The regex namespace provides patterns for parsing specific data formats
- Composite path literals (like `origin_format`) are convenience constants for common dot-notation paths
- Using namespace-qualified access (`literal::`) rather than `using namespace literal` is recommended to avoid name collisions
