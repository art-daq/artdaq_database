# dispatch_filedb.cpp

**Path:** `artdaq-database/ConfigurationDB/dispatch_filedb.cpp`

**Implements:** [dispatch_filedb.h](./dispatch_filedb.h.md)

**Purpose:** Implements the FileSystemDB storage provider dispatch functions that route database operations to file-based storage. This file contains the logic for storing, retrieving, and managing configuration documents as JSON files organized in a directory structure.

## Implementation Overview

This file provides the bridge between the ConfigurationDB API layer and the low-level FileSystemDB storage provider. Each function follows a consistent pattern:

1. **Validate operation parameters** - Check operation type and provider name
2. **Create database connection** - Instantiate `DBConfig` and `DB::create()`
3. **Create provider** - Instantiate `DBProvider<JSONDocument>` for document operations
4. **Execute operation** - Call the appropriate provider method
5. **Post-process results** - Filter, format, or extract data using regex as needed

The implementation uses regex patterns to extract structured data from JSON filter strings, enabling version and entity name extraction for result formatting.

## Thread Safety

- **Thread-safe:** Conditional
- **Concurrent access:** File-level operations are protected by the underlying provider; concurrent access to different files is safe
- **Locking:** Relies on filesystem-level locking in the `DBProvider` layer

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/ConfigurationDB/common.h` | Common utilities, `runtime_error`, `confirm` macro |
| `artdaq-database/ConfigurationDB/dispatch_filedb.h` | Function declarations being implemented |
| `artdaq-database/ConfigurationDB/options_operations.h` | `ManageDocumentOperation`, `ManageAliasesOperation` classes |
| `artdaq-database/BasicTypes/basictypes.h` | Basic type definitions |
| `artdaq-database/DataFormats/shared_literals.h` | JSON literal string definitions (`jsonliteral::name`, etc.) |
| `artdaq-database/JsonDocument/JSONDocumentBuilder.h` | Document construction and modification utilities |
| `artdaq-database/StorageProviders/FileSystemDB/provider_filedb.h` | Low-level `DBConfig`, `DB`, `DBProvider` classes |
| `<cstdio>` | `popen()`, `pclose()`, `fgets()` for shell command execution |
| `<sstream>` | `std::ostringstream`, `std::istringstream` for string building |

## Key Algorithms

### Document Version Resolution

When reading a single document and multiple matches exist, the implementation selects the latest version using `compareDocumentVersions`:

```cpp
std::swap(data,
  collection.size() > 1
    ? *std::max_element(collection.begin(), collection.end(),
                        docrec::compareDocumentVersions)
    : *collection.begin());
```

**Why this approach:** Ensures deterministic behavior when queries match multiple versions, always returning the most recent.

### Filter Parsing with Regex

Several operations use regex to extract structured information from JSON filter strings. This approach parses the provider's internal filter format to extract human-readable names.

**Version extraction pattern:**
```cpp
auto ex = std::regex(
    "\"version\"\\s*:\\s*\"((\\\\\"|[^\"])*)\"\\,\\s*"
    "\"entities\\.name\"\\s*:\\s*\"((\\\\\"|[^\"])*)\"");
```
- Captures: `results[1]` = version string, `results[3]` = entity name

**Configuration composition pattern:**
```cpp
auto ex = std::regex(
    "\"configurations\\.name\"\\s*:\\s*\"((\\\\\"|[^\"])*)\"\\,"
    "\\s*\"entities\\.name\"\\s*:\\s*\"((\\\\\"|[^\"])*)\"");
```
- Captures: `results[1]` = configuration name, `results[3]` = entity name

**Entity extraction pattern:**
```cpp
auto ex = std::regex(R"lit(\s"(entities\.name)"\s:\s"((\\"|[^"])*)")lit");
```
- Captures: `results[2]` = entity name

### Duplicate Filtering

The `findConfigurations` function uses a seen-values list to filter duplicates efficiently:

```cpp
auto seenValues = std::list<std::string>{};
seenValues.emplace_back(apiliteral::notprovided);  // Pre-seed with sentinel

auto isNew = [&v = seenValues](auto const& name) {
  confirm(!name.empty());
  if (std::find(v.begin(), v.end(), name) != v.end()) {
    return false;
  }
  v.emplace_back(name);
  return true;
};
```

**Why `std::list`:** O(n) lookup is acceptable for typical configuration counts; list allows efficient appending without invalidating iterators.

### Shell Command Execution for findCompositionsContaining

The filesystem implementation of `findCompositionsContaining` uses shell commands to search JSON files:

```cpp
std::ostringstream grep_cmd;
grep_cmd << "grep -l '\"" << configurationType << "\"' "
         << search_dir << "/*.json 2>/dev/null";
grep_cmd << " | xargs grep -l '\"" << version << "\"' 2>/dev/null";

if (!entity.empty() && entity != apiliteral::notprovided) {
  grep_cmd << " | xargs grep -l '\"" << entity << "\"' 2>/dev/null";
}

FILE* pipe = popen(grep_cmd.str().c_str(), "r");
```

**Why shell commands:** Leverages optimized system utilities for file searching rather than implementing custom directory traversal.

**Security note:** Input parameters should be validated before use to prevent shell injection.

## Internal Functions

### `writeDocument(ManageDocumentOperation const& options, JSONDocument const& insert_payload) -> void`

**Brief:** Writes a JSON document to the filesystem storage using the configured database provider.

**Called by:** `detail::write_document()` when provider is "filesystem"

**Implementation Steps:**
1. Validate operation type is `writedocument` or `overwritedocument`
2. Validate provider is `filesystem`
3. Create `DBConfig` with default settings
4. Create database instance via `DB::create(config)`
5. Create `DBProvider<JSONDocument>` instance
6. Call `provider->writeDocument()` and log the returned object ID

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Wrong operation option |
| `runtime_error` | Wrong provider option |
| `runtime_error` | File write failure (propagated from provider) |

### `readDocument(ManageDocumentOperation const& options, JSONDocument const& search_payload) -> JSONDocument`

**Brief:** Reads a single document from the filesystem, selecting the highest version if multiple matches exist.

**Called by:** `detail::read_document()` when provider is "filesystem"

**Implementation Steps:**
1. Delegate to `readDocuments()` to get all matches
2. If empty, throw `runtime_error`
3. If multiple results, use `std::max_element` with `compareDocumentVersions` to find latest
4. Return the selected document via `std::swap`

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | No documents found matching query |

### `readDocuments(ManageDocumentOperation const& options, JSONDocument const& search_payload) -> std::vector<JSONDocument>`

**Brief:** Reads all documents matching the search query from the filesystem.

**Called by:** `readDocument()`, `detail::read_documents()`

**Implementation Steps:**
1. Validate operation is `readdocument` and provider is `filesystem`
2. Create database configuration and provider
3. Call `provider->readDocument()` (returns vector)
4. Return the result vector

### `findConfigurations(ManageDocumentOperation const& options, JSONDocument const& search_payload) -> std::vector<JSONDocument>`

**Brief:** Finds all unique global configurations in the database, filtering duplicates.

**Implementation Steps:**
1. Validate operation (`findconfigs`) and provider (`filesystem`)
2. Call `provider->findConfigurations()`
3. Initialize seen-values list with sentinel
4. For each result, extract configuration name from filter field
5. Skip if already seen; otherwise add to seen list
6. Build result JSON with `name` and `query` fields
7. Return formatted results

### `configurationComposition(ManageDocumentOperation const& options, JSONDocument const& search_payload) -> JSONDocument`

**Brief:** Retrieves the complete configuration composition, listing all component configurations.

**Implementation Steps:**
1. Validate operation (`confcomposition`) and provider (`filesystem`)
2. Call `provider->configurationComposition()`
3. If empty, throw `runtime_error`
4. For each query payload, parse filter JSON with regex
5. Extract configuration name and entity name
6. Build JSON array with `name` (format: "config:entity") and `query` fields
7. Return wrapped in `{ "search": [...] }` structure

### `findVersions(ManageDocumentOperation const& options, JSONDocument const& query_payload) -> std::vector<JSONDocument>`

**Brief:** Finds all versions of documents for the specified entity and collection.

**Implementation Steps:**
1. Validate operation (`findversions`) and provider (`filesystem`)
2. Call `provider->findVersions<JSONDocument>(options)`
3. For each result, parse filter JSON with version regex
4. Extract version string (capture group 1)
5. Build result JSON with `name` (version) and `query` fields
6. Return formatted results

### `findEntities(ManageDocumentOperation const& options, JSONDocument const& query_payload) -> std::vector<JSONDocument>`

**Brief:** Finds all unique entities in the specified collection.

**Implementation Steps:**
1. Validate operation (`findentities`) and provider (`filesystem`)
2. Call `provider->findEntities<JSONDocument>(options)`
3. For each result, parse filter JSON with entity regex
4. Extract entity name (capture group 2)
5. Build result JSON with `name` (entity) and `query` fields
6. Return formatted results

### `assignConfiguration(ManageDocumentOperation const& options, JSONDocument const& search_payload) -> JSONDocument`

**Brief:** Assigns a document to a global configuration by adding a configuration reference.

**Implementation Steps:**
1. Validate operation (`assignconfig`) and provider (`filesystem`)
2. Create temporary options with `readdocument` operation
3. Build search query with filter and collection
4. Read the existing document
5. Create `JSONDocumentBuilder` from document
6. Call `builder.addConfiguration()` with configuration JSON
7. Update options to `writedocument`
8. Build update payload with filter (object ID) and modified document
9. Write updated document
10. Return configuration composition for the updated configuration

### `removeConfiguration(ManageDocumentOperation const& options, JSONDocument const& search_payload) -> JSONDocument`

**Brief:** Removes a document assignment from a global configuration.

**Implementation Steps:**
1. Validate operation (`removeconfig`) and provider (`filesystem`)
2. Create temporary options with `readdocument` operation
3. Build search query and read existing document
4. Create `JSONDocumentBuilder` and call `builder.removeConfiguration()`
5. Write updated document
6. Return updated configuration composition

### `findVersionAliases(ManageAliasesOperation const&, JSONDocument const&) -> std::vector<JSONDocument>`

**Brief:** Placeholder function for finding version aliases. Currently not implemented for FileSystemDB.

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Always - "findVersionAliases: is not implemented" |

### `listCollections(ManageDocumentOperation const& options, JSONDocument const& search_payload) -> std::vector<JSONDocument>`

**Brief:** Lists all collections (directories) in the database.

**Implementation Steps:**
1. Validate operation (`listcollections`) and provider (`filesystem`)
2. Create provider and call `provider->listCollections()`
3. For each result, extract collection name
4. Build result JSON with `name` and `query` fields
5. Return formatted results

### `listDatabases(ManageDocumentOperation const& options, JSONDocument const& search_payload) -> std::vector<JSONDocument>`

**Brief:** Lists all databases (top-level directories) available.

**Implementation Steps:**
1. Validate operation (`listdatabases`) and provider (`filesystem`)
2. Create provider and call `provider->listDatabases()`
3. For each result, extract database name
4. Build result JSON with `name` and `query` fields
5. Return formatted results

### `readDbInfo(ManageDocumentOperation const& options, JSONDocument const& search_payload) -> JSONDocument`

**Brief:** Reads database-level metadata and statistics.

**Implementation Steps:**
1. Validate operation (`readdbinfo`) and provider (`filesystem`)
2. Create provider and call `provider->databaseMetadata()`
3. If empty, return empty search result literal
4. Return first result wrapped in `{ "search": ... }` structure

### `searchCollection(ManageDocumentOperation const&, JSONDocument const&) -> std::vector<JSONDocument>`

**Brief:** Placeholder for collection search. Currently not implemented for FileSystemDB.

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Always - "findVersionAliases: is not implemented" (error message incorrectly refers to wrong function) |

### `findCompositionsContaining(ManageDocumentOperation const& options, JSONDocument const& search_payload) -> std::vector<JSONDocument>`

**Brief:** Finds all global configuration compositions that contain a specific configuration type, version, and optionally entity.

**Implementation Steps:**
1. Validate operation (`findcompositionscontaining`) and provider (`filesystem`)
2. Extract search parameters from options
3. Validate configuration type and version are provided
4. Build grep command to search SystemConfiguration directory
5. Execute command via `popen()`
6. Parse output line by line to extract filenames
7. Extract composition name from filename (basename without extension)
8. Build result JSON with composition name
9. Return formatted results

**Note:** Uses shell commands for searching, which may have security implications if inputs are not sanitized.

### `debug::FileSystemDB() -> void`

**Brief:** Enables TRACE debugging for the FileSystemDB dispatch module.

**Implementation:**
```cpp
void cf::debug::FileSystemDB() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);
  TLOG(10) << "artdaq::database::configuration::FileSystemDB trace_enable";
}
```

## Error Handling Strategy

All functions follow a consistent error handling pattern:

1. **Precondition validation:** Check operation and provider options at function entry
2. **Exception propagation:** Low-level exceptions from provider are not caught; they propagate to caller
3. **Meaningful errors:** `runtime_error` thrown with descriptive messages including actual vs. expected values
4. **TRACE logging:** Key operations log at various TRACE levels for debugging

## Performance Considerations

- **Regex compilation:** Regex patterns are compiled on each function call; could be cached as static locals for better performance in hot paths
- **Shell commands:** `findCompositionsContaining` uses `popen()` which has significant process creation overhead
- **Multiple reads:** Operations like `assignConfiguration` read and write the same document, requiring two I/O operations
- **String stream allocation:** Frequent `std::ostringstream` usage for JSON construction; consider using a JSON library for complex operations
- **Linear search:** Duplicate filtering uses linear search through `std::list`; acceptable for typical configuration counts

## Testing Notes

- **Unit tests:** `test/ConfigurationDB/FileSystemDB_t.cc`
- **Integration tests:** Tests that exercise the full provider stack
- **Key test cases:**
  - Write and read document round-trip
  - Version resolution with multiple matches
  - Configuration assignment and removal
  - List operations for collections and databases
  - Error handling for invalid operations

## Maintenance Notes

### Regex Patterns

The regex patterns for filter parsing are fragile and depend on specific JSON formatting from the provider:
- Changes to provider JSON output format may break parsing
- Patterns assume specific field ordering and spacing
- Consider using a JSON parser instead of regex for robustness

### Shell Command Security

The `findCompositionsContaining` function constructs shell commands from user-provided input:
- Current implementation assumes input is sanitized by caller
- Potential for shell injection if malicious input is provided
- Consider replacing with native filesystem operations using Boost.Filesystem

### Error Message Inconsistency

The `searchCollection` function throws with message "findVersionAliases: is not implemented" - this appears to be a copy-paste error and should be corrected to reference the correct function name.

### Unimplemented Functions

Two functions are intentionally unimplemented:
- `findVersionAliases()` - Alias system not complete for filesystem
- `searchCollection()` - Full-text search not implemented

These throw `runtime_error` to indicate the feature is unavailable. Document this limitation clearly for users.

## See Also

- [dispatch_filedb.h](./dispatch_filedb.h.md) - Header with function declarations
- [StorageProviders/FileSystemDB/provider_filedb.h](../StorageProviders/FileSystemDB/provider_filedb.h.md) - Low-level provider implementation
- [dispatch_mongodb.cpp](./dispatch_mongodb.cpp.md) - MongoDB provider implementation for comparison
- [JSONDocumentBuilder.h](../JsonDocument/JSONDocumentBuilder.h.md) - Document modification utilities
- [options_operations.h](./options_operations.h.md) - Operation options classes

---

**Documentation generated for artdaq-database ConfigurationDB module**
