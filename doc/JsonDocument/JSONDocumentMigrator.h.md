# JSONDocumentMigrator.h

**Path:** `artdaq-database/JsonDocument/JSONDocumentMigrator.h`

**Purpose:** This header file defines the `JSONDocumentMigrator` class, which provides functionality for migrating JSON documents from legacy formats to the current database schema. It converts older document structures into the modern format with proper metadata, versioning, and database fields using an implicit conversion operator pattern.


## Key Concepts

### Conversion Operator Pattern
The migrator uses a conversion operator to perform migration:
```cpp
JSONDocument legacy = loadOldFormat();
JSONDocumentMigrator migrator(legacy);
JSONDocument modern = migrator;  // Implicit conversion performs migration
```

This pattern makes migration feel natural and self-documenting.

### Single-Use Design
The class is designed for immediate, single-use migration:
- Takes a reference to the source document
- Non-copyable and non-movable
- No default constructor
- Conversion operator produces the result

### Reference-Based Architecture
The migrator holds a reference (not a copy) to the source document:
- Avoids copying potentially large documents
- Allows reading from original multiple times during migration
- **Important:** Source document must outlive the migrator

## Thread Safety

- **Thread-safe:** No
- **Concurrent access:** Not safe for concurrent read/write operations
- **Locking:** No internal locks; external synchronization required for multi-threaded use

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/JsonDocument/JSONDocument.h` | Core document class for input and output |
| `artdaq-database/JsonDocument/common.h` | Module-wide types and utilities |

## Classes/Structures

### `JSONDocumentMigrator`

A final, non-copyable class that converts legacy documents to modern format using an implicit conversion operator. Uses `JSONDocumentBuilder` internally to construct the output document.

**Thread Safety:** Not thread-safe. Holds reference to external document.

#### Constructor

##### `JSONDocumentMigrator(JSONDocument& document)`

**Brief:** Constructor that initializes the migrator with a non-const reference to the document to be migrated.

**Parameters:**
- `document` - Non-const reference to the legacy JSONDocument

**Preconditions:**
- Document must contain valid legacy format JSON with required fields
- Document must outlive the migrator instance

**Postconditions:**
- Migrator holds reference to source document
- Ready for conversion via operator

**Throws:** None

**Thread Safety:** safe (construction)

**Example:**
```cpp
#include "artdaq-database/JsonDocument/JSONDocumentMigrator.h"

using namespace artdaq::database::docrecord;

void example() {
  JSONDocument legacy = JSONDocument::loadFromFile("old_format.json");
  JSONDocumentMigrator migrator(legacy);
}
```

#### Conversion Operator

##### `operator JSONDocument()`

**Brief:** Conversion operator that performs the actual migration from legacy format to modern format by rebuilding the document using JSONDocumentBuilder.

**Parameters:** None

**Preconditions:**
- Source document must be valid
- Required fields must exist in legacy document:
  - `version` - Version string
  - `configurable_entity` - Entity object with name/type
  - `configurations` - Array of configuration objects
  - `_id._oid` - MongoDB ObjectId string

**Returns:** New JSONDocument in modern format with proper metadata structure

**Postconditions:**
- New document has proper metadata structure
- Original document is unchanged (read-only access)

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `notfound_exception` | When required field is missing (version, configurable_entity, configurations, _id._oid) |
| `invalid_argument` | When data format is invalid or cannot be parsed |

**Thread Safety:** unsafe

**Example:**
```cpp
// Explicit conversion via assignment
JSONDocument modern = migrator;

// One-liner migration (creates temporary migrator)
auto modern = static_cast<JSONDocument>(JSONDocumentMigrator(legacyDoc));
```

#### Destructor

##### `~JSONDocumentMigrator() = default`

**Brief:** Default destructor that releases the reference to the source document.

**Thread Safety:** safe

#### Deleted Special Members

The class explicitly deletes copy and move operations to enforce single-use semantics:

##### `JSONDocumentMigrator() = delete`

**Brief:** Default constructor is deleted to require a source document.

##### `JSONDocumentMigrator(JSONDocumentMigrator const&) = delete`

**Brief:** Copy constructor is deleted to prevent copying migrator instances.

##### `JSONDocumentMigrator& operator=(JSONDocumentMigrator const&) = delete`

**Brief:** Copy assignment is deleted to prevent copying migrator instances.

##### `JSONDocumentMigrator(JSONDocumentMigrator&&) = delete`

**Brief:** Move constructor is deleted to prevent moving migrator instances.

##### `JSONDocumentMigrator& operator=(JSONDocumentMigrator&&) = delete`

**Brief:** Move assignment is deleted to prevent moving migrator instances.

## Functions

### `debug::JSONDocumentMigrator()`

**Brief:** Enables detailed TRACE logging for migration operations to aid debugging at maximum verbosity.

**Parameters:** None

**Returns:** None

**Side Effects:**
- Configures TRACE logging for migration debugging

**Thread Safety:** safe

**Example:**
```cpp
artdaq::database::docrecord::debug::JSONDocumentMigrator();
// Migration operations now produce detailed trace output
```

## Migration Process

The conversion operator performs these steps:
1. Create a new `JSONDocumentBuilder`
2. Call `createFromData()` with the legacy document
3. Extract and set version from `version` field
4. Extract and add entity from `configurable_entity` field
5. Iterate `configurations` array and add each configuration
6. Extract ObjectId from `_id._oid` and set via `setObjectID()`
7. Return the built document via `extract()`

## Legacy Document Format

The migration expects this legacy structure:
```json
{
  "version": "v1.0.0",
  "configurable_entity": {
    "name": "detector_A",
    "type": "some_type"
  },
  "configurations": [
    {"name": "config1"},
    {"name": "config2"}
  ],
  "_id": {
    "_oid": "507f1f77bcf86cd799439011"
  },
  // ... other user data
}
```

## Modern Document Format

After migration:
```json
{
  "version": "v1.0.0",
  "_id": "507f1f77bcf86cd799439011",
  "document": {
    "data": { /* user data */ },
    "metadata": { }
  },
  "entities": [
    {"entity": {"name": "detector_A", "type": "some_type"}}
  ],
  "configurations": [
    {"configuration": {"name": "config1"}},
    {"configuration": {"name": "config2"}}
  ],
  "bookkeeping": {
    "isreadonly": false,
    "isdeleted": false
  }
}
```

## Relationship to Other Components

### Within the JsonDocument Module
- **JSONDocument** - Input (legacy) and output (modern) document type
- **JSONDocumentBuilder** - Used internally to construct the modern format document with proper structure
- **common.h** - Provides shared utilities and types

### Dependencies
- **JSONDocumentBuilder** - Used to construct properly structured output documents with metadata, versioning, and bookkeeping
- **Overlay types** - Indirectly used via builder for structured access

### Workflow Integration
The migrator fits into document lifecycle:
1. Load legacy document from storage
2. Create migrator with document reference
3. Convert to modern format via assignment
4. Optionally use builder for additional modifications
5. Store in database

## See Also

- [JSONDocumentMigrator.cpp.md](./JSONDocumentMigrator.cpp.md) - Implementation details
- [JSONDocumentBuilder.h.md](./JSONDocumentBuilder.h.md) - Builder used for document construction
- [JSONDocument.h.md](./JSONDocument.h.md) - Core document class
- [common.h.md](./common.h.md) - Common includes and types

## Notes for Developers

### Lifetime Management
The source document must outlive the migrator since the migrator holds a reference:
```cpp
// CORRECT - document outlives migrator
JSONDocument legacy = loadDoc();
JSONDocument modern = JSONDocumentMigrator(legacy);

// DANGEROUS - don't do this with temporaries
// The temporary JSONDocument is destroyed before conversion!
// auto modern = JSONDocumentMigrator(loadDoc());  // Undefined behavior!
```

### Common Pitfalls

- **Dangling reference:** Creating migrator from temporary document results in undefined behavior. Always store the source document in a variable first.
- **Missing fields:** Migration throws `notfound_exception` if required legacy fields are missing. Validate documents before migration.
- **Reusing migrator:** Migrator is designed for single use; create new instance for each document to migrate.
- **Reference semantics:** The migrator does not copy the source document. Any modifications to the source before conversion will affect the result.

### Usage Examples

#### Basic Migration
```cpp
#include "artdaq-database/JsonDocument/JSONDocumentMigrator.h"

using namespace artdaq::database::docrecord;

void migrateFile() {
  try {
    JSONDocument legacy = JSONDocument::loadFromFile("old_config.json");
    JSONDocument modern = JSONDocumentMigrator(legacy);
    modern.saveToFile("new_config.json");
  } catch (const notfound_exception& e) {
    std::cerr << "Missing required field: " << e.what() << std::endl;
  } catch (const std::exception& e) {
    std::cerr << "Migration failed: " << e.what() << std::endl;
  }
}
```

#### Batch Migration
```cpp
#include "artdaq-database/JsonDocument/JSONDocumentMigrator.h"

using namespace artdaq::database::docrecord;

void migrateBatch(const std::vector<std::string>& legacy_files) {
  for (auto const& filename : legacy_files) {
    try {
      auto legacy = JSONDocument::loadFromFile(filename);
      auto modern = JSONDocumentMigrator(legacy);
      modern.saveToFile(filename + ".migrated");
      std::cout << "Migrated: " << filename << std::endl;
    } catch (std::exception const& ex) {
      std::cerr << "Failed: " << filename << " - " << ex.what() << std::endl;
    }
  }
}
```

#### Migration with Additional Modifications
```cpp
#include "artdaq-database/JsonDocument/JSONDocumentMigrator.h"
#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"

using namespace artdaq::database::docrecord;

void migrateAndEnhance() {
  JSONDocument legacy = JSONDocument::loadFromFile("old.json");
  JSONDocument modern = JSONDocumentMigrator(legacy);

  // Add more metadata via builder
  JSONDocumentBuilder builder(modern);
  builder.addAlias(JSONDocument(R"({"alias":"migrated"})"));
  builder.setCollection(JSONDocument(R"({"collection":"migrated_configs"})"));
  auto enhanced = builder.extract();

  enhanced.saveToFile("enhanced.json");
}
```
