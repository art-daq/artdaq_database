# Overlay Module

**Last Updated:** 2026-01-14

## Module Overview

The **Overlay module** provides object-oriented C++ wrappers around JSON data structures representing database records. It implements the **Overlay Pattern**, where C++ objects provide type-safe, convenient access to underlying JSON data without data duplication.

**Location**: `artdaq-database/Overlay/`

## Purpose

The Overlay module serves as the **primary interface** for working with configuration database records in artdaq-database. It provides:

1. **Type Safety**: C++ types and methods instead of raw JSON manipulation
2. **Zero Copy**: References JSON directly without duplication
3. **Access Control**: Enforces readonly and deleted states
4. **Audit Trail**: Automatic tracking of all modifications
5. **Comparison**: Deep equality checks with selective masking
6. **Validation**: Ensures data integrity and format compliance

## Files in This Module

### Entry Points
| File | Purpose |
|------|---------|
| JSONDocumentOverlay.h | Public API entry point for external code |
| common.h | Module foundation - types, constants, comparison flags |

### Base Classes
| File | Purpose |
|------|---------|
| ovlKeyValue.h/.cpp | Base class for all overlay objects |

### Template Helpers (Header-Only)
| File | Purpose |
|------|---------|
| ovlStringKeyValue.h | String fields with default value initialization |
| ovlKeyValueWithMask.h | Object fields with comparison masking |
| ovlKeyValueWithDefault.h | Object fields with default initialization |
| ovlKeyValueTimeStamp.h | Named entries with timestamp tracking |
| ovlFixedList.h | Array overlay with single list |
| ovlMovableList.h | Array overlay with active/history lists |

### Simple Component Classes
| File | Purpose |
|------|---------|
| ovlTimeStamp.h/.cpp | ISO8601 timestamp strings |
| ovlId.h/.cpp | Unique record identifiers (OID) |
| ovlComment.h/.cpp | Line-associated configuration comments |
| ovlUpdate.h/.cpp | Single update event in audit trail |
| ovlChangeLog.h/.cpp | Human-readable change descriptions |

### Composite Component Classes
| File | Purpose |
|------|---------|
| ovlBookkeeping.h/.cpp | State flags and update history |
| ovlOrigin.h/.cpp | Document provenance tracking |
| ovlDocument.h/.cpp | User content (data, metadata, search) |

### Main Aggregator
| File | Purpose |
|------|---------|
| ovlDatabaseRecord.h/.cpp | Complete database record with all 14 components |

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

### Class Hierarchy

```
ovlKeyValue (base)
     |
     +-- ovlTimeStamp
     +-- ovlId
     +-- ovlComment
     +-- ovlUpdate
     +-- ovlBookkeeping
     +-- ovlOrigin
     +-- ovlDocument
     +-- ovlDatabaseRecord
     |
     +-- Template Classes:
           +-- ovlStringKeyValue<mask>
           +-- ovlKeyValueWithMask<mask>
           +-- ovlKeyValueWithDefault<mask>
           +-- ovlKeyValueTimeStamp<mask, A, R>
           +-- ovlFixedList<T, mask>
           +-- ovlMovableList<T, mask>
```

## Component Relationships

```
ovlDatabaseRecord (Main Entry Point)
|
+-- ovlId (Record Identifier)
+-- ovlVersion (Version String)
+-- ovlCollection (Collection Name)
+-- ovlConfigurationType (Configuration Type)
|
+-- ovlOrigin (Provenance)
|  +-- format, name, source
|  +-- ovlTimeStamp (created)
|  +-- ovlRawDataList
|
+-- ovlBookkeeping (State & History)
|  +-- readonly/deleted flags
|  +-- ovlTimeStamp (created)
|  +-- ovlUpdates[] (audit trail)
|
+-- ovlChangeLog (Human Log)
|
+-- ovlDocument (User Content)
|  +-- ovlData (configuration)
|  +-- ovlMetadata (description)
|  +-- ovlSearches[] (indexes)
|
+-- ovlConfigurations[] (Fixed List)
+-- ovlEntities[] (Fixed List)
+-- ovlRuns[] (Fixed List)
+-- ovlComments[] (Fixed List)
+-- ovlAttachments[] (Fixed List)
+-- ovlAliases (Movable List - active/history)
```

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
| `DOCUMENT_COMPARE_MUTE_DATA` | Ignore document data |
| `DOCUMENT_COMPARE_MUTE_METADATA` | Ignore metadata |

## Best Practices

1. **Always Check State** before modifications
2. **Check Operation Results** - result_t provides detailed error messages
3. **Use Type Aliases** (ovlConfiguration) not raw templates
4. **Set Comparison Masks Explicitly** in tests
5. **Include JSONDocumentOverlay.h** for external code
6. **Respect Lifetime Rules** - JSON must outlive overlay objects

## Thread Safety

**Not thread-safe**:
- References mutable JSON
- Static comparison mask (global state)
- No internal synchronization

**Recommendations**:
- One record per thread
- External synchronization if sharing records
- Reset comparison mask before each comparison

## Documentation Files

Each component has detailed documentation:

| Documentation File | Source File |
|-------------------|-------------|
| JSONDocumentOverlay.h.md | JSONDocumentOverlay.h |
| common.h.md | common.h |
| ovlKeyValue.h.md, ovlKeyValue.cpp.md | ovlKeyValue.h/.cpp |
| ovlBookkeeping.h.md, ovlBookkeeping.cpp.md | ovlBookkeeping.h/.cpp |
| ovlChangeLog.h.md, ovlChangeLog.cpp.md | ovlChangeLog.h/.cpp |
| ovlComment.h.md, ovlComment.cpp.md | ovlComment.h/.cpp |
| ovlDatabaseRecord.h.md, ovlDatabaseRecord.cpp.md | ovlDatabaseRecord.h/.cpp |
| ovlDocument.h.md, ovlDocument.cpp.md | ovlDocument.h/.cpp |
| ovlId.h.md, ovlId.cpp.md | ovlId.h/.cpp |
| ovlOrigin.h.md, ovlOrigin.cpp.md | ovlOrigin.h/.cpp |
| ovlTimeStamp.h.md, ovlTimeStamp.cpp.md | ovlTimeStamp.h/.cpp |
| ovlUpdate.h.md, ovlUpdate.cpp.md | ovlUpdate.h/.cpp |
| ovlKeyValueTimeStamp.h.md | ovlKeyValueTimeStamp.h |
| ovlKeyValueWithDefault.h.md | ovlKeyValueWithDefault.h |
| ovlKeyValueWithMask.h.md | ovlKeyValueWithMask.h |
| ovlMovableList.h.md | ovlMovableList.h |
| ovlFixedList.h.md | ovlFixedList.h |
| ovlStringKeyValue.h.md | ovlStringKeyValue.h |

## Summary

The Overlay module provides the **essential abstraction layer** for working with configuration database records in artdaq-database. Its key strengths are:

- **Type Safety**: C++ types instead of raw JSON
- **Zero Copy**: Efficient reference-based access
- **Access Control**: Readonly/deleted enforcement
- **Audit Trail**: Automatic modification tracking
- **Flexibility**: Selective comparison masking
- **Consistency**: Uniform interface across all records

The module is used extensively by the Configuration API and forms the foundation for safe, trackable configuration management in the artdaq data acquisition framework.
