# ovlDocument.h

## File Overview

Overlay class representing the user-facing content of a database record, consisting of data (actual configuration) and metadata (descriptive information). Also includes search metadata for query optimization.

**Location**: `/home/user/artdaq-database/artdaq-database/Overlay/ovlDocument.h`

## Purpose

Separates user content from system metadata:
- **Data**: The actual configuration (DAQ settings, trigger configuration, etc.)
- **Metadata**: Descriptive information (tags, descriptions, etc.)
- **Search**: Indexable fields for efficient querying

## Class Definition

```cpp
class ovlDocument final : public ovlKeyValue {
 public:
  ovlDocument(object_t::key_type const& key, value_t& document);

  // accessors
  ovlData& data();
  ovlData const& data() const;

  ovlMetadata& metadata();
  ovlMetadata const& metadata() const;

  // utils
  void swap(ovlDataUPtr_t& data);
  void swap(ovlMetadataUPtr_t& metadata);

  void make_empty();

  // overrides
  std::string to_string() const override;

  // ops
  result_t operator==(ovlDocument const&) const;

 private:
  ovlDataUPtr_t _data;
  ovlMetadataUPtr_t _metadata;
  ovlSearchesUPtr_t _search;
};
```

## Type Aliases

```cpp
using ovlData = ovlKeyValueWithMask<DOCUMENT_COMPARE_MUTE_DATA>;
using ovlMetadata = ovlKeyValueWithMask<DOCUMENT_COMPARE_MUTE_METADATA>;
using ovlSearch = ovlKeyValue;
using ovlSearches = ovlFixedList<ovlSearch, DOCUMENT_COMPARE_MUTE_SEARCH>;
```

## JSON Structure

```json
{
  "document": {
    "data": {
      "daq_configuration": {
        "trigger_rate": 1000,
        "buffer_size": 1024,
        "channels": [0, 1, 2, 3]
      }
    },
    "metadata": {
      "description": "Production DAQ configuration for Run 123",
      "tags": ["production", "validated", "run123"],
      "author": "operator"
    },
    "search": [
      {"field": "trigger_rate", "value": "1000"},
      {"field": "tags", "value": "production"}
    ]
  }
}
```

## Implementation

### Constructor
```cpp
ovlDocument::ovlDocument(object_t::key_type const& key, value_t& document)
    : ovlKeyValue(key, document),
      _data{overlay<ovlData>(document, jsonliteral::data)},
      _metadata{overlay<ovlMetadata>(document, jsonliteral::metadata)},
      _search{overlay<ovlSearches, array_t>(document, jsonliteral::search)} {}
```

Creates overlays for all three components.

### Data Access
```cpp
ovlData& data() { return *_data; }
ovlData const& data() const { return *_data; }
```

### Metadata Access
```cpp
ovlMetadata& metadata() { return *_metadata; }
ovlMetadata const& metadata() const { return *_metadata; }
```

### Swapping Content
```cpp
void swap(ovlDataUPtr_t& data) {
  confirm(data);
  std::swap(_data, data);
}

void swap(ovlMetadataUPtr_t& metadata) {
  confirm(metadata);
  std::swap(_metadata, metadata);
}
```

**Use Case**: Efficiently replace document content without copying.

### Making Empty
```cpp
void make_empty() {
  value_t tmp1 = object_t{};
  auto data = overlay<ovlData>(tmp1, jsonliteral::data);
  std::swap(_data, data);

  value_t tmp2 = object_t{};
  auto metadata = overlay<ovlMetadata>(tmp2, jsonliteral::metadata);
  std::swap(_metadata, metadata);

  value_t tmp3 = array_t{};
  auto search = overlay<ovlSearches, array_t>(tmp3, jsonliteral::metadata);
  std::swap(_search, search);
}
```

**Purpose**: Clear all document content, replacing with empty objects.

### Comparison
```cpp
result_t operator==(ovlDocument const& other) const {
  std::ostringstream oss;
  oss << "\nUser data disagree.";
  auto noerror_pos = oss.tellp();

  auto result = *_data == *other._data;

  if (!result.first) {
    oss << "\n  Data are different:\n  " << result.second;
  }

  result = *_metadata == *other._metadata;

  if (!result.first) {
    oss << "\n  Metadata are different:\n  " << result.second;
  }

  if (oss.tellp() == noerror_pos) {
    return Success();
  }

  return Failure(oss);
}
```

**Comparison**: Checks both data and metadata. Search is not compared.

## Usage Examples

### Accessing Data
```cpp
ovlDatabaseRecord record{json};

auto& document = record.document();
auto& data = document.data();

// Access/modify data
auto& dataObj = data.object_value();
dataObj["trigger_rate"] = 2000;
dataObj["buffer_size"] = 2048;
```

### Accessing Metadata
```cpp
auto& metadata = document.metadata();
auto& metaObj = metadata.object_value();

// Set metadata
metaObj["description"] = "Updated configuration";
metaObj["tags"] = array_t{"production", "validated", "updated"};
metaObj["author"] = "admin";
```

### Swapping Content
```cpp
// Create new data
value_t newDataJson = object_t{};
newDataJson["trigger_rate"] = 3000;
auto newData = overlay<ovlData>(newDataJson, "data");

// Swap into document
document.swap(newData);
// Old data now in newData, new data in document
```

### Clearing Document
```cpp
document.make_empty();
// Data and metadata now empty objects
```

## Design Rationale

### Separation of Data and Metadata

**Why separate?**
- **Clarity**: Configuration vs. description
- **Querying**: Can search metadata without loading full data
- **Access Control**: Different permissions for data vs. metadata
- **Masking**: Can compare ignoring metadata or data

### Search Array

**Purpose**:
- **Performance**: Pre-indexed fields for fast queries
- **Flexibility**: Can index nested or computed values
- **Database Optimization**: Some backends need explicit search fields

### Swap vs. Copy

**Why swap instead of assignment?**
- **Efficiency**: O(1) pointer swap vs. O(n) deep copy
- **Unique Ownership**: Maintains unique_ptr semantics
- **Old Value Retrieval**: Can access swapped-out value

## Common Patterns

### Setting New Configuration
```cpp
auto& data = document.data().object_value();
data.clear();
data["new_config"] = loadConfiguration("new_config.json");
```

### Adding Metadata
```cpp
auto& meta = document.metadata().object_value();
meta["description"] = "Configuration description";
meta["version"] = "2.0";
meta["created_by"] = getUsername();
```

### Comparing Content Only
```cpp
// Ignore metadata differences
useCompareMask(DOCUMENT_COMPARE_MUTE_METADATA);
auto result = doc1 == doc2;  // Only compares data
```

## Related Files

- **ovlKeyValue.h** - Base class
- **ovlKeyValueWithMask.h** - For ovlData and ovlMetadata
- **ovlFixedList.h** - For search array
- **ovlDatabaseRecord.h** - Uses ovlDocument
- **common.h** - Mask constants

## Notes

- Final class
- Search array not included in comparison
- Swap methods enable efficient content replacement
- make_empty() creates new empty structures
- Data and metadata can be independently masked in comparisons
- No automatic search index generation - must be manually populated
