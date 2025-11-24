# JsonDocument Module

## Module Overview

The JsonDocument module provides a comprehensive framework for working with JSON documents in the artdaq-database system. It offers a high-level, type-safe API for creating, manipulating, querying, and migrating JSON document structures with proper metadata, versioning, and database compliance.

**Module Location**: `/home/user/artdaq-database/artdaq-database/JsonDocument/`

## Module Purpose

The JsonDocument module serves as the core abstraction layer for JSON document operations within the artdaq database system. It provides:

1. **Document Manipulation** - Path-based navigation and modification of JSON structures
2. **Builder Pattern** - Structured creation of database-compliant documents
3. **Migration Support** - Converting legacy formats to modern schema
4. **Type Safety** - Strong typing and validation throughout the API
5. **Metadata Management** - Automatic handling of versioning, IDs, and bookkeeping

## Key Components

### Core Classes

#### JSONDocument
**Files**: `JSONDocument.h`, `JSONDocument.cpp`, `JSONDocument_utils.cpp`

The fundamental document abstraction that wraps JSON data in a convenient, path-based API.

**Features**:
- Dot-notation path navigation (e.g., `"document.data.field"`)
- Tree manipulation operations (insert, replace, delete, find)
- Array operations (append, remove with matching)
- File I/O with backup support
- JSON serialization with caching
- Type conversion utilities

**Example**:
```cpp
JSONDocument doc(R"({"server": {"port": 8080}})");
int port = doc.value_as<int>("server.port");
doc.replaceChild(JSONDocument(R"({"port": 9090})"), "server.port");
doc.saveToFile("config.json");
```

#### JSONDocumentBuilder
**Files**: `JSONDocumentBuilder.h`, `JSONDocumentBuilder.cpp`

A builder pattern implementation for creating properly structured database documents with metadata, versioning, and bookkeeping.

**Features**:
- Fluent interface for document construction
- Automatic metadata scaffolding
- Version management
- Alias support
- Configuration and entity management
- Read-only and deletion marking
- Tag extraction for indexing
- Integration with overlay system

**Example**:
```cpp
JSONDocumentBuilder builder;
builder.createFromData(userData)
       .setVersion(JSONDocument(R"({"name":"v1.0.0"})"))
       .addEntity(JSONDocument(R"({"entity":"detector_A"})"))
       .addAlias(JSONDocument(R"({"alias":"latest"})"))
       .newObjectID();

auto dbDocument = builder.extract();
```

#### JSONDocumentMigrator
**Files**: `JSONDocumentMigrator.h`, `JSONDocumentMigrator.cpp`

Provides migration from legacy document formats to the current database schema.

**Features**:
- Single-use conversion pattern
- Automatic field reorganization
- Version extraction and conversion
- Entity format transformation
- Object ID format conversion
- Uses builder for proper structure

**Example**:
```cpp
JSONDocument legacy = JSONDocument::loadFromFile("old_format.json");
JSONDocument modern = JSONDocumentMigrator(legacy);
modern.saveToFile("new_format.json");
```

### Support Files

#### common.h
Aggregator header providing:
- JSON data types
- Module exceptions
- String literals
- Shared utilities

#### docrecord_exceptions.h
Custom exception types:
- `notfound_exception` - Missing elements or paths
- `readonly_exception` - Write-protection violations

#### docrecord_literals.h
String constants for:
- Action names (addAlias, markDeleted, etc.)
- Document templates
- Field path constants (commented out)

## Document Structure

### User Document Format

Simple user-provided data:
```json
{
  "name": "my_config",
  "value": 42,
  "settings": {
    "enabled": true
  }
}
```

### Database Document Format

After wrapping with JSONDocumentBuilder:
```json
{
  "version": "v1.0.0",
  "_id": "507f1f77bcf86cd799439011",
  "collection": "configurations",
  "document": {
    "data": {
      "name": "my_config",
      "value": 42,
      "settings": {"enabled": true}
    },
    "metadata": {
      "comments": "Optional user comments"
    }
  },
  "entities": [
    {"entity": {"name": "detector_A"}}
  ],
  "configurations": [
    {"configuration": {"name": "default"}}
  ],
  "aliases": {
    "active": ["latest", "production"]
  },
  "bookkeeping": {
    "isreadonly": false,
    "isdeleted": false,
    "updates": []
  }
}
```

## Path-Based Navigation

The module uses dot-notation for accessing nested fields:

```cpp
// Navigation
auto data = doc.findChild("document.data");
auto setting = doc.findChild("document.data.settings.enabled");

// Modification
doc.insertChild(newValue, "document.data.newfield");
doc.replaceChild(updatedValue, "document.data.settings");
doc.deleteChild("document.data.obsolete");

// Type conversion
std::string name = doc.value_as<std::string>("document.data.name");
int value = doc.value_as<int>("document.data.value");
bool enabled = doc.value_as<bool>("document.data.settings.enabled");
```

## Typical Workflows

### Creating a New Database Document

```cpp
// 1. User provides configuration data
JSONDocument userData(R"({
  "detector": "ICARUS",
  "channels": 1024,
  "settings": {"gain": 2.5}
})");

// 2. Create builder and wrap data
JSONDocumentBuilder builder;
builder.createFromData(userData);

// 3. Set metadata
builder.setVersion(JSONDocument(R"({"name":"v1.0.0"})"));
builder.setCollection(JSONDocument(R"({"collection":"detector_configs"})"));
builder.newObjectID();

// 4. Add organizational metadata
builder.addEntity(JSONDocument(R"({"entity":"ICARUS_TPC"})"));
builder.addConfiguration(JSONDocument(R"({"configuration":"nominal"})"));
builder.addAlias(JSONDocument(R"({"alias":"latest"})"));

// 5. Extract final document
auto dbDoc = builder.extract();

// 6. Save to database or file
dbDoc.saveToFile("icarus_config.json");
```

### Loading and Modifying Existing Document

```cpp
// 1. Load from file
auto doc = JSONDocument::loadFromFile("config.json");

// 2. Modify specific fields
auto newSetting = JSONDocument(R"({"gain": 3.0})");
doc.replaceChild(newSetting, "document.data.settings.gain");

// 3. Add new field
auto newField = JSONDocument(R"({"timestamp": "2024-01-15"})");
doc.insertChild(newField, "document.data.timestamp");

// 4. Save back with backup
doc.saveToFile("config.json");  // Creates config.json.bak
```

### Migrating Legacy Documents

```cpp
// 1. Load legacy format
auto legacy = JSONDocument::loadFromFile("legacy_config.json");

// 2. Migrate to modern format
auto modern = JSONDocumentMigrator(legacy);

// 3. Optionally add more metadata
JSONDocumentBuilder builder(modern);
builder.addAlias(JSONDocument(R"({"alias":"migrated"})"));
auto enhanced = builder.extract();

// 4. Save modern format
enhanced.saveToFile("modern_config.json");
```

### Searching and Querying

```cpp
// Find child elements
try {
  auto entity = doc.findChildDocument("entities");
  // Use entity...
} catch (notfound_exception const&) {
  // Handle missing entity
}

// Extract values with type conversion
std::string version = doc.value_as<std::string>("version");
bool isReadonly = doc.value_as<bool>("bookkeeping.isreadonly");

// Check for existence
if (doc.findChild("optional.field") != emptyDoc) {
  // Field exists
}
```

### Working with Arrays

```cpp
// Append to array
JSONDocument item(R"({"name":"item1"})");
doc.appendChild(item, "items");

// Remove matching items
JSONDocument pattern(R"({"name":"old_item"})");
auto removed = doc.removeChild(pattern, "items");  // Partial match supported
```

## Exception Handling

### Common Exceptions

```cpp
try {
  auto doc = JSONDocument::loadFromFile("config.json");
  auto value = doc.findChild("document.data.field");

} catch (notfound_exception const& ex) {
  // Path or element doesn't exist
  std::cerr << "Element not found: " << ex.what() << std::endl;

} catch (readonly_exception const& ex) {
  // Attempted to modify read-only document
  std::cerr << "Cannot modify: " << ex.what() << std::endl;

} catch (invalid_argument const& ex) {
  // Invalid path, empty document, or malformed JSON
  std::cerr << "Invalid argument: " << ex.what() << std::endl;

} catch (std::exception const& ex) {
  // Other errors
  std::cerr << "Error: " << ex.what() << std::endl;
}
```

## Integration Points

### With DataFormats Module

The JsonDocument module depends on the DataFormats/Json module for:
- JSON type definitions (`value_t`, `object_t`, `array_t`, `type_t`)
- JSON reader/writer implementations
- Type visitors for variant access
- String literals for common fields

### With Overlay Module

JSONDocumentBuilder integrates with overlays for:
- Structured access to document fields
- Type-safe field manipulation
- Validation of document structure
- Bookkeeping management

### With Database Operations

Documents created by this module are used by:
- Database providers (MongoDB, filesystem)
- Configuration API
- Search and query systems
- Archive and versioning systems

## Performance Considerations

### Caching Strategy

JSONDocument uses lazy serialization:
- `_cached_json_buffer` stores serialized JSON
- `_isDirty` flag tracks modifications
- Serialization only occurs when needed
- Multiple reads incur no overhead

### Move Semantics

The module extensively uses move semantics:
- Document extraction from builder (move, not copy)
- Constructor parameters (moved when possible)
- Return value optimization (RVO)
- Efficient ownership transfer

### Reference Returns

Internal methods return references to avoid copying:
- `findChildValue()` returns `value_t const&`
- `cached_json_buffer()` returns `std::string const&`
- Visitor functions operate on references

## Thread Safety

**Not Thread-Safe**: The module is designed for single-threaded use:
- Mutable state in documents
- No synchronization mechanisms
- Cache invalidation without locks
- Builder state management

**Multi-threading**: Requires external synchronization or thread-local instances.

## Debugging Support

### TRACE Logging

Enable detailed logging with debug functions:

```cpp
// Enable all JSONDocument traces
artdaq::database::docrecord::debug::JSONDocument();

// Enable all JSONDocumentBuilder traces
artdaq::database::docrecord::debug::JSONDocumentBuilder();

// Enable all JSONDocumentMigrator traces
artdaq::database::docrecord::debug::JSONDocumentMigrator();

// Enable all utility traces
artdaq::database::docrecord::debug::JSONDocumentUtils();
```

### Stream Output

All classes support streaming for easy debugging:

```cpp
std::cout << doc << std::endl;
std::cout << builder << std::endl;
TLOG(10) << "Document: " << doc;
```

## Design Patterns

### Builder Pattern
`JSONDocumentBuilder` implements the builder pattern:
- Fluent interface (method chaining)
- Step-by-step construction
- Validation during build
- Final extraction

### Visitor Pattern
JSON value access uses the visitor pattern:
- Type-safe variant access
- Extensible operation set
- No type switches needed

### Path-based API
Intuitive dot-notation for navigation:
- Matches JSON structure visually
- Easy to construct programmatically
- Consistent with MongoDB-style queries

### RAII and Smart Pointers
Resource management through:
- Unique pointers for overlays
- Automatic cleanup
- Exception safety

## Best Practices

### Document Creation

1. **Use Builder for database documents** - Don't manually construct database structure
2. **Set version early** - Required for most database operations
3. **Generate new IDs** - Call `newObjectID()` for new documents
4. **Validate input** - Check user data before importing

### Document Modification

1. **Use insert vs. replace correctly**:
   - `insertChild()` - For new fields (fails if exists)
   - `replaceChild()` - For existing fields (fails if missing)
2. **Handle exceptions** - Catch `notfound_exception` for optional fields
3. **Check readonly status** - Before attempting modifications
4. **Save with backup** - `saveToFile()` automatically creates backups

### Performance

1. **Prefer move** - Use `std::move` when transferring ownership
2. **Extract once** - Builder shouldn't be used after `extract()`
3. **Cache-friendly** - Call `to_string()` multiple times without penalty
4. **Batch operations** - Group modifications before serialization

### Error Handling

1. **Catch specific exceptions** - Better than catching `std::exception`
2. **Validate paths** - Check paths exist before operations on critical fields
3. **Enable logging** - Use TRACE when debugging
4. **Provide context** - Add meaningful error messages

## Known Issues and Limitations

### Array Matching Bug
In `JSONDocument_utils.cpp`, the `matches()` function has a bug at line 79:
```cpp
auto const& rightObj = unwrap(left).value_as<const array_t>();
// Should be: unwrap(right).value_as<const array_t>();
```

### Undo Mechanism
`SaveUndo()` and `CallUndo()` are currently placeholders:
- Always return `Success()`
- Don't actually save or restore state
- Future implementation needed for true rollback

### Type System Limitations
- JSON values must be objects for most operations
- Limited support for root-level arrays or primitives
- Type conversions rely on `boost::lexical_cast`

### Naming Inconsistencies
- Method `comapreUsingOverlays` has a typo (should be "compare")
- Some action names use camelCase, others lowercase
- Header guard naming inconsistency (JSONUTILS vs JSONDOCUMENT)

## File Manifest

| File | Purpose | Size |
|------|---------|------|
| `common.h` | Module-wide includes and type aliases | 10 lines |
| `docrecord_exceptions.h` | Custom exception definitions | 26 lines |
| `docrecord_literals.h` | String constants and templates | 68 lines |
| `JSONDocument.h` | Main document class declaration | 129 lines |
| `JSONDocument.cpp` | Document tree manipulation implementation | 387 lines |
| `JSONDocumentBuilder.h` | Builder class declaration | 117 lines |
| `JSONDocumentBuilder.cpp` | Builder implementation | 306 lines |
| `JSONDocumentMigrator.h` | Migrator class declaration | 38 lines |
| `JSONDocumentMigrator.cpp` | Migration implementation | 62 lines |
| `JSONDocument_utils.cpp` | Utility functions and support | 459 lines |

**Total**: 10 files, ~1,600 lines of code

## Dependencies

### Internal Dependencies
- `artdaq-database/DataFormats/Json` - JSON types and operations
- `artdaq-database/BasicTypes` - Basic data types
- `artdaq-database/SharedCommon` - Shared utilities
- `artdaq-database/Overlay` - Document overlay classes

### External Dependencies
- **Boost** - filesystem, lexical_cast, variant visitors
- **C++14 Standard Library** - containers, algorithms, smart pointers
- **TRACE** - Logging and debugging

## Testing Recommendations

### Unit Tests Should Cover

1. **Path Navigation**
   - Valid paths
   - Invalid paths
   - Deep nesting
   - Missing elements

2. **Document Modification**
   - Insert new fields
   - Replace existing fields
   - Delete fields
   - Array operations

3. **Builder Operations**
   - Version setting
   - Alias management
   - Configuration handling
   - Entity management
   - Bookkeeping flags

4. **Migration**
   - Legacy format conversion
   - Missing field handling
   - Multiple configurations
   - Object ID transformation

5. **Error Handling**
   - Empty paths
   - Type mismatches
   - Missing files
   - Malformed JSON

6. **File I/O**
   - Load from file
   - Save to file
   - Backup creation
   - Error conditions

## Future Enhancements

Potential improvements:

1. **Undo/Redo** - Implement actual SaveUndo/CallUndo functionality
2. **Validation** - JSON schema validation
3. **Optimization** - Reduce overlay creation overhead
4. **Array Support** - Better support for root-level arrays
5. **Bulk Operations** - Batch modifications for performance
6. **Query Language** - JSONPath or similar query support
7. **Diffing** - Document comparison and diff generation
8. **Merge** - Document merging capabilities

## Additional Resources

### Related Documentation
- [DataFormats Module](../DataFormats/) - JSON type system
- [Overlay Module](../Overlay/) - Document overlay classes
- [Database Providers](../Providers/) - Database backend integration

### Code Examples
See the usage examples throughout this document and in individual file documentation.

### API Reference
Refer to individual file documentation for detailed API information:
- [JSONDocument.h.md](JSONDocument.h.md) - Core document API
- [JSONDocumentBuilder.h.md](JSONDocumentBuilder.h.md) - Builder API
- [JSONDocumentMigrator.h.md](JSONDocumentMigrator.h.md) - Migration API

## Summary

The JsonDocument module provides a robust, type-safe framework for JSON document operations in the artdaq-database system. With its path-based API, builder pattern for structured documents, and migration support, it serves as the foundation for all document handling in the database system. While designed for single-threaded use and having some known limitations, it provides comprehensive functionality for creating, manipulating, and managing JSON documents with proper metadata and database compliance.
