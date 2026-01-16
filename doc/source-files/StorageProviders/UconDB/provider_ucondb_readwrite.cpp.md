# provider_ucondb_readwrite.cpp

**Path:** `artdaq-database/StorageProviders/UconDB/provider_ucondb_readwrite.cpp`

**Implements:** [provider_ucondb.h](./provider_ucondb.h.md)

**Purpose:** Implements the core read and write document operations for the UconDB storage provider using REST API calls via libcurl. These are the primary operations supported by UconDB, enabling configuration storage and retrieval.

## Implementation Overview

This file provides template specializations of `StorageProvider<JSONDocument, UconDB>` for the two fundamental database operations:
- `readDocument()` - Retrieve documents from UconDB by object ID
- `writeDocument()` - Store documents to UconDB with automatic folder creation

Both operations interact with the UconDB REST API through the `ucondb_api.h` functions (`folders()`, `get_object()`, `create_folder()`, `put_object()`).

## Key Algorithms

### Read Document Algorithm

**Steps:**
1. Parse the filter document from the query argument using `findChildDocument()`
2. Extract the collection name from filter or directly from query
3. Verify the collection (folder) exists by calling `folders()` API and checking membership
4. Extract object IDs from the filter:
   - Single OID: `{"id": {"$oid": "..."}}`
   - Multiple OIDs: `{"id": {"$in": [{"$oid": "..."}, ...]}}`
5. For each object ID, call `get_object()` REST API
6. Collect results and return as vector of JSONDocument

**Why this approach:** UconDB uses folder/object organization rather than document-based queries. Objects are retrieved by their unique identifier within a folder, requiring explicit folder verification.

### Write Document Algorithm

**Steps:**
1. Extract the user document using `findChildDocument(jsonliteral::document)`
2. Extract the optional filter document
3. Determine the collection name (priority: user document > filter > arg)
4. Create the folder if it doesn't exist via `create_folder()` API
5. Generate a new object ID using `generate_oid()`
6. Build the document using `JSONDocumentBuilder`:
   - Set object ID
   - Set collection metadata
   - Mark as readonly
7. Extract tags for version labeling via `builder.extractTags()`
8. Call `put_object()` REST API to store the document
9. Return the assigned object ID

**Why this approach:** UconDB requires explicit folder creation and uses object IDs for retrieval. Tags provide versioning capability through the UconDB web interface.

## Template Specializations

### `readDocument(JSONDocument const& arg) -> std::vector<JSONDocument>`

**Brief:** Reads one or more documents from UconDB by their object identifiers within a specified folder (collection).

**Parameters:**
- `arg` - JSON document containing query filter with collection name and object ID(s)

**Preconditions:**
- `arg` must contain a `collection` field (directly or in filter)
- The collection (folder) must exist in UconDB
- Object IDs must be valid

**Returns:** Vector of JSONDocument objects retrieved from UconDB

**Postconditions:**
- Each returned document matches one of the requested object IDs

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_error("UconDB")` | Collection (folder) not found in UconDB |
| `runtime_error("UconDB")` | Object retrieval failed (API error) |

**Thread Safety:** Safe (uses thread-safe HTTP client)

**Side Effects:**
- Network calls to UconDB REST API

**TLOG Level:** 13

**Example Filter Structure (Single ID):**
```json
{
  "filter": {
    "collection": "Detectors",
    "id": {
      "$oid": "507f1f77bcf86cd799439011"
    }
  }
}
```

**Example Filter Structure (Multiple IDs):**
```json
{
  "filter": {
    "collection": "Detectors",
    "id": {
      "$in": [
        {"$oid": "507f1f77bcf86cd799439011"},
        {"$oid": "507f1f77bcf86cd799439012"}
      ]
    }
  }
}
```

---

### `writeDocument(JSONDocument const& arg) -> object_id_t`

**Brief:** Writes a document to UconDB, creating the folder if necessary and assigning a unique object ID.

**Parameters:**
- `arg` - JSON document containing the document to store and optional filter

**Preconditions:**
- `arg` must contain a `document` field with the data to store
- Collection name must be determinable from document, filter, or arg

**Returns:** The object ID assigned to the stored document

**Postconditions:**
- Document is stored in UconDB
- Folder exists (created if necessary)
- Document has unique object ID assigned
- Document is marked as readonly

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_error("UconDB")` | Folder creation failed |
| `runtime_error("UconDB")` | Document insertion failed |

**Thread Safety:** Safe (uses thread-safe HTTP client)

**Side Effects:**
- Network calls to UconDB REST API
- May create new folder in UconDB
- Stores new object in UconDB

**TLOG Level:** 14

**Example Input Structure:**
```json
{
  "document": {
    "collection": "Detectors",
    "data": {
      "voltage": 500,
      "gain": 1.5
    }
  }
}
```

## Internal Functions

### `debug::ReadWrite() -> void`

**Brief:** Enables TRACE debugging for read/write operations.

**Called by:** `debug::enable()` in provider_ucondb.cpp

**Purpose:** Configures TRACE logging at maximum verbosity for read/write operation troubleshooting

**Side Effects:**
- Sets TRACE name to "provider_ucondb_readwrite.cpp"
- Enables all trace levels (0xFFFFFFFFFFFFFFFFLL)

**Implementation:**
```cpp
void ReadWrite() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TLOG(10) << "artdaq::database::ucon::ReadWrite trace_enable";
}
```

## UconDB API Functions Used

| Function | Purpose |
|----------|---------|
| `folders(_provider)` | List available folders to verify collection exists |
| `get_object(_provider, collection, oid)` | Retrieve an object by folder and object ID |
| `create_folder(_provider, collection)` | Create a new folder (collection) if needed |
| `put_object(_provider, collection, json, oid, 0, tags, key)` | Store an object in a folder |
| `generate_oid()` | Generate a unique object ID |
| `to_id(oid)` | Convert object ID to JSON ID format |
| `to_json(jsonliteral::collection, name)` | Create collection metadata JSON |

## JSON Parsing Details

The implementation uses Boost.Spirit-based JSON parsing via:
- `JsonReader` for parsing ID JSON strings
- `unwrap()` to access typed values from the AST
- `value_as<>()` template for type-safe access

### ID Extraction Patterns

The code handles two ID filter patterns:

**Single ID:**
```cpp
oids.push_back(unwrap(id).value_as<std::string>(jsonliteral::oid));
```

**Array of IDs ($in operator):**
```cpp
auto& oidvals = unwrap(id).value_as<array_t>(jsonliteral::in);
for (auto& oidval : oidvals) {
  oids.push_back(unwrap(oidval).value_as<std::string>(jsonliteral::oid));
}
```

Both patterns are wrapped in try-catch blocks to handle missing or malformed ID specifications gracefully.

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/StorageProviders/UconDB/provider_ucondb_headers.h` | All UconDB headers |
| `artdaq-database/BasicTypes/data_json.h` | JSON data type |
| `artdaq-database/DataFormats/Json/json_common.h` | JSON parsing utilities |

## Type Aliases Used

| Type | Purpose |
|------|---------|
| `array_t` | JSON array type from json namespace |
| `object_t` | JSON object type from json namespace |
| `value_t` | JSON value variant type |
| `JsonReader` | JSON parser |

## Performance Considerations

- Each object read requires a separate HTTP request (no batch retrieval supported)
- Folder list is fetched for every read operation to verify existence (could be cached)
- Write operations may make up to 2 HTTP requests (create_folder + put_object)
- Network latency dominates performance; consider batching at higher levels
- String case conversion (`to_lower()`) is performed for folder name comparison

## Error Handling Strategy

1. **Validation:** Uses `confirm()` macro for precondition checks
2. **Folder Verification:** Checks folder existence using case-insensitive comparison before read operations
3. **API Errors:** Checks `result.first` boolean from API calls and throws on failure
4. **Exception Propagation:** Throws `runtime_error("UconDB")` with descriptive messages including collection names and error details
5. **Graceful Degradation:** ID extraction uses try-catch to handle both single and array ID formats

## Testing Notes

- **Unit tests:** Requires UconDB server or mock HTTP server
- **Key test cases:**
  - Read single document by ID
  - Read multiple documents by ID array
  - Write new document with folder creation
  - Write to existing folder
  - Handle missing folder on read (expect exception)
  - Handle API errors (expect exception)
  - Handle malformed ID specifications (graceful handling)

## Maintenance Notes

### Object ID Generation

Currently uses `generate_oid()` for all new documents. The code includes commented-out logic for extracting existing OIDs from filters (for potential update operations in the future):

```cpp
#if 0
try {
  auto oid_json = filter_document.findChild(jsonliteral::id).value();
  oid = extract_oid(oid_json);
} catch (...) {
}
#endif
```

### Tag Extraction

Tags are extracted from the document via `builder.extractTags()` and passed to `put_object()` for version labeling. This enables tag-based retrieval through the UconDB web interface.

### Collection Name Resolution

The collection name is determined with the following priority:
1. From user document's `collection` field
2. From filter document's `collection` field
3. From arg's direct `collection` field

This provides flexibility in how callers structure their requests.

## See Also

- [provider_ucondb.h.md](./provider_ucondb.h.md) - Header file with class declarations
- [provider_ucondb.cpp.md](./provider_ucondb.cpp.md) - Query operations (unimplemented)
- [ucondb_api.cpp.md](./ucondb_api.cpp.md) - REST API implementation using libcurl
- [JSONDocumentBuilder.cpp.md](../../JsonDocument/JSONDocumentBuilder.cpp.md) - Document construction
- [provider_ucondb_headers.h.md](./provider_ucondb_headers.h.md) - Aggregator header
