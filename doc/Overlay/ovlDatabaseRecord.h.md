# ovlDatabaseRecord.h

**Path:** `artdaq-database/Overlay/ovlDatabaseRecord.h`

**Purpose:** Defines the `ovlDatabaseRecord` class, the central overlay class that aggregates all components of a database document into a unified, type-safe interface. This class provides the complete representation of a configuration database document including user data, system metadata, tracking information, and access control. It is the primary interface for working with complete documents in the artdaq-database system.


## Key Concepts

### Complete Document Aggregation

`ovlDatabaseRecord` combines 14 distinct components into a single coherent interface. Each component is auto-initialized during construction if not present in the source JSON:

| Component | Type | Purpose |
|-----------|------|---------|
| `_id` | `ovlId` | Unique identifier (MongoDB OID compatible) |
| `_collection` | `ovlCollection` | Collection name for grouping documents |
| `_version` | `ovlVersion` | Version string for tracking revisions |
| `_origin` | `ovlOrigin` | Document provenance (source, format, creation time) |
| `_bookkeeping` | `ovlBookkeeping` | State flags and update history |
| `_changelog` | `ovlChangeLog` | Human-readable change log |
| `_document` | `ovlDocument` | User configuration data container |
| `_configurations` | `ovlConfigurations` | List of associated configurations |
| `_entities` | `ovlEntities` | List of associated entities |
| `_aliases` | `ovlAliases` | Named version aliases (movable list) |
| `_runs` | `ovlRuns` | List of associated run numbers |
| `_attachments` | `ovlAttachments` | File attachments |
| `_comments` | `ovlComments` | Line-associated comments |
| `_configurationtype` | `ovlConfigurationType` | Configuration type identifier |

### Access Control Integration

All modification methods integrate with bookkeeping to:
- Check readonly/deleted state before modifications
- Automatically record all changes in update history
- Enforce immutability for archived documents
- Prevent modifications to documents marked for deletion

### Modification Tracking

Every add/remove/set operation automatically posts an update to bookkeeping, creating a complete audit trail without requiring explicit tracking by calling code. The update includes:
- Operation name (e.g., "addConfiguration", "setVersion")
- Timestamp of the change
- Reference to the affected component

## Thread Safety

- **Thread-safe:** No
- **Concurrent access:** Not supported; external synchronization required
- **Locking:** None internal; uses static comparison mask

All methods access mutable JSON data through references without synchronization. The comparison mask is stored in static storage, affecting all threads.

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/Overlay/common.h` | Module foundation types, constants, result type |
| `artdaq-database/Overlay/ovlBookkeeping.h` | State and history tracking overlay |
| `artdaq-database/Overlay/ovlChangeLog.h` | Human-readable change log overlay |
| `artdaq-database/Overlay/ovlComment.h` | Line-associated comments overlay |
| `artdaq-database/Overlay/ovlDocument.h` | User data container overlay |
| `artdaq-database/Overlay/ovlId.h` | Unique identifier overlay |
| `artdaq-database/Overlay/ovlKeyValue.h` | Base class |
| `artdaq-database/Overlay/ovlOrigin.h` | Provenance tracking overlay |
| `artdaq-database/Overlay/ovlKeyValueTimeStamp.h` | Timestamped entry templates |
| `artdaq-database/Overlay/ovlKeyValueWithDefault.h` | Default value support |
| `artdaq-database/Overlay/ovlFixedList.h` | Fixed list container template |
| `artdaq-database/Overlay/ovlMovableList.h` | Movable list container template (for aliases) |

## Type Aliases

The header defines numerous type aliases for document components:

### Simple String Fields

```cpp
using ovlVersion = ovlStringKeyValue<DOCUMENT_COMPARE_MUTE_VERSION>;
using ovlVersionUPtr_t = std::unique_ptr<ovlVersion>;

using ovlCollection = ovlStringKeyValue<DOCUMENT_COMPARE_MUTE_COLLECTION>;
using ovlCollectionUPtr_t = std::unique_ptr<ovlCollection>;

using ovlConfigurationType = ovlStringKeyValue<DOCUMENT_COMPARE_MUTE_CONFIGURATION>;
using ovlConfigurationTypeUPtr_t = std::unique_ptr<ovlConfigurationType>;
```

### Timestamped Entries

```cpp
using ovlAlias = ovlKeyValueTimeStamp<DOCUMENT_COMPARE_MUTE_ALIAS, true, true>;
using ovlAliasUPtr_t = std::unique_ptr<ovlAlias>;

using ovlConfiguration = ovlKeyValueTimeStamp<DOCUMENT_COMPARE_MUTE_CONFIGURATION>;
using ovlConfigurationUPtr_t = std::unique_ptr<ovlConfiguration>;

using ovlRun = ovlKeyValueTimeStamp<DOCUMENT_COMPARE_MUTE_RUN>;
using ovlRunUPtr_t = std::unique_ptr<ovlRun>;

using ovlEntity = ovlKeyValueTimeStamp<DOCUMENT_COMPARE_MUTE_CONFIGENTITY>;
using ovlEntityUPtr_t = std::unique_ptr<ovlEntity>;

using ovlUpdateEntry = ovlKeyValueTimeStamp<DOCUMENT_COMPARE_MUTE_UPDATE_VALUES>;
using ovlUpdateEntryUPtr_t = std::unique_ptr<ovlUpdateEntry>;
```

### List Containers

```cpp
using ovlConfigurations = ovlFixedList<ovlConfiguration, DOCUMENT_COMPARE_MUTE_CONFIGURATION>;
using ovlConfigurationsUPtr_t = std::unique_ptr<ovlConfigurations>;

using ovlEntities = ovlFixedList<ovlEntity, DOCUMENT_COMPARE_MUTE_CONFIGENTITY>;
using ovlEntitiesUPtr_t = std::unique_ptr<ovlEntities>;

using ovlAliases = ovlMovableList<ovlAlias, DOCUMENT_COMPARE_MUTE_ALIAS>;
using ovlAliasesUPtr_t = std::unique_ptr<ovlAliases>;

using ovlAttachments = ovlFixedList<ovlAttachment, DOCUMENT_COMPARE_MUTE_ATTACHMENT>;
using ovlAttachmentsUPtr_t = std::unique_ptr<ovlAttachments>;

using ovlRuns = ovlFixedList<ovlRun, DOCUMENT_COMPARE_MUTE_RUN>;
using ovlRunsUPtr_t = std::unique_ptr<ovlRuns>;

using ovlComments = ovlFixedList<ovlComment, DOCUMENT_COMPARE_MUTE_COMMENTS>;
using ovlCommentsUPtr_t = std::unique_ptr<ovlComments>;
```

### Other

```cpp
using ovlAttachment = ovlKeyValue;
using ovlAttachmentUPtr_t = std::unique_ptr<ovlAttachment>;
```

## Classes/Structures

### `ovlDatabaseRecord`

The central overlay class for complete database documents, inheriting from `ovlKeyValue`. Provides type-safe access to all document components with integrated access control and modification tracking.

**Thread Safety:** Not thread-safe; references mutable JSON data and uses static comparison mask.

#### Constructor

##### `ovlDatabaseRecord(value_t& record)`

**Brief:** Constructs overlays for all 14 document components, auto-initializing missing fields with appropriate defaults. This is the primary way to work with database documents.

**Parameters:**
- `record` - Reference to the JSON object representing the complete database document

**Preconditions:**
- `record` must be a valid JSON object or convertible to one

**Postconditions:**
- All 14 component overlays are initialized
- Missing fields are auto-populated with defaults by each component's constructor
- Document is ready for use with all accessor and modification methods
- A unique OID is generated if not present

**Throws:**
| Exception | Condition |
|-----------|-----------|
| Assertion failure | If any component initialization fails |

**Thread Safety:** Not applicable (construction)

**Example:**
```cpp
#include "artdaq-database/Overlay/ovlDatabaseRecord.h"
#include <iostream>

using namespace artdaq::database::overlay;

void createDocument() {
  try {
    // Create document from empty JSON - all components auto-initialized
    value_t recordJson = object_t{};
    ovlDatabaseRecord record{recordJson};

    // All components are ready to use
    std::cout << "Document OID: " << record.id().oid() << std::endl;
    std::cout << "Origin format: " << record.origin().format() << std::endl;
    std::cout << "Is readonly: " << record.bookkeeping().isReadonly() << std::endl;

    // Set some values
    record.version().string_value() = "v1_0_0";
    record.origin().format() = "fcl";
    record.origin().name() = "my_config.fcl";
  } catch (const std::exception& e) {
    std::cerr << "Error creating document: " << e.what() << std::endl;
  }
}
```

#### Special Member Functions

##### `ovlDatabaseRecord(ovlDatabaseRecord&&) = default`

**Brief:** Default move constructor allowing transfer of document overlay ownership. The moved-from object should not be used after the move.

**Thread Safety:** Not applicable (construction)

##### `~ovlDatabaseRecord() = default`

**Brief:** Default destructor; no special cleanup required as the overlay does not own the underlying JSON data.

**Thread Safety:** Not applicable (destruction)

#### Component Accessor Methods

##### `document() -> ovlDocument&`

**Brief:** Returns a reference to the document component containing user configuration data. This is where the actual configuration content is stored.

**Returns:** Reference to the document overlay.

**Thread Safety:** Unsafe

##### `comments() -> ovlComments&`

**Brief:** Returns a reference to the comments list component, which stores line-associated annotations.

**Returns:** Reference to the comments list overlay.

**Thread Safety:** Unsafe

##### `origin() -> ovlOrigin&`

**Brief:** Returns a reference to the origin component for tracking document provenance (source file, format, creation time).

**Returns:** Reference to the origin overlay.

**Thread Safety:** Unsafe

##### `version() -> ovlVersion&`

**Brief:** Returns a reference to the version string component, typically used for tracking configuration revisions.

**Returns:** Reference to the version overlay.

**Thread Safety:** Unsafe

##### `entities() -> ovlEntities&`

**Brief:** Returns a reference to the entities list component, which tracks which system components use this configuration.

**Returns:** Reference to the entities list overlay.

**Thread Safety:** Unsafe

##### `configurations() -> ovlConfigurations&`

**Brief:** Returns a reference to the configurations list component for tracking related configurations.

**Returns:** Reference to the configurations list overlay.

**Thread Safety:** Unsafe

##### `bookkeeping() -> ovlBookkeeping&`

**Brief:** Returns a reference to the bookkeeping component for state (readonly/deleted) and update history.

**Returns:** Reference to the bookkeeping overlay.

**Thread Safety:** Unsafe

##### `id() -> ovlId&`

**Brief:** Returns a reference to the unique identifier component containing the MongoDB-compatible OID.

**Returns:** Reference to the ID overlay.

**Thread Safety:** Unsafe

#### Swap Methods

All swap methods follow the same pattern: check access control, then swap component contents.

##### `swap(ovlDocumentUPtr_t& document) -> result_t`

**Brief:** Swaps the document component with the provided one, checking access control first. Use this to replace the entire user data section.

**Parameters:**
- `document` - Unique pointer to the new document overlay

**Preconditions:**
- Document must not be readonly or deleted

**Returns:** `Success()` on success, `Failure(msg_IsReadonly)` if protected.

**Thread Safety:** Unsafe

##### `swap(ovlCommentsUPtr_t& comments) -> result_t`

**Brief:** Swaps the comments component with the provided one, checking access control.

**Parameters:**
- `comments` - Unique pointer to the new comments overlay

**Returns:** `Success()` on success, `Failure(msg_IsReadonly)` if protected.

**Thread Safety:** Unsafe

##### `swap(ovlOriginUPtr_t& origin) -> result_t`

**Brief:** Swaps the origin component with the provided one, checking access control.

**Parameters:**
- `origin` - Unique pointer to the new origin overlay

**Returns:** `Success()` on success, `Failure(msg_IsReadonly)` if protected.

**Thread Safety:** Unsafe

##### `swap(ovlVersionUPtr_t& version) -> result_t`

**Brief:** Swaps the version component with the provided one, checking access control.

**Parameters:**
- `version` - Unique pointer to the new version overlay

**Returns:** `Success()` on success, `Failure(msg_IsReadonly)` if protected.

**Thread Safety:** Unsafe

##### `swap(ovlConfigurationTypeUPtr_t& configtype) -> result_t`

**Brief:** Swaps the configuration type component with the provided one, checking access control.

**Parameters:**
- `configtype` - Unique pointer to the new configuration type overlay

**Returns:** `Success()` on success, `Failure(msg_IsReadonly)` if protected.

**Thread Safety:** Unsafe

##### `swap(ovlCollectionUPtr_t& collection) -> result_t`

**Brief:** Swaps the collection component with the provided one, checking access control.

**Parameters:**
- `collection` - Unique pointer to the new collection overlay

**Returns:** `Success()` on success, `Failure(msg_IsReadonly)` if protected.

**Thread Safety:** Unsafe

##### `swap(ovlIdUPtr_t& id) -> result_t`

**Brief:** Swaps the ID component with the provided one, checking access control. Use with caution as this changes the document's identity.

**Parameters:**
- `id` - Unique pointer to the new ID overlay

**Returns:** `Success()` on success, `Failure(msg_IsReadonly)` if protected.

**Thread Safety:** Unsafe

#### State Query and Modification Methods

##### `isReadonlyOrDeleted() const -> bool`

**Brief:** Checks if the document is protected from modification (either readonly or deleted). Call this before attempting modifications.

**Returns:** `true` if document is readonly or deleted, `false` otherwise.

**Postconditions:**
- Does not modify the document

**Thread Safety:** Unsafe

**Example:**
```cpp
if (!record.isReadonlyOrDeleted()) {
  // Safe to modify
  record.addEntity(entity);
} else {
  std::cerr << "Cannot modify protected document" << std::endl;
}
```

##### `markReadonly() -> result_t`

**Brief:** Marks the document as readonly, preventing future modifications. Use this to protect finalized configurations from accidental changes.

**Preconditions:**
- Document must not already be readonly or deleted

**Returns:** `Success()` on success, `Failure(msg_IsReadonly)` if already protected.

**Postconditions:**
- Document cannot be modified further (all modification methods will fail)

**Thread Safety:** Unsafe

##### `markDeleted() -> result_t`

**Brief:** Marks the document as deleted (soft delete). The document remains in storage but is flagged as deleted.

**Returns:** `Success()` always (can mark deleted even if already readonly).

**Postconditions:**
- Document is marked as deleted
- `isReadonlyOrDeleted()` will return `true`

**Thread Safety:** Unsafe

**Note:** Unlike `markReadonly()`, this method succeeds even if the document is already readonly, allowing soft-delete of archived documents.

#### Configuration Management Methods

##### `addConfiguration(ovlConfigurationUPtr_t& configuration) -> result_t`

**Brief:** Adds a configuration to the document's configuration list with automatic update tracking in bookkeeping.

**Parameters:**
- `configuration` - Unique pointer to the configuration to add

**Preconditions:**
- `configuration` must not be null
- Document must not be readonly or deleted

**Returns:** `Success()` on success, `Failure()` with error details on failure.

**Postconditions:**
- Configuration added to list
- Update recorded in bookkeeping with "addConfiguration" event

**Thread Safety:** Unsafe

##### `removeConfiguration(ovlConfigurationUPtr_t& configuration) -> result_t`

**Brief:** Removes a configuration from the document's configuration list with automatic update tracking.

**Parameters:**
- `configuration` - Unique pointer to the configuration to remove

**Preconditions:**
- `configuration` must not be null
- Document must not be readonly or deleted

**Returns:** `Success()` on success, `Failure()` with error details on failure.

**Postconditions:**
- Configuration removed from list
- Update recorded in bookkeeping with "removeConfiguration" event

**Thread Safety:** Unsafe

#### Alias Management Methods

##### `addAlias(ovlAliasUPtr_t& alias) -> result_t`

**Brief:** Adds an alias to the document's alias list with automatic update tracking. Aliases provide human-readable names for document versions.

**Parameters:**
- `alias` - Unique pointer to the alias to add

**Preconditions:**
- `alias` must not be null
- Document must not be readonly or deleted

**Returns:** `Success()` on success, `Failure()` with error details on failure.

**Postconditions:**
- Alias added to active alias list
- Update recorded in bookkeeping with "addAlias" event

**Thread Safety:** Unsafe

##### `removeAlias(ovlAliasUPtr_t& alias) -> result_t`

**Brief:** Removes an alias from the document's alias list (moves to history) with automatic update tracking.

**Parameters:**
- `alias` - Unique pointer to the alias to remove

**Preconditions:**
- `alias` must not be null
- Document must not be readonly or deleted

**Returns:** `Success()` on success, `Failure()` with error details on failure.

**Postconditions:**
- Alias moved from active to history list
- Update recorded in bookkeeping with "removeAlias" event

**Thread Safety:** Unsafe

#### Entity Management Methods

##### `addEntity(ovlEntityUPtr_t& entity) -> result_t`

**Brief:** Adds an entity to the document's entity list with automatic update tracking. Entities represent system components that use this configuration.

**Parameters:**
- `entity` - Unique pointer to the entity to add

**Preconditions:**
- `entity` must not be null
- Document must not be readonly or deleted

**Returns:** `Success()` on success, `Failure()` with error details on failure.

**Postconditions:**
- Entity added to list
- Update recorded in bookkeeping with "addEntity" event

**Thread Safety:** Unsafe

##### `removeEntity(ovlEntityUPtr_t& entity) -> result_t`

**Brief:** Removes an entity from the document's entity list with automatic update tracking.

**Parameters:**
- `entity` - Unique pointer to the entity to remove

**Preconditions:**
- `entity` must not be null
- Document must not be readonly or deleted

**Returns:** `Success()` on success, `Failure()` with error details on failure.

**Postconditions:**
- Entity removed from list
- Update recorded in bookkeeping with "removeEntity" event

**Thread Safety:** Unsafe

#### Run Management Method

##### `addRun(ovlRunUPtr_t& run) -> result_t`

**Brief:** Adds a run to the document's run list with automatic update tracking. Runs represent data-taking sessions that used this configuration.

**Parameters:**
- `run` - Unique pointer to the run to add

**Preconditions:**
- `run` must not be null
- Document must not be readonly or deleted

**Returns:** `Success()` on success, `Failure()` with error details on failure.

**Postconditions:**
- Run added to list
- Update recorded in bookkeeping with "addRun" event

**Thread Safety:** Unsafe

#### Setter Methods

##### `setVersion(ovlVersionUPtr_t& version) -> result_t`

**Brief:** Sets the document version with deduplication (no-op if value unchanged) and automatic update tracking.

**Parameters:**
- `version` - Unique pointer to the new version

**Preconditions:**
- `version` must not be null
- Document must not be readonly or deleted

**Returns:** `Success()` on success, `Success(msg_Ignored)` if value unchanged, `Failure()` on error.

**Postconditions:**
- Version updated if changed
- Update recorded in bookkeeping with "setVersion" event (only if changed)

**Thread Safety:** Unsafe

##### `setCollection(ovlCollectionUPtr_t& collection) -> result_t`

**Brief:** Sets the document collection with deduplication and automatic update tracking.

**Parameters:**
- `collection` - Unique pointer to the new collection

**Returns:** `Success()` on success, `Success(msg_Ignored)` if value unchanged, `Failure()` on error.

**Thread Safety:** Unsafe

##### `setConfigurationType(ovlConfigurationTypeUPtr_t& configtype) -> result_t`

**Brief:** Sets the configuration type with deduplication and automatic update tracking.

**Parameters:**
- `configtype` - Unique pointer to the new configuration type

**Returns:** `Success()` on success, `Success(msg_Ignored)` if value unchanged, `Failure()` on error.

**Thread Safety:** Unsafe

#### Serialization and Comparison

##### `to_string() const -> std::string` [override]

**Brief:** Serializes the complete document to a JSON-formatted string, including all 14 components. Useful for debugging and logging.

**Returns:** JSON representation of all components.

**Thread Safety:** Unsafe

##### `operator==(ovlDatabaseRecord const& other) const -> result_t`

**Brief:** Compares all components of two documents with comprehensive difference reporting. Each component is compared using its own comparison operator, respecting individual mask settings.

**Parameters:**
- `other` - The document to compare against

**Returns:** `result_t` where `first` is `true` if all components match, `false` otherwise with aggregated difference details in `second`.

**Thread Safety:** Unsafe

## Type Alias

```cpp
using ovlDatabaseRecordUPtr_t = std::unique_ptr<ovlDatabaseRecord>;
```

Unique pointer type for managing `ovlDatabaseRecord` instances.

## Complete JSON Structure

```json
{
  "id": {"oid": "507f1f77bcf86cd799439011"},
  "collection": "DAQ_Configurations",
  "version": "v2_0_0",
  "origin": {
    "format": "json",
    "name": "daq_config.json",
    "source": "file",
    "created": "2025-01-15T10:00:00Z",
    "rawdatalist": []
  },
  "aliases": {
    "active": [{"name": "production", "assigned": "2025-01-15T10:00:00Z"}],
    "history": []
  },
  "bookkeeping": {
    "isreadonly": false,
    "isdeleted": false,
    "created": "2025-01-15T10:00:00Z",
    "updates": []
  },
  "changelog": "Initial creation.\n",
  "entities": [{"name": "BoardReader01", "assigned": "2025-01-15T10:00:00Z"}],
  "runs": [],
  "attachments": [],
  "configurationtype": "DAQ",
  "configurations": [{"name": "TriggerConfig", "assigned": "2025-01-15T10:00:00Z"}],
  "comments": [{"linenum": 1, "value": "Production configuration"}],
  "document": {
    "data": {"trigger_rate": 1000},
    "metadata": {"description": "DAQ configuration"},
    "search": []
  }
}
```

## Component Hierarchy

```
ovlDatabaseRecord
+-- ovlId (unique identifier)
+-- ovlCollection (collection name)
+-- ovlVersion (version string)
+-- ovlConfigurationType (configuration type)
+-- ovlOrigin (provenance)
|   +-- format, name, source
|   +-- created timestamp
|   +-- raw data list
+-- ovlBookkeeping (state & history)
|   +-- readonly/deleted flags
|   +-- created timestamp
|   +-- updates list
+-- ovlChangeLog (human-readable log)
+-- ovlDocument (user content)
|   +-- data (configuration)
|   +-- metadata (description)
|   +-- search (indexes)
+-- ovlConfigurations (list)
+-- ovlEntities (list)
+-- ovlAliases (movable list)
+-- ovlRuns (list)
+-- ovlAttachments (list)
+-- ovlComments (list)
```

## Notes for Developers

### Usage Example

```cpp
#include "artdaq-database/Overlay/ovlDatabaseRecord.h"
#include <iostream>

using namespace artdaq::database::overlay;
using namespace artdaq::database::result;

void workWithDocument() {
  try {
    // Create document (auto-initializes all components)
    value_t recordJson = object_t{};
    ovlDatabaseRecord record{recordJson};

    // Set basic info
    record.version().string_value() = "v1_0_0";

    // Set origin
    auto& origin = record.origin();
    origin.format() = "fcl";
    origin.source() = "file";
    origin.name() = "production_config.fcl";

    // Add configuration data
    auto& data = record.document().data().object_value();
    data["trigger_rate"] = 1000;
    data["buffer_size"] = 2048;

    // Check state before modification
    if (!record.isReadonlyOrDeleted()) {
      // Add entity with error handling
      value_t entityJson = object_t{};
      entityJson["name"] = "BoardReader01";
      entityJson["assigned"] = timestamp();
      auto entity = std::make_unique<ovlEntity>("entity", entityJson);

      auto result = record.addEntity(entity);
      if (!result.first) {
        std::cerr << "Failed to add entity: " << result.second << std::endl;
        return;
      }
    }

    // Mark readonly for production
    auto result = record.markReadonly();
    if (!result.first) {
      std::cerr << "Failed to mark readonly: " << result.second << std::endl;
    }

    // Verify document structure
    std::cout << "Document JSON:\n" << record.to_string() << std::endl;
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }
}
```

### Common Pitfalls

- **Check isReadonlyOrDeleted():** Always check before attempting modifications. All modification methods fail on protected documents.
- **Use result_t:** All modification methods return `result_t`; check `.first` before assuming success.
- **Comparison Mask:** Set the comparison mask explicitly before comparing documents. The mask is global and affects all comparisons.
- **Component Initialization:** All 14 components are auto-initialized; don't assume any are null or uninitialized.
- **Unique Pointers:** Add/set methods take unique_ptr references; the pointer may be modified/moved by the operation.

### Best Practices

1. **Check `isReadonlyOrDeleted()`** before all modifications
2. **Use `result_t` return values** - check `first` before proceeding
3. **Set comparison mask explicitly** in tests to ensure consistent behavior
4. **Initialize components through methods**, not direct JSON manipulation
5. **Use add/remove methods for lists**, not direct array access
6. **Mark readonly for production/archived documents** to prevent accidental changes
7. **Use `to_string()` for debugging** - shows complete structure
8. **Handle failures gracefully** - log error messages from `result.second`

### Anti-patterns

```cpp
// DON'T: Modify without checking access control
record.addEntity(entity);  // May silently fail if readonly

// DO: Check first and handle result
if (!record.isReadonlyOrDeleted()) {
  auto result = record.addEntity(entity);
  if (!result.first) {
    handleError(result.second);
  }
}

// DON'T: Ignore result_t
record.setVersion(version);  // Ignores success/failure

// DO: Check result
auto result = record.setVersion(version);
if (result.second == msg_Ignored) {
  // Value was unchanged - this is normal
}

// DON'T: Assume comparison mask state
auto same = (record1 == record2).first;  // Mask state unknown

// DO: Set mask explicitly
useCompareMask(DOCUMENT_COMPARE_MUTE_TIMESTAMPS | DOCUMENT_COMPARE_MUTE_OUIDS);
auto same = (record1 == record2).first;
useCompareMask(0);  // Reset for other code
```

## See Also

- [ovlDatabaseRecord.cpp](./ovlDatabaseRecord.cpp.md) - Implementation file with method details
- [JSONDocumentOverlay.h](./JSONDocumentOverlay.h.md) - Public API header for overlay module
- [ovlDocument.h](./ovlDocument.h.md) - User data component documentation
- [ovlBookkeeping.h](./ovlBookkeeping.h.md) - State and history component documentation
- [ovlOrigin.h](./ovlOrigin.h.md) - Provenance component documentation
- [ovlId.h](./ovlId.h.md) - Identifier component documentation
- [ovlComment.h](./ovlComment.h.md) - Comment component documentation
- [common.h](./common.h.md) - Types, constants, and comparison masks
