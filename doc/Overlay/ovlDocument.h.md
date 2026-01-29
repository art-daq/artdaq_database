# ovlDocument.h

**Path:** `artdaq-database/Overlay/ovlDocument.h`

**Purpose:** Defines the `ovlDocument` overlay class that represents the user-facing content of a database record. This class provides type-safe access to the three main components of a document: data (actual configuration), metadata (descriptive information), and search (indexable fields). It separates user content from system-level bookkeeping information.

## Key Concepts

### Content Separation

The document overlay separates content into three distinct parts:
- **Data** - The actual configuration (DAQ settings, trigger configuration, etc.)
- **Metadata** - Descriptive information (tags, descriptions, author, etc.)
- **Search** - Indexable fields for efficient database querying

### Swap-Based Content Replacement

Instead of copying content, the class uses swap operations for efficient content replacement while maintaining unique ownership semantics. This allows O(1) content replacement rather than O(n) deep copying.

### Independent Masking

Data and metadata can be independently masked during comparisons using `DOCUMENT_COMPARE_MUTE_DATA` and `DOCUMENT_COMPARE_MUTE_METADATA` flags, allowing selective comparison of document content.

## Thread Safety

- **Thread-safe:** No
- **Concurrent access:** Not safe for concurrent access; external synchronization required
- **Locking:** No internal locking

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/Overlay/common.h` | Module foundation types, constants, and comparison masks |
| `artdaq-database/Overlay/ovlKeyValue.h` | Base class for key-value overlay pattern |
| `artdaq-database/Overlay/ovlKeyValueWithMask.h` | Maskable key-value overlay for data/metadata |
| `artdaq-database/Overlay/ovlFixedList.h` | Fixed-size list overlay for search array |

## Type Aliases

### `ovlData`

```cpp
using ovlData = ovlKeyValueWithMask<DOCUMENT_COMPARE_MUTE_DATA>;
```

**Brief:** Type alias for the data component overlay with comparison masking capability. When the `DOCUMENT_COMPARE_MUTE_DATA` flag is set, data comparisons return success without checking content.

### `ovlMetadata`

```cpp
using ovlMetadata = ovlKeyValueWithMask<DOCUMENT_COMPARE_MUTE_METADATA>;
```

**Brief:** Type alias for the metadata component overlay with comparison masking capability. When the `DOCUMENT_COMPARE_MUTE_METADATA` flag is set, metadata comparisons return success without checking content.

### `ovlSearch`

```cpp
using ovlSearch = ovlKeyValue;
```

**Brief:** Type alias for a single search entry overlay, used for indexable fields in the search array.

### `ovlSearches`

```cpp
using ovlSearches = ovlFixedList<ovlSearch, DOCUMENT_COMPARE_MUTE_SEARCH>;
```

**Brief:** Type alias for the search array overlay containing multiple search entries with maskable comparison.

### Unique Pointer Types

```cpp
using ovlDataUPtr_t = std::unique_ptr<ovlData>;
using ovlMetadataUPtr_t = std::unique_ptr<ovlMetadata>;
using ovlSearchUPtr_t = std::unique_ptr<ovlSearch>;
using ovlSearchesUPtr_t = std::unique_ptr<ovlSearches>;
using ovlDocumentUPtr_t = std::unique_ptr<ovlDocument>;
```

**Brief:** Unique pointer type aliases for managing overlay ownership with RAII semantics.

## Classes/Structures

### `ovlDocument`

A final overlay class that provides type-safe access to document content consisting of data, metadata, and search components. Inherits from `ovlKeyValue` to participate in the overlay pattern.

**Thread Safety:** Not thread-safe; external synchronization required for concurrent access.

#### Constructor

##### `ovlDocument(object_t::key_type const& key, value_t& document)`

**Brief:** Constructs a document overlay by wrapping an existing JSON object and creating sub-overlays for data, metadata, and search components.

**Parameters:**
- `key` - The JSON key for this document (typically "document")
- `document` - Reference to the JSON object containing data, metadata, and search fields

**Preconditions:**
- `document` must be a valid JSON object
- `document` must contain "data", "metadata", and "search" fields

**Postconditions:**
- Document overlay is initialized with sub-overlays for all three components
- Modifications through the overlay affect the underlying JSON

**Throws:**
| Exception | Condition |
|-----------|-----------|
| Assertion failure | If required fields are missing from the JSON object |

**Thread Safety:** Not thread-safe

#### Move Constructor and Destructor

##### `ovlDocument(ovlDocument&&) = default`

**Brief:** Default move constructor for transferring document overlay ownership.

##### `~ovlDocument() = default`

**Brief:** Default destructor that releases resources.

#### Methods

##### `data() -> ovlData&`

**Brief:** Returns a mutable reference to the data component overlay, allowing modification of the actual configuration content.

**Returns:** Mutable reference to the data overlay

**Thread Safety:** Not thread-safe

##### `data() const -> ovlData const&`

**Brief:** Returns a const reference to the data component overlay for read-only access to configuration content.

**Returns:** Const reference to the data overlay

**Thread Safety:** Not thread-safe

##### `metadata() -> ovlMetadata&`

**Brief:** Returns a mutable reference to the metadata component overlay, allowing modification of descriptive information.

**Returns:** Mutable reference to the metadata overlay

**Thread Safety:** Not thread-safe

##### `metadata() const -> ovlMetadata const&`

**Brief:** Returns a const reference to the metadata component overlay for read-only access to descriptive information.

**Returns:** Const reference to the metadata overlay

**Thread Safety:** Not thread-safe

##### `swap(ovlDataUPtr_t& data) -> void`

**Brief:** Exchanges the data component with the provided data overlay using pointer swap for O(1) efficiency.

**Parameters:**
- `data` - Unique pointer to the new data overlay; after swap contains the old data

**Preconditions:**
- `data` must be non-null (validated via `confirm()`)

**Postconditions:**
- Document contains the new data component
- `data` parameter now points to the old data component

**Throws:**
| Exception | Condition |
|-----------|-----------|
| Assertion failure | If `data` is null |

**Thread Safety:** Not thread-safe

##### `swap(ovlMetadataUPtr_t& metadata) -> void`

**Brief:** Exchanges the metadata component with the provided metadata overlay using pointer swap for O(1) efficiency.

**Parameters:**
- `metadata` - Unique pointer to the new metadata overlay; after swap contains the old metadata

**Preconditions:**
- `metadata` must be non-null (validated via `confirm()`)

**Postconditions:**
- Document contains the new metadata component
- `metadata` parameter now points to the old metadata component

**Throws:**
| Exception | Condition |
|-----------|-----------|
| Assertion failure | If `metadata` is null |

**Thread Safety:** Not thread-safe

##### `make_empty() -> void`

**Brief:** Clears all document content by replacing data, metadata, and search with new empty structures.

**Postconditions:**
- Data, metadata, and search components are replaced with empty overlays
- Original content is released

**Thread Safety:** Not thread-safe

##### `to_string() const -> std::string` [override]

**Brief:** Serializes the document to a formatted JSON string representation including all three components.

**Returns:** JSON string representation of the document

**Thread Safety:** Not thread-safe

**Example:**
```cpp
ovlDocument doc("document", jsonValue);
std::string serialized = doc.to_string();
// Output: "document":{
//   "data": {...},
//   "metadata": {...},
//   "search": [...]
// }
```

##### `operator==(ovlDocument const& other) const -> result_t`

**Brief:** Compares this document with another for equality, checking data and metadata components while respecting comparison masks.

**Parameters:**
- `other` - The document to compare against

**Returns:** `Success()` if documents are equal (considering masks), `Failure(message)` with details if different

**Postconditions:**
- Returns detailed error message describing which components differ

**Thread Safety:** Not thread-safe

**Note:** Search array is intentionally excluded from comparison as it contains derived/computed data.

## Relationship to Other Components

### In the Overlay Hierarchy

```
ovlKeyValue (base)
     ^
     |
ovlDocument (this class)
     |
     +-- ovlData (ovlKeyValueWithMask<DOCUMENT_COMPARE_MUTE_DATA>)
     +-- ovlMetadata (ovlKeyValueWithMask<DOCUMENT_COMPARE_MUTE_METADATA>)
     +-- ovlSearches (ovlFixedList<ovlSearch, DOCUMENT_COMPARE_MUTE_SEARCH>)
```

### Used By

- **ovlDatabaseRecord** - Contains document as one of its components representing user content

### JSON Structure

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
      "description": "Production DAQ configuration",
      "tags": ["production", "validated"],
      "author": "operator"
    },
    "search": [
      {"field": "trigger_rate", "value": "1000"}
    ]
  }
}
```

## See Also

- [ovlDocument.cpp](./ovlDocument.cpp.md) - Implementation details
- [ovlKeyValue.h](./ovlKeyValue.h.md) - Base class for overlay pattern
- [ovlKeyValueWithMask.h](./ovlKeyValueWithMask.h.md) - Maskable overlay template
- [ovlFixedList.h](./ovlFixedList.h.md) - Fixed-size list overlay
- [common.h](./common.h.md) - Comparison mask constants

## Notes for Developers

### Usage Examples

```cpp
#include "artdaq-database/Overlay/ovlDocument.h"

using namespace artdaq::database::overlay;

void accessDocumentContent() {
  // Assume jsonValue contains a properly structured document
  value_t jsonValue = /* ... */;

  // Create document overlay
  ovlDocument doc("document", jsonValue);

  // Access data
  auto& data = doc.data();
  auto& dataObj = data.object_value();
  dataObj["trigger_rate"] = 2000;  // Modify underlying JSON

  // Access metadata
  auto& meta = doc.metadata().object_value();
  meta["description"] = "Updated configuration";
}

void swapContent() {
  value_t docJson = /* ... */;
  ovlDocument doc("document", docJson);

  // Create new data
  value_t newDataJson = object_t{};
  newDataJson["new_field"] = "new_value";
  auto newData = overlay<ovlData>(newDataJson, "data");

  // Swap - O(1) operation
  doc.swap(newData);
  // newData now contains the old data
}

void compareDocuments() {
  value_t json1 = /* ... */;
  value_t json2 = /* ... */;

  ovlDocument doc1("document", json1);
  ovlDocument doc2("document", json2);

  // Compare with masking
  useCompareMask(DOCUMENT_COMPARE_MUTE_METADATA);
  auto result = doc1 == doc2;  // Ignores metadata differences

  if (!result.first) {
    std::cerr << "Documents differ: " << result.second << std::endl;
  }
}
```

### Common Pitfalls

- **Search not compared:** The search array is excluded from comparison since it is derived data. Do not rely on search content for equality checks.
- **make_empty() creates new structures:** This method does not clear content in-place; it replaces with new empty overlays.
- **No automatic search generation:** Search index must be manually populated; it is not automatically derived from data.

### Anti-patterns

```cpp
// DON'T do this: Assuming search is compared
auto result = doc1 == doc2;
// Search differences are ignored!

// DO this instead: Check search separately if needed
auto dataResult = doc1.data() == doc2.data();
// Then manually compare search if required
```
