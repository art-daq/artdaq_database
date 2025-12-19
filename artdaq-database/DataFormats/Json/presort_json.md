# presort_json - GUI JSON Search Array Sorter

## Overview

The `presort_gui_json_by_name` function sorts the `search` array entries in a GUI JSON document by their `name` field. This ensures deterministic ordering for comparison operations and consistent output regardless of the order in which database queries return results.

## File Format

### Input JSON Structure

The function expects a JSON object containing a `search` array. Each element in the array is an object with at least a `name` field:

```json
{
  "search": [
    {
      "name": "config:entity003",
      "query": { ... }
    },
    {
      "name": "config:entity001",
      "query": { ... }
    },
    {
      "name": "config:entity002",
      "query": { ... }
    }
  ]
}
```

### Output JSON Structure

After sorting, entries are ordered alphabetically by `name`:

```json
{
  "search": [
    {
      "name": "config:entity001",
      "query": { ... }
    },
    {
      "name": "config:entity002",
      "query": { ... }
    },
    {
      "name": "config:entity003",
      "query": { ... }
    }
  ]
}
```

### Typical Entry Schema

Each search entry typically contains:

| Field | Type | Description |
|-------|------|-------------|
| `name` | string | Identifier used for sorting (format: `configName:entityName`) |
| `query` | object | Query parameters for loading the configuration |
| `query.collection` | string | MongoDB collection name |
| `query.dbprovider` | string | Database provider (`mongo`, `filesystem`) |
| `query.dataformat` | string | Output format (`gui`, `json`, `fhicl`) |
| `query.operation` | string | Operation type (`load`, `store`, etc.) |
| `query.filter` | object | Filter criteria for the query |

## Algorithm

### AST-Based Approach

The implementation uses the existing JSON AST (Abstract Syntax Tree) infrastructure rather than custom string parsing:

```
┌─────────────────┐
│  Input JSON     │
│    (string)     │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│  JsonReader     │
│  parse → AST    │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│  Find "search"  │
│  key in AST     │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│  Extract array  │
│  (array_t)      │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│  Sort by "name" │
│  field value    │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│  JsonWriter     │
│  AST → string   │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│  Output JSON    │
│    (string)     │
└─────────────────┘
```

### Implementation Steps

1. **Parse**: Convert JSON string to `object_t` AST using `JsonReader::read()`

2. **Locate**: Find the `"search"` key in the root object using `ast.find("search")`

3. **Extract**: Get the `array_t` value using `boost::get<array_t>()`

4. **Sort**: Use `std::list::sort()` with a custom comparator that:
   - Extracts the `"name"` field from each object element
   - Compares names using standard string comparison (`<`)

5. **Serialize**: Convert modified AST back to JSON using `JsonWriter::write()`

### Key Data Structures

```cpp
// AST types (from json_types.h)
object_t  // table_of<data_t> - ordered key-value pairs
array_t   // vector_of<value_t> - uses std::list internally
value_t   // variant: object_t | array_t | string | decimal | integer | bool
data_t    // key-value pair: { key: string, value: value_t }
```

### Sorting Details

The `array_t` type uses `std::list<value_t>` internally (not `std::vector`), so the implementation uses `list::sort()` instead of `std::sort()`:

```cpp
search_array->values.sort(NameComparator{});
```

The `NameComparator` functor:
1. Attempts to extract each element as `object_t`
2. Looks up the `"name"` key in each object
3. Extracts the string value
4. Returns `name_a < name_b` for lexicographic ordering

## Error Handling

The function returns the original input unchanged when:

- JSON parsing fails
- No `"search"` key exists in the root object
- The `"search"` value is not an array
- The array is empty
- JSON serialization fails

## Usage

```cpp
#include "artdaq-database/DataFormats/Json/presort_json.h"

std::string input = R"({"search": [{"name": "b"}, {"name": "a"}]})";
std::string sorted = artdaq::database::json::presort_gui_json_by_name(input);
// Result: {"search": [{"name": "a"}, {"name": "b"}]}
```

## Design Rationale

### Why AST-Based?

The previous implementation used a custom JSON parser (~300 lines). The AST-based approach:

1. **Reuses existing code**: Leverages `JsonReader`/`JsonWriter` already in the codebase
2. **Reduces maintenance**: Single JSON parsing implementation to maintain
3. **Ensures consistency**: Same parsing rules as `compare_json_objects` and other JSON operations
4. **Improves reliability**: Well-tested Spirit Qi/Karma grammars handle edge cases

### Comparison with `compare_json_objects`

Both functions follow the same pattern:

```cpp
// compare_json_objects
JsonReader{}.read(first, firstAST);
JsonReader{}.read(second, secondAST);
return firstAST == secondAST;

// presort_gui_json_by_name
JsonReader{}.read(json, ast);
// ... modify ast ...
JsonWriter{}.write(ast, result);
```

## File Location

- Header: `artdaq-database/DataFormats/Json/presort_json.h`
- Implementation: `artdaq-database/DataFormats/Json/presort_json.cpp`
- Documentation: `artdaq-database/DataFormats/Json/presort_json.md`
