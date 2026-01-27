# ovlDocument.cpp

**Path:** `artdaq-database/Overlay/ovlDocument.cpp`

**Implements:** [ovlDocument.h](./ovlDocument.h.md)

**Purpose:** Implements the `ovlDocument` class, providing component overlay creation, efficient swap operations, document clearing, serialization, and comparison for user-facing document content consisting of data, metadata, and search components.

## Implementation Overview

This file implements the document overlay that wraps a JSON object containing user content. The implementation creates sub-overlays for each component (data, metadata, search) during construction, provides efficient swap-based content replacement, and implements comparison logic that respects masking flags.

## Key Algorithms

### Component Overlay Creation

**Steps:**
1. Initialize base class `ovlKeyValue` with the document key and value
2. Create data overlay using `overlay<ovlData>()` factory function with `jsonliteral::data` key
3. Create metadata overlay using `overlay<ovlMetadata>()` factory function with `jsonliteral::metadata` key
4. Create search overlay using `overlay<ovlSearches, array_t>()` factory function with `jsonliteral::search` key

**Why this approach:** Using the `overlay<>()` factory function ensures consistent initialization and proper type deduction for each component. The template parameter specifies the overlay type and optionally the underlying JSON type (array_t for search).

### Document Comparison

**Steps:**
1. Create output stream for error accumulation
2. Record the initial stream position to detect if errors were added
3. Compare data components using `operator==` on `*_data`
4. If data differs, append descriptive error message
5. Compare metadata components using `operator==` on `*_metadata`
6. If metadata differs, append descriptive error message
7. Return `Success()` if no errors accumulated, otherwise return `Failure()` with details

**Why this approach:** Aggregating all differences into a single error message provides comprehensive feedback for debugging comparison failures, showing exactly which components differ.

## Internal Functions

### Constructor

```cpp
ovlDocument::ovlDocument(object_t::key_type const& key, value_t& document)
    : ovlKeyValue(key, document),
      _data{overlay<ovlData>(document, jsonliteral::data)},
      _metadata{overlay<ovlMetadata>(document, jsonliteral::metadata)},
      _search{overlay<ovlSearches, array_t>(document, jsonliteral::search)} {}
```

**Brief:** Initializes the document overlay by creating sub-overlays for all three components using the `overlay<>()` factory function.

**Called by:** Client code creating document overlays

**Purpose:** Wraps the JSON document structure with type-safe overlays for each component.

### Accessor Methods

```cpp
ovlData& ovlDocument::data() { return *_data; }
ovlData const& ovlDocument::data() const { return *_data; }
ovlMetadata& ovlDocument::metadata() { return *_metadata; }
ovlMetadata const& ovlDocument::metadata() const { return *_metadata; }
```

**Brief:** Dereference the unique pointers to return references to the sub-overlays.

**Called by:** Client code accessing document components

**Purpose:** Provides direct access to component overlays without ownership transfer.

### Swap Methods

```cpp
void ovlDocument::swap(ovlDataUPtr_t& data) {
  confirm(data);
  std::swap(_data, data);
}

void ovlDocument::swap(ovlMetadataUPtr_t& metadata) {
  confirm(metadata);
  std::swap(_metadata, metadata);
}
```

**Brief:** Validates input is non-null and performs pointer swap for O(1) content replacement.

**Called by:** Client code replacing document components

**Purpose:** Enables efficient content replacement while providing the old content back to the caller through the reference parameter.

### make_empty Method

```cpp
void ovlDocument::make_empty() {
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

**Brief:** Creates new empty structures for each component and swaps them in to replace existing content.

**Called by:** Client code needing to clear document content

**Purpose:** Resets document to empty state by replacing each component with a new empty overlay.

**Note:** There is a minor inconsistency in the implementation where the search overlay uses `jsonliteral::metadata` instead of `jsonliteral::search` as the key. This appears to be a bug in the source code.

### to_string Method

```cpp
std::string ovlDocument::to_string() const {
  std::ostringstream oss;
  oss << quoted_(jsonliteral::document) << ":{\n";
  oss << debrace(_data->to_string()) << ",\n";
  oss << debrace(_metadata->to_string()) << ",\n";
  oss << debrace(_search->to_string()) << "\n";
  oss << "}";
  return oss.str();
}
```

**Brief:** Serializes the document by combining serialized sub-components with proper JSON formatting.

**Called by:** Debugging and logging code

**Purpose:** Produces human-readable JSON representation of the document. Uses `debrace()` to remove outer braces from sub-component output so they can be properly nested.

### Comparison Operator

```cpp
result_t ovlDocument::operator==(ovlDocument const& other) const {
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

**Brief:** Compares data and metadata components, aggregating error messages for all differences found.

**Called by:** Client code comparing documents for equality

**Purpose:** Provides detailed comparison results showing exactly which components differ.

**Note:** The search array is intentionally not compared because it contains derived/computed data.

## Performance Considerations

- **O(1) swap operations:** Content replacement uses pointer swaps rather than deep copies
- **Lazy evaluation:** Sub-overlays are created during construction but no deep copies of JSON data occur
- **Memory overhead:** Three unique pointers per document overlay instance (24 bytes on 64-bit systems)

## Error Handling Strategy

- Uses `confirm()` macro for precondition validation (null pointer checks)
- Returns `result_t` pairs with success/failure status and error messages
- Aggregates multiple errors into single detailed failure message

## Testing Notes

- **Unit tests:** Located in `test/Overlay/` directory
- **Key test cases:**
  - Document construction from JSON
  - Data and metadata access
  - Swap operations preserving old content
  - Comparison with and without masking
  - Empty document creation via make_empty()

## Maintenance Notes

### Search Not Compared

The search array is intentionally excluded from the comparison operator because:
1. Search data is typically derived from the data component
2. Including it would cause false negatives when search indices differ
3. Search content is for query optimization, not semantic equality

### Swap Semantics

After a swap operation:
- The document contains the new content
- The caller receives the old content via the reference parameter

This enables:
- Transaction-like semantics (rollback by swapping back)
- Old value inspection before discarding
- Efficient content migration between documents

### Helper Functions Used

- `quoted_()` - Wraps a string in double quotes for JSON output
- `debrace()` - Removes outer braces from a JSON string for proper nesting

## See Also

- [ovlDocument.h](./ovlDocument.h.md) - Class declaration and type aliases
- [ovlKeyValue.cpp](./ovlKeyValue.cpp.md) - Base class implementation
- [common.h](./common.h.md) - Comparison mask constants and helper functions
