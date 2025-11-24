# JSONDocumentMigrator.h

## File Overview

This header file defines the `JSONDocumentMigrator` class, which provides functionality for migrating JSON documents from older formats to the current database schema. It converts legacy document structures into the modern format with proper metadata, versioning, and database fields.

**Location**: `/home/user/artdaq-database/artdaq-database/JsonDocument/JSONDocumentMigrator.h`

## Dependencies

```cpp
#include "artdaq-database/JsonDocument/JSONDocument.h"
#include "artdaq-database/JsonDocument/common.h"
```

**Key Dependencies**:
- **JSONDocument.h** - Core document class
- **common.h** - Module-wide types and utilities

## Namespace Structure

```cpp
namespace artdaq {
namespace database {
namespace docrecord {
  // JSONDocumentMigrator class
}
}
}
```

## JSONDocumentMigrator Class

### Class Declaration

```cpp
class JSONDocumentMigrator final {
public:
  JSONDocumentMigrator(JSONDocument&);
  operator JSONDocument();

  // Defaults and deleted members...
private:
  JSONDocument& _document;
};
```

**Key Characteristics**:
- **final** - Cannot be inherited from
- **Single-purpose** - Designed for one-time migration operations
- **Reference-based** - Holds reference to document being migrated

### Constructor

```cpp
JSONDocumentMigrator(JSONDocument&);
```

**Purpose**: Initializes migrator with a reference to a document to be migrated.

**Parameter**: Non-const reference to `JSONDocument` (document to migrate)

**Design Choice**: Takes reference (not copy) because migration may involve reading from the original document multiple times.

### Conversion Operator

```cpp
operator JSONDocument();
```

**Purpose**: Performs the migration and returns the migrated document.

**Return Type**: `JSONDocument` (new document in current format)

**Usage Pattern**:
```cpp
JSONDocument legacy = loadOldFormat();
JSONDocumentMigrator migrator(legacy);
JSONDocument modernDoc = migrator;  // Implicit conversion performs migration
```

**Alternative Usage**:
```cpp
JSONDocument modernDoc = JSONDocumentMigrator(legacyDoc);
```

### Special Member Functions

```cpp
// Defaults
~JSONDocumentMigrator() = default;

// Deleted
JSONDocumentMigrator() = delete;
JSONDocumentMigrator(JSONDocumentMigrator const&) = delete;
JSONDocumentMigrator& operator=(JSONDocumentMigrator const&) = delete;
JSONDocumentMigrator& operator=(JSONDocumentMigrator&&) = delete;
JSONDocumentMigrator(JSONDocumentMigrator&&) = delete;
```

**Design Constraints**:
- **No default constructor** - Must be constructed with a document
- **Non-copyable** - Cannot copy migrator instances
- **Non-movable** - Cannot move migrator instances
- **Single-use** - Intended for immediate use and conversion

**Rationale**:
- Holds reference to external document, copying/moving would be problematic
- Designed for immediate use via conversion operator
- Prevents accidental misuse or state management issues

### Private Member

```cpp
private:
  JSONDocument& _document;
```

**_document**: Reference to the document being migrated.

**Why Reference?**
- Avoids copying potentially large document
- Allows reading from original format multiple times during migration
- Clear ownership semantics (migrator doesn't own document)

## Free Functions

### Debug Function

```cpp
namespace debug {
  void JSONDocumentMigrator();
}
```

**Purpose**: Enables detailed TRACE logging for migration operations.

**Usage**:
```cpp
artdaq::database::docrecord::debug::JSONDocumentMigrator();
// Now migration operations will produce detailed trace output
```

## Usage Patterns

### Basic Migration

```cpp
// Load document in old format
JSONDocument oldFormat = JSONDocument::loadFromFile("old_config.json");

// Create migrator
JSONDocumentMigrator migrator(oldFormat);

// Convert to new format
JSONDocument newFormat = migrator;

// Save in new format
newFormat.saveToFile("new_config.json");
```

### Inline Migration

```cpp
// One-liner migration
auto modernDoc = JSONDocumentMigrator(legacyDoc);
```

### Batch Migration

```cpp
std::vector<JSONDocument> legacyDocs = loadLegacyDocuments();
std::vector<JSONDocument> modernDocs;

for (auto& doc : legacyDocs) {
  modernDocs.push_back(JSONDocumentMigrator(doc));
}
```

## Design Pattern

### Conversion Operator Pattern

The class uses the conversion operator pattern:

**Benefits**:
- Natural syntax (`JSONDocument modern = migrator;`)
- Implicit conversion when needed
- Clear intent (constructor takes old, conversion returns new)
- One-time use enforced by deleted copy/move

**Pattern Structure**:
```
Input (old format) → Constructor → Migrator → Conversion → Output (new format)
```

### Reference-Based Design

Holding a reference instead of a copy:

**Advantages**:
- No copying overhead
- Can read from original multiple times
- Clear ownership (migrator doesn't own document)

**Disadvantages**:
- Source document must outlive migrator
- Cannot be copied or moved safely
- Must be used immediately

## Migration Process

While the header doesn't show implementation details, the typical migration process involves:

1. **Extract Legacy Fields**: Read fields from old document structure
2. **Use Builder**: Create new document using `JSONDocumentBuilder`
3. **Transfer Data**: Move data to new structure with proper paths
4. **Add Metadata**: Add version, ID, and bookkeeping information
5. **Return Result**: Return the properly structured modern document

## When to Use JSONDocumentMigrator

Use this class when:

1. **Format Changes**: Document schema has changed between versions
2. **Database Upgrades**: Migrating from old database structure
3. **Legacy Support**: Need to read old configuration files
4. **Field Reorganization**: Fields moved to different paths
5. **Metadata Addition**: Old documents lack required metadata

## Related Classes

**JSONDocumentBuilder**: Likely used internally for constructing the new format document:
```cpp
operator JSONDocument() {
  JSONDocumentBuilder builder;
  // Use builder to construct new format
  // Transfer data from _document
  return builder.extract();
}
```

## Error Handling

The class doesn't declare any explicit exception specifications, so migration can throw:
- `notfound_exception` - If expected fields missing in old format
- `invalid_argument` - If old format is malformed
- Other exceptions from builder operations

**Best Practice**: Wrap migration in try-catch:
```cpp
try {
  auto modernDoc = JSONDocumentMigrator(legacyDoc);
  modernDoc.saveToFile("migrated.json");
} catch (notfound_exception const& ex) {
  std::cerr << "Missing required field: " << ex.what() << std::endl;
} catch (std::exception const& ex) {
  std::cerr << "Migration failed: " << ex.what() << std::endl;
}
```

## Thread Safety

Not thread-safe:
- Holds reference to external document
- Migration process likely modifies builder state
- No synchronization mechanisms
- Intended for single-threaded use

## Typical Legacy Format vs. Modern Format

### Legacy Format Example

```json
{
  "version": "v1.0.0",
  "configurable_entity": {
    "name": "detector_A"
  },
  "configurations": [
    {"name": "config1"},
    {"name": "config2"}
  ],
  "_id": {
    "_oid": "507f1f77bcf86cd799439011"
  }
}
```

### Modern Format (After Migration)

```json
{
  "version": "v1.0.0",
  "document": {
    "data": { /* user data */ },
    "metadata": { /* metadata */ }
  },
  "entities": [
    {"entity": {"name": "detector_A"}}
  ],
  "configurations": [
    {"configuration": {"name": "config1"}},
    {"configuration": {"name": "config2"}}
  ],
  "_id": "507f1f77bcf86cd799439011",
  "collection": "configurations",
  "bookkeeping": {
    "isreadonly": false,
    "isdeleted": false
  }
}
```

## Best Practices

1. **Validate Before Migration**: Check that source document has required fields
2. **Test Migration**: Test with sample documents before batch migration
3. **Backup Original**: Keep original documents before migration
4. **Log Migration**: Enable debug traces for troubleshooting
5. **Version Check**: Verify source document version before migrating
6. **Immediate Use**: Use migrator immediately, don't store instances

## Performance Considerations

1. **Single-Use Object**: Designed for one-time use, not reusable
2. **Reference Not Copy**: Avoids copying source document
3. **Builder Overhead**: Uses builder which creates overlays
4. **Field Extraction**: May need to traverse source document multiple times

## Limitations

1. **Single Document**: Migrates one document at a time
2. **No State**: Cannot track progress across multiple migrations
3. **No Rollback**: Migration is one-way, no undo mechanism
4. **Format Specific**: Designed for specific legacy format

## Related Files

- **JSONDocumentMigrator.cpp** - Implementation of migration logic
- **JSONDocumentBuilder.h** - Used for building migrated documents
- **JSONDocument.h** - Input and output document type

## Future Enhancements

Possible improvements to migration system:

1. **Version Detection**: Automatically detect source format version
2. **Multiple Formats**: Support migration from multiple legacy formats
3. **Validation**: Validate migrated document before returning
4. **Progress Callbacks**: Support for batch migration progress
5. **Dry Run**: Option to validate without actually migrating

## Notes

- The class is very lightweight (only one member variable)
- Conversion operator makes migration feel natural
- Deleted copy/move constructors prevent misuse
- Reference-based design requires careful lifetime management
- Migration is one-way (no reverse migration support)
- Designed for use with `JSONDocumentBuilder` for constructing output
