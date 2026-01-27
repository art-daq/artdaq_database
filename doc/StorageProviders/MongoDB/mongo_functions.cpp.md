# mongo_functions.cpp

**Path:** `artdaq-database/StorageProviders/MongoDB/mongo_functions.cpp`

**Purpose:** Provides utility functions for MongoDB operations, specifically for extracting values from BSON documents and building MongoDB aggregation pipelines from JSON specifications.

## Implementation Overview

This file implements helper functions that bridge the gap between artdaq-database's JSON-based query interface and MongoDB's native BSON/aggregation framework. The functions handle:
- Value extraction from BSON documents by key
- Conversion of JSON pipeline specifications to `mongocxx::pipeline` objects

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/BasicTypes/basictypes.h` | Core type definitions |
| `artdaq-database/StorageProviders/MongoDB/mongo_json.h` | JSON/BSON conversion utilities (`compat::to_json`) |
| `artdaq-database/StorageProviders/MongoDB/provider_mongodb.h` | MongoDB provider types |
| `artdaq-database/StorageProviders/common.h` | Common storage utilities |
| `artdaq-database/JsonDocument/JSONDocumentBuilder.h` | Document construction |
| `artdaq-database/SharedCommon/sharedcommon_common.h` | Common utilities including `runtime_error` |
| `<bsoncxx/*>` | BSON document handling (builders, views, types) |
| `<mongocxx/*>` | MongoDB C++ driver (client, pipeline, options) |

## Functions

### `extract_value_from_document(bsoncxx::document::value const& document, std::string const& key) -> bsoncxx::types::bson_value::view`

**Brief:** Extracts a value from a BSON document by its key name, throwing an error if the key is not found.

**Parameters:**
- `document` - The BSON document to search
- `key` - The key name to find

**Returns:** A view of the BSON value associated with the key

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_error("MongoDB")` | When the key is not found in the document |

**Thread Safety:** Safe (operates on const document)

**Complexity:** O(n) where n is the number of keys in the document

**Example:**
```cpp
#include "artdaq-database/StorageProviders/MongoDB/mongo_json.h"
#include <bsoncxx/json.hpp>
#include <iostream>

void extractValue() {
  auto doc = bsoncxx::from_json(R"({"name": "test", "value": 42})");

  try {
    auto name_value = extract_value_from_document(doc, "name");
    // name_value is a view to the "test" string
    std::cout << "Found: " << compat::to_json(name_value) << std::endl;
  } catch (const artdaq::database::runtime_error& e) {
    std::cerr << "Key not found: " << e.what() << std::endl;
  }
}
```

**Implementation:**
```cpp
bsoncxx::types::bson_value::view extract_value_from_document(
    bsoncxx::document::value const& document, std::string const& key) {
  auto view = document.view();
  auto element = view.find(key);

  if (element == view.end()) {
    throw runtime_error("MongoDB") << "Search JSONDocument is missing the \""
                                   << key << "\" element.";
  }

  return element->get_value();
}
```

---

### `pipeline_from_document(bsoncxx::document::value const& document) -> mongocxx::pipeline`

**Brief:** Converts a JSON aggregation pipeline specification into a `mongocxx::pipeline` object for executing MongoDB aggregation queries.

**Parameters:**
- `document` - BSON document containing a "pipeline" array with aggregation stages

**Returns:** A configured `mongocxx::pipeline` ready for execution

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_error("MongoDB")` | When a pipeline stage has more than one key-value pair |

**Thread Safety:** Safe (operates on const document)

**Supported Aggregation Stages:**
| Stage | MongoDB Operator | Description |
|-------|------------------|-------------|
| `$match` | `pipeline.match()` | Filter documents by criteria |
| `$project` | `pipeline.project()` | Shape output documents (include/exclude fields) |
| `$group` | `pipeline.group()` | Group documents and aggregate values |
| `$sort` | `pipeline.sort()` | Order results by field values |
| `$addFields` | `pipeline.add_fields()` | Add computed fields to documents |
| `$unwind` | `pipeline.unwind()` | Deconstruct arrays into separate documents |

**Input Format:**
```json
{
  "pipeline": [
    {"$match": {"status": "active"}},
    {"$project": {"name": 1, "value": 1}},
    {"$sort": {"name": 1}}
  ]
}
```

**Example:**
```cpp
#include <bsoncxx/json.hpp>
#include <mongocxx/pipeline.hpp>
#include <iostream>

void buildPipeline() {
  // Create a pipeline specification
  auto spec = bsoncxx::from_json(R"({
    "pipeline": [
      {"$match": {"collection": "Detectors"}},
      {"$project": {"_id": 1, "name": 1}},
      {"$sort": {"name": 1}}
    ]
  })");

  try {
    auto pipeline = pipeline_from_document(spec);
    // Use pipeline with collection.aggregate(pipeline)
  } catch (const artdaq::database::runtime_error& e) {
    std::cerr << "Invalid pipeline: " << e.what() << std::endl;
  }
}
```

## Key Algorithms

### Pipeline Stage Processing

**Steps:**
1. Extract "pipeline" array from the document using `view.find("pipeline")->get_array().value`
2. Iterate over each element in the array
3. For each stage:
   a. Get the document view from the array element
   b. Verify exactly one key-value pair using `std::distance()`
   c. Extract the stage operator key (e.g., "$match")
   d. Build the stage document using `bsoncxx::builder::core`
   e. Call the appropriate `pipeline` method based on the operator key
4. Return the configured pipeline

**Why this approach:** MongoDB aggregation pipelines require specific method calls for each stage type. This function maps JSON specifications to the mongocxx API while validating the input format.

### Stage Validation

Each stage must have exactly one key-value pair:

```cpp
auto length = std::distance(stage.cbegin(), stage.cend());
if (length != 1) {
  throw runtime_error("MongoDB") << "Invalid MongoDB search; an aggregation pipeline must have only one"
                                 << " key-value-pair on each stage, stage=<" << compat::to_json(stage)
                                 << ">, length=" << length << ".";
}
```

**Why this validation:** MongoDB aggregation stages are defined as single-key documents where the key is the operator. Multiple keys would be ambiguous and indicate malformed input.

## Implementation Details

### BSON Value Extraction

The `extract_value_from_document` function uses the mongocxx view API:

```cpp
auto view = document.view();
auto element = view.find(key);

if (element == view.end()) {
  throw runtime_error("MongoDB") << "Search JSONDocument is missing...";
}

return element->get_value();
```

### Document Building for Pipeline Stages

Pipeline stage documents are built using `bsoncxx::builder::core`:

```cpp
auto document = [&kvp]() {
  auto doc = bsoncxx::builder::core(false);  // false = document (not array)
  doc.concatenate(kvp->get_document());
  return doc;
};
```

The `false` parameter indicates this is a document builder (not an array builder).

## Compatibility

### `compat::to_json()`

The code uses `compat::to_json()` for error message formatting, which wraps the appropriate bsoncxx JSON conversion for the MongoDB driver version in use.

## Performance Considerations

- **`extract_value_from_document`:** O(n) linear search through document keys
- **`pipeline_from_document`:** O(n * m) where n is stages and m is keys per stage (validation)
- Both functions return views or moved objects to avoid unnecessary copies
- Pipeline construction involves building intermediate BSON documents

## Error Handling Strategy

1. **Key not found:** Throws with descriptive message including the missing key name
2. **Invalid stage format:** Throws with the problematic stage JSON for debugging
3. **Unsupported operators:** Silently skipped (no error thrown) - allows forward compatibility

## Testing Notes

- **Unit tests:** `test/StorageProviders/MongoDB/` directory
- **Key test cases:**
  - Extract existing key
  - Extract missing key (error case)
  - Valid pipeline conversion
  - Invalid pipeline stage (multiple keys)
  - All supported aggregation operators
  - Unsupported operators (should be silently skipped)

## Maintenance Notes

### Adding New Aggregation Stages

To support additional MongoDB aggregation operators:

1. Add a new `else if` block in `pipeline_from_document`:
```cpp
} else if (key == "$lookup") {
  pipeline.lookup(document().view_document());
}
```

2. Ensure the mongocxx API supports the corresponding method
3. Update the supported stages table in documentation

### Currently Unsupported Operators

The following operators (and others not listed) are silently ignored:
- `$lookup` - Join with other collections
- `$limit` - Limit result count
- `$skip` - Skip documents
- `$count` - Count documents
- `$facet` - Multi-faceted aggregation
- `$out` - Write results to collection

Consider adding these if required by artdaq-database query operations.

## See Also

- [provider_mongodb.h.md](./provider_mongodb.h.md) - MongoDB provider class
- [mongo_json.h.md](./mongo_json.h.md) - JSON/BSON conversion utilities
- [External: MongoDB Aggregation](https://www.mongodb.com/docs/manual/aggregation/) - Aggregation pipeline documentation
- [External: mongocxx pipeline](http://mongocxx.org/api/current/classmongocxx_1_1pipeline.html) - mongocxx pipeline API
