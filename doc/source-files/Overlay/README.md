# Overlay Module

## Module Overview

The **Overlay module** provides object-oriented C++ wrappers around JSON data structures representing database records. It implements the **Overlay Pattern**, where C++ objects provide type-safe, convenient access to underlying JSON data without data duplication.

**Location**: `/home/user/artdaq-database/artdaq-database/Overlay/`

## Purpose

The Overlay module serves as the **primary interface** for working with configuration database records in artdaq-database. It provides:

1. **Type Safety**: C++ types and methods instead of raw JSON manipulation
2. **Zero Copy**: References JSON directly without duplication
3. **Access Control**: Enforces readonly and deleted states
4. **Audit Trail**: Automatic tracking of all modifications
5. **Comparison**: Deep equality checks with selective masking
6. **Validation**: Ensures data integrity and format compliance

## Architecture

### The Overlay Pattern

```
C++ Application Code
        |
        | includes JSONDocumentOverlay.h
        v
  ovlDatabaseRecord (C++ object layer)
        |
        | references (no copy)
        v
  JSON AST (actual data storage)
```

**Key Characteristics**:
- **References, Not Copies**: Overlay objects hold references to JSON, not copies
- **Direct Modification**: Changes through overlay modify underlying JSON
- **Lazy Construction**: Overlay objects created on-demand
- **Type-Safe Interface**: Methods enforce correct types and operations

### Module Structure

The module consists of:
1. **Base Classes**: Foundation overlay types
2. **Template Helpers**: Reusable patterns for common structures
3. **Concrete Overlays**: Specific record components
4. **Lists**: Container overlays with add/remove operations
5. **Aggregator**: The main database record class

## Core Components

### 1. Base Layer

#### ovlKeyValue (ovlKeyValue.h/.cpp)
**Purpose**: Base class for all overlay objects

**Key Features**:
- Wraps JSON key-value pairs
- Template accessors for typed value retrieval
- Comparison operators returning result_t
- Virtual to_string() for serialization

**Usage**: All overlay classes derive from this.

#### common.h
**Purpose**: Module-wide types, constants, and comparison flags

**Key Contents**:
- JSON type aliases (array_t, object_t, value_t)
- Error message constants
- DOCUMENT_COMPARE_FLAGS enumeration
- useCompareMask() function

### 2. Template Helper Classes

These provide reusable patterns for common overlay needs:

#### ovlStringKeyValue\<mask\>
- String-valued fields with comparison masking
- Auto-initializes to "not-provided" if empty
- **Used for**: version, collection, changelog, raw data

#### ovlKeyValueWithMask\<mask\>
- Object-valued fields with comparison masking
- No default initialization
- **Used for**: document data and metadata

#### ovlKeyValueWithDefault\<mask\>
- Creates empty object if field missing
- Comparison masking support
- **Used for**: optional object fields

#### ovlKeyValueTimeStamp\<mask, A, R\>
- Name + optional assigned/removed timestamps
- Template parameters control timestamp inclusion
- **Used for**: configurations, entities, runs, aliases

#### ovlFixedList\<T, mask\>
- Array of named elements
- Add/remove with duplicate prevention
- Elements stay in single array (removed items deleted)
- **Used for**: configurations, entities, runs, comments, attachments

#### ovlMovableList\<T, mask\>
- Two arrays: active and history
- Removed items move to history
- **Used for**: aliases (need historical tracking)

### 3. Concrete Component Classes

These represent specific parts of a database record:

#### ovlId (ovlId.h/.cpp)
- Unique record identifier (OID)
- Auto-generates if missing
- Comparison can be masked

#### ovlTimeStamp (ovlTimeStamp.h/.cpp)
- ISO8601 timestamp strings
- Validates format on construction
- Comparison respects timestamp mask

#### ovlComment (ovlComment.h/.cpp)
- Line number + text comments
- For annotating configurations
- Stored in ovlFixedList (ovlComments)

#### ovlUpdate (ovlUpdate.h/.cpp)
- Single update event
- Event name + timestamp + changed value
- Forms audit trail in bookkeeping

#### ovlChangeLog (ovlChangeLog.h/.cpp)
- Human-readable change description
- String buffer with append support
- Complements structured update history

#### ovlBookkeeping (ovlBookkeeping.h/.cpp)
- **Most Critical Component**
- Readonly/deleted state flags
- Creation timestamp
- Complete update history
- Access control enforcement

#### ovlOrigin (ovlOrigin.h/.cpp)
- Document provenance
- Format, source, name, creation time
- Raw data list for format preservation

#### ovlDocument (ovlDocument.h/.cpp)
- User-facing content
- Data (actual configuration)
- Metadata (descriptive information)
- Search (query optimization fields)

#### ovlDatabaseRecord (ovlDatabaseRecord.h/.cpp)
- **Central Aggregator Class**
- Combines all 14 components
- Provides unified record interface
- Enforces access control
- Tracks all modifications

## Component Relationships

```
ovlDatabaseRecord (Main Entry Point)
│
├─ ovlId (Record Identifier)
├─ ovlVersion (Version String)
├─ ovlCollection (Collection Name)
├─ ovlConfigurationType (Configuration Type)
│
├─ ovlOrigin (Provenance)
│  ├─ format, name, source
│  ├─ ovlTimeStamp (created)
│  └─ ovlRawDataList
│
├─ ovlBookkeeping (State & History) ★
│  ├─ readonly/deleted flags
│  ├─ ovlTimeStamp (created)
│  └─ ovlUpdates[] (audit trail)
│
├─ ovlChangeLog (Human Log)
│
├─ ovlDocument (User Content)
│  ├─ ovlData (configuration)
│  ├─ ovlMetadata (description)
│  └─ ovlSearches[] (indexes)
│
├─ ovlConfigurations[] (Fixed List)
├─ ovlEntities[] (Fixed List)
├─ ovlRuns[] (Fixed List)
├─ ovlComments[] (Fixed List)
├─ ovlAttachments[] (Fixed List)
└─ ovlAliases (Movable List - active/history)
```

★ = Critical for access control

## Usage Flow

### 1. Creating a Record

```cpp
#include "artdaq-database/Overlay/JSONDocumentOverlay.h"

// Start with JSON (empty or template)
value_t json = object_t{};

// Create record overlay (auto-initializes all components)
ovlDatabaseRecord record{json};

// Access components
auto& document = record.document();
auto& bookkeeping = record.bookkeeping();
auto& origin = record.origin();
```

### 2. Modifying Content

```cpp
// Check state
if (record.isReadonlyOrDeleted()) {
  // Cannot modify
  return;
}

// Set basic fields
record.version().string_value() = "v2_0_0";
record.collection().string_value() = "MyConfigs";

// Set document data
auto& data = document.data().object_value();
data["trigger_rate"] = 1000;
data["buffer_size"] = 2048;

// Add configuration
auto config = std::make_unique<ovlConfiguration>("cfg", cfgJson);
config->name("TriggerConfig");
auto result = record.addConfiguration(config);
// Automatically posts update to bookkeeping!
```

### 3. State Management

```cpp
// Mark readonly for production
record.markReadonly();

// Future modifications fail
auto result = record.addEntity(entity);
// result.first == false
// result.second == msg_IsReadonly

// Mark deleted (soft delete)
record.markDeleted();
```

### 4. Comparison

```cpp
// Set comparison mask
useCompareMask(DOCUMENT_COMPARE_MUTE_TIMESTAMPS |
               DOCUMENT_COMPARE_MUTE_BOOKKEEPING);

// Compare records
auto result = record1 == record2;
if (result.first) {
  std::cout << "Equal" << std::endl;
} else {
  std::cout << "Differences:\n" << result.second << std::endl;
}
```

## Comparison Masking System

The module provides fine-grained comparison control through flags:

### Common Masks

| Flag | Effect |
|------|--------|
| `DOCUMENT_COMPARE_MUTE_TIMESTAMPS` | Ignore all timestamps |
| `DOCUMENT_COMPARE_MUTE_BOOKKEEPING` | Ignore bookkeeping completely |
| `DOCUMENT_COMPARE_MUTE_UPDATES` | Ignore update history only |
| `DOCUMENT_COMPARE_MUTE_ORIGIN` | Ignore provenance |
| `DOCUMENT_COMPARE_MUTE_COMMENTS` | Ignore comments |
| `DOCUMENT_COMPARE_MUTE_VERSION` | Ignore version field |
| `DOCUMENT_COMPARE_MUTE_DATA` | Ignore document data (⚠️ use carefully) |
| `DOCUMENT_COMPARE_MUTE_METADATA` | Ignore metadata (⚠️ use carefully) |

### Usage

```cpp
// Compare content only (ignore metadata)
auto functionalMask = DOCUMENT_COMPARE_MUTE_BOOKKEEPING |
                      DOCUMENT_COMPARE_MUTE_TIMESTAMPS |
                      DOCUMENT_COMPARE_MUTE_ORIGIN |
                      DOCUMENT_COMPARE_MUTE_COMMENTS |
                      DOCUMENT_COMPARE_MUTE_METADATA;

useCompareMask(functionalMask);
```

## File Organization

```
Overlay/
├── common.h                      # Module foundation
├── JSONDocumentOverlay.h         # Public API entry point
│
├── ovlKeyValue.h/.cpp            # Base class
│
├── Template Helpers:
│   ├── ovlStringKeyValue.h
│   ├── ovlKeyValueWithMask.h
│   ├── ovlKeyValueWithDefault.h
│   ├── ovlKeyValueTimeStamp.h
│   ├── ovlFixedList.h
│   └── ovlMovableList.h
│
├── Simple Components:
│   ├── ovlTimeStamp.h/.cpp
│   ├── ovlId.h/.cpp
│   ├── ovlComment.h/.cpp
│   ├── ovlUpdate.h/.cpp
│   └── ovlChangeLog.h/.cpp
│
├── Composite Components:
│   ├── ovlBookkeeping.h/.cpp     # State & audit trail
│   ├── ovlOrigin.h/.cpp          # Provenance
│   └── ovlDocument.h/.cpp        # User content
│
└── Main Aggregator:
    └── ovlDatabaseRecord.h/.cpp  # Complete record
```

## Key Design Patterns

### 1. Overlay Pattern
**Zero-copy access to JSON**
- C++ objects reference JSON, not copy
- Changes modify underlying JSON directly
- Memory efficient for large configurations

### 2. Access Control Pattern
**Readonly enforcement**
- All modifications check bookkeeping state
- Readonly/deleted flags prevent changes
- Audit trail for accountability

### 3. Template Pattern
**Reusable overlay structures**
- Template mask parameter for comparison
- Template type parameter for lists
- Template bool parameters for optional fields

### 4. Result Pattern
**Informative error handling**
- `result_t = pair<bool, string>`
- Success/failure + detailed message
- Composable results

### 5. Factory Pattern
**Overlay creation**
- `overlay<T>()` template function
- Auto-initializes missing fields
- Returns unique_ptr

## Common Use Cases

### Configuration Management
```cpp
// Add configuration
auto config = std::make_unique<ovlConfiguration>(/*...*/);
config->name("DAQConfiguration");
record.addConfiguration(config);

// Remove configuration
auto oldConfig = std::make_unique<ovlConfiguration>(/*...*/);
oldConfig->name("DAQConfiguration");
record.removeConfiguration(oldConfig);
```

### Entity Tracking
```cpp
// Add entity
auto entity = std::make_unique<ovlEntity>(/*...*/);
entity->name("BoardReader01");
record.addEntity(entity);
```

### Alias Management
```cpp
// Add alias
auto alias = std::make_unique<ovlAlias>(/*...*/);
alias->name("production");
record.addAlias(alias);

// Remove alias (moves to history, not deleted)
auto oldAlias = std::make_unique<ovlAlias>(/*...*/);
oldAlias->name("production");
record.removeAlias(oldAlias);
```

### Version Control
```cpp
// Set version
auto version = std::make_unique<ovlVersion>("version", versionJson);
version->string_value() = "v2_0_0";
record.setVersion(version);
// Posts "setVersion" update to bookkeeping
```

## Best Practices

### 1. Always Check State
```cpp
if (record.isReadonlyOrDeleted()) {
  return Failure("Cannot modify protected record");
}
```

### 2. Check Operation Results
```cpp
auto result = record.addConfiguration(config);
if (!result.first) {
  std::cerr << "Operation failed: " << result.second << std::endl;
  return result;
}
```

### 3. Use Type Aliases
```cpp
// DO: Use type alias
ovlConfiguration config(/*...*/);

// DON'T: Use raw template
ovlKeyValueTimeStamp<DOCUMENT_COMPARE_MUTE_CONFIGURATION> config(/*...*/);
```

### 4. Set Comparison Masks Explicitly
```cpp
// In tests, always set mask explicitly
useCompareMask(DOCUMENT_COMPARE_MUTE_TIMESTAMPS);
auto result = record1 == record2;
```

### 5. Include JSONDocumentOverlay.h
```cpp
// External code should include this, not individual headers
#include "artdaq-database/Overlay/JSONDocumentOverlay.h"
```

### 6. Respect Lifetime Rules
```cpp
// BAD: JSON destroyed before overlay
ovlDatabaseRecord& createRecord() {
  value_t json = object_t{};
  return ovlDatabaseRecord{json};  // Dangling reference!
}

// GOOD: JSON outlives overlay
void useRecord(value_t& json) {
  ovlDatabaseRecord record{json};
  // Use record...
} // record destroyed before json
```

## Performance Considerations

### Efficient Operations
- Component access: O(1)
- Value access through overlay: O(1)
- Add/remove from lists: O(n) due to duplicate checking + list recreation

### Expensive Operations
- Record creation: O(n) - creates overlays for all components
- Comparison: O(n) - traverses all components
- Serialization to_string(): O(n) - formats entire record

### Optimization Tips
1. **Cache overlay objects** when accessing repeatedly
2. **Use move semantics** for overlay unique_ptrs
3. **Minimize to_string()** calls (only for debugging)
4. **Set focused comparison masks** to skip irrelevant components

## Thread Safety

**Not thread-safe**:
- References mutable JSON
- Static comparison mask (global state)
- No internal synchronization

**Recommendations**:
- One record per thread
- External synchronization if sharing records
- Clone records for thread-local use
- Reset comparison mask before each comparison in multi-threaded environments

## Testing Considerations

### Comparison Masking
```cpp
// Test functional equality ignoring metadata
useCompareMask(DOCUMENT_COMPARE_MUTE_BOOKKEEPING |
               DOCUMENT_COMPARE_MUTE_TIMESTAMPS |
               DOCUMENT_COMPARE_MUTE_ORIGIN);

EXPECT_TRUE((record1 == record2).first);
```

### State Testing
```cpp
// Test readonly enforcement
record.markReadonly();
auto result = record.addConfiguration(config);
EXPECT_FALSE(result.first);
EXPECT_EQ(result.second, msg_IsReadonly);
```

### Update Tracking
```cpp
// Verify update posted
size_t beforeCount = record.bookkeeping()._updates.size();
record.setVersion(newVersion);
size_t afterCount = record.bookkeeping()._updates.size();
EXPECT_EQ(afterCount, beforeCount + 1);
```

## Relationship to Other Modules

### DataFormats/Json
- Provides underlying JSON types (array_t, object_t, value_t)
- JSON parsing and serialization
- Type system and visitors

### SharedCommon
- Common utilities and standard library includes
- Result types and error handling
- Timestamp generation functions

### Configuration API
- **Primary Consumer**: Uses ovlDatabaseRecord as main interface
- Reads/writes records to/from database
- Applies business logic using overlay methods

## Migration and Compatibility

### From Raw JSON
```cpp
// Old way: Raw JSON manipulation
json["document"]["data"]["trigger_rate"] = 1000;

// New way: Type-safe overlay
record.document().data().object_value()["trigger_rate"] = 1000;
```

### Benefits of Overlay
- Type safety
- Validation
- Access control
- Audit trailing
- Easier refactoring

## Future Enhancements

Potential improvements:
1. **Immutable Records**: Const-correct overlay types
2. **Change Tracking**: Dirty flag tracking for optimized saves
3. **Lazy Loading**: Load components on-demand for large records
4. **Schema Validation**: Validate against JSON schema
5. **Visitor Pattern**: Traverse record structure generically
6. **Copy-on-Write**: Share JSON between records until modified

## Summary

The Overlay module provides the **essential abstraction layer** for working with configuration database records in artdaq-database. Its key strengths are:

✅ **Type Safety**: C++ types instead of raw JSON
✅ **Zero Copy**: Efficient reference-based access
✅ **Access Control**: Readonly/deleted enforcement
✅ **Audit Trail**: Automatic modification tracking
✅ **Flexibility**: Selective comparison masking
✅ **Consistency**: Uniform interface across all records

The module is used extensively by the Configuration API and forms the foundation for safe, trackable configuration management in the artdaq data acquisition framework.

## Quick Reference

### Including the Module
```cpp
#include "artdaq-database/Overlay/JSONDocumentOverlay.h"
using namespace artdaq::database::overlay;
```

### Creating Records
```cpp
value_t json = object_t{};
ovlDatabaseRecord record{json};
```

### Checking State
```cpp
if (record.isReadonlyOrDeleted()) { /* ... */ }
```

### Modifying Records
```cpp
record.version().string_value() = "v2";
record.addConfiguration(config);
record.markReadonly();
```

### Comparing Records
```cpp
useCompareMask(flags);
auto result = record1 == record2;
```

## Documentation Files

Each component has detailed documentation:
- **common.h.md** - Module foundation
- **JSONDocumentOverlay.h.md** - Public API
- **ovlKeyValue.h.md** - Base class
- **Template classes** - Reusable patterns
- **Component classes** - Individual overlays
- **ovlDatabaseRecord.h.md** - Main aggregator

See individual documentation files for detailed information on each component.
