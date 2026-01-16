# ovlDatabaseRecord.cpp

**Path:** `artdaq-database/Overlay/ovlDatabaseRecord.cpp`

**Implements:** [ovlDatabaseRecord.h](./ovlDatabaseRecord.h.md)

**Purpose:** Provides the implementation for the `ovlDatabaseRecord` class, including component initialization, accessor methods, state management, add/remove/set operations with automatic update tracking, and comprehensive comparison of all 14 document components. The implementation ensures consistent access control and audit trail for all document modifications.

## Implementation Overview

The implementation follows a consistent pattern for all modification operations:

1. **Validate input** - Use `confirm()` to assert pointer is not null
2. **Check access control** - Call `isReadonlyOrDeleted()` and return failure if protected
3. **Perform operation** - Add/remove/swap on the component list
4. **Check result** - If operation failed, return the error immediately
5. **Post update** - Record the change in bookkeeping for audit trail
6. **Return result** - Return the final status to caller

This ensures consistency, access control enforcement, and complete audit trails for all document modifications.

## Key Algorithms

### Component Initialization

The constructor creates overlay wrappers for all 14 components using the `overlay<>()` factory function:

**Steps:**
1. Initialize base class with `jsonliteral::database_record` key
2. For each component:
   - Call appropriate `overlay<ComponentType>()` or `overlay<ComponentType, ValueType>()`
   - Pass document JSON and component key
   - Store resulting unique_ptr in member variable

**Initialization order (in constructor):**
```cpp
_document, _comments, _origin, _version, _entities, _configurations,
_changelog, _bookkeeping, _id, _collection, _aliases, _runs,
_attachments, _configurationtype
```

**Why this approach:** Ensures all components are initialized uniformly with auto-population of missing fields by each component's constructor.

### Modification Pattern

All add/remove/set operations follow the same pattern:

```cpp
result_t operation(ComponentUPtr_t& component) {
  confirm(component);                    // 1. Validate input

  if (isReadonlyOrDeleted()) {           // 2. Check access control
    return Failure(msg_IsReadonly);
  }

  auto update = std::string("operationName");
  auto result = _list->operation(component);  // 3. Perform operation

  if (!result.first) {                   // 4. Check result
    return result;
  }

  return _bookkeeping->postUpdate(update, component);  // 5. Post update
}
```

### Set Operations with Deduplication

Set operations add a deduplication check to avoid recording updates when nothing changed:

```cpp
result_t setVersion(ovlVersionUPtr_t& version) {
  confirm(version);

  if (isReadonlyOrDeleted()) {
    return Failure(msg_IsReadonly);
  }

  // Deduplication check
  if (_version->string_value() == version->string_value()) {
    return Success(msg_Ignored);
  }

  auto result = swap(version);
  if (!result.first) {
    return result;
  }

  auto update = std::string("setVersion");
  return _bookkeeping->postUpdate(update, _version);
}
```

### Comprehensive Comparison

The comparison operator aggregates all component differences:

**Steps:**
1. Initialize error message stream with header
2. Compare each of 14 components using their `operator==`
3. Append any differences to error stream
4. Check if any differences were found using `tellp()` position
5. Return `Success()` if no differences, `Failure(oss)` with aggregated message otherwise

**Why this approach:** Provides complete difference report rather than failing on first mismatch, making debugging easier.

## Internal Functions

### Constructor

```cpp
ovlDatabaseRecord::ovlDatabaseRecord(value_t& record)
    : ovlKeyValue(jsonliteral::database_record, record),
      _document{overlay<ovlDocument>(record, jsonliteral::document)},
      _comments{overlay<ovlComments, array_t>(record, jsonliteral::comments)},
      _origin{overlay<ovlOrigin>(record, jsonliteral::origin)},
      _version{overlay<ovlVersion, std::string>(record, jsonliteral::version)},
      _entities{overlay<ovlEntities, array_t>(record, jsonliteral::entities)},
      _configurations{overlay<ovlConfigurations, array_t>(record, jsonliteral::configurations)},
      _changelog{overlay<ovlChangeLog, std::string>(record, jsonliteral::changelog)},
      _bookkeeping{overlay<ovlBookkeeping>(record, jsonliteral::bookkeeping)},
      _id{overlay<ovlId>(record, jsonliteral::id)},
      _collection{overlay<ovlCollection, std::string>(record, jsonliteral::collection)},
      _aliases{overlay<ovlAliases>(record, jsonliteral::aliases)},
      _runs{overlay<ovlRuns, array_t>(record, jsonliteral::runs)},
      _attachments{overlay<ovlAttachments, array_t>(record, jsonliteral::attachments)},
      _configurationtype{overlay<ovlConfigurationType, std::string>(record, jsonliteral::configurationtype)} {}
```

**Brief:** Initializes all 14 component overlays, auto-populating missing fields through each component's constructor.

**Called by:** Code that needs to work with database documents

**Uses factory function patterns:**
- `overlay<Type>(json, key)` - For object-type components
- `overlay<Type, ValueType>(json, key)` - For components with specific value types (string, array)

### Component Accessors

```cpp
ovlDocument& ovlDatabaseRecord::document() { return *_document; }
ovlComments& ovlDatabaseRecord::comments() { return *_comments; }
ovlOrigin& ovlDatabaseRecord::origin() { return *_origin; }
ovlVersion& ovlDatabaseRecord::version() { return *_version; }
ovlEntities& ovlDatabaseRecord::entities() { return *_entities; }
ovlConfigurations& ovlDatabaseRecord::configurations() { return *_configurations; }
ovlBookkeeping& ovlDatabaseRecord::bookkeeping() { return *_bookkeeping; }
ovlId& ovlDatabaseRecord::id() { return *_id; }
```

**Brief:** Simple accessors returning references to managed components by dereferencing unique_ptrs.

**Called by:** All code that accesses document components

### State Management

```cpp
bool ovlDatabaseRecord::isReadonlyOrDeleted() const {
  return _bookkeeping->isReadonly() || _bookkeeping->isDeleted();
}

result_t ovlDatabaseRecord::markReadonly() {
  if (isReadonlyOrDeleted()) {
    return Failure(msg_IsReadonly);
  }
  _bookkeeping->markReadonly(true);
  return Success();
}

result_t ovlDatabaseRecord::markDeleted() {
  _bookkeeping->markDeleted(true);
  return Success();
}
```

**Brief:** State query and modification with access control.

**Note:** `markDeleted()` can be called even if document is already readonly, allowing soft-delete of archived documents. This is intentional behavior.

### Swap Operations

```cpp
result_t ovlDatabaseRecord::swap(ovlDocumentUPtr_t& document) {
  if (isReadonlyOrDeleted()) {
    return Failure(msg_IsReadonly);
  }
  return _document->ovlKeyValue::swap(document.get());
}
```

**Brief:** All swap operations check readonly/deleted state before swapping component contents.

**Called by:** Direct API use, set operations (setVersion, etc.)

**Note:** Swap operations do NOT call `confirm()` for null check - the base class swap handles this.

### Add/Remove Operations

```cpp
result_t ovlDatabaseRecord::addConfiguration(ovlConfigurationUPtr_t& configuration) {
  confirm(configuration);

  if (isReadonlyOrDeleted()) {
    return Failure(msg_IsReadonly);
  }

  auto update = std::string("addConfiguration");
  auto result = _configurations->add(configuration);

  if (!result.first) {
    return result;
  }

  return _bookkeeping->postUpdate(update, configuration);
}

result_t ovlDatabaseRecord::removeConfiguration(ovlConfigurationUPtr_t& configuration) {
  confirm(configuration);

  if (isReadonlyOrDeleted()) {
    return Failure(msg_IsReadonly);
  }

  auto update = std::string("removeConfiguration");
  auto result = _configurations->remove(configuration);

  if (!result.first) {
    return result;
  }

  return _bookkeeping->postUpdate(update, configuration);
}
```

**Brief:** Pattern followed by all add/remove operations: validate, check access, perform, track.

**Update event names:**
| Method | Event Name |
|--------|------------|
| `addConfiguration` | `"addConfiguration"` |
| `removeConfiguration` | `"removeConfiguration"` |
| `addAlias` | `"addAlias"` |
| `removeAlias` | `"removeAlias"` |
| `addEntity` | `"addEntity"` |
| `removeEntity` | `"removeEntity"` |
| `addRun` | `"addRun"` |
| `setVersion` | `"setVersion"` |
| `setCollection` | `"setCollection"` |
| `setConfigurationType` | `"setConfigurationType"` |

### Set Operations with Deduplication

```cpp
result_t ovlDatabaseRecord::setVersion(ovlVersionUPtr_t& version) {
  confirm(version);

  if (isReadonlyOrDeleted()) {
    return Failure(msg_IsReadonly);
  }

  if (_version->string_value() == version->string_value()) {
    return Success(msg_Ignored);  // No change needed
  }

  auto result = swap(version);
  if (!result.first) {
    return result;
  }

  auto update = std::string("setVersion");
  return _bookkeeping->postUpdate(update, _version);
}
```

**Brief:** Set operations check if value is unchanged before recording update. Returns `Success(msg_Ignored)` when no change is needed.

**Called by:** Code that updates document metadata

### Serialization

```cpp
std::string ovlDatabaseRecord::to_string() const {
  std::ostringstream oss;
  oss << "{";
  oss << debrace(_id->to_string()) << ",\n";
  oss << debrace(_collection->to_string()) << ",\n";
  oss << debrace(_version->to_string()) << ",\n";
  oss << debrace(_origin->to_string()) << ",\n";
  oss << debrace(_aliases->to_string()) << ",\n";
  oss << debrace(_bookkeeping->to_string()) << ",\n";
  oss << debrace(_changelog->to_string()) << ",\n";
  oss << debrace(_entities->to_string()) << ",\n";
  oss << debrace(_runs->to_string()) << ",\n";
  oss << debrace(_attachments->to_string()) << ",\n";
  oss << debrace(_configurationtype->to_string()) << ",\n";
  oss << debrace(_configurations->to_string()) << ",\n";
  oss << debrace(_comments->to_string()) << ",\n";
  oss << debrace(_document->to_string()) << "\n";
  oss << "}";

  return oss.str();
}
```

**Brief:** Produces complete JSON representation of all components using `debrace()` helper to combine nested JSON objects.

**Called by:** Debugging code, logging, error messages

### Comparison Operator

```cpp
result_t ovlDatabaseRecord::operator==(ovlDatabaseRecord const& other) const {
  std::ostringstream oss;
  oss << "\nDatabase records disagree.";
  auto noerror_pos = oss.tellp();

  auto result = *_document == *other._document;
  if (!result.first) oss << result.second;

  result = *_comments == *other._comments;
  if (!result.first) oss << result.second;

  // ... (compare all 14 components) ...

  if (oss.tellp() == noerror_pos) {
    return Success();
  }

  return Failure(oss);
}
```

**Brief:** Comprehensive component-by-component comparison with aggregated error messages. Uses `tellp()` to detect if any differences were added.

**Component comparison order:**
1. `_document`
2. `_comments`
3. `_origin`
4. `_version`
5. `_collection`
6. `_configurationtype`
7. `_configurations`
8. `_changelog`
9. `_bookkeeping`
10. `_id`
11. `_entities`
12. `_runs`
13. `_aliases`
14. `_attachments`

## Performance Considerations

- **Construction:** O(n) where n = total elements across all lists (each component auto-initializes)
- **Component Access:** O(1) - simple pointer dereference
- **Modifications:** O(1) operation + O(1) update post + O(n) potential list recreation
- **Comparison:** O(n) to compare all components
- **Serialization:** O(n) to traverse all components

## Error Handling Strategy

The implementation uses:
- `confirm()` assertions for null pointer checks on inputs
- `result_t` return types for operation status
- Access control checks (`isReadonlyOrDeleted()`) before all modifications
- Aggregated error messages in comparison for comprehensive debugging

## Testing Notes

- **Unit tests:** Part of Overlay module tests in `test/Overlay/`
- **Key test cases:**
  - Create document with empty JSON (all defaults applied)
  - Add/remove configurations, entities, aliases with success/failure cases
  - Set version, collection, configuration type (test deduplication)
  - Mark readonly and verify all modifications fail
  - Mark deleted and verify modifications fail
  - Compare equal documents (should succeed)
  - Compare different documents (verify difference messages)
  - Test comparison with various mask settings

## Maintenance Notes

### Initialization Order

The constructor initializes all 14 components in a specific order. If adding new components:
1. Add member variable declaration in header (private section)
2. Add type alias in header if needed
3. Add initialization in constructor member initializer list
4. Add accessor method
5. Add to `to_string()` output
6. Add to comparison operator

### Update Event Names

Update tracking uses string event names (defined in each method):

| Category | Events |
|----------|--------|
| Configuration | `"addConfiguration"`, `"removeConfiguration"` |
| Alias | `"addAlias"`, `"removeAlias"` |
| Entity | `"addEntity"`, `"removeEntity"` |
| Run | `"addRun"` |
| Setters | `"setVersion"`, `"setCollection"`, `"setConfigurationType"` |

### Readonly vs Deleted Behavior

- `markReadonly()` fails if already protected (returns `Failure`)
- `markDeleted()` succeeds even if already readonly (returns `Success`)

This allows soft-delete of archived documents while preventing accidental modification of readonly documents.

### Component Types Reference

| Member | Type | Factory Call |
|--------|------|--------------|
| `_document` | `ovlDocumentUPtr_t` | `overlay<ovlDocument>` |
| `_comments` | `ovlCommentsUPtr_t` | `overlay<ovlComments, array_t>` |
| `_origin` | `ovlOriginUPtr_t` | `overlay<ovlOrigin>` |
| `_version` | `ovlVersionUPtr_t` | `overlay<ovlVersion, std::string>` |
| `_entities` | `ovlEntitiesUPtr_t` | `overlay<ovlEntities, array_t>` |
| `_configurations` | `ovlConfigurationsUPtr_t` | `overlay<ovlConfigurations, array_t>` |
| `_changelog` | `ovlChangeLogUPtr_t` | `overlay<ovlChangeLog, std::string>` |
| `_bookkeeping` | `ovlBookkeepingUPtr_t` | `overlay<ovlBookkeeping>` |
| `_id` | `ovlIdUPtr_t` | `overlay<ovlId>` |
| `_collection` | `ovlCollectionUPtr_t` | `overlay<ovlCollection, std::string>` |
| `_aliases` | `ovlAliasesUPtr_t` | `overlay<ovlAliases>` |
| `_runs` | `ovlRunsUPtr_t` | `overlay<ovlRuns, array_t>` |
| `_attachments` | `ovlAttachmentsUPtr_t` | `overlay<ovlAttachments, array_t>` |
| `_configurationtype` | `ovlConfigurationTypeUPtr_t` | `overlay<ovlConfigurationType, std::string>` |

## See Also

- [ovlDatabaseRecord.h](./ovlDatabaseRecord.h.md) - Class declaration and type aliases
- [ovlKeyValue.h](./ovlKeyValue.h.md) - Base class
- [ovlBookkeeping.h](./ovlBookkeeping.h.md) - State and update tracking
- [ovlDocument.h](./ovlDocument.h.md) - User data component
- [ovlFixedList.h](./ovlFixedList.h.md) - List container template
- [ovlMovableList.h](./ovlMovableList.h.md) - Movable list container (for aliases)
- [common.h](./common.h.md) - Comparison masks and result types
