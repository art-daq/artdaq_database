# ovlDatabaseRecord.h

## File Overview

The central class of the Overlay module, aggregating all components of a database record into a unified, type-safe interface. This class provides the complete representation of a configuration database record including user data, system metadata, tracking information, and access control.

**Location**: `/home/user/artdaq-database/artdaq-database/Overlay/ovlDatabaseRecord.h`

## Purpose

`ovlDatabaseRecord` is the primary interface for working with database records:

- **Complete Record Representation**: Aggregates all record components
- **Type-Safe Access**: Provides accessors for all subcomponents
- **Access Control**: Enforces readonly and deleted state checks
- **Modification Tracking**: Automatically records updates via bookkeeping
- **Comparison Support**: Deep equality comparison with selective masking
- **Component Management**: Add/remove configurations, entities, aliases, runs

## Class Definition

```cpp
class ovlDatabaseRecord final : public ovlKeyValue {
 public:
  ovlDatabaseRecord(value_t& record);

  // Component accessors
  ovlDocument& document();
  ovlComments& comments();
  ovlOrigin& origin();
  ovlVersion& version();
  ovlEntities& entities();
  ovlConfigurations& configurations();
  ovlBookkeeping& bookkeeping();
  ovlId& id();

  // Swap operations
  result_t swap(ovlDocumentUPtr_t& document);
  result_t swap(ovlCommentsUPtr_t& comments);
  result_t swap(ovlOriginUPtr_t& origin);
  result_t swap(ovlVersionUPtr_t& version);
  result_t swap(ovlConfigurationTypeUPtr_t& configtype);
  result_t swap(ovlCollectionUPtr_t& collection);
  result_t swap(ovlIdUPtr_t& id);

  // Overrides
  std::string to_string() const override;

  // Comparison
  result_t operator==(ovlDatabaseRecord const&) const;

  // State queries
  bool isReadonlyOrDeleted() const;

  // State modification
  result_t markReadonly();
  result_t markDeleted();

  // Configuration management
  result_t addConfiguration(ovlConfigurationUPtr_t& configuration);
  result_t removeConfiguration(ovlConfigurationUPtr_t& configuration);

  // Alias management
  result_t addAlias(ovlAliasUPtr_t& alias);
  result_t removeAlias(ovlAliasUPtr_t& alias);

  // Entity management
  result_t addEntity(ovlEntityUPtr_t& entity);
  result_t removeEntity(ovlEntityUPtr_t& entity);

  // Run management
  result_t addRun(ovlRunUPtr_t& run);

  // Version/Collection/Type setters
  result_t setVersion(ovlVersionUPtr_t& version);
  result_t setCollection(ovlCollectionUPtr_t& version);
  result_t setConfigurationType(ovlConfigurationTypeUPtr_t& configtype);

 private:
  ovlDocumentUPtr_t _document;
  ovlCommentsUPtr_t _comments;
  ovlOriginUPtr_t _origin;
  ovlVersionUPtr_t _version;
  ovlEntitiesUPtr_t _entities;
  ovlConfigurationsUPtr_t _configurations;
  ovlChangeLogUPtr_t _changelog;
  ovlBookkeepingUPtr_t _bookkeeping;
  ovlIdUPtr_t _id;
  ovlCollectionUPtr_t _collection;
  ovlAliasesUPtr_t _aliases;
  ovlRunsUPtr_t _runs;
  ovlAttachmentsUPtr_t _attachments;
  ovlConfigurationTypeUPtr_t _configurationtype;
};
```

## Type Aliases

The header defines numerous type aliases for record components:

```cpp
// Simple fields
using ovlVersion = ovlStringKeyValue<DOCUMENT_COMPARE_MUTE_VERSION>;
using ovlCollection = ovlStringKeyValue<DOCUMENT_COMPARE_MUTE_COLLECTION>;
using ovlConfigurationType = ovlStringKeyValue<DOCUMENT_COMPARE_MUTE_CONFIGURATION>;

// Timestamped entries
using ovlConfiguration = ovlKeyValueTimeStamp<DOCUMENT_COMPARE_MUTE_CONFIGURATION>;
using ovlEntity = ovlKeyValueTimeStamp<DOCUMENT_COMPARE_MUTE_CONFIGENTITY>;
using ovlRun = ovlKeyValueTimeStamp<DOCUMENT_COMPARE_MUTE_RUN>;
using ovlAlias = ovlKeyValueTimeStamp<DOCUMENT_COMPARE_MUTE_ALIAS, true, true>;

// Lists
using ovlConfigurations = ovlFixedList<ovlConfiguration, DOCUMENT_COMPARE_MUTE_CONFIGURATION>;
using ovlEntities = ovlFixedList<ovlEntity, DOCUMENT_COMPARE_MUTE_CONFIGENTITY>;
using ovlRuns = ovlFixedList<ovlRun, DOCUMENT_COMPARE_MUTE_RUN>;
using ovlComments = ovlFixedList<ovlComment, DOCUMENT_COMPARE_MUTE_COMMENTS>;
using ovlAttachments = ovlFixedList<ovlAttachment, DOCUMENT_COMPARE_MUTE_ATTACHMENT>;
using ovlAliases = ovlMovableList<ovlAlias, DOCUMENT_COMPARE_MUTE_ALIAS>;

// Other components (defined elsewhere)
// ovlDocument, ovlBookkeeping, ovlOrigin, ovlId
```

## Complete JSON Structure

```json
{
  "id": {"oid": "507f1f77bcf86cd799439011"},
  "collection": "DAQ_Configurations",
  "version": "v2_0_0",
  "origin": {
    "format": "json",
    "name": "daq_config.json",
    "source": "file",
    "created": "2025-01-15T10:00:00Z",
    "rawdatalist": []
  },
  "aliases": {
    "active": [
      {"name": "production", "assigned": "2025-01-15T10:00:00Z"}
    ],
    "history": []
  },
  "bookkeeping": {
    "isreadonly": false,
    "isdeleted": false,
    "created": "2025-01-15T10:00:00Z",
    "updates": []
  },
  "changelog": "Initial creation.\n",
  "entities": [
    {"name": "BoardReader01", "assigned": "2025-01-15T10:00:00Z"}
  ],
  "runs": [],
  "attachments": [],
  "configurationtype": "DAQ",
  "configurations": [
    {"name": "TriggerConfig", "assigned": "2025-01-15T10:00:00Z"}
  ],
  "comments": [
    {"linenum": 1, "value": "Production configuration"}
  ],
  "document": {
    "data": {"trigger_rate": 1000},
    "metadata": {"description": "DAQ configuration"},
    "search": []
  }
}
```

## Implementation

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

**Initialization**: Creates overlay wrappers for all 14 components, auto-initializing missing fields.

### Component Accessors

```cpp
ovlDocument& document() { return *_document; }
ovlBookkeeping& bookkeeping() { return *_bookkeeping; }
ovlId& id() { return *_id; }
// ... etc for all components
```

Simple accessors return references to managed components.

### State Management

```cpp
bool isReadonlyOrDeleted() const {
  return _bookkeeping->isReadonly() || _bookkeeping->isDeleted();
}

result_t markReadonly() {
  if (isReadonlyOrDeleted()) {
    return Failure(msg_IsReadonly);
  }

  _bookkeeping->markReadonly(true);

  return Success();
}

result_t markDeleted() {
  _bookkeeping->markDeleted(true);
  return Success();
}
```

**Note**: Can mark deleted even if already readonly.

### Configuration Management

```cpp
result_t addConfiguration(ovlConfigurationUPtr_t& configuration) {
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

result_t removeConfiguration(ovlConfigurationUPtr_t& configuration) {
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

**Pattern**:
1. Check readonly/deleted state
2. Perform operation on list
3. Post update to bookkeeping
4. Return result

**All modification methods follow this pattern**: entities, aliases, runs, version changes.

### Version Setting

```cpp
result_t setVersion(ovlVersionUPtr_t& version) {
  confirm(version);

  if (isReadonlyOrDeleted()) {
    return Failure(msg_IsReadonly);
  }

  if (_version->string_value() == version->string_value()) {
    return Success(msg_Ignored);  // No change
  }

  auto result = swap(version);

  if (!result.first) {
    return result;
  }

  auto update = std::string("setVersion");

  return _bookkeeping->postUpdate(update, _version);
}
```

**Optimization**: Checks if new version same as old, returns ignored status.

### Comparison

```cpp
result_t operator==(ovlDatabaseRecord const& other) const {
  std::ostringstream oss;
  oss << "\nDatabase records disagree.";
  auto noerror_pos = oss.tellp();

  // Compare all components
  auto result = *_document == *other._document;
  if (!result.first) oss << result.second;

  result = *_comments == *other._comments;
  if (!result.first) oss << result.second;

  // ... continues for all 14 components

  if (oss.tellp() == noerror_pos) {
    return Success();
  }

  return Failure(oss);
}
```

**Comprehensive Comparison**: Compares all components, aggregating error messages.

## Usage Examples

### Creating a New Record

```cpp
// Start with JSON (empty or template)
value_t recordJson = object_t{};

// Create record (auto-initializes)
ovlDatabaseRecord record{recordJson};

// Set basic info
record.version().string_value() = "v1_0_0";
record.collection().string_value() = "DAQ_Configurations";

// Set origin
auto& origin = record.origin();
origin.format() = "json";
origin.source() = "template";
origin.name() = "new_config";

// Add configuration data
auto& data = record.document().data().object_value();
data["trigger_rate"] = 1000;
data["buffer_size"] = 2048;

// Add metadata
auto& metadata = record.document().metadata().object_value();
metadata["description"] = "New DAQ configuration";
metadata["created_by"] = "admin";
```

### Adding Components

```cpp
// Add configuration
auto config = std::make_unique<ovlConfiguration>("configuration", configJson);
config->name("TriggerConfiguration");

auto result = record.addConfiguration(config);
if (!result.first) {
  std::cerr << "Failed to add: " << result.second << std::endl;
}

// Add entity
auto entity = std::make_unique<ovlEntity>("entity", entityJson);
entity->name("BoardReader01");

record.addEntity(entity);

// Add alias
auto alias = std::make_unique<ovlAlias>("alias", aliasJson);
alias->name("production");

record.addAlias(alias);
```

### Managing Record State

```cpp
// Check state before modification
if (record.isReadonlyOrDeleted()) {
  std::cout << "Cannot modify protected record" << std::endl;
  return;
}

// Make modifications
record.setVersion(newVersion);
record.addEntity(entity);

// Mark readonly for production
record.markReadonly();

// Future modifications will fail
auto result = record.addEntity(anotherEntity);
// result.first == false, result.second == msg_IsReadonly
```

### Comparing Records

```cpp
ovlDatabaseRecord record1{json1};
ovlDatabaseRecord record2{json2};

// Full comparison
useCompareMask(0);
auto result = record1 == record2;

if (result.first) {
  std::cout << "Records are identical" << std::endl;
} else {
  std::cout << "Differences:\n" << result.second << std::endl;
}

// Compare ignoring bookkeeping and timestamps
useCompareMask(DOCUMENT_COMPARE_MUTE_BOOKKEEPING |
               DOCUMENT_COMPARE_MUTE_TIMESTAMPS);
result = record1 == record2;
// Now compares only functional content
```

### Iterating Components

```cpp
// Iterate configurations
for (auto const& config : record.configurations()) {
  std::cout << "Config: " << config.name() << std::endl;
}

// Iterate entities
for (auto const& entity : record.entities()) {
  std::cout << "Entity: " << entity.name() << std::endl;
}

// Iterate comments
for (auto const& comment : record.comments()) {
  std::cout << "Line " << comment.linenum() << ": "
            << comment.text() << std::endl;
}
```

## Design Rationale

### Aggregation Pattern

**Why aggregate all components?**
- **Single Entry Point**: One class for all record operations
- **Type Safety**: Each component properly typed
- **Consistency**: All records have same structure
- **Auto-Initialization**: Missing fields created automatically

### Access Control Integration

**Why integrate bookkeeping checks?**
- **Safety**: Prevents accidental modification of protected records
- **Consistency**: All modifications go through same checks
- **Audit Trail**: All changes automatically recorded
- **Enforcement**: Cannot bypass readonly/deleted checks

### Update Tracking

**Why post-update pattern?**
- **Automatic Tracking**: No separate update recording needed
- **Completeness**: Every change is recorded
- **Consistency**: Update format is uniform
- **Debugging**: Complete history available

### Component Independence

**Why independent overlay objects?**
- **Modularity**: Each component managed separately
- **Flexibility**: Can swap/replace components
- **Testing**: Can test components in isolation
- **Performance**: Can compare subsets selectively

## Component Hierarchy

```
ovlDatabaseRecord
├── ovlId (unique identifier)
├── ovlCollection (collection name)
├── ovlVersion (version string)
├── ovlConfigurationType (configuration type)
├── ovlOrigin (provenance)
│   ├── format, name, source
│   ├── created timestamp
│   └── raw data list
├── ovlBookkeeping (state & history)
│   ├── readonly/deleted flags
│   ├── created timestamp
│   └── updates list
├── ovlChangeLog (human-readable log)
├── ovlDocument (user content)
│   ├── data (configuration)
│   ├── metadata (description)
│   └── search (indexes)
├── ovlConfigurations (list)
├── ovlEntities (list)
├── ovlAliases (movable list)
├── ovlRuns (list)
├── ovlAttachments (list)
└── ovlComments (list)
```

## Performance Considerations

- **Construction**: O(n) where n = total elements across all lists
- **Component Access**: O(1) reference return
- **Modifications**: O(1) operation + O(1) update post + O(n) list recreation
- **Comparison**: O(n) for all components
- **Serialization**: O(n) to traverse all components

## Thread Safety

**Not thread-safe**:
- References mutable JSON
- No internal synchronization
- Static comparison mask
- Concurrent modifications undefined

**Recommendations**:
- One record per thread
- External synchronization if shared
- Clone records for thread-local use

## Related Files

- **JSONDocumentOverlay.h** - Public API header
- **common.h** - Types and constants
- All overlay component headers (ovlDocument.h, ovlBookkeeping.h, etc.)

## Best Practices

1. **Check isReadonlyOrDeleted()** before all modifications
2. **Use result_t return values** - check first element before proceeding
3. **Set comparison mask explicitly** in tests
4. **Initialize components through methods** not direct JSON manipulation
5. **Use add/remove methods** for lists, not direct array access
6. **Mark readonly** for production/archived records
7. **Post meaningful updates** with descriptive event names
8. **Serialize for debugging** - `to_string()` shows complete structure

## Common Patterns

### Safe Modification Wrapper
```cpp
result_t safeModify(ovlDatabaseRecord& record,
                    std::function<result_t()> modification) {
  if (record.isReadonlyOrDeleted()) {
    return Failure("Record is protected");
  }

  return modification();
}
```

### Bulk Operations
```cpp
// Add multiple entities
std::vector<std::string> entityNames = {"BR01", "BR02", "BR03"};
for (auto const& name : entityNames) {
  auto entity = std::make_unique<ovlEntity>("entity", json);
  entity->name(name);
  record.addEntity(entity);
}
```

### Cloning Content
```cpp
// Clone document from one record to another
auto docCopy = std::make_unique<ovlDocument>("document", json);
// ... populate docCopy ...
record.swap(docCopy);
```

## Notes

- Final class (cannot be derived from)
- Central class of the Overlay module
- Auto-initializes 14 components
- Enforces access control via bookkeeping
- Comparison supports extensive masking
- All modifications tracked automatically
- Readonly enforcement is policy-based
- No component can be null (all initialized)
- Used by Configuration API as primary interface
- Serialization produces complete JSON representation
