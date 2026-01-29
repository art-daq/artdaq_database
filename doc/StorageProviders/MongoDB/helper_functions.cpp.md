# helper_functions.cpp

**Path:** `artdaq-database/StorageProviders/MongoDB/helper_functions.cpp`

**Purpose:** Implements MongoDB-specific helper functions, primarily the `rewrite_query_with_regex()` function that transforms wildcard search patterns into MongoDB regex queries for flexible configuration searching.

## Implementation Overview

This file provides utility functions that support MongoDB query operations. The primary function transforms user-friendly wildcard patterns (using `*`) into MongoDB's `$regex` query operators, enabling pattern-based searches across configurations and entities.

## Key Algorithms

### Wildcard to Regex Transformation

The `rewrite_query_with_regex()` function converts wildcard patterns to MongoDB regex expressions.

**Transformation Rules:**
| Input Pattern | MongoDB Query |
|---------------|---------------|
| `"exact_match"` | `"exact_match"` (unchanged) |
| `"prefix_*"` | `{"$regex": "^prefix_.*", "$options": ""}` |
| `"*"` | `{"$regex": ".*", "$options": ""}` |

**Steps:**
1. Parse the input query JSON into an AST (Abstract Syntax Tree) using `JsonReader`
2. If the AST is empty or fields list is empty, return the original query unchanged
3. For each specified field:
   a. Check if the field exists in the query
   b. Extract the string value using `unwrap().value_as<std::string>()`
   c. If the value ends with `*`:
      - Replace the field value with a regex object
      - Convert `prefix_*` to `^prefix_.*` regex (with `^` anchor)
      - Handle `*` alone as `.*` (match anything)
4. Serialize the modified AST back to JSON using `JsonWriter`
5. Return the transformed query as a new JSONDocument

**Why this approach:**
- Allows users to use intuitive wildcards without knowing MongoDB syntax
- Maintains compatibility with exact matches (no `*` suffix)
- Uses MongoDB's native regex engine for efficient pattern matching
- The `^` anchor ensures prefix matching starts at the beginning of the string

## Functions

### `rewrite_query_with_regex(JSONDocument const& query, std::vector<std::string> const& fields) -> JSONDocument`

**Brief:** Transforms wildcard patterns in specified query fields into MongoDB regex expressions for pattern-based searching.

**Parameters:**
- `query` - Original JSON query document
- `fields` - List of field names to check for wildcard patterns

**Preconditions:**
- `query` must be valid JSON
- Fields containing wildcards must have string values

**Postconditions:**
- Fields ending with `*` are replaced with `$regex` objects
- Fields without `*` suffix are unchanged
- Fields not present in the query are ignored

**Returns:** New JSON document with wildcards converted to regex queries

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_error("MongoDB")` | When JSON parsing fails |
| `runtime_error("MongoDB")` | When JSON serialization fails |

**Thread Safety:** Safe (no shared state)

**Example:**
```cpp
#include "artdaq-database/JsonDocument/JSONDocument.h"

using namespace artdaq::database;
using namespace artdaq::database::mongo;

void searchWithWildcard() {
  // Original query with wildcards
  docrecord::JSONDocument query(R"({
    "configurations": "production_*",
    "version": "1.0.0"
  })");

  // Specify which fields to transform
  std::vector<std::string> fields = {"configurations"};

  try {
    // Transform wildcards to regex
    auto regexQuery = rewrite_query_with_regex(query, fields);

    // Result:
    // {
    //   "configurations": {"$regex": "^production_.*", "$options": ""},
    //   "version": "1.0.0"
    // }
  } catch (const runtime_error& e) {
    std::cerr << "Query transformation failed: " << e.what() << std::endl;
  }
}
```

**Match-All Wildcard Example:**
```cpp
void matchAllEntities() {
  docrecord::JSONDocument query(R"({"entity": "*"})");
  std::vector<std::string> fields = {"entity"};

  auto regexQuery = rewrite_query_with_regex(query, fields);
  // Result: {"entity": {"$regex": ".*", "$options": ""}}
  // This matches any entity name
}
```

## Implementation Details

### JSON AST Manipulation

The function uses the JSON reader/writer infrastructure to parse and modify the query:

```cpp
object_t query_ast;

if (!JsonReader().read(query, query_ast)) {
  throw runtime_error("MongoDB") << "Failed to create AST in rewrite_query_with_regex()";
}

// Modify AST...

auto buffer = std::string{};
if (!JsonWriter().write(query_ast, buffer)) {
  throw runtime_error("MongoDB") << "Failed to write AST in rewrite_query_with_regex()";
}
```

### Regex Pattern Construction

For wildcard patterns, the function constructs a regex object:

```cpp
// For "prefix_*" pattern (removes trailing * and adds regex anchors)
value.pop_back();  // Remove '*'
regex_value["$regex"] = std::string("^") + value + ".*";  // "^prefix_.*"
regex_value["$options"] = std::string("");                 // No special options

// For "*" alone (match anything)
regex_value["$regex"] = std::string(".*");  // Match any string
regex_value["$options"] = std::string("");
```

### Value Extraction with `unwrap`

The function uses the `sharedtypes::unwrap` utility to access typed values from the AST:

```cpp
auto value = unwrap(query_ast.at(field)).value_as<std::string>();
```

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/BasicTypes/basictypes.h` | Basic type definitions |
| `artdaq-database/DataFormats/Json/json_common.h` | JSON format utilities |
| `artdaq-database/DataFormats/Json/json_types_impl.h` | JSON AST types (`object_t`, `value_t`) |
| `artdaq-database/JsonDocument/JSONDocument.h` | JSON document class |
| `artdaq-database/SharedCommon/sharedcommon_common.h` | Common utilities including `unwrap` |

## Type Aliases Used

| Type | Source | Purpose |
|------|--------|---------|
| `object_t` | `artdaq::database::json` | JSON object (map) type |
| `value_t` | `artdaq::database::json` | JSON value variant type |
| `JsonReader` | `artdaq::database::json` | JSON parser |
| `JsonWriter` | `artdaq::database::json` | JSON serializer |

## Performance Considerations

- **Parsing overhead:** The function parses and re-serializes JSON, adding overhead for each call. For frequently used queries, consider caching transformed versions.

- **Field scanning:** Each field in the `fields` vector triggers a lookup in the AST. Keep the fields list minimal.

- **String operations:** The wildcard pattern extraction involves string manipulation (checking suffix, removing last character).

## Error Handling Strategy

1. **Empty fields list:** Returns original query unchanged (early return)
2. **Empty query:** Returns unchanged if query AST is empty (early return)
3. **Missing fields:** Silently skips fields not present in the query (uses `count()` check)
4. **Parse failures:** Throws `runtime_error` with descriptive message
5. **Non-wildcard values:** Skips fields where value does not end with `*`

## Testing Notes

- **Unit tests:** Located in `test/StorageProviders/MongoDB/`
- **Key test cases:**
  - Exact match (no wildcard) - unchanged
  - Prefix wildcard (`name_*`)
  - Match-all wildcard (`*`)
  - Multiple fields with wildcards
  - Missing fields (should be skipped)
  - Empty query (should return unchanged)
  - Empty fields list (should return unchanged)

## Maintenance Notes

- This function is used by `findConfigurations()` and `findVersions()` in `provider_mongodb.cpp`.

- The regex pattern uses case-sensitive matching by default (`$options: ""`). For case-insensitive matching, modify to use `"$options": "i"`.

- The `^` anchor in prefix patterns ensures matching starts at the beginning of the string, preventing false matches in the middle of values.

## See Also

- [provider_mongodb.cpp.md](./provider_mongodb.cpp.md) - Uses this function for query operations
- [JSONDocument.h.md](../../JsonDocument/JSONDocument.h.md) - Document class definition
- [json_types_impl.h.md](../../DataFormats/Json/json_types_impl.h.md) - JSON AST implementation
- [External: MongoDB $regex](https://www.mongodb.com/docs/manual/reference/operator/query/regex/) - MongoDB regex query documentation
