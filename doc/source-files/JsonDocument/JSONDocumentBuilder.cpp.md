# JSONDocumentBuilder.cpp

## File Overview

This file implements the `JSONDocumentBuilder` class methods, providing the concrete implementation of the builder pattern API for creating and modifying structured JSON database documents. It integrates with the overlay system to ensure proper document structure, metadata, and bookkeeping.

**Location**: `/home/user/artdaq-database/artdaq-database/JsonDocument/JSONDocumentBuilder.cpp`

## Dependencies

```cpp
#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"
#include <utility>
#include "artdaq-database/JsonDocument/common.h"
```

## TRACE Configuration

```cpp
#ifdef TRACE_NAME
#undef TRACE_NAME
#endif
#define TRACE_NAME "JSONDocumentBuilder.cpp"
```

Sets the TRACE subsystem name to "JSONDocumentBuilder.cpp" for all logging.

## Using Declarations

```cpp
using artdaq::database::result_t;
using artdaq::database::ThrowOnFailure;
using artdaq::database::json::value_t;
using artdaq::database::docrecord::JSONDocument;
using artdaq::database::docrecord::JSONDocumentBuilder;

namespace db = artdaq::database;
namespace utl = db::docrecord;
namespace ovl = db::overlay;
namespace dbdr = artdaq::database::docrecord;
namespace jsonliteral = artdaq::database::dataformats::literal;
```

Simplifies type names and provides convenient namespace aliases.

## Constructors

### Default Constructor

```cpp
JSONDocumentBuilder::JSONDocumentBuilder()
    : _document(std::string(template__empty_document)),
      _overlay(std::make_unique<ovlDatabaseRecord>(_document._value)),
      _initOK(init()) {}
```

**Initialization Sequence**:
1. Creates document from empty template (`{}`)
2. Creates overlay for structured access to the empty document
3. Calls `init()` and stores result

**Starting State**: Empty but valid JSON object with overlay ready.

### Document Constructor

```cpp
JSONDocumentBuilder::JSONDocumentBuilder(JSONDocument document)
    : _document(std::move(document)),
      _overlay(std::make_unique<ovlDatabaseRecord>(_document._value)),
      _initOK(init()) {}
```

**Initialization Sequence**:
1. Moves provided document into `_document`
2. Creates overlay for the document
3. Calls `init()` and stores result

**Use Case**: Building from existing document for modification.

## Initialization

### init()

```cpp
bool JSONDocumentBuilder::init() {
  TLOG(20) << "JSONDocumentBuilder::init() new document=<"
           << _document.cached_json_buffer() << ">";
  return true;
}
```

**Purpose**: Logs document state at initialization.

**Returns**: Always `true` (currently no failure conditions).

**TRACE**: Level 20 - Logs the initial document JSON.

## Alias Management

### addAlias

```cpp
JSONDocumentBuilder& JSONDocumentBuilder::addAlias(JSONDocument const& alias)
```

**Implementation**:
```cpp
try {
  TLOG(21) << "addAlias() args  alias=<" << alias << ">";

  JSONDocument copy(alias);
  auto ovl = overlay<ovl::ovlAlias>(copy, jsonliteral::alias);

  ThrowOnFailure(SaveUndo());
  ThrowOnFailure(_overlay->addAlias(ovl));

  return self();
} catch (std::exception const& ex) {
  TLOG(22) << "addAlias() Exception:" << ex.what();
  ThrowOnFailure(CallUndo());
  return self();
}
```

**Process**:
1. Log the alias being added
2. Create copy of alias document
3. Create alias overlay
4. Save undo point
5. Add alias via database record overlay
6. Return self (builder pattern)
7. On exception: log error, call undo, return self

**TRACE Levels**:
- 21: Entry with alias value
- 22: Exception caught

### removeAlias

```cpp
JSONDocumentBuilder& JSONDocumentBuilder::removeAlias(JSONDocument const& alias)
```

**Implementation**: Similar pattern to `addAlias` but calls `_overlay->removeAlias()`.

**TRACE Levels**:
- 23: Entry
- 24: Exception

## Configuration Management

### addConfiguration

```cpp
JSONDocumentBuilder& JSONDocumentBuilder::addConfiguration(JSONDocument const& config)
```

**Process**:
1. Log configuration being added
2. Create copy and overlay
3. Save undo, perform operation
4. Handle exceptions with undo

**TRACE Levels**:
- 25: Entry
- 26: Exception

### removeConfiguration

```cpp
JSONDocumentBuilder& JSONDocumentBuilder::removeConfiguration(JSONDocument const& config)
```

**Implementation**: Mirrors `addConfiguration` but removes instead.

**TRACE Levels**:
- 27: Entry
- 28: Exception

### removeAllConfigurations

```cpp
JSONDocumentBuilder& JSONDocumentBuilder::removeAllConfigurations()
```

**Implementation**:
```cpp
try {
  TLOG(29) << "removeAllConfigurations()";
  _overlay->configurations().wipe();
  return self();
} catch (std::exception const& ex) {
  TLOG(30) << "removeAllConfigurations() Exception:" << ex.what();
  ThrowOnFailure(CallUndo());
  return self();
}
```

**Characteristics**:
- Directly wipes configurations array via overlay
- No undo save (wipe is atomic)
- Logs exception and calls undo on failure

**TRACE Levels**:
- 29: Entry
- 30: Exception

## Object ID Management

### setObjectID

```cpp
JSONDocumentBuilder& JSONDocumentBuilder::setObjectID(JSONDocument const& objectId)
```

**Implementation**:
```cpp
try {
  TLOG(31) << "setObjectID() args  objectId=<" << objectId << ">";

  JSONDocument copy(objectId);
  auto id = std::make_unique<ovl::ovlId>(
      jsonliteral::id,
      copy.findChildValue(jsonliteral::id));

  ThrowOnFailure(SaveUndo());
  ThrowOnFailure(_overlay->swap(id));

  return self();
} catch (std::exception const& ex) {
  TLOG(32) << "setObjectID() Exception:" << ex.what();
  ThrowOnFailure(CallUndo());
  return self();
}
```

**Key Difference**: Uses `swap()` instead of `add()` to replace ID.

**TRACE Levels**:
- 31: Entry
- 32: Exception

### getObjectID

```cpp
JSONDocument JSONDocumentBuilder::getObjectID() const {
  return {_overlay->id().to_string()};
}
```

**Returns**: Document containing the object ID.

### getObjectOUID

```cpp
std::string JSONDocumentBuilder::getObjectOUID() const {
  return _overlay->id().oid();
}
```

**Returns**: Object ID as string (OID format).

### newObjectID

```cpp
bool JSONDocumentBuilder::newObjectID() {
  return _overlay->id().newId();
}
```

**Purpose**: Generates and assigns a new unique object ID.

**Returns**: `true` if successful.

## Version Management

### setVersion

```cpp
JSONDocumentBuilder& JSONDocumentBuilder::setVersion(JSONDocument const& version)
```

**Implementation**:
```cpp
try {
  TLOG(33) << "setVersion() args  version=<" << version << ">";

  JSONDocument copy(version);
  auto ovl = std::make_unique<ovl::ovlVersion>(
      jsonliteral::version,
      copy.findChildValue(jsonliteral::name));

  ThrowOnFailure(SaveUndo());
  ThrowOnFailure(_overlay->setVersion(ovl));

  return self();
} catch (std::exception const& ex) {
  TLOG(34) << "setVersion() Exception:" << ex.what();
  ThrowOnFailure(CallUndo());
  return self();
}
```

**Note**: Extracts version from `name` field of the provided document.

**TRACE Levels**:
- 33: Entry
- 34: Exception

## Collection Management

### setCollection

```cpp
JSONDocumentBuilder& JSONDocumentBuilder::setCollection(JSONDocument const& collection)
```

**Implementation**:
```cpp
try {
  TLOG(35) << "collection() args  collection=<" << collection << ">";

  auto copy = collection.findChild(jsonliteral::collection);
  auto ovl = std::make_unique<ovl::ovlCollection>(
      jsonliteral::collection,
      copy.findChildValue(jsonliteral::collection));

  ThrowOnFailure(SaveUndo());
  ThrowOnFailure(_overlay->setCollection(ovl));

  return self();
} catch (std::exception const& ex) {
  ThrowOnFailure(CallUndo());
  return self();
}
```

**Note**: No exception logging (unlike other methods).

**TRACE Level**: 35 (entry only)

## Entity Management

### addEntity

```cpp
JSONDocumentBuilder& JSONDocumentBuilder::addEntity(JSONDocument const& entity)
```

**Process**: Standard pattern - copy, create overlay, save undo, add via overlay.

**TRACE Levels**:
- 36: Entry
- 37: Exception

### removeEntity

```cpp
JSONDocumentBuilder& JSONDocumentBuilder::removeEntity(JSONDocument const& entity)
```

**Process**: Standard removal pattern.

**TRACE Levels**:
- 40: Entry
- 41: Exception

### removeAllEntities

```cpp
JSONDocumentBuilder& JSONDocumentBuilder::removeAllEntities()
```

**Implementation**:
```cpp
try {
  TLOG(42) << "removeAllEntities()";
  _overlay->entities().wipe();
  return self();
} catch (std::exception const& ex) {
  TLOG(43) << "removeAllEntities() Exception:" << ex.what();
  ThrowOnFailure(CallUndo());
  return self();
}
```

**TRACE Levels**:
- 42: Entry
- 43: Exception

## Run Management

### addRun

```cpp
JSONDocumentBuilder& JSONDocumentBuilder::addRun(JSONDocument const& run)
```

**Process**: Similar to entity/configuration management.

**TRACE Levels**:
- 38: Entry
- 39: Exception

## Bookkeeping Operations

### markReadonly

```cpp
JSONDocumentBuilder& JSONDocumentBuilder::markReadonly()
```

**Implementation**:
```cpp
try {
  TLOG(44) << "markReadonly()";

  ThrowOnFailure(SaveUndo());
  ThrowOnFailure(_overlay->markReadonly());

  return self();
} catch (std::exception const& ex) {
  TLOG(45) << "markReadonly() Exception:" << ex.what();
  ThrowOnFailure(CallUndo());
  return self();
}
```

**No Parameters**: Simply marks the document readonly.

**TRACE Levels**:
- 44: Entry
- 45: Exception

### markDeleted

```cpp
JSONDocumentBuilder& JSONDocumentBuilder::markDeleted()
```

**Implementation**: Identical pattern to `markReadonly` but calls `markDeleted()`.

**TRACE Levels**:
- 46: Entry
- 47: Exception

### isReadonlyOrDeleted

```cpp
bool JSONDocumentBuilder::isReadonlyOrDeleted() const {
  return _overlay->isReadonlyOrDeleted();
}
```

**Simple Delegation**: Queries overlay for status.

## Tag Extraction

### extractTags

```cpp
std::list<std::string> JSONDocumentBuilder::extractTags() const
```

**Implementation**:
```cpp
auto retValue = std::list<std::string>{};

retValue.push_back(
    std::string(jsonliteral::version)
    .append(":")
    .append(_overlay->version().string_value()));

auto const& configurations = _overlay->configurations();
for (auto const& configuration : configurations) {
  retValue.push_back(
      std::string(jsonliteral::configuration)
      .append(":")
      .append(configuration.name()));
}

auto const& entities = _overlay->entities();
for (auto const& entity : entities) {
  retValue.push_back(
      std::string(jsonliteral::entity)
      .append(":")
      .append(entity.name()));
}

return retValue;
```

**Tag Format**: `"type:value"` (e.g., `"version:v1.0.0"`, `"configuration:detector_A"`)

**Tags Extracted**:
1. Version tag (always included)
2. Configuration tags (one per configuration)
3. Entity tags (one per entity)

**Use Case**: Building search indexes, categorizing documents.

## Comparison

### comapreUsingOverlays

```cpp
result_t JSONDocumentBuilder::comapreUsingOverlays(JSONDocumentBuilder const& other) const {
  return *_overlay == *other._overlay;
}
```

**Note**: Typo in method name ("comapreUsingOverlays" should be "compareUsingOverlays").

**Comparison Method**: Delegates to overlay's `operator==`.

**Returns**: `result_t` with comparison result and details.

## String Conversion

### to_string

```cpp
std::string JSONDocumentBuilder::to_string() const {
  return _document.to_string();
}
```

**Simple Delegation**: Returns document's JSON string representation.

## Stream Output Operator

```cpp
std::ostream& utl::operator<<(std::ostream& os, JSONDocumentBuilder const& data) {
  os << data.to_string();
  return os;
}
```

**Purpose**: Enables streaming builder to output streams.

**Usage**:
```cpp
std::cout << builder << std::endl;
```

## Debug Function

### debug::JSONDocumentBuilder

```cpp
void dbdr::debug::JSONDocumentBuilder() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TLOG(48) << "artdaq::database::JSONDocumentBuilder trace_enable";
}
```

**Purpose**: Enables maximum TRACE logging for debugging.

**Configuration**:
- Sets trace name
- Enables all trace levels (all bits set)
- Configures trace modes
- Logs activation

## Implementation Patterns

### Standard Operation Pattern

Most modification methods follow this pattern:
```cpp
JSONDocumentBuilder& operation(JSONDocument const& param) try {
  TLOG(N) << "operation() args param=<" << param << ">";

  JSONDocument copy(param);
  auto ovl = overlay<OVL_TYPE>(copy, literal_key);

  ThrowOnFailure(SaveUndo());
  ThrowOnFailure(_overlay->performOperation(ovl));

  return self();
} catch (std::exception const& ex) {
  TLOG(N+1) << "operation() Exception:" << ex.what();
  ThrowOnFailure(CallUndo());
  return self();
}
```

**Pattern Steps**:
1. Log operation with parameters
2. Create local copy of parameter
3. Create overlay for the parameter
4. Save undo state
5. Perform operation via overlay
6. Return self (builder pattern)
7. On exception: log, undo, return self

### Wipe Operations Pattern

`removeAll*` methods use simpler pattern:
```cpp
try {
  TLOG(N) << "removeAll()";
  _overlay->field().wipe();
  return self();
} catch (std::exception const& ex) {
  TLOG(N+1) << "removeAll() Exception:" << ex.what();
  ThrowOnFailure(CallUndo());
  return self();
}
```

**Characteristics**:
- No parameters
- Direct wipe via overlay
- No undo save (wipe is atomic operation)
- Exception handling with undo

## Error Handling Strategy

All public methods use exception handling:

**On Success**: Return `self()` for method chaining

**On Failure**:
1. Catch exception
2. Log exception message
3. Call undo to restore state
4. Return self (allows continued use)

**Result**: Builder remains in consistent state even after failures.

## TRACE Logging Levels

The implementation uses systematic TRACE levels:

- **20**: Initialization
- **21-24**: Alias operations
- **25-30**: Configuration operations
- **31-32**: Object ID operations
- **33-34**: Version operations
- **35**: Collection operations
- **36-43**: Entity and run operations
- **44-47**: Bookkeeping operations
- **48**: Debug activation

**Pattern**: Consecutive pairs (N, N+1) for operation entry and exception.

## Overlay Integration

The implementation heavily relies on overlays:

**Overlay Creation**:
```cpp
auto ovl = overlay<ovl::ovlAlias>(copy, jsonliteral::alias);
```

**Overlay Operations**:
```cpp
_overlay->addAlias(ovl);
_overlay->configurations().wipe();
_overlay->markReadonly();
```

**Benefits**:
- Type-safe access to document fields
- Validation of document structure
- Abstraction of JSON details
- Consistent field manipulation

## Performance Considerations

1. **Copy-then-overlay**: Each operation copies the parameter before creating overlay
2. **Unique pointers**: Overlays use `std::unique_ptr` for efficient ownership
3. **Move semantics**: Constructor uses `std::move` for document parameter
4. **No redundant serialization**: Delegates to document's cached serialization

## Thread Safety

Not thread-safe:
- Modifies internal state (`_document`, `_overlay`)
- No synchronization mechanisms
- Intended for single-threaded use

## Undo Mechanism Status

Current implementation:
```cpp
result_t JSONDocumentBuilder::SaveUndo() { return Success(); }

result_t JSONDocumentBuilder::CallUndo() noexcept try {
  return Success();
} catch (...) {
  return Failure();
}
```

**Status**: Placeholder implementation - always succeeds, doesn't actually save/restore.

**Future Work**: Could be implemented to provide true rollback capability.

## Related Files

- **JSONDocumentBuilder.h** - Class definition
- **JSONDocument_utils.cpp** - Contains `createFromData` and `_importUserData`
- **JSONDocumentOverlay.h** - Overlay classes used throughout

## Best Practices When Using This Implementation

1. **Check return values**: Although builder pattern returns self, operations can fail silently
2. **Enable TRACE for debugging**: Call `debug::JSONDocumentBuilder()` when troubleshooting
3. **Handle exceptions upstream**: While builder handles exceptions, consider catching specific ones
4. **Don't reuse after extract**: Builder state is undefined after `extract()`
5. **Use undo for critical operations**: Even though current implementation is a stub

## Notes

- All modification methods return `self()` to enable method chaining
- Exception handling allows builder to continue after errors
- Overlay is never null after construction
- TRACE logging is comprehensive for debugging
- Method naming is inconsistent (some use camelCase, some lowercase)
- Typo in `comapreUsingOverlays` method name
- `setCollection` doesn't log exceptions (inconsistent with other methods)
