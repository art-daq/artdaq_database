# JSONDocumentBuilder.cpp

**Path:** `artdaq-database/JsonDocument/JSONDocumentBuilder.cpp`

**Implements:** [JSONDocumentBuilder.h](./JSONDocumentBuilder.h.md)

**Purpose:** This file implements the `JSONDocumentBuilder` class methods, providing the concrete implementation of the builder pattern API for creating and modifying structured JSON database documents. It integrates with the overlay system to ensure proper document structure, metadata, and bookkeeping through a fluent interface.

## Implementation Overview

### Fluent Interface Implementation
Each modification method follows the pattern:
1. Log operation with TRACE including arguments
2. Create copy of parameter document (preserves caller's document)
3. Create typed overlay for the parameter using `overlay<>()` template
4. Save undo state (placeholder - always succeeds)
5. Perform operation via internal `_overlay` methods
6. Return `self()` for method chaining
7. On exception: log, call undo, return `self()`

### Overlay-Based Operations
All document modifications go through overlay classes:
- `ovlAlias` - Alias management (add/remove)
- `ovlConfiguration` - Configuration management (add/remove)
- `ovlEntity` - Entity management (add/remove)
- `ovlRun` - Run association (add)
- `ovlVersion` - Version setting
- `ovlCollection` - Collection assignment
- `ovlId` - Object ID management

### Exception Recovery Pattern
All public methods use try-catch with undo to maintain builder consistency:
```cpp
try {
  ThrowOnFailure(SaveUndo());
  ThrowOnFailure(/* operation */);
  return self();
} catch (std::exception const& ex) {
  TLOG(N) << "Exception:" << ex.what();
  ThrowOnFailure(CallUndo());
  return self();
}
```

This ensures the builder remains usable even after operation failures.

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/JsonDocument/JSONDocumentBuilder.h` | Class definition |
| `<utility>` | `std::move` for efficient parameter passing |
| `artdaq-database/JsonDocument/common.h` | Module-wide types, literals, and utilities |

## Functions

### Constructors

#### `JSONDocumentBuilder::JSONDocumentBuilder()`

**Brief:** Default constructor that creates a builder with an empty document template using `template__empty_document` and initializes the overlay for structured access.

**Parameters:** None

**Postconditions:**
- Document contains empty template string
- Overlay is created using `ovlDatabaseRecord` for structured access
- `_initOK` is set based on `init()` result

**Thread Safety:** safe (construction)

#### `JSONDocumentBuilder::JSONDocumentBuilder(JSONDocument document)`

**Brief:** Constructor that initializes the builder by moving a provided document and creating an overlay for it.

**Parameters:**
- `document` - Existing JSONDocument to build upon (moved)

**Postconditions:**
- Document is moved into builder
- Overlay is created for the document

**Thread Safety:** safe (construction)

### Initialization

#### `JSONDocumentBuilder::init() -> bool`

**Brief:** Private initialization method that logs the new document and returns true.

**Parameters:** None

**Returns:** Always returns `true`

**TRACE Level:** 20

### Alias Operations

#### `JSONDocumentBuilder::addAlias(JSONDocument const& alias) -> JSONDocumentBuilder&`

**Brief:** Adds an alias to the document by creating an `ovlAlias` overlay from the alias document and delegating to the internal overlay's addAlias method.

**Parameters:**
- `alias` - Document containing alias in format `{"alias": "alias_name"}`

**Returns:** Reference to this builder for method chaining

**Postconditions:**
- Alias is added to document's active aliases
- On failure: undo is called, builder remains usable

**Throws:** None (exceptions are caught internally)

**Thread Safety:** unsafe

**TRACE Levels:** 21-22

#### `JSONDocumentBuilder::removeAlias(JSONDocument const& alias) -> JSONDocumentBuilder&`

**Brief:** Removes an alias from the document by creating an `ovlAlias` overlay and delegating to the internal overlay's removeAlias method.

**Parameters:**
- `alias` - Document containing alias to remove

**Returns:** Reference to this builder for method chaining

**Thread Safety:** unsafe

**TRACE Levels:** 23-24

### Configuration Operations

#### `JSONDocumentBuilder::addConfiguration(JSONDocument const& config) -> JSONDocumentBuilder&`

**Brief:** Adds a configuration to the document by creating an `ovlConfiguration` overlay and delegating to the internal overlay's addConfiguration method.

**Parameters:**
- `config` - Document containing configuration in format `{"configuration": "config_name"}`

**Returns:** Reference to this builder for method chaining

**Thread Safety:** unsafe

**TRACE Levels:** 25-26

#### `JSONDocumentBuilder::removeConfiguration(JSONDocument const& config) -> JSONDocumentBuilder&`

**Brief:** Removes a configuration from the document by creating an `ovlConfiguration` overlay and delegating to the internal overlay's removeConfiguration method.

**Parameters:**
- `config` - Document containing configuration to remove

**Returns:** Reference to this builder for method chaining

**Thread Safety:** unsafe

**TRACE Levels:** 27-28

#### `JSONDocumentBuilder::removeAllConfigurations() -> JSONDocumentBuilder&`

**Brief:** Removes all configurations from the document by calling `wipe()` on the overlay's configurations list.

**Parameters:** None

**Returns:** Reference to this builder for method chaining

**Postconditions:**
- Document's configurations list is empty

**Thread Safety:** unsafe

**TRACE Levels:** 29-30

### Object ID Operations

#### `JSONDocumentBuilder::setObjectID(JSONDocument const& objectId) -> JSONDocumentBuilder&`

**Brief:** Sets the document's object ID by creating an `ovlId` overlay and using `swap()` to replace the existing ID.

**Parameters:**
- `objectId` - Document containing object ID

**Returns:** Reference to this builder for method chaining

**Thread Safety:** unsafe

**TRACE Levels:** 31-32

#### `JSONDocumentBuilder::getObjectID() const -> JSONDocument`

**Brief:** Returns the document's object ID as a JSONDocument by extracting it from the overlay's `id().to_string()`.

**Parameters:** None

**Returns:** JSONDocument containing the object ID

**Thread Safety:** safe (const method)

#### `JSONDocumentBuilder::getObjectOUID() const -> std::string`

**Brief:** Returns the document's object ID as a string in OID format by calling `_overlay->id().oid()`.

**Parameters:** None

**Returns:** String representation of the object ID

**Thread Safety:** safe (const method)

#### `JSONDocumentBuilder::newObjectID() -> bool`

**Brief:** Generates a new unique object ID for the document by calling `_overlay->id().newId()`.

**Parameters:** None

**Returns:** `true` if new ID generated successfully

**Thread Safety:** unsafe

### Version and Collection Operations

#### `JSONDocumentBuilder::setVersion(JSONDocument const& version) -> JSONDocumentBuilder&`

**Brief:** Sets the document's version by extracting the "name" value from the provided document and creating an `ovlVersion` overlay.

**Parameters:**
- `version` - Document containing version in format `{"name": "version_string"}`

**Returns:** Reference to this builder for method chaining

**Thread Safety:** unsafe

**TRACE Levels:** 33-34

#### `JSONDocumentBuilder::setCollection(JSONDocument const& collection) -> JSONDocumentBuilder&`

**Brief:** Sets the document's collection by finding the "collection" child, creating an `ovlCollection` overlay, and using swap.

**Parameters:**
- `collection` - Document containing collection in format `{"collection": "collection_name"}`

**Returns:** Reference to this builder for method chaining

**Thread Safety:** unsafe

**TRACE Level:** 35

**Note:** This method does not log exceptions, unlike other builder methods.

### Entity and Run Operations

#### `JSONDocumentBuilder::addEntity(JSONDocument const& entity) -> JSONDocumentBuilder&`

**Brief:** Adds an entity to the document by creating an `ovlEntity` overlay and delegating to the internal overlay's addEntity method.

**Parameters:**
- `entity` - Document containing entity in format `{"entity": "entity_name"}`

**Returns:** Reference to this builder for method chaining

**Thread Safety:** unsafe

**TRACE Levels:** 36-37

#### `JSONDocumentBuilder::addRun(JSONDocument const& run) -> JSONDocumentBuilder&`

**Brief:** Adds a run association to the document by creating an `ovlRun` overlay and delegating to the internal overlay's addRun method.

**Parameters:**
- `run` - Document containing run information

**Returns:** Reference to this builder for method chaining

**Thread Safety:** unsafe

**TRACE Levels:** 38-39

#### `JSONDocumentBuilder::removeEntity(JSONDocument const& entity) -> JSONDocumentBuilder&`

**Brief:** Removes an entity from the document by creating an `ovlEntity` overlay and delegating to the internal overlay's removeEntity method.

**Parameters:**
- `entity` - Document containing entity to remove

**Returns:** Reference to this builder for method chaining

**Thread Safety:** unsafe

**TRACE Levels:** 40-41

#### `JSONDocumentBuilder::removeAllEntities() -> JSONDocumentBuilder&`

**Brief:** Removes all entities from the document by calling `wipe()` on the overlay's entities list.

**Parameters:** None

**Returns:** Reference to this builder for method chaining

**Postconditions:**
- Document's entities list is empty

**Thread Safety:** unsafe

**TRACE Levels:** 42-43

### Bookkeeping Operations

#### `JSONDocumentBuilder::markReadonly() -> JSONDocumentBuilder&`

**Brief:** Marks the document as read-only by calling `_overlay->markReadonly()`.

**Parameters:** None

**Returns:** Reference to this builder for method chaining

**Postconditions:**
- Document's bookkeeping.isreadonly flag is set to true

**Thread Safety:** unsafe

**TRACE Levels:** 44-45

#### `JSONDocumentBuilder::markDeleted() -> JSONDocumentBuilder&`

**Brief:** Marks the document as deleted by calling `_overlay->markDeleted()`.

**Parameters:** None

**Returns:** Reference to this builder for method chaining

**Postconditions:**
- Document's bookkeeping.isdeleted flag is set to true

**Thread Safety:** unsafe

**TRACE Levels:** 46-47

#### `JSONDocumentBuilder::isReadonlyOrDeleted() const -> bool`

**Brief:** Checks if the document is marked as read-only or deleted by calling `_overlay->isReadonlyOrDeleted()`.

**Parameters:** None

**Returns:** `true` if document is readonly or deleted, `false` otherwise

**Thread Safety:** safe (const method)

### Extraction and Comparison

#### `JSONDocumentBuilder::extractTags() const -> std::list<std::string>`

**Brief:** Extracts searchable tags from the document by iterating through version, configurations, and entities from the overlay.

**Parameters:** None

**Returns:** List of tags in format `"type:value"` (e.g., `"version:v1.0.0"`)

**Thread Safety:** safe (const method)

**Implementation Details:**
1. Add version tag: `"version:<version_string>"`
2. Iterate configurations, add: `"configuration:<config_name>"`
3. Iterate entities, add: `"entity:<entity_name>"`

**Example:**
```cpp
auto tags = builder.extractTags();
// tags = ["version:v1.0.0", "configuration:nominal", "entity:TPC_West"]
```

#### `JSONDocumentBuilder::comapreUsingOverlays(JSONDocumentBuilder const& other) const -> result_t`

**Brief:** Compares this builder's document with another using overlay equality operator `*_overlay == *other._overlay`.

**Parameters:**
- `other` - Builder to compare with

**Returns:** result_t indicating success/failure of comparison

**Thread Safety:** safe (const method)

**Note:** Method name has typo (should be "compareUsingOverlays")

#### `JSONDocumentBuilder::to_string() const -> std::string`

**Brief:** Converts the document to its JSON string representation by setting `_isDirty = true` and calling `_document.to_string()`.

**Parameters:** None

**Returns:** JSON string of the document

**Thread Safety:** conditional (modifies document's _isDirty flag)

### Undo Mechanism

#### `JSONDocumentBuilder::SaveUndo() -> result_t`

**Brief:** Placeholder method for saving undo state that currently always returns `Success()`.

**Parameters:** None

**Returns:** `Success()` (always)

**Note:** Actual state preservation not implemented. Reserved for future transaction support.

#### `JSONDocumentBuilder::CallUndo() noexcept -> result_t`

**Brief:** Placeholder method for restoring undo state that currently always returns `Success()`.

**Parameters:** None

**Returns:** `Success()` (always, returns `Failure()` if exception caught)

**Note:** Actual state restoration not implemented. Reserved for future transaction support.

### Stream and Debug

#### `operator<<(std::ostream& os, JSONDocumentBuilder const& data) -> std::ostream&`

**Brief:** Stream output operator that writes the builder's JSON string to an output stream by calling `data.to_string()`.

**Parameters:**
- `os` - Output stream
- `data` - Builder to output

**Returns:** Reference to the output stream

**Thread Safety:** conditional

#### `debug::JSONDocumentBuilder()`

**Brief:** Enables maximum TRACE logging for debugging builder operations by setting all TRACE levels.

**Parameters:** None

**Returns:** None

**Side Effects:**
- Configures TRACE logging for JSONDocumentBuilder debugging

**TRACE Level:** 48 (activation message)

## Maintenance Notes

### TRACE Logging Levels
- **20**: Initialization
- **21-24**: Alias operations (add: 21-22, remove: 23-24)
- **25-30**: Configuration operations (add: 25-26, remove: 27-28, removeAll: 29-30)
- **31-32**: Object ID operations
- **33-34**: Version operations
- **35**: Collection operations
- **36-43**: Entity and run operations (addEntity: 36-37, addRun: 38-39, removeEntity: 40-41, removeAll: 42-43)
- **44-47**: Bookkeeping operations (readonly: 44-45, deleted: 46-47)
- **48**: Debug activation

Pattern: Consecutive pairs (N, N+1) for operation entry and exception.

### Known Issues
1. **Method name typo** - `comapreUsingOverlays` should be `compareUsingOverlays`
2. **Inconsistent logging** - `setCollection` doesn't log exceptions in catch block
3. **Placeholder undo** - SaveUndo/CallUndo don't actually save/restore state

### Error Recovery Pattern
All modification methods ensure builder remains in consistent state:
- On success: return `self()` for chaining
- On failure: log error with TRACE, call undo (no-op currently), return `self()` (allows continued use)

This pattern means exceptions do not propagate to caller, which may hide errors. Check TRACE output for failures.

## See Also

- [JSONDocumentBuilder.h.md](./JSONDocumentBuilder.h.md) - Class declaration
- [JSONDocument_utils.cpp.md](./JSONDocument_utils.cpp.md) - Contains `createFromData` and `_importUserData` implementations
- [docrecord_literals.h.md](./docrecord_literals.h.md) - Provides JSON field name literals (`jsonliteral::*`)
- [Overlay Module](../Overlay/README.md) - Overlay classes used for structured access
