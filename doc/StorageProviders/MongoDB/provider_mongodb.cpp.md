# provider_mongodb.cpp

**Path:** `artdaq-database/StorageProviders/MongoDB/provider_mongodb.cpp`

**Implements:** [provider_mongodb.h](./provider_mongodb.h.md)

**Purpose:** Implements query operations for the MongoDB storage provider, including configuration search, entity discovery, version lookup, and collection/database enumeration. These operations form the core search and metadata functionality for MongoDB-backed configuration storage.

## Implementation Overview

This file provides template specializations of `StorageProvider<JSONDocument, MongoDB>` for various query operations. Each method:
1. Parses the search query from a `JSONDocument`
2. Converts it to BSON format using the mongocxx driver
3. Executes MongoDB aggregation pipelines or queries
4. Transforms results back to `JSONDocument` format

The implementation heavily uses MongoDB's aggregation framework for complex queries and supports regex-based pattern matching for flexible configuration searches.

## Key Algorithms

### Configuration Search Algorithm (`findConfigurations`)

Searches across all collections to find configurations matching a pattern.

**Steps:**
1. Parse the search query and extract configuration name filter
2. If filter ends with `*`, prepare regex pattern for prefix matching
3. Iterate over all collections in the database (skipping system collections)
4. For each collection, run an aggregation pipeline with match and project stages
5. Extract configuration names and their assignment timestamps
6. Build a map of configuration names to their most recent timestamps
7. Sort results by timestamp in reverse chronological order
8. Return JSON documents describing each matching configuration

**Why this approach:** Configurations may exist across multiple collections, so a cross-collection search is necessary. Sorting by timestamp ensures the most recently modified configurations appear first.

### Regex Query Rewriting

The `rewrite_query_with_regex()` helper converts wildcard patterns to MongoDB regex queries.

**Transformation:**
| Input Pattern | MongoDB Regex |
|---------------|---------------|
| `"config_*"` | `{"$regex": "^config_.*", "$options": ""}` |
| `"*"` | `{"$regex": ".*", "$options": ""}` |
| `"exact"` | `"exact"` (no change) |

## Internal Functions

### `pipeline_from_document(bsoncxx::document::value const&) -> mongocxx::pipeline`

**Brief:** Converts a BSON document into a MongoDB aggregation pipeline.

**Called by:** `searchCollection()`

**Purpose:** Enables dynamic pipeline construction from JSON-based search specifications.

### `rewrite_query_with_regex(JSONDocument const&, std::vector<std::string> const&) -> JSONDocument`

**Brief:** Transforms query fields containing wildcard patterns into MongoDB regex expressions.

**Called by:** `findConfigurations()`, `findVersions()`

**Purpose:** Enables pattern-based searching using `*` wildcards in query fields.

**Note:** Defined in `helper_functions.cpp`, declared in this file.

## Template Specializations

### `StorageProvider<JSONDocument, MongoDB>::findConfigurations`

**Brief:** Searches for configuration names across all collections matching the specified pattern.

**Parameters:**
- `search` - JSON document containing search criteria with `configurations` filter

**Returns:** Vector of JSON documents, each describing a matching configuration with provider, format, and operation metadata

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_error("MongoDB")` | When search document is missing required `configurations` element |
| `runtime_error("MongoDB")` | When MongoDB returns invalid collection descriptors |

**Query Format:**
```json
{
  "configurations": "config_name*"
}
```

**Result Format:**
```json
{
  "provider": "mongo",
  "format": "gui",
  "operation": "confcomposition",
  "searchfilter": {
    "configurations": "config_name"
  }
}
```

### `StorageProvider<JSONDocument, MongoDB>::configurationComposition`

**Brief:** Retrieves all entities belonging to a specific configuration across all collections.

**Parameters:**
- `search` - JSON document containing the configuration name to search for

**Returns:** Vector of JSON documents, each representing an entity within the configuration

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_error("MongoDB")` | When search document is missing required elements |
| `runtime_error("MongoDB")` | When MongoDB returns invalid data |

**Implementation Notes:**
- Uses MongoDB aggregation with `$project` to extract configuration and entity names
- Filters results to match the exact requested configuration name
- Tracks seen entities to avoid duplicates

### `StorageProvider<JSONDocument, MongoDB>::findVersions`

**Brief:** Finds all versions of documents matching the specified entity pattern within a collection.

**Parameters:**
- `query_payload` - JSON document with collection name and search filter

**Returns:** Vector of JSON documents describing each version/entity combination

**Query Format:**
```json
{
  "collection": "collection_name",
  "searchfilter": {
    "entities": "entity_name*"
  }
}
```

### `StorageProvider<JSONDocument, MongoDB>::findEntities`

**Brief:** Searches for entity names across all collections matching the provided criteria.

**Parameters:**
- `search` - JSON document containing search filter with optional entity pattern

**Returns:** Vector of JSON documents, each representing a unique entity

**Implementation Notes:**
- Iterates over all non-system collections
- Uses aggregation pipeline to extract entity names
- Tracks seen entities to prevent duplicates in results

### `StorageProvider<JSONDocument, MongoDB>::addConfiguration`

**Brief:** Placeholder for adding a new configuration (currently returns empty result).

**Parameters:**
- `query_payload` - Configuration data to add

**Returns:** Empty vector (not yet implemented)

**Note:** This operation's logic is handled elsewhere; this method may be deprecated.

### `StorageProvider<JSONDocument, MongoDB>::listCollections`

**Brief:** Enumerates all user collections in the current database.

**Parameters:**
- `query_payload` - Query parameters (contents not used, but must not be empty)

**Returns:** Vector of JSON documents, each describing a collection

**Implementation Notes:**
- Filters out system collections (`system.indexes`, system metadata collection)
- Returns collection names with provider and format metadata

### `StorageProvider<JSONDocument, MongoDB>::listDatabases`

**Brief:** Enumerates all user databases on the MongoDB server.

**Parameters:**
- `query_payload` - Query parameters (contents not used, but must not be empty)

**Returns:** Vector of JSON documents, each describing a database

**Implementation Notes:**
- Filters out internal databases (`local`, `admin`)
- Uses the mongocxx client's `list_databases()` method

### `StorageProvider<JSONDocument, MongoDB>::databaseMetadata`

**Brief:** Retrieves the system metadata document for the current database.

**Parameters:**
- `query_payload` - Query parameters (marked as unused with `[[gnu::unused]]`)

**Returns:** Vector containing the system metadata document(s)

**Implementation Notes:**
- Reads from the special `system.metadata` collection
- Delegates to `readDocument()` for actual retrieval

### `StorageProvider<JSONDocument, MongoDB>::searchCollection`

**Brief:** Executes a custom aggregation pipeline on a specified collection.

**Parameters:**
- `query_payload` - JSON document with collection name and pipeline specification

**Returns:** Vector of JSON documents containing aggregation results

**Query Format:**
```json
{
  "collection": "collection_name",
  "searchfilter": {
    "$match": {"field": "value"},
    "$project": {"field": 1}
  }
}
```

**Implementation Notes:**
- Converts the search filter to a MongoDB aggregation pipeline
- Supports full MongoDB aggregation framework syntax

## Debug Functions

### `artdaq::database::mongo::debug::enable()`

**Brief:** Enables all TRACE debugging for MongoDB operations.

**Implementation:**
- Sets TRACE control parameters for maximum verbosity
- Calls `connection()` and `ReadWrite()` debug enablers
- Logs confirmation message at TRACE level 10

## Performance Considerations

- **Cross-collection searches:** Operations like `findConfigurations()` and `findEntities()` iterate over all collections, which can be slow with many collections. Consider indexing strategies if performance is critical.

- **Aggregation pipelines:** MongoDB aggregation is generally efficient with proper indexes, but complex pipelines on large collections may require optimization.

- **Result sorting:** `findConfigurations()` maintains timestamp-sorted results in memory before returning. For very large result sets, this could consume significant memory.

## Error Handling Strategy

All operations use a consistent error handling pattern:
1. Validate input with `confirm(!search.empty())` assertion
2. Wrap BSON conversions that may fail
3. Throw `runtime_error("MongoDB")` with descriptive messages for recoverable errors
4. Let mongocxx exceptions propagate for driver-level errors

## Testing Notes

- **Unit tests:** See `test/StorageProviders/MongoDB/` for MongoDB-specific tests
- **Key test cases:**
  - Configuration search with exact and wildcard patterns
  - Entity discovery across multiple collections
  - Version enumeration within a collection
  - Empty result handling

## Maintenance Notes

- The `goto exact_match` pattern in `findConfigurations()` is intentional for early exit on exact matches. While goto is generally discouraged, it provides clean exit from nested loops here.

- String literals are accessed through `apiliteral` and `jsonliteral` namespaces for consistency with the rest of the codebase.

## See Also

- [provider_mongodb.h.md](./provider_mongodb.h.md) - Header declarations
- [provider_mongodb_readwrite.cpp.md](./provider_mongodb_readwrite.cpp.md) - Read/write operations
- [helper_functions.cpp.md](./helper_functions.cpp.md) - Regex rewriting implementation
- [mongo_json.h.md](./mongo_json.h.md) - BSON/JSON conversion utilities

---

**Last Updated:** 2026-01-14
