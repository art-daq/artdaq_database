# JSONDocumentBuilder.h

## File Overview

This header file defines the `JSONDocumentBuilder` class, which provides a high-level builder pattern API for creating and modifying JSON documents with proper metadata, versioning, and bookkeeping. It wraps the lower-level `JSONDocument` API and integrates with the overlay system to ensure documents have correct structure and maintain database integrity.

**Location**: `/home/user/artdaq-database/artdaq-database/JsonDocument/JSONDocumentBuilder.h`

## Dependencies

```cpp
#include "artdaq-database/JsonDocument/JSONDocument.h"
#include "artdaq-database/JsonDocument/common.h"
#include "artdaq-database/Overlay/JSONDocumentOverlay.h"
```

**Key Dependencies**:
- **JSONDocument.h** - Core document manipulation class
- **common.h** - Module-wide types and utilities
- **JSONDocumentOverlay.h** - Overlay classes for structured access to document fields

## Namespace Structure

```cpp
namespace artdaq {
namespace database {
namespace docrecord {
  // JSONDocumentBuilder class
}
}
}
```

## Type Aliases

```cpp
using artdaq::database::json::array_t;
using artdaq::database::json::object_t;
using artdaq::database::json::type_t;
using artdaq::database::json::value_t;

using artdaq::database::overlay::ovlDatabaseRecord;
using artdaq::database::overlay::ovlDatabaseRecordUPtr_t;
```

Makes JSON types and overlay types available without full qualification.

## JSONDocumentBuilder Class

### Class Declaration

```cpp
class JSONDocumentBuilder final {
public:
  // ... methods ...
private:
  // ... implementation ...
};
```

**Key Characteristic**: Marked `final` - cannot be inherited from.

### Constructors

```cpp
JSONDocumentBuilder();
JSONDocumentBuilder(JSONDocument);
```

**Default Constructor**: Creates a new document from the empty template.

**Document Constructor**: Initializes builder with an existing document.

**Both constructors**:
- Create an overlay for structured access
- Call `init()` for initialization
- Store initialization status in `_initOK`

### Document Creation

```cpp
JSONDocumentBuilder& createFromData(JSONDocument);
```

**Purpose**: Creates a new database document from user data, wrapping it in proper database structure.

**Process**:
1. Resets overlay
2. Creates empty template document
3. Creates new overlay
4. Imports user data into template structure
5. Refreshes overlay

**Use Case**: Converting raw configuration data into a proper database document with metadata.

**Returns**: Reference to self (builder pattern)

### Alias Management

```cpp
JSONDocumentBuilder& addAlias(JSONDocument const&);
JSONDocumentBuilder& removeAlias(JSONDocument const&);
```

**addAlias**:
- Adds an alternate name/identifier to the document
- Uses overlay system to manage alias arrays
- Throws on failure, calls undo

**removeAlias**:
- Removes an alias from the document
- Maintains alias history
- Throws on failure, calls undo

**Alias Use Cases**:
- "latest_stable" pointing to specific version
- "production" pointing to current production config
- Multiple names for the same configuration

### Configuration Management

```cpp
JSONDocumentBuilder& addConfiguration(JSONDocument const&);
JSONDocumentBuilder& removeConfiguration(JSONDocument const&);
JSONDocumentBuilder& removeAllConfigurations();
```

**addConfiguration**:
- Adds a configuration to the document's configuration array
- Configurations represent different settings/variants

**removeConfiguration**:
- Removes specific configuration
- Uses overlay for safe removal

**removeAllConfigurations**:
- Clears all configurations using `wipe()`
- Useful for rebuilding configuration list

### Entity Management

```cpp
JSONDocumentBuilder& addEntity(JSONDocument const&);
JSONDocumentBuilder& removeEntity(JSONDocument const&);
JSONDocumentBuilder& removeAllEntities();
```

**addEntity**:
- Adds an entity (component/subsystem) to the document
- Entities represent configurable components

**removeEntity**:
- Removes specific entity

**removeAllEntities**:
- Clears all entities using `wipe()`

### Run Management

```cpp
JSONDocumentBuilder& addRun(JSONDocument const&);
```

**Purpose**: Associates a run number/identifier with the document.

**Use Case**: Tracking which experimental runs used this configuration.

### Versioning

```cpp
JSONDocumentBuilder& setVersion(JSONDocument const&);
```

**Purpose**: Sets the version identifier for the document.

**Version Format**: Typically string like "v1.0.0" or "2023-01-15"

**Importance**: Critical for tracking document evolution and compatibility.

### Collection Assignment

```cpp
JSONDocumentBuilder& setCollection(JSONDocument const&);
```

**Purpose**: Assigns the document to a specific database collection.

**Collections**: Group related documents (e.g., "run_configurations", "detector_configs")

### Bookkeeping Operations

```cpp
JSONDocumentBuilder& markReadonly();
JSONDocumentBuilder& markDeleted();
```

**markReadonly**:
- Sets bookkeeping flag indicating document is immutable
- Future modification attempts will throw `readonly_exception`
- Used for archiving production configurations

**markDeleted**:
- Soft-delete: marks document as deleted without removing it
- Document remains in database for audit trail
- Queries typically filter out deleted documents

### Object ID Management

```cpp
JSONDocumentBuilder& setObjectID(JSONDocument const&);
JSONDocument getObjectID() const;
std::string getObjectOUID() const;
bool newObjectID();
```

**setObjectID**:
- Sets the database object ID (typically MongoDB-style _id)

**getObjectID**:
- Returns the object ID as a document

**getObjectOUID**:
- Returns the object ID as a string (OID format)

**newObjectID**:
- Generates a new unique object ID
- Returns true if successful

### State Queries

```cpp
bool isReadonlyOrDeleted() const;
```

**Purpose**: Checks if document is in a protected state (readonly or deleted).

**Use Case**: Validating whether modifications are allowed before attempting them.

### Tag Extraction

```cpp
std::list<std::string> extractTags() const;
```

**Purpose**: Extracts searchable tags from the document.

**Tags Include**:
- Version: "version:v1.0.0"
- Configurations: "configuration:detector_A"
- Entities: "entity:daq_component_1"

**Use Case**: Building search indexes, filtering documents.

### Document Extraction

```cpp
JSONDocument extract();
```

**Purpose**: Extracts the final document from the builder (move semantics).

**Characteristics**:
- Transfers ownership
- Builder should not be used after extraction
- Efficient (no copying)

### Comparison

```cpp
result_t comapreUsingOverlays(JSONDocumentBuilder const&) const;
result_t operator==(JSONDocumentBuilder const& other) const;
```

**Note**: Method name has typo: "comapreUsingOverlays" (should be "compare").

**Purpose**: Compares two builders by comparing their overlay structures.

**Returns**: `result_t` indicating success/failure with details.

### String Conversion

```cpp
std::string to_string() const;
```

**Purpose**: Converts the document to JSON string representation.

**Use Case**: Logging, debugging, serialization.

### Special Member Functions

```cpp
// Defaults
~JSONDocumentBuilder() = default;

// Deleted
JSONDocumentBuilder(JSONDocumentBuilder const&) = delete;
JSONDocumentBuilder& operator=(JSONDocumentBuilder const&) = delete;
JSONDocumentBuilder& operator=(JSONDocumentBuilder&&) = delete;
JSONDocumentBuilder(JSONDocumentBuilder&&) = delete;
```

**Design**: Non-copyable, non-movable.

**Rationale**:
- Builder maintains state through overlay
- Copying/moving would complicate overlay management
- Use `extract()` to transfer ownership of document
- Intended for single-use, stack-allocated builders

## Private Methods

### Template Method

```cpp
template <typename OVL>
std::unique_ptr<OVL> overlay(JSONDocument&, object_t::key_type const&);
```

**Purpose**: Creates an overlay of specific type for a document field.

**Template Parameter**: Overlay type (e.g., `ovlAlias`, `ovlConfiguration`, `ovlEntity`)

**Implementation**:
```cpp
template <typename OVL>
std::unique_ptr<OVL> JSONDocumentBuilder::overlay(
    JSONDocument& document, object_t::key_type const& self_key) {
  confirm(!self_key.empty());
  confirm(type(document._value) == type_t::OBJECT);

  if (self_key.empty())
    throw std::runtime_error("Errror: self_key is empty");

  if (type(document._value) != type_t::OBJECT)
    throw std::runtime_error("Errror: document._value is not a type_t::OBJECT type");

  using artdaq::database::sharedtypes::unwrap;
  return std::make_unique<OVL>(self_key, document._value);
}
```

**Usage**:
```cpp
auto ovl = overlay<ovl::ovlConfiguration>(copy, jsonliteral::configuration);
```

### Internal Helpers

```cpp
void _createFromTemplate(JSONDocument document);
JSONDocumentBuilder& self();
JSONDocumentBuilder const& self() const;
bool init();
void _importUserData(JSONDocument const& document);
```

**_createFromTemplate**: Sets internal document from template

**self()**: Returns reference to this (builder pattern)

**init()**: Initialization logic

**_importUserData**: Imports user-provided data into document structure

### Undo Mechanism

```cpp
result_t SaveUndo();
result_t CallUndo() noexcept;
```

**Purpose**: Transaction-like rollback for failed operations.

**SaveUndo**: Saves document state before modification

**CallUndo**: Restores document state if operation fails

**Pattern Used In**:
```cpp
try {
  ThrowOnFailure(SaveUndo());
  ThrowOnFailure(_overlay->addAlias(ovl));
  return self();
} catch (std::exception const& ex) {
  ThrowOnFailure(CallUndo());
  return self();
}
```

## Private Member Variables

```cpp
private:
  JSONDocument _document;
  ovlDatabaseRecordUPtr_t _overlay;
  bool _initOK;
```

**_document**: The JSON document being built

**_overlay**: Smart pointer to overlay providing structured access

**_initOK**: Initialization status flag

## Free Functions

### Template Function

```cpp
template <typename T>
JSONDocument toJSONDocument(T const&);
```

**Purpose**: Converts various types to `JSONDocument`.

**Use Case**: Creating documents from different data types (pairs, structs, etc.)

### Debug Function

```cpp
namespace debug {
  void JSONDocumentBuilder();
}
```

**Purpose**: Enables detailed TRACE logging for builder operations.

### Stream Output

```cpp
std::ostream& operator<<(std::ostream&, JSONDocumentBuilder const&);
```

**Purpose**: Allows streaming builder to output streams.

**Usage**:
```cpp
std::cout << builder << std::endl;
TLOG(10) << "Builder state: " << builder;
```

## Usage Patterns

### Building a New Document

```cpp
JSONDocumentBuilder builder;

// Set metadata
builder.setVersion(JSONDocument(R"({"name":"v1.0.0"})"));
builder.setCollection(JSONDocument(R"({"collection":"configurations"})"));

// Add content
builder.addEntity(JSONDocument(R"({"entity":"detector_A"})"));
builder.addConfiguration(JSONDocument(R"({"configuration":"default"})"));

// Add aliases
builder.addAlias(JSONDocument(R"({"alias":"latest"})"));

// Extract final document
auto doc = builder.extract();
```

### Importing User Data

```cpp
// User provides raw configuration
JSONDocument userData = loadUserConfig();

// Wrap in database structure
JSONDocumentBuilder builder;
builder.createFromData(userData);
builder.setVersion(version);
builder.newObjectID();

// Extract database-ready document
auto dbDoc = builder.extract();
```

### Modifying Existing Document

```cpp
JSONDocument existing = loadFromDatabase();

JSONDocumentBuilder builder(existing);

// Make modifications
builder.addConfiguration(newConfig);
builder.removeAlias(oldAlias);

// Mark as updated
builder.setVersion(newVersion);

// Extract modified document
auto updated = builder.extract();
```

### Protecting Documents

```cpp
JSONDocumentBuilder builder(productionDoc);

// Lock down production configuration
builder.markReadonly();

auto protected_doc = builder.extract();
// Future modification attempts will fail
```

## Design Patterns

### Builder Pattern

The class follows the classic builder pattern:
- Fluent interface (methods return `*this`)
- Step-by-step construction
- Final extraction via `extract()`
- Validates and structures data

### Overlay Pattern

Uses overlay classes for structured access:
- Type-safe field access
- Validation of document structure
- Abstraction over JSON details
- Consistent field naming

### Transaction Pattern

Undo mechanism provides transaction-like behavior:
- Save state before operation
- Perform operation
- On failure, restore state
- Ensures consistency

## Integration with Overlay System

The builder heavily relies on overlays:

**ovlDatabaseRecord**: Top-level overlay providing access to:
- Version
- ID
- Collection
- Configurations
- Entities
- Aliases
- Bookkeeping

**Specialized Overlays**:
- `ovlAlias`: Alias management
- `ovlConfiguration`: Configuration management
- `ovlEntity`: Entity management
- `ovlRun`: Run association
- `ovlVersion`: Version management
- `ovlCollection`: Collection assignment
- `ovlId`: Object ID management

## Error Handling

All modification methods use try-catch with undo:
```cpp
try {
  ThrowOnFailure(SaveUndo());
  ThrowOnFailure(/* operation */);
  return self();
} catch (std::exception const& ex) {
  TLOG(N) << "Operation failed: " << ex.what();
  ThrowOnFailure(CallUndo());
  return self();
}
```

This ensures:
- Exceptions are logged
- State is restored on failure
- Builder remains in consistent state

## Thread Safety

Not thread-safe:
- Maintains mutable state
- No synchronization
- Intended for single-threaded use

## Related Files

- **JSONDocumentBuilder.cpp** - Implementation
- **JSONDocument.h** - Wrapped document class
- **JSONDocumentOverlay.h** - Overlay classes
- **JSONDocumentMigrator.h** - Uses builder for migrations

## Best Practices

1. **Use builder for database documents** - Don't manually construct database documents
2. **Extract once** - Builder should not be used after `extract()`
3. **Set version early** - Required for most database operations
4. **Generate new IDs** - Call `newObjectID()` for new documents
5. **Check readonly status** - Before attempting modifications
6. **Use createFromData** - When importing user configurations
7. **Mark production docs readonly** - Protect important configurations

## Notes

- Builder is non-copyable and non-movable by design
- All operations are logged via TRACE
- Overlay is recreated after `createFromData()`
- Undo mechanism is currently a placeholder (returns Success())
- Comparison method name has a typo ("comapreUsingOverlays")
- The class enforces database document structure through overlays
