# JSONDocumentMigrator.cpp

## File Overview

This file implements the `JSONDocumentMigrator` class, providing the concrete migration logic for converting legacy JSON document formats into the modern database schema. The migration extracts data from an old document structure and reconstructs it using `JSONDocumentBuilder` to ensure proper metadata, versioning, and database compliance.

**Location**: `/home/user/artdaq-database/artdaq-database/JsonDocument/JSONDocumentMigrator.cpp`

## Dependencies

```cpp
#include "artdaq-database/JsonDocument/JSONDocumentMigrator.h"
#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"
#include "artdaq-database/JsonDocument/common.h"
```

**Key Dependencies**:
- **JSONDocumentMigrator.h** - Class definition
- **JSONDocumentBuilder.h** - Used to build the migrated document
- **common.h** - Module-wide types and utilities

## TRACE Configuration

```cpp
#ifdef TRACE_NAME
#undef TRACE_NAME
#endif
#define TRACE_NAME "JSONDocumentMigrator.cpp"
```

Sets TRACE subsystem name for logging migration operations.

## Using Declarations

```cpp
using artdaq::database::result_t;
using artdaq::database::json::array_t;
using artdaq::database::json::object_t;
using artdaq::database::json::value_t;
using artdaq::database::overlay::ovlDatabaseRecordUPtr_t;
using artdaq::database::docrecord::JSONDocument;
using artdaq::database::docrecord::JSONDocumentBuilder;
using artdaq::database::docrecord::JSONDocumentMigrator;

namespace db = artdaq::database;
namespace dbdr = artdaq::database::docrecord;
namespace jsonliteral = artdaq::database::dataformats::literal;
```

## Constructor

### JSONDocumentMigrator Constructor

```cpp
JSONDocumentMigrator::JSONDocumentMigrator(JSONDocument& document)
    : _document(document) {}
```

**Simple Initialization**: Stores reference to the document to be migrated.

**Parameter**: Non-const reference to document (allows reading during migration)

## Conversion Operator Implementation

### operator JSONDocument()

```cpp
JSONDocumentMigrator::operator JSONDocument()
```

**Purpose**: Performs the actual migration from legacy format to modern format.

**Implementation**:
```cpp
JSONDocumentMigrator::operator JSONDocument() {
  JSONDocumentBuilder builder{};

  builder.createFromData(_document);

  auto version = jsn::object_t{};
  version[jsonliteral::name] = _document.findChildValue(jsonliteral::version);
  builder.setVersion({version});

  auto entity = _document.findChildDocument("configurable_entity");
  builder.addEntity(entity);

  jsn::value_t configs = _document.findChildValue(jsonliteral::configurations);

  for (auto const& config : unwrap(configs).value_as<jsn::array_t>()) {
    builder.addConfiguration({config});
  }

  jsn::value_t oid = _document.findChildValue("_id._oid");
  builder.setObjectID(db::to_id(unwrap(oid).value_as<std::string>()));

  return builder.extract();
}
```

## Migration Steps

The migration process follows these steps:

### Step 1: Create Builder

```cpp
JSONDocumentBuilder builder{};
```

Creates a new builder starting with an empty document template.

### Step 2: Import User Data

```cpp
builder.createFromData(_document);
```

**Purpose**: Imports the document data into the builder's document structure.

**Effect**: Copies user data fields into proper document structure with metadata scaffolding.

### Step 3: Extract and Set Version

```cpp
auto version = jsn::object_t{};
version[jsonliteral::name] = _document.findChildValue(jsonliteral::version);
builder.setVersion({version});
```

**Process**:
1. Create object for version
2. Extract version value from legacy document
3. Wrap in object with "name" key
4. Set version via builder

**Legacy Format**: `{"version": "v1.0.0"}`

**Expected Builder Input**: `{"name": "v1.0.0"}`

### Step 4: Extract and Add Entity

```cpp
auto entity = _document.findChildDocument("configurable_entity");
builder.addEntity(entity);
```

**Purpose**: Migrates the legacy "configurable_entity" field to modern "entities" array.

**Legacy Field**: `"configurable_entity"`

**Modern Field**: `"entities"` (array)

**Transformation**: Single entity becomes first element in entities array.

### Step 5: Migrate Configurations

```cpp
jsn::value_t configs = _document.findChildValue(jsonliteral::configurations);

for (auto const& config : unwrap(configs).value_as<jsn::array_t>()) {
  builder.addConfiguration({config});
}
```

**Process**:
1. Extract configurations array from legacy document
2. Unwrap value to get actual array
3. Iterate through each configuration
4. Add each to builder (wraps in proper structure)

**Legacy Format**: Configurations may have different structure

**Modern Format**: Each configuration wrapped with proper metadata

### Step 6: Extract and Set Object ID

```cpp
jsn::value_t oid = _document.findChildValue("_id._oid");
builder.setObjectID(db::to_id(unwrap(oid).value_as<std::string>()));
```

**Process**:
1. Extract OID from nested path `"_id._oid"`
2. Unwrap value to get string
3. Convert string to ID format using `db::to_id()`
4. Set as object ID via builder

**Legacy Format**: `{"_id": {"_oid": "507f1f77bcf86cd799439011"}}`

**Modern Format**: `{"_id": "507f1f77bcf86cd799439011"}`

### Step 7: Extract and Return

```cpp
return builder.extract();
```

**Final Step**: Extracts the fully migrated document from the builder.

**Returns**: Modern format document with all fields properly structured.

## Legacy Document Structure

Based on the implementation, the expected legacy format is:

```json
{
  "version": "v1.0.0",
  "configurable_entity": {
    "name": "detector_A",
    "type": "some_type"
  },
  "configurations": [
    {"name": "config1", "value": "val1"},
    {"name": "config2", "value": "val2"}
  ],
  "_id": {
    "_oid": "507f1f77bcf86cd799439011"
  },
  "document": {
    "data": { /* actual configuration data */ }
  }
}
```

## Modern Document Structure

After migration, the document has this structure:

```json
{
  "version": "v1.0.0",
  "_id": "507f1f77bcf86cd799439011",
  "document": {
    "data": { /* actual configuration data */ },
    "metadata": { }
  },
  "entities": [
    {
      "entity": {
        "name": "detector_A",
        "type": "some_type"
      }
    }
  ],
  "configurations": [
    {"configuration": {"name": "config1", "value": "val1"}},
    {"configuration": {"name": "config2", "value": "val2"}}
  ],
  "bookkeeping": {
    "isreadonly": false,
    "isdeleted": false,
    "updates": []
  }
}
```

## Key Transformations

### 1. Version Field
- **From**: Direct string value
- **To**: Wrapped in structure with proper path

### 2. Configurable Entity
- **From**: Single object `"configurable_entity"`
- **To**: Array of entities `"entities": [...]`

### 3. Configurations
- **From**: Array of simple configuration objects
- **To**: Array of wrapped configuration objects with metadata

### 4. Object ID
- **From**: Nested structure `{"_id": {"_oid": "..."}}`
- **To**: Flat string `"_id": "..."`

### 5. Document Structure
- **Added**: Bookkeeping metadata
- **Added**: Proper document.metadata structure
- **Preserved**: document.data content

## Error Handling

The migration can throw exceptions:

**notfound_exception**: If required fields missing:
- `version` field not found
- `configurable_entity` not found
- `configurations` not found
- `_id._oid` not found

**invalid_argument**: If data format is wrong:
- `configurations` is not an array
- `_id._oid` is not a string
- Document structure invalid

**Example Error Handling**:
```cpp
try {
  auto migrated = JSONDocumentMigrator(legacy);
  migrated.saveToFile("output.json");
} catch (notfound_exception const& ex) {
  std::cerr << "Missing required field: " << ex.what() << std::endl;
  // Legacy document is incomplete or wrong format
} catch (std::exception const& ex) {
  std::cerr << "Migration failed: " << ex.what() << std::endl;
}
```

## Debug Function

### debug::JSONDocumentMigrator

```cpp
void dbdr::debug::JSONDocumentMigrator() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TLOG(20) << "artdaq::database::JSONDocumentMigrator trace_enable";
}
```

**Purpose**: Enables maximum TRACE logging for debugging migrations.

**TRACE Level**: 20 - Logs activation message

**Usage**:
```cpp
artdaq::database::docrecord::debug::JSONDocumentMigrator();
// Now all migration trace messages will output
```

## Usage Examples

### Basic Migration

```cpp
JSONDocument legacy = JSONDocument::loadFromFile("old_format.json");
JSONDocument modern = JSONDocumentMigrator(legacy);
modern.saveToFile("new_format.json");
```

### With Error Handling

```cpp
try {
  JSONDocument legacy("{\"version\":\"v1\", ...}");
  JSONDocumentMigrator migrator(legacy);
  JSONDocument modern = migrator;

  std::cout << "Migration successful" << std::endl;
  std::cout << modern.to_string() << std::endl;

} catch (notfound_exception const& ex) {
  std::cerr << "Legacy document missing required field: "
            << ex.what() << std::endl;
}
```

### Batch Migration

```cpp
for (auto const& filename : legacy_files) {
  try {
    auto legacy = JSONDocument::loadFromFile(filename);
    auto modern = JSONDocumentMigrator(legacy);
    modern.saveToFile(filename + ".migrated");
    std::cout << "Migrated: " << filename << std::endl;
  } catch (std::exception const& ex) {
    std::cerr << "Failed to migrate " << filename
              << ": " << ex.what() << std::endl;
  }
}
```

## Performance Considerations

1. **Builder Overhead**: Creates builder and overlay infrastructure
2. **Field Extraction**: Multiple calls to `findChildValue` and `findChildDocument`
3. **Array Iteration**: Loops through configurations
4. **String Conversions**: OID conversion from string
5. **Document Copy**: `createFromData` copies document content

**Optimization Note**: Migration is typically a one-time operation, so optimization is less critical than correctness.

## Assumptions

The implementation assumes:

1. **Version field exists**: Required field in legacy format
2. **Configurable entity exists**: Single entity to migrate
3. **Configurations is array**: Must be array type
4. **Object ID exists**: At path `_id._oid`
5. **Document data exists**: Content to import via `createFromData`

## Limitations

1. **Single Entity**: Only migrates one entity (first/only one)
2. **No Validation**: Doesn't validate legacy format before migration
3. **No Rollback**: Migration is one-way
4. **Hard-coded Paths**: Assumes specific legacy field names
5. **No Version Check**: Doesn't verify legacy document version

## Integration with Builder

The migrator is essentially a thin wrapper that:
1. Extracts fields from legacy format
2. Uses `JSONDocumentBuilder` to construct modern format
3. Returns the result

**Builder Benefits**:
- Ensures proper document structure
- Adds metadata automatically
- Validates field types
- Maintains bookkeeping

## Thread Safety

Not thread-safe:
- Relies on `JSONDocumentBuilder` which is not thread-safe
- Reads from shared document reference
- No synchronization

## Related Files

- **JSONDocumentMigrator.h** - Class definition
- **JSONDocumentBuilder.cpp** - Used for building migrated document
- **JSONDocument.cpp** - Used for field extraction

## Best Practices

1. **Validate First**: Check legacy document has required fields before migrating
2. **Backup Originals**: Keep backup of legacy documents
3. **Test Sample**: Test migration on sample document before batch
4. **Enable Logging**: Use debug function to trace migration steps
5. **Handle Exceptions**: Catch specific exceptions for better error messages

## Migration Workflow

```
Legacy Document
    ↓
JSONDocumentMigrator Constructor (stores reference)
    ↓
Conversion Operator Called
    ↓
Create JSONDocumentBuilder
    ↓
Extract Fields from Legacy:
  - version
  - configurable_entity
  - configurations
  - _id._oid
  - document data
    ↓
Build Modern Structure:
  - Set version
  - Add entity
  - Add configurations
  - Set object ID
  - Import data
    ↓
Extract from Builder
    ↓
Return Modern Document
```

## Notes

- Migration is destructive to builder but not to source document
- Uses builder's `createFromData` to import document content
- Configurations are migrated individually through a loop
- Object ID undergoes format conversion via `db::to_id()`
- Entity changes from singular to plural (entity → entities array)
- The implementation is specific to one legacy format version
- No reverse migration capability (modern → legacy)
