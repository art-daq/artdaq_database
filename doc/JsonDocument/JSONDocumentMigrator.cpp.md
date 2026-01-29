# JSONDocumentMigrator.cpp

**Path:** `artdaq-database/JsonDocument/JSONDocumentMigrator.cpp`

**Implements:** [JSONDocumentMigrator.h](./JSONDocumentMigrator.h.md)

**Purpose:** This file implements the `JSONDocumentMigrator` class, providing the concrete migration logic for converting legacy JSON document formats into the modern database schema. The migration extracts data from old document structures and reconstructs them using `JSONDocumentBuilder` to ensure proper metadata, versioning, and database compliance.

## Implementation Overview

The `JSONDocumentMigrator` class provides a conversion operator that transforms legacy document formats into the current schema. It uses the builder pattern via `JSONDocumentBuilder` to construct a properly structured document while preserving all data from the source.

### Migration Process

The conversion operator performs these steps:
1. Create a new `JSONDocumentBuilder`
2. Import document data using `createFromData()`
3. Extract and set version from the legacy format
4. Extract and add entity (from `configurable_entity` field)
5. Iterate and add all configurations
6. Extract and set object ID (from nested `_id._oid` path)
7. Return the extracted document

### Field Transformation

Key transformations during migration:

| Legacy Field | Modern Field | Transformation |
|--------------|--------------|----------------|
| `version` | `version.name` | Wrapped in object with "name" key |
| `configurable_entity` | `entities[]` | Object becomes array element |
| `configurations[]` | `configurations[]` | Each wrapped in configuration object |
| `_id._oid` | `_id` | Nested structure flattened to string |

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/JsonDocument/JSONDocumentMigrator.h` | Class declaration for `JSONDocumentMigrator` |
| `artdaq-database/JsonDocument/JSONDocumentBuilder.h` | Builder class used to construct the migrated document |
| `artdaq-database/JsonDocument/common.h` | Module-wide types, literals, and utility functions |

## Internal Functions

### `JSONDocumentMigrator::JSONDocumentMigrator(JSONDocument& document)`

**Brief:** Constructor that stores a reference to the source document for later migration.

**Parameters:**
- `document` - Non-const reference to the legacy JSONDocument to be migrated

**Postconditions:**
- Internal `_document` reference is initialized and points to the source document

**Thread Safety:** safe (construction only)

### `JSONDocumentMigrator::operator JSONDocument()`

**Brief:** Conversion operator that performs the complete migration from legacy format to modern format by creating a builder, importing data, and transferring all metadata fields.

**Returns:** New `JSONDocument` in modern format with proper metadata structure

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `notfound_exception` | When `version` field is missing from source document |
| `notfound_exception` | When `configurable_entity` field is missing |
| `notfound_exception` | When `configurations` field is missing |
| `notfound_exception` | When `_id._oid` path is missing |

**Thread Safety:** unsafe (modifies internal builder state)

**Migration Algorithm:**

1. **Create Builder and Import Data**
   ```cpp
   JSONDocumentBuilder builder{};
   builder.createFromData(_document);
   ```
   Creates an empty builder and imports the document data, adding the required metadata scaffolding.

2. **Extract and Set Version**
   ```cpp
   auto version = jsn::object_t{};
   version[jsonliteral::name] = _document.findChildValue(jsonliteral::version);
   builder.setVersion({version});
   ```
   Wraps the legacy version string value in an object with a "name" key as expected by `setVersion()`.

3. **Extract and Add Entity**
   ```cpp
   auto entity = _document.findChildDocument("configurable_entity");
   builder.addEntity(entity);
   ```
   The single legacy entity becomes the first element in the modern entities array.

4. **Migrate Configurations**
   ```cpp
   jsn::value_t configs = _document.findChildValue(jsonliteral::configurations);
   for (auto const& config : unwrap(configs).value_as<jsn::array_t>()) {
     builder.addConfiguration({config});
   }
   ```
   Iterates through the configurations array and adds each via the builder.

5. **Extract and Set Object ID**
   ```cpp
   jsn::value_t oid = _document.findChildValue("_id._oid");
   builder.setObjectID(db::to_id(unwrap(oid).value_as<std::string>()));
   ```
   Extracts the nested OID and converts it to the flat ID format.

6. **Extract and Return**
   ```cpp
   return builder.extract();
   ```
   Returns the fully migrated document.

### `debug::JSONDocumentMigrator()`

**Brief:** Enables maximum TRACE logging for debugging migration operations by configuring the TRACE subsystem.

**Returns:** None (void)

**Side Effects:**
- Configures TRACE logging name to "JSONDocumentMigrator.cpp"
- Sets TRACE level to maximum (0xFFFFFFFFFFFFFFFFLL)
- Configures memory and slow modes

**TRACE Level:** 20 (activation message)

## Expected Document Formats

### Legacy Document Structure (Input)

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

### Modern Document Structure (Output)

```json
{
  "version": "v1.0.0",
  "_id": "507f1f77bcf86cd799439011",
  "document": {
    "data": { /* configuration data */ },
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

## Error Handling Strategy

Migration throws `notfound_exception` for missing required fields. All fields (version, configurable_entity, configurations, _id._oid) are required:

```cpp
#include "artdaq-database/JsonDocument/JSONDocumentMigrator.h"

using namespace artdaq::database::docrecord;

void migrateDocument(JSONDocument& legacy) {
  try {
    JSONDocument migrated = JSONDocumentMigrator(legacy);
    // Use migrated document
  } catch (notfound_exception const& ex) {
    // Handle missing required field
    std::cerr << "Migration failed: " << ex.what() << std::endl;
    // Possible causes:
    // - version field missing
    // - configurable_entity field missing
    // - configurations field missing
    // - _id._oid field missing
  }
}
```

## Performance Considerations

- **Single Pass:** Migration reads source document once
- **Move Semantics:** Builder uses move semantics where possible
- **No Validation:** Source document is not validated before migration

## Testing Notes

- **Unit tests:** `test/JsonDocument/JSONDocumentMigrator_t.cc`
- **Key test cases:** Legacy format detection, field transformation, error handling for missing fields

## Maintenance Notes

### Assumptions

The implementation assumes:
1. **Version field exists** - Required in legacy format
2. **Configurable entity exists** - Single entity to migrate
3. **Configurations is array** - Must be array type, not object
4. **Object ID exists** - At nested path `_id._oid`
5. **Document data exists** - Content to import via `createFromData`

### Limitations

1. **Single Entity** - Only migrates one entity (the first/only one)
2. **No Validation** - Does not validate legacy format before migration
3. **No Rollback** - Migration is one-way; no reverse migration
4. **Hard-coded Paths** - Assumes specific legacy field names
5. **No Version Check** - Does not verify legacy document schema version

### TRACE Logging

| Level | Purpose |
|-------|---------|
| 20 | Debug activation message |

## See Also

- [JSONDocumentMigrator.h.md](./JSONDocumentMigrator.h.md) - Class declaration
- [JSONDocumentBuilder.h.md](./JSONDocumentBuilder.h.md) - Builder used for document construction
- [JSONDocumentBuilder.cpp.md](./JSONDocumentBuilder.cpp.md) - Builder implementation
- [JSONDocument.h.md](./JSONDocument.h.md) - Core document class
- [docrecord_exceptions.h.md](./docrecord_exceptions.h.md) - Exception types thrown during migration
