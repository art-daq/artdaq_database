# detail_managedocument.cpp

**Path:** `artdaq-database/ConfigurationDB/detail_managedocument.cpp`

**Implements:** [dboperation_managedocument.h](./dboperation_managedocument.h.md)

**Purpose:** Contains the core implementation logic for document management operations including reading, writing, and modifying configuration documents. This file handles data format conversion, storage provider dispatch, and document metadata management. It is the central implementation file for all document-level operations in the ConfigurationDB module.

## Implementation Overview

This file provides the internal (`detail` namespace) implementations of document operations:
- **write_document**: Stores a document with format conversion
- **read_document**: Retrieves a document with format conversion
- **find_versions**: Searches for document versions
- **find_entities**: Searches for entity names
- **add_entity / remove_entity**: Modifies document entity associations
- **mark_document_readonly / mark_document_deleted**: Changes document state
- **read_documents / write_documents**: Bulk document operations

All functions dispatch to the appropriate storage provider (MongoDB, FileSystemDB, or UconDB) based on the configured provider name.

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/ConfigurationDB/common.h` | Common definitions and TRACE configuration |
| `artdaq-database/ConfigurationDB/dboperation_managedocument.h` | Header being implemented |
| `artdaq-database/ConfigurationDB/shared_helper_functions.h` | Validation and helper functions |
| `artdaq-database/DataFormats/shared_literals.h` | String literal constants |
| `artdaq-database/SharedCommon/configuraion_api_literals.h` | API literal constants |
| `artdaq-database/ConfigurationDB/configuration_dbproviders.h` | Provider dispatch declarations |
| `artdaq-database/BasicTypes/basictypes.h` | JsonData, FhiclData, XmlData types |
| `artdaq-database/DataFormats/Json/json_common.h` | JSON utilities |
| `artdaq-database/DataFormats/Fhicl/fhicl_common.h` | FHiCL utilities (conditional, when USE_FHICLCPP defined) |
| `artdaq-database/DataFormats/Xml/xml_common.h` | XML utilities |
| `artdaq-database/JsonDocument/JSONDocumentBuilder.h` | Document building and manipulation |
| `artdaq-database/ConfigurationDB/options_operations.h` | ManageDocumentOperation class |
| `boost/variant.hpp` | Variant type for data handling |

## Type Definitions

### `JsonAnyHandle_t`

```cpp
using JsonAnyHandle_t = boost::variant<JsonData, JSONDocument>;
```

A variant type that can hold either raw JSON data or a structured JSON document, used during format conversion to flexibly handle different stages of document processing.

### Provider Function Types

```cpp
using provider_write_t = void (*)(const Options&, const JSONDocument&);
using provider_read_t = JSONDocument (*)(const Options&, const JSONDocument&);
using provider_call_t = std::vector<JSONDocument> (*)(const Options&, const JSONDocument&);
```

Function pointer types for storage provider dispatch:
- `provider_write_t`: For write operations that do not return a document
- `provider_read_t`: For read operations that return a single document
- `provider_call_t`: For search operations that return multiple documents

## Internal Functions

### `write_document(Options const&, std::string&) -> void`

**Brief:** Writes a configuration document to the database, performing format conversion as needed and adding metadata (version, collection, entity, configuration).

**Parameters:**
- `options` - `Options const&` - Write configuration including format, collection, version, entity, and configuration names
- `conf` - `std::string&` - Document content (may be modified for format conversion feedback)

**Preconditions:**
- `options.operation()` must be `writedocument` or `overwritedocument`
- `conf` must not be empty
- Provider name must be valid

**Postconditions:**
- Document is stored in the database with metadata
- For certain formats, `conf` may be updated with converted content

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | When document is empty |
| `runtime_error` | When format is unknown |
| `runtime_error` | When format conversion fails (GUI, FHiCL, or XML) |

**Thread Safety:** Conditional - depends on underlying provider implementation

**Implementation Details:**

1. Validates operation type and non-empty document
2. Validates provider name using `validate_dbprovider_name()`
3. Converts input to JSON based on format:
   - **json**: Wraps in `{"data": ...}`
   - **db**: Uses as-is (already in database format)
   - **gui**: Converts from GUI format to database format via `json_gui_to_db()`
   - **fhicl**: Parses FHiCL to JSON AST using `fhicljson::fhicl_to_ast()` (if USE_FHICLCPP enabled)
   - **xml**: Converts XML to JSON and back for validation
4. Creates `JSONDocumentBuilder` based on format
5. Adds metadata (configuration, version, collection, entity, run) if not readonly
6. Constructs insert payload with document and filter
7. Dispatches to appropriate provider's `writeDocument` function

**Format Conversion Table:**

| Input Format | Conversion Process |
|--------------|-------------------|
| json | Wraps in `{"data": <content>}` |
| db | No conversion, uses JSONDocumentBuilder directly |
| gui | Calls `json_gui_to_db()` conversion |
| fhicl | Calls `fhicljson::fhicl_to_ast()` |
| xml | Converts via XmlData -> JsonData -> XmlData for validation |

---

### `read_document(Options const&, std::string&) -> void`

**Brief:** Reads a configuration document from the database and converts it to the requested output format.

**Parameters:**
- `options` - `Options const&` - Read configuration including format and query filter
- `conf` - `std::string&` - Output buffer (must be empty)

**Preconditions:**
- `options.operation()` must be `readdocument`
- `conf` must be empty
- Provider name must be valid

**Postconditions:**
- `conf` contains the document in the requested format

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | When conf is not empty |
| `runtime_error` | When format is unknown |
| `runtime_error` | When format conversion fails |

**Thread Safety:** Conditional - depends on underlying provider implementation

**Implementation Details:**

1. Validates operation type and empty output buffer
2. Constructs search payload from query filter
3. Dispatches to provider's `readDocument` function
4. If format is `origin`, detects format from document metadata
5. Converts result to requested format:
   - **db**: Returns raw document string
   - **gui**: Calls `json_db_to_gui()` conversion
   - **json**: Extracts `document.data` object
   - **fhicl**: Converts to FHiCL using FhiclWriter
   - **xml**: Converts to XML using XmlWriter

---

### `find_versions(Options const&, std::string&) -> void`

**Brief:** Searches for all versions of documents matching the query filter and returns them in the requested format.

**Parameters:**
- `options` - `Options const&` - Search configuration
- `versions` - `std::string&` - Output buffer for version list (must be empty)

**Preconditions:**
- `options.operation()` must be `findversions`
- `versions` must be empty

**Postconditions:**
- `versions` contains list of versions in requested format (gui or csv)

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | When format is unsupported (db, json, fhicl, unknown) |

**Thread Safety:** Conditional - depends on underlying provider implementation

**Output Formats:**
- **gui**: `{"search": [<version objects>]}`
- **csv**: Comma-separated version names

---

### `find_entities(Options const&, std::string&) -> void`

**Brief:** Searches for all entity names matching the query filter and returns them in the requested format.

**Parameters:**
- `options` - `Options const&` - Search configuration
- `entities` - `std::string&` - Output buffer for entity list (must be empty)

**Preconditions:**
- `options.operation()` must be `findentities`
- `entities` must be empty

**Postconditions:**
- `entities` contains list of entity names in requested format

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `runtime_error` | When format is unsupported (db, json, fhicl, unknown) |

**Thread Safety:** Conditional - depends on underlying provider implementation

**Output Formats:**
- **gui**: `{"search": [<entity objects>]}`
- **csv**: Comma-separated entity names

---

### `add_entity(Options const&, std::string&) -> void`

**Brief:** Adds an entity association to an existing document using a read-modify-write pattern.

**Parameters:**
- `options` - `Options const&` - Operation configuration with entity name
- `conf` - `std::string&` - Output buffer for updated document (must be empty)

**Preconditions:**
- `options.operation()` must be `addentity`
- `conf` must be empty

**Postconditions:**
- Document has the new entity association
- `conf` contains the updated document in the requested format

**Thread Safety:** Not atomic - uses read-modify-write pattern

**Implementation Details:**
1. Reads the document in database format
2. Uses `JSONDocumentBuilder.addEntity()` to add the entity
3. Writes the updated document back
4. Reads and returns the document in the requested format

---

### `remove_entity(Options const&, std::string&) -> void`

**Brief:** Removes an entity association from an existing document using a read-modify-write pattern.

**Parameters:**
- `options` - `Options const&` - Operation configuration with entity name
- `conf` - `std::string&` - Output buffer for updated document (must be empty)

**Preconditions:**
- `options.operation()` must be `rmentity`
- `conf` must be empty

**Postconditions:**
- Entity association is removed from the document
- `conf` contains the updated document in the requested format

**Thread Safety:** Not atomic - uses read-modify-write pattern

**Implementation Details:**
1. Reads the document in database format
2. Uses `JSONDocumentBuilder.removeEntity()` to remove the entity
3. Writes the updated document back
4. Reads and returns the document in the requested format

---

### `mark_document_readonly(Options const&, std::string&) -> void`

**Brief:** Marks a document as read-only, preventing future modifications.

**Parameters:**
- `options` - `Options const&` - Operation configuration
- `conf` - `std::string&` - Output buffer for updated document (must be empty)

**Preconditions:**
- `options.operation()` must be `markreadonly`
- `conf` must be empty

**Postconditions:**
- Document is marked as readonly
- `conf` contains the updated document in the requested format

**Thread Safety:** Not atomic - uses read-modify-write pattern

**Implementation Details:**
1. Reads the document in database format
2. Uses `JSONDocumentBuilder.markReadonly()` to set the flag
3. Uses `overwritedocument` operation to save (bypasses readonly check)
4. Returns the updated document

---

### `mark_document_deleted(Options const&, std::string&) -> void`

**Brief:** Marks a document as deleted (soft delete), preventing it from appearing in normal queries.

**Parameters:**
- `options` - `Options const&` - Operation configuration
- `conf` - `std::string&` - Output buffer for updated document (must be empty)

**Preconditions:**
- `options.operation()` must be `markdeleted`
- `conf` must be empty

**Postconditions:**
- Document is marked as deleted
- `conf` contains the updated document in the requested format

**Thread Safety:** Not atomic - uses read-modify-write pattern

**Implementation Details:**
1. Reads the document in database format
2. Uses `JSONDocumentBuilder.markDeleted()` to set the flag
3. Uses `overwritedocument` operation to save
4. Returns the updated document

---

### `read_documents(ManageDocumentOperation const&, std::vector<JSONDocument>&) -> void`

**Brief:** Reads multiple documents from a collection matching the query filter for bulk operations.

**Parameters:**
- `options` - Operation configuration with collection and filter
- `document_list` - Output vector for documents (must be empty)

**Preconditions:**
- `options.operation()` must be `readdocument`
- `document_list` must be empty

**Postconditions:**
- `document_list` contains all matching documents

**Thread Safety:** Conditional - depends on underlying provider implementation

**Implementation Details:**
1. Validates provider and empty document list
2. Constructs search payload with filter and collection
3. Dispatches to provider's `readDocuments` function
4. Returns all matching documents

---

### `write_documents(ManageDocumentOperation const&, std::vector<JSONDocument> const&) -> void`

**Brief:** Writes multiple documents to a collection for bulk operations.

**Parameters:**
- `options` - Operation configuration with collection
- `document_list` - Documents to write (must not be empty)

**Preconditions:**
- `options.operation()` must be `writedocument`
- `document_list` must not be empty

**Postconditions:**
- All documents are written to the database

**Thread Safety:** Conditional - depends on underlying provider implementation

**Implementation Details:**
1. Validates non-empty document list
2. For each document:
   - Creates `JSONDocumentBuilder` to access metadata
   - Generates new Object ID if document is readonly/deleted
   - Constructs insert payload with document, filter, and collection
   - Dispatches to provider's `writeDocument` function

---

### `debug::detail::ManageDocuments() -> void`

**Brief:** Enables TRACE logging for the detail implementation of document management operations.

**Purpose:** Configure TRACE logging infrastructure for debugging document operations.

**Postconditions:**
- TRACE name set to "detail_managedocument.cpp"
- All trace levels enabled
- Trace modes configured per `trace_mode::modeM` and `trace_mode::modeS`

**Thread Safety:** Not thread-safe - should be called during initialization only

---

## Key Algorithms

### Storage Provider Dispatch

All operations use a dispatch pattern to select the appropriate storage provider:

```cpp
auto dispatch_persistence_provider = [](std::string const& name) {
  auto providers = std::map<std::string, provider_type>{
    {apiliteral::provider::mongo, cf::mongo::function},
    {apiliteral::provider::filesystem, cf::filesystem::function},
    {apiliteral::provider::ucon, cf::ucon::function}
  };
  return providers.at(name);
};

auto result = dispatch_persistence_provider(options.provider())(options, payload);
```

**Why this approach:**
- Allows runtime selection of storage backend
- Provides uniform interface across providers
- Easy to add new providers by extending the map

### Document Metadata Management

When writing a document (non-readonly), the following metadata is added:

```cpp
builder.addConfiguration({options.configuration_to_JsonData()});
builder.setVersion({options.version_to_JsonData()});
builder.setCollection({options.collection_to_JsonData()});
builder.addEntity({options.entity_to_JsonData()});
if (options.run() != jsonliteral::notprovided) {
  builder.addRun({options.run_to_JsonData()});
}
```

### Readonly Document Handling

When a readonly document is written with `writedocument` operation, a new Object ID is generated:

```cpp
if (builder.isReadonlyOrDeleted() &&
    options.operation() == apiliteral::operation::writedocument) {
  builder.newObjectID();
}
```

This creates a new document version rather than modifying the protected original.

---

## Performance Considerations

- **Format conversion**: Converting between formats (especially FHiCL) adds CPU overhead
- **Provider dispatch**: Map lookup is O(1) but creates a new map on each call
- **Bulk operations**: `read_documents`/`write_documents` are more efficient than individual calls for multiple documents
- **Memory usage**: Large documents are held in memory during conversion
- **Read-modify-write**: Entity and status operations require three database operations

## Error Handling Strategy

1. **Precondition assertions**: Uses `confirm()` macro for internal consistency checks
2. **Validation**: Validates provider name before dispatch
3. **Exception propagation**: Exceptions from providers and converters propagate to caller
4. **Detailed errors**: Uses `runtime_error` with descriptive messages

## Testing Notes

- **Unit tests:** `test/ConfigurationDB/ManageDocument_t.cc`
- **Key test cases:**
  - Format conversion round-trips
  - Entity add/remove operations
  - Readonly/deleted document handling
  - Provider dispatch for each backend

## Maintenance Notes

- The `USE_FHICLCPP` conditional compilation controls FHiCL support
- The `EXTRA_TRACES` macro enables additional tracing for debugging
- Format conversion functions are in separate modules (fhicljson, xml, etc.)
- The `confirm()` macro is used for internal assertions (not for user input validation)

## Common Pitfalls

- **Non-empty output buffer**: Most functions require the output buffer to be empty
- **Concurrent modifications**: No locking is performed; concurrent modifications may conflict
- **Format support**: Not all formats are supported for all operations

## See Also

- [dboperation_managedocument.h](./dboperation_managedocument.h.md) - Header file
- [configuration_dbproviders.h](./configuration_dbproviders.h.md) - Provider declarations
- [JSONDocumentBuilder.h](../JsonDocument/JSONDocumentBuilder.h.md) - Document manipulation
- [dispatch_mongodb.h](./dispatch_mongodb.h.md) - MongoDB provider
- [dispatch_filedb.h](./dispatch_filedb.h.md) - FileSystemDB provider

---

**Documentation generated for artdaq-database ConfigurationDB module**
