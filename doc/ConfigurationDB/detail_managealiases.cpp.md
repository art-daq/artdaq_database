# detail_managealiases.cpp

**Path:** `artdaq-database/ConfigurationDB/detail_managealiases.cpp`

**Implements:** [dboperation_managealiases.h](./dboperation_managealiases.h.md)

**Purpose:** Contains the core implementation logic for version alias management operations including finding, adding, and removing version aliases. Version aliases provide human-readable names (like "production", "latest", "stable") that point to specific document versions, making it easier to reference configurations without knowing exact version numbers.

## Implementation Overview

This file provides the internal (`detail` namespace) implementations for alias operations:
- **find_version_aliases**: Searches for aliases matching query criteria
- **add_version_alias**: Associates an alias name with a document version
- **remove_version_alias**: Removes an alias association from a document

Unlike document operations that only modify metadata, alias operations perform read-modify-write cycles to update the document's alias list.

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/ConfigurationDB/common.h` | Common definitions and TRACE configuration |
| `artdaq-database/ConfigurationDB/dboperation_managealiases.h` | Header being implemented |
| `artdaq-database/ConfigurationDB/shared_helper_functions.h` | Validation and helper functions |
| `artdaq-database/DataFormats/shared_literals.h` | String literal constants |
| `artdaq-database/SharedCommon/configuraion_api_literals.h` | API literal constants |
| `artdaq-database/ConfigurationDB/configuration_dbproviders.h` | Provider dispatch declarations |
| `artdaq-database/BasicTypes/basictypes.h` | Basic type definitions |
| `artdaq-database/DataFormats/Json/json_common.h` | JSON utilities |
| `artdaq-database/ConfigurationDB/options_operations.h` | ManageDocumentOperation, ManageAliasesOperation classes |
| `artdaq-database/JsonDocument/JSONDocumentBuilder.h` | Document manipulation |

## Type Definitions

### Options Alias

```cpp
using Options = cf::ManageAliasesOperation;
```

The `ManageAliasesOperation` class extends `ManageDocumentOperation` with alias-specific fields like `versionAlias`.

### Provider Function Type

```cpp
using provider_call_t = std::vector<JSONDocument> (*)(const Options&, const JSONDocument&);
```

Function pointer type for provider dispatch operations that return a list of documents.

## Internal Function Declarations

The file references detail functions from `detail_managedocument.cpp`:

```cpp
void write_document(cf::ManageDocumentOperation const&, std::string&);
void read_document(cf::ManageDocumentOperation const&, std::string&);
```

These are used to implement the read-modify-write pattern for alias operations.

## Internal Functions

### `find_version_aliases(Options const&, std::string&) -> void`

**Brief:** Searches for version aliases matching the query filter and returns them in the requested format.

**Parameters:**
- `options` - `Options const&` - Search configuration with query filter
- `configs` - `std::string&` - Output buffer for alias list (must be empty)

**Preconditions:**
- `options.operation()` must be `findversionalias`
- `configs` must be empty
- Provider name must be valid

**Postconditions:**
- `configs` contains list of matching aliases in requested format

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | When format is unsupported (db, json, fhicl, xml, unknown) |

**Thread Safety:** Conditional - depends on underlying provider implementation

**Implementation Details:**

1. Validates operation type and empty output buffer
2. Validates provider name using `validate_dbprovider_name()`
3. Dispatches to provider's `findVersionAliases` function with query filter
4. Formats results based on requested format

**Output Formats:**
- **gui**: `{"search": [<alias objects>]}`
- **csv**: Comma-separated alias names

---

### `add_version_alias(Options const&, std::string&) -> void`

**Brief:** Adds a version alias to a document, associating a human-readable name with the document's version using a read-modify-write pattern.

**Parameters:**
- `options` - `Options const&` - Operation configuration containing:
  - Version alias name to add
  - Document identifier (version, collection, entity)
- `conf` - `std::string&` - Output buffer for updated document (must be empty)

**Preconditions:**
- `options.operation()` must be `addversionalias`
- `conf` must be empty
- Provider name must be valid

**Postconditions:**
- The alias is associated with the specified document version
- `conf` contains the updated document in the requested format

**Throws:**

| Exception | Condition |
|-----------|-----------|
| Various | Exceptions from read/write operations |

**Thread Safety:** Not atomic - uses read-modify-write pattern

**Implementation Details:**

1. Validates operation type and empty output buffer
2. Validates provider name
3. Creates a `ManageDocumentOperation` from the alias options via `readJsonData()`
4. Sets operation to `readdocument` and format to `db`
5. Reads the document using `detail::read_document()`
6. Uses `JSONDocumentBuilder.addAlias()` to add the alias
7. Sets operation to `writedocument` and format to `db`
8. Writes the updated document using `detail::write_document()`
9. Reads the document again in the requested format
10. Returns the updated document

**Read-Modify-Write Pattern:**
```cpp
// Read
read_document(newOptions, document);
JSONDocumentBuilder builder{{document}};

// Modify
builder.addAlias({options.versionAlias_to_JsonData()});

// Write
auto updated = builder.to_string();
write_document(newOptions, updated);
```

---

### `remove_version_alias(Options const&, std::string&) -> void`

**Brief:** Removes a version alias from a document using a read-modify-write pattern.

**Parameters:**
- `options` - `Options const&` - Operation configuration containing:
  - Version alias name to remove
  - Document identifier
- `conf` - `std::string&` - Output buffer for updated document (must be empty)

**Preconditions:**
- `options.operation()` must be `rmversionalias`
- `conf` must be empty
- Provider name must be valid

**Postconditions:**
- The alias is removed from the document
- `conf` contains the updated document in the requested format

**Throws:**

| Exception | Condition |
|-----------|-----------|
| Various | Exceptions from read/write operations |

**Thread Safety:** Not atomic - uses read-modify-write pattern

**Implementation Details:**

1. Validates operation type and empty output buffer
2. Validates provider name
3. Creates a `ManageDocumentOperation` from the alias options
4. Sets operation to `readdocument` (no explicit format set)
5. Reads the document using `detail::read_document()`
6. Uses `JSONDocumentBuilder.removeAlias()` to remove the alias
7. Sets operation to `writedocument` and format to `db`
8. Writes the updated document
9. Clears query filter for re-read (uses document ID instead)
10. Reads the document again in the requested format
11. Returns the updated document

**Note:** The `queryFilter` is cleared before the final read because the document is now identified by its internal ID rather than the original query parameters.

---

### `debug::detail::ManageAliases() -> void`

**Brief:** Enables TRACE logging for the detail implementation of alias management operations.

**Purpose:** Configure TRACE logging infrastructure for debugging alias operations.

**Postconditions:**
- TRACE name set to "detail_managealiases.cpp"
- All trace levels enabled
- Trace modes configured per `trace_mode::modeM` and `trace_mode::modeS`

**Thread Safety:** Not thread-safe - should be called during initialization only

---

## Key Algorithms

### Read-Modify-Write Pattern

Both `add_version_alias` and `remove_version_alias` follow a read-modify-write pattern:

```cpp
// 1. Read the document in database format
newOptions.operation(apiliteral::operation::readdocument);
newOptions.format(data_format_t::db);
read_document(newOptions, document);

// 2. Modify using JSONDocumentBuilder
JSONDocumentBuilder builder{{document}};
builder.addAlias({options.versionAlias_to_JsonData()});  // or removeAlias

// 3. Write back to database
newOptions.operation(apiliteral::operation::writedocument);
newOptions.format(data_format_t::db);
auto updated = builder.to_string();
write_document(newOptions, updated);

// 4. Return document in requested format
newOptions.format(options.format());
document.clear();
read_document(newOptions, document);
```

**Why this approach:**
- Ensures atomic modification of the document (at the application level)
- Preserves all other document metadata
- Allows flexible output format selection

### Options Conversion

The alias-specific options are converted to document operations:

```cpp
auto newOptions = cf::ManageDocumentOperation{apiliteral::operation::addversionalias};
newOptions.readJsonData(options.to_JsonData());
```

This copies all relevant fields (collection, version, entity, etc.) from the alias options to the document operation.

### Output Formatting

For find operations, results are formatted similarly to other search operations:

**GUI Format:**
```cpp
std::ostringstream oss;
oss << "{ \"search\": [\n";
for (auto const& search_result : search_results) {
  oss << search_result << ",";
}
oss.seekp(-1, oss.cur);
oss << "] }";
```

**CSV Format:**
```cpp
std::ostringstream oss;
for (auto const& search_result : search_results) {
  oss << search_result.value_as<std::string>(apiliteral::name) << ",";
}
```

---

## Performance Considerations

- **Triple I/O**: Each add/remove operation requires three database operations (read, write, read)
- **No caching**: Documents are always read fresh from the database
- **Format conversion**: Final read may involve format conversion overhead
- **Provider dispatch**: Map lookup is O(1)

## Error Handling Strategy

1. **Precondition assertions**: Uses `confirm()` macro for internal consistency checks
2. **Validation**: Validates provider name before dispatch
3. **Exception propagation**: Exceptions from read/write operations propagate to caller
4. **Format validation**: Throws `runtime_error` for unsupported formats in find operation

## Testing Notes

- **Unit tests:** `test/ConfigurationDB/ManageAliases_t.cc`
- **Key test cases:**
  - Add/remove alias round-trip
  - Find aliases with various filters
  - Multiple aliases on same document
  - Alias on non-existent document (error case)

## Maintenance Notes

- The `queryFilter` clearing in `remove_version_alias` is intentional to allow re-read by document ID
- Format is explicitly set to `db` for internal read/write operations
- The final read uses the original format from the options
- Trace levels 15 and 16 are used for remove and add operations respectively

## Common Pitfalls

- **Alias uniqueness**: The system does not enforce unique alias names across documents; the same alias can point to multiple documents
- **Concurrent modifications**: No locking is performed; concurrent alias modifications may conflict
- **Output buffer**: Must be empty before calling; non-empty buffers will cause assertion failures
- **Race conditions**: The read-modify-write pattern is not atomic at the database level

## See Also

- [dboperation_managealiases.h](./dboperation_managealiases.h.md) - Header file
- [detail_managedocument.cpp](./detail_managedocument.cpp.md) - Document read/write operations
- [JSONDocumentBuilder.h](../JsonDocument/JSONDocumentBuilder.h.md) - Document manipulation
- [configuration_dbproviders.h](./configuration_dbproviders.h.md) - Provider declarations

---

**Documentation generated for artdaq-database ConfigurationDB module**
