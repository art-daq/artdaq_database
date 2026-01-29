# JSONDocumentBuilder.h

**Path:** `artdaq-database/JsonDocument/JSONDocumentBuilder.h`

**Purpose:** This header file defines the `JSONDocumentBuilder` class, which provides a high-level builder pattern API for creating and modifying JSON documents with proper metadata, versioning, aliases, and bookkeeping. It wraps the lower-level `JSONDocument` API and integrates with the overlay system to ensure documents have correct structure for database storage.


## Key Concepts

### Builder Pattern
The `JSONDocumentBuilder` implements the classic builder pattern:
- **Fluent interface** - Methods return `JSONDocumentBuilder&` allowing method chaining
- **Step-by-step construction** - Add metadata, versions, aliases incrementally
- **Final extraction** - Use `extract()` to get the completed document
- **Non-copyable** - Ensures clear ownership of the document being built

### Database Document Structure
The builder creates documents with proper database structure including:
- **Version** - Document version identifier
- **Object ID** - Unique database identifier (MongoDB ObjectId format)
- **Collection** - Database collection assignment
- **Entities** - Configurable components/subsystems
- **Configurations** - Configuration set associations
- **Aliases** - Alternate names/identifiers for version lookup
- **Bookkeeping** - Readonly/deleted flags, update tracking

### Overlay Integration
The builder uses overlay classes (`ovlDatabaseRecord`) to provide:
- Type-safe field access to document metadata
- Validation of document structure
- Abstraction over raw JSON details
- Consistent bookkeeping management

### Undo Mechanism
The builder includes `SaveUndo()` and `CallUndo()` methods that form a placeholder for transaction-like rollback. Current implementation always returns `Success()` but the pattern allows for future state preservation.

## Thread Safety

- **Thread-safe:** No
- **Concurrent access:** Not safe for concurrent read/write operations
- **Locking:** No internal locks; external synchronization required for multi-threaded use

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/JsonDocument/JSONDocument.h` | Core document manipulation class |
| `artdaq-database/JsonDocument/common.h` | Module-wide types and utilities |
| `artdaq-database/Overlay/JSONDocumentOverlay.h` | Overlay classes for structured access to document fields |

## Classes/Structures

### `JSONDocumentBuilder`

A final, non-copyable class that provides a builder API for creating database-compliant JSON documents with proper metadata structure.

**Thread Safety:** Not thread-safe. Maintains mutable internal state including `_document`, `_overlay`, and `_initOK`.

#### Constructors

##### `JSONDocumentBuilder()`

**Brief:** Default constructor that creates a new builder with an empty document template and initializes the overlay.

**Parameters:** None

**Preconditions:** None

**Postconditions:**
- Document contains empty template from `template__empty_document`
- Overlay is initialized for structured access
- `_initOK` is set to true

**Throws:** None

**Thread Safety:** safe (construction)

**Example:**
```cpp
#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"

using namespace artdaq::database::docrecord;

void example() {
  JSONDocumentBuilder builder;
  builder.createFromData(userData)
         .setVersion(version);
}
```

##### `JSONDocumentBuilder(JSONDocument doc)`

**Brief:** Constructor that initializes builder with an existing document for modification by moving the document and creating an overlay.

**Parameters:**
- `doc` - Existing JSONDocument to build upon (moved)

**Preconditions:**
- Document should contain valid JSON structure

**Postconditions:**
- Builder wraps the provided document (moved)
- Overlay is initialized for the document structure
- `_initOK` is set to true

**Throws:** None

**Thread Safety:** safe (construction)

**Example:**
```cpp
JSONDocument existing = JSONDocument::loadFromFile("config.json");
JSONDocumentBuilder builder(existing);
builder.addAlias(newAlias);
```

#### Data Import Methods

##### `createFromData(JSONDocument userDoc) -> JSONDocumentBuilder&`

**Brief:** Creates a new database document from user data, wrapping it in proper database structure with metadata scaffolding.

**Parameters:**
- `userDoc` - User-provided document containing configuration data (moved internally)

**Preconditions:**
- None

**Returns:** Reference to this builder for method chaining

**Postconditions:**
- Document contains user data wrapped in database structure
- Overlay is refreshed to reflect new structure
- Metadata fields (metadata, changelog, origin, collection, attachments, data) are imported if present in source

**Throws:** None directly, but overlay operations may throw

**Thread Safety:** unsafe

**Example:**
```cpp
JSONDocument userData(R"({"detector": "ICARUS", "channels": 1024})");
JSONDocumentBuilder builder;
builder.createFromData(userData);
```

#### Alias Methods

##### `addAlias(JSONDocument const& alias) -> JSONDocumentBuilder&`

**Brief:** Adds an alternate name/identifier for the document to the active aliases list.

**Parameters:**
- `alias` - Document containing alias in format `{"alias": "alias_name"}`

**Preconditions:**
- Alias name should be unique within the document

**Returns:** Reference to this builder for method chaining

**Postconditions:**
- Alias is added to document's active aliases via overlay
- On failure: undo is called, builder remains usable

**Throws:** None (exceptions are caught internally and logged)

**Thread Safety:** unsafe

**Example:**
```cpp
builder.addAlias(JSONDocument(R"({"alias":"latest_stable"})"))
       .addAlias(JSONDocument(R"({"alias":"production"})"));
```

##### `removeAlias(JSONDocument const& alias) -> JSONDocumentBuilder&`

**Brief:** Removes an alias from the document's active aliases list.

**Parameters:**
- `alias` - Document containing alias to remove in format `{"alias": "alias_name"}`

**Returns:** Reference to this builder for method chaining

**Postconditions:**
- Alias is removed from document's active aliases
- On failure: undo is called, builder remains usable

**Throws:** None (exceptions are caught internally)

**Thread Safety:** unsafe

#### Configuration Methods

##### `addConfiguration(JSONDocument const& config) -> JSONDocumentBuilder&`

**Brief:** Adds a configuration set association to the document.

**Parameters:**
- `config` - Document containing configuration in format `{"configuration": "config_name"}`

**Returns:** Reference to this builder for method chaining

**Postconditions:**
- Configuration is added to document's configurations list via overlay

**Throws:** None (exceptions are caught internally)

**Thread Safety:** unsafe

**Example:**
```cpp
builder.addConfiguration(JSONDocument(R"({"configuration":"nominal"})"))
       .addConfiguration(JSONDocument(R"({"configuration":"high_rate"})"));
```

##### `removeConfiguration(JSONDocument const& config) -> JSONDocumentBuilder&`

**Brief:** Removes a configuration association from the document.

**Parameters:**
- `config` - Document containing configuration to remove

**Returns:** Reference to this builder for method chaining

**Thread Safety:** unsafe

##### `removeAllConfigurations() -> JSONDocumentBuilder&`

**Brief:** Removes all configuration associations from the document by calling wipe() on the overlay's configurations list.

**Parameters:** None

**Returns:** Reference to this builder for method chaining

**Postconditions:**
- Document's configurations list is empty

**Thread Safety:** unsafe

#### Entity Methods

##### `addEntity(JSONDocument const& entity) -> JSONDocumentBuilder&`

**Brief:** Adds a configurable component/subsystem entity to the document.

**Parameters:**
- `entity` - Document containing entity in format `{"entity": "entity_name"}`

**Returns:** Reference to this builder for method chaining

**Postconditions:**
- Entity is added to document's entities list via overlay

**Throws:** None (exceptions are caught internally)

**Thread Safety:** unsafe

**Example:**
```cpp
builder.addEntity(JSONDocument(R"({"entity":"TPC_West"})"))
       .addEntity(JSONDocument(R"({"entity":"TPC_East"})"));
```

##### `removeEntity(JSONDocument const& entity) -> JSONDocumentBuilder&`

**Brief:** Removes an entity association from the document.

**Parameters:**
- `entity` - Document containing entity to remove

**Returns:** Reference to this builder for method chaining

**Thread Safety:** unsafe

##### `removeAllEntities() -> JSONDocumentBuilder&`

**Brief:** Removes all entity associations from the document by calling wipe() on the overlay's entities list.

**Parameters:** None

**Returns:** Reference to this builder for method chaining

**Postconditions:**
- Document's entities list is empty

**Thread Safety:** unsafe

#### Run Methods

##### `addRun(JSONDocument const& run) -> JSONDocumentBuilder&`

**Brief:** Associates a run number/identifier with the document.

**Parameters:**
- `run` - Document containing run information

**Returns:** Reference to this builder for method chaining

**Thread Safety:** unsafe

#### Metadata Methods

##### `setVersion(JSONDocument const& version) -> JSONDocumentBuilder&`

**Brief:** Sets the version identifier for the document by extracting the "name" field from the provided document.

**Parameters:**
- `version` - Document containing version in format `{"name": "version_string"}`

**Returns:** Reference to this builder for method chaining

**Postconditions:**
- Document's version field is updated via overlay

**Throws:** None (exceptions are caught internally)

**Thread Safety:** unsafe

**Example:**
```cpp
builder.setVersion(JSONDocument(R"({"name":"v1.0.0"})"));
```

##### `setCollection(JSONDocument const& collection) -> JSONDocumentBuilder&`

**Brief:** Sets the database collection for the document.

**Parameters:**
- `collection` - Document containing collection in format `{"collection": "collection_name"}`

**Returns:** Reference to this builder for method chaining

**Thread Safety:** unsafe

**Example:**
```cpp
builder.setCollection(JSONDocument(R"({"collection":"detector_configs"})"));
```

##### `setObjectID(JSONDocument const& objectId) -> JSONDocumentBuilder&`

**Brief:** Sets the unique database identifier for the document by extracting "id" field and using swap via overlay.

**Parameters:**
- `objectId` - Document containing object ID

**Returns:** Reference to this builder for method chaining

**Thread Safety:** unsafe

##### `getObjectID() const -> JSONDocument`

**Brief:** Returns the document's object ID as a JSONDocument by extracting from the overlay.

**Parameters:** None

**Returns:** JSONDocument containing the object ID

**Thread Safety:** safe (const method)

##### `getObjectOUID() const -> std::string`

**Brief:** Returns the document's object ID as a string (OID format) by extracting from the overlay.

**Parameters:** None

**Returns:** String representation of the object ID

**Thread Safety:** safe (const method)

##### `newObjectID() -> bool`

**Brief:** Generates a new unique object ID for the document by calling the overlay's newId() method.

**Parameters:** None

**Returns:** `true` if ID was successfully generated

**Postconditions:**
- Document has a new unique object ID

**Thread Safety:** unsafe

**Example:**
```cpp
builder.newObjectID();
std::string oid = builder.getObjectOUID();
```

#### Bookkeeping Methods

##### `markReadonly() -> JSONDocumentBuilder&`

**Brief:** Marks the document as read-only, preventing future modifications by setting bookkeeping flag via overlay.

**Parameters:** None

**Returns:** Reference to this builder for method chaining

**Postconditions:**
- Document's bookkeeping.isreadonly flag is set to true

**Thread Safety:** unsafe

**Example:**
```cpp
// Lock production configuration
builder.markReadonly();
```

##### `markDeleted() -> JSONDocumentBuilder&`

**Brief:** Marks the document as soft-deleted without physical removal by setting bookkeeping flag via overlay.

**Parameters:** None

**Returns:** Reference to this builder for method chaining

**Postconditions:**
- Document's bookkeeping.isdeleted flag is set to true

**Thread Safety:** unsafe

##### `isReadonlyOrDeleted() const -> bool`

**Brief:** Checks if the document is marked as read-only or deleted by querying the overlay's bookkeeping flags.

**Parameters:** None

**Returns:** `true` if document is readonly or deleted, `false` otherwise

**Thread Safety:** safe (const method)

#### Extraction and Comparison Methods

##### `extractTags() const -> std::list<std::string>`

**Brief:** Extracts searchable tags from the document by collecting version, configuration, and entity information from the overlay.

**Parameters:** None

**Returns:** List of tags in format `"type:value"` (e.g., `"version:v1.0.0"`, `"configuration:nominal"`, `"entity:TPC_West"`)

**Thread Safety:** safe (const method)

**Example:**
```cpp
auto tags = builder.extractTags();
// tags = ["version:v1.0.0", "configuration:nominal", "entity:TPC_West"]
```

##### `extract() -> JSONDocument`

**Brief:** Extracts the final document from the builder using move semantics.

**Parameters:** None

**Returns:** The completed JSONDocument (moved)

**Postconditions:**
- Builder's internal document is moved out
- Builder should not be used after this call

**Thread Safety:** unsafe

**Example:**
```cpp
auto finalDoc = builder.extract();
finalDoc.saveToFile("config.json");
// builder should not be used after this
```

##### `comapreUsingOverlays(JSONDocumentBuilder const& other) const -> result_t`

**Brief:** Compares this builder's document with another using overlay comparison by delegating to the overlay's equality operator.

**Parameters:**
- `other` - Builder to compare with

**Returns:** result_t indicating success/failure of comparison

**Thread Safety:** safe (const method)

**Note:** Method name has typo (should be "compareUsingOverlays")

##### `operator==(JSONDocumentBuilder const& other) const -> result_t`

**Brief:** Equality comparison operator using overlay comparison, delegates to comapreUsingOverlays().

**Parameters:**
- `other` - Builder to compare with

**Returns:** result_t from overlay comparison

**Thread Safety:** safe (const method)

##### `to_string() const -> std::string`

**Brief:** Converts the document to its JSON string representation by calling the document's to_string() method.

**Parameters:** None

**Returns:** JSON string of the document

**Thread Safety:** conditional (modifies document's _isDirty flag)

## Template Methods

### `overlay<OVL>(JSONDocument& document, object_t::key_type const& self_key) -> std::unique_ptr<OVL>`

**Brief:** Template method that creates an overlay of specific type for a document field.

**Parameters:**
- `document` - Document to create overlay for (non-const reference)
- `self_key` - Key name for the overlay

**Preconditions:**
- `self_key` must not be empty
- Document's value must be an object type

**Returns:** Unique pointer to the created overlay of type OVL

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `std::runtime_error` | When self_key is empty |
| `std::runtime_error` | When document value is not an object type |

**Thread Safety:** unsafe

## Functions

### `toJSONDocument<T>(T const& value) -> JSONDocument`

**Brief:** Template function that converts various types to JSONDocument.

**Parameters:**
- `value` - Value to convert

**Returns:** JSONDocument containing the converted value

**Thread Safety:** safe

**Note:** Specialization for `string_pair_t` is provided in JSONDocument_utils.cpp.

### `debug::JSONDocumentBuilder()`

**Brief:** Enables detailed TRACE logging for builder operations at maximum verbosity.

**Parameters:** None

**Returns:** None

**Side Effects:**
- Configures TRACE logging for JSONDocumentBuilder debugging

**Thread Safety:** safe

### `operator<<(std::ostream& os, JSONDocumentBuilder const& builder) -> std::ostream&`

**Brief:** Stream output operator for JSONDocumentBuilder that writes JSON string to output stream.

**Parameters:**
- `os` - Output stream
- `builder` - Builder to output

**Returns:** Reference to the output stream

**Thread Safety:** conditional

## Copy/Move Semantics

The class is non-copyable and non-movable by design:
```cpp
JSONDocumentBuilder(JSONDocumentBuilder const&) = delete;
JSONDocumentBuilder& operator=(JSONDocumentBuilder const&) = delete;
JSONDocumentBuilder& operator=(JSONDocumentBuilder&&) = delete;
JSONDocumentBuilder(JSONDocumentBuilder&&) = delete;
```

**Rationale:**
- Builder maintains state through overlay pointer
- Copying/moving would complicate overlay management and ownership
- Use `extract()` to transfer ownership of the completed document
- Intended for single-use, stack-allocated builders

## Relationship to Other Components

### Within the JsonDocument Module
- **JSONDocument** - The builder wraps and manipulates `JSONDocument` internally
- **JSONDocumentMigrator** - Uses builder for constructing migrated documents
- **docrecord_literals.h** - Provides template strings and field name constants

### Dependencies
- **Overlay Module** - Provides `ovlDatabaseRecord` and specialized overlays (`ovlAlias`, `ovlConfiguration`, `ovlEntity`, `ovlVersion`, `ovlCollection`, `ovlId`) for structured field access
- **DataFormats/Json** - Provides JSON types used by overlays

### Used By
- **StorageProviders** - Use builder to create properly structured documents for storage
- **ConfigurationDB** - Uses builder for document creation workflows
- **Migration Utilities** - Uses builder to construct modern format documents

## See Also

- [JSONDocumentBuilder.cpp.md](./JSONDocumentBuilder.cpp.md) - Implementation details
- [JSONDocument.h.md](./JSONDocument.h.md) - Core document class
- [JSONDocumentMigrator.h.md](./JSONDocumentMigrator.h.md) - Document migration
- [JSONDocument_utils.cpp.md](./JSONDocument_utils.cpp.md) - Contains `createFromData` and `_importUserData` implementations
- [Overlay Module](../Overlay/README.md) - Overlay classes used by builder
- [docrecord_literals.h.md](./docrecord_literals.h.md) - Template constants

## Notes for Developers

### Common Pitfalls

- **Using builder after extract():** The builder's document is moved out; do not reuse the builder.
- **Forgetting to set version:** Many database operations require a version.
- **Not generating new ID:** Call `newObjectID()` for new documents before storing.
- **Modifying readonly documents:** Check `isReadonlyOrDeleted()` first.

### Anti-patterns

```cpp
// DON'T do this - builder is invalid after extract():
auto doc = builder.extract();
builder.addAlias(alias);  // Builder is in invalid state!

// DO this instead - extract() last:
builder.addAlias(alias);
auto doc = builder.extract();

// DON'T try to copy a builder:
auto builder2 = builder;  // Compilation error - deleted

// DO create separate builders:
JSONDocumentBuilder builder1;
JSONDocumentBuilder builder2;
```

### Complete Example

```cpp
#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"

using namespace artdaq::database::docrecord;

void createDatabaseDocument() {
  // User provides configuration data
  JSONDocument userData(R"({
    "detector": "ICARUS",
    "channels": 1024,
    "settings": {"gain": 2.5}
  })");

  // Create builder and wrap data
  JSONDocumentBuilder builder;
  builder.createFromData(userData);

  // Set metadata
  builder.setVersion(JSONDocument(R"({"name":"v1.0.0"})"));
  builder.setCollection(JSONDocument(R"({"collection":"detector_configs"})"));
  builder.newObjectID();

  // Add organizational metadata
  builder.addEntity(JSONDocument(R"({"entity":"ICARUS_TPC"})"));
  builder.addConfiguration(JSONDocument(R"({"configuration":"nominal"})"));
  builder.addAlias(JSONDocument(R"({"alias":"latest"})"));

  // Extract and save final document
  try {
    auto dbDoc = builder.extract();
    dbDoc.saveToFile("icarus_config.json");
  } catch (const std::exception& e) {
    std::cerr << "Failed to create document: " << e.what() << std::endl;
  }
}
```
