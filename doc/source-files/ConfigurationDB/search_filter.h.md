# search_filter.h

**Path:** `artdaq-database/ConfigurationDB/search_filter.h`

**Purpose:** Defines the `filters` class, a minimal wrapper providing stream-based filter interface for search operations. This is a legacy/placeholder implementation that has been largely superseded by JSON-based query filters in the modern ConfigurationDB API.


**Status:** Legacy - prefer using `queryFilter()` methods in operation classes for new code.

## Key Concepts

### Stream-Based Filtering (Legacy)

The `filters` class represents an early approach to search filtering using C++ input streams. This approach has been superseded by:

1. **JSON Query Filters**: String-based filters passed to operation classes
2. **Option-Based Filtering**: Filter criteria embedded in operation options

### Modern Alternative

New code should use the query filter approach available in operation classes:

```cpp
// Modern approach (preferred)
ManageDocumentOperation opts("myapp");
opts.queryFilter(R"({"version": "v1.0", "entity": "detector"})");

// Legacy approach (this class)
filters f;
std::istream& stream = f.filter();  // Less flexible, harder to construct
```

## Thread Safety

- **Thread-safe:** No
- **Concurrent access:** Not designed for concurrent use
- **Locking:** None

The class stores a reference to an external stream, so thread safety depends on the underlying stream's thread safety.

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/ConfigurationDB/common.h` | Common utilities, std::istream, std::tuple |

## Classes

### `filters`

A minimal wrapper class that provides access to a stream-based filter. Currently implemented as a tuple containing a single stream reference.

**Thread Safety:** Not thread-safe - depends on underlying stream

#### Type Aliases

```cpp
using filter_type = std::istream;
```

**Brief:** Defines the underlying filter type as a standard input stream.

#### Methods

##### `filter() -> filter_type&`

**Brief:** Returns a reference to the underlying filter stream.

**Returns:** Reference to `std::istream` containing filter data

**Thread Safety:** Not thread-safe

**Example:**
```cpp
#include "artdaq-database/ConfigurationDB/search_filter.h"

using namespace artdaq::database::configuration;

void useFilter(filters& f) {
  std::istream& stream = f.filter();
  // Read filter criteria from stream...
}
```

#### Private Members

```cpp
std::tuple<filter_type&> filters;
```

**Purpose:** Storage for the filter stream reference using tuple.

**Note:** The naming collision between the class name `filters` and the member `filters` is a code smell that suggests this class was minimally implemented or is a placeholder.

## Usage Examples

### Legacy Pattern (Not Recommended)

```cpp
#include "artdaq-database/ConfigurationDB/search_filter.h"
#include <sstream>

using namespace artdaq::database::configuration;

void legacySearch() {
  std::istringstream filter_stream(R"({"version": "v1.0"})");

  // Note: This class doesn't provide a way to set the stream,
  // making it difficult to use in practice
  // filters f;  // How do we inject filter_stream?
}
```

### Modern Pattern (Recommended)

```cpp
#include "artdaq-database/ConfigurationDB/options_operation_managedocument.h"

using namespace artdaq::database::configuration;

void modernSearch() {
  ManageDocumentOperation opts("myapp");

  // Set filter as JSON string - much more flexible
  opts.queryFilter(R"({"version": "v1.0"})");

  // Or use individual setters
  opts.version("v1.0");
  opts.entity("detector_001");

  // Execute search
  auto result = search_collection(opts.to_string());
}
```

## Relationship to Other Components

```
search_filter.h (this file - legacy)
        |
        | largely replaced by
        v
options_operation_base.h
        |
        | queryFilter() method
        v
JSON-based query strings
```

- **options_operation_base.h**: Provides `queryFilter()` method that accepts JSON strings
- **dboperation_searchcollection.h**: Uses JSON filters for collection searches
- **common.h**: Provides common includes including stream types

## Common Pitfalls

### Limited Usability

```cpp
// Problem: No obvious way to construct with a stream
filters f;  // Default constructor - what stream does it reference?

// The class appears incomplete or was intended for a different pattern
// that was never fully implemented
```

### Name Collision

```cpp
class filters {
  // ...
private:
  std::tuple<filter_type&> filters;  // Same name as class!
};
```

This naming is confusing and suggests the implementation was rushed or placeholder.

## Notes for Developers

### Migration Guidance

If you encounter code using this class:

1. Identify what filter criteria is being passed
2. Convert to JSON string format
3. Use `queryFilter()` on the appropriate operation class

**Before (legacy):**
```cpp
// Unclear how this was intended to work
filters f;
performSearch(f);
```

**After (modern):**
```cpp
ManageDocumentOperation opts("app");
opts.queryFilter(R"({"collection": "configs", "version": "v1.0"})");
auto result = search_collection(opts.to_string());
```

### Future Considerations

This class should be considered for:
- Deprecation if no longer used
- Removal in a future major version
- Documentation as legacy-only

## See Also

- [options_operation_base.h](./options_operation_base.h.md) - Modern filter handling via `queryFilter()`
- [dboperation_searchcollection.h](./dboperation_searchcollection.h.md) - Search operations using JSON filters
- [common.h](./common.h.md) - Common utilities and includes

---

**Documentation generated for artdaq-database ConfigurationDB module**
