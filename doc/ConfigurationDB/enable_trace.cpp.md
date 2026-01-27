# enable_trace.cpp

**Path:** `artdaq-database/ConfigurationDB/enable_trace.cpp`

**Purpose:** Provides a centralized function to enable TRACE debugging for all components of the artdaq-database ConfigurationDB module. This utility file activates comprehensive trace logging across the entire database subsystem with a single function call, making it invaluable for debugging complex configuration database operations.


## Key Concepts

### TRACE Logging System
TRACE is a high-performance logging system used throughout the ARTDAQ ecosystem. It provides multiple logging levels and can be configured at runtime to enable or disable specific trace points without recompilation. Each component has its own trace name, allowing fine-grained control over what gets logged.

### Centralized Debug Enablement
Rather than requiring developers to know and call each individual component's debug enablement function, this file provides a single entry point that enables all trace output. This is particularly useful when debugging complex operations that span multiple components.

## Thread Safety

- **Thread-safe:** No
- **Concurrent access:** Should be called once during initialization before any concurrent operations
- **Locking:** None - modifies global TRACE state without synchronization

Calling `enable_trace()` from multiple threads simultaneously is undefined behavior. The function should be called once at application startup, before spawning worker threads.

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/BasicTypes/basictypes.h` | Basic type definitions and wrappers |
| `artdaq-database/ConfigurationDB/configurationdb.h` | Main ConfigurationDB definitions and debug functions |
| `artdaq-database/ConfigurationDB/dispatch_common.h` | Common dispatch utilities with debug declarations |
| `artdaq-database/ConfigurationDB/options_operations.h` | Operation options classes with debug functions |
| `artdaq-database/DataFormats/Json/json_reader.h` | JSON parsing utilities |
| `artdaq-database/JsonDocument/JSONDocumentBuilder.h` | Document builder with debug functions |
| `artdaq-database/StorageProviders/FileSystemDB/provider_filedb.h` | FileSystemDB provider with debug functions |
| `artdaq-database/StorageProviders/FileSystemDB/provider_filedb_index.h` | FileSystemDB index with debug functions |
| `artdaq-database/StorageProviders/MongoDB/provider_mongodb.h` | MongoDB provider with debug functions |

## Implementation Overview

This file defines a single function `enable_trace()` in the `artdaq::database::configuration::json` namespace that systematically enables TRACE debugging for all major components of the artdaq-database system. The function acts as a master switch that calls individual `debug::*()` functions from each module.

The enabled components span the entire stack from high-level operations down to storage providers:
- ConfigurationDB operations (export/import, alias management, configuration management)
- Detail implementations of those operations
- Options handling
- Storage provider dispatch layers (MongoDB, UconDB, FileSystemDB)
- Storage provider implementations
- Document handling (JSONDocument, JSONDocumentBuilder)

## Functions

### `enable_trace() -> void`

**Brief:** Enables TRACE debugging for all components of the artdaq-database ConfigurationDB module and its dependencies, providing comprehensive logging output for debugging purposes.

**Namespace:** `artdaq::database::configuration::json`

**Parameters:** None

**Preconditions:**
- Should be called before any database operations begin
- Should be called only once during application initialization

**Returns:** Nothing (void)

**Postconditions:**
- TRACE logging is enabled for all ConfigurationDB components
- All trace levels are set to maximum (0xFFFFFFFFFFFFFFFFLL)
- Memory and slow trace modes are configured

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | This function does not throw exceptions |

**Thread Safety:** Not thread-safe. Modifies global TRACE state. Should be called once during initialization before any concurrent operations.

**Side Effects:**
- Modifies global TRACE configuration for multiple components
- Significantly increases logging output volume
- May impact performance due to logging overhead
- Increases memory usage for trace buffers

**Example:**
```cpp
#include "artdaq-database/ConfigurationDB/configurationdb.h"
#include <iostream>

namespace json = artdaq::database::configuration::json;

int main(int argc, char* argv[]) {
  // Enable comprehensive tracing for debugging
  // Call ONCE at startup, before any database operations
  json::enable_trace();

  std::cout << "TRACE debugging enabled for all ConfigurationDB components\n";

  // Now all database operations will produce detailed trace output
  // Traces will be written to configured TRACE output destinations

  // Perform database operations...

  return 0;
}
```

### Components Enabled

The function enables tracing for the following component categories:

#### High-Level Operations

| Debug Function | Component |
|----------------|-----------|
| `debug::ExportImport()` | Configuration export/import operations |
| `debug::ManageAliases()` | Alias management operations |
| `debug::ManageConfigs()` | Configuration management operations |
| `debug::ManageDocuments()` | Document management operations |
| `debug::Metadata()` | Metadata operations |
| `debug::SearchCollection()` | Collection search operations |

#### Detail Implementations

| Debug Function | Component |
|----------------|-----------|
| `debug::detail::ExportImport()` | Export/import implementation details |
| `debug::detail::ManageAliases()` | Alias management implementation |
| `debug::detail::ManageConfigs()` | Configuration management implementation |
| `debug::detail::ManageDocuments()` | Document management implementation |
| `debug::detail::Metadata()` | Metadata handling implementation |
| `debug::detail::SearchCollection()` | Search implementation details |

#### Options Handling

| Debug Function | Component |
|----------------|-----------|
| `debug::options::OperationBase()` | Base operation options |
| `debug::options::BulkOperations()` | Bulk operation options |
| `debug::options::ManageDocuments()` | Document management options |
| `debug::options::ManageConfigs()` | Configuration management options |
| `debug::options::ManageAliases()` | Alias management options |

#### Storage Provider Dispatch

| Debug Function | Component |
|----------------|-----------|
| `debug::MongoDB()` | MongoDB dispatch layer |
| `debug::UconDB()` | UconDB dispatch layer |
| `debug::FileSystemDB()` | FileSystemDB dispatch layer |

#### Low-Level Components

| Debug Function | Component |
|----------------|-----------|
| `filesystem::index::debug::enable()` | FileSystemDB index operations |
| `filesystem::debug::enable()` | FileSystemDB provider |
| `mongo::debug::enable()` | MongoDB provider |
| `docrecord::debug::JSONDocumentBuilder()` | JSON document builder |
| `docrecord::debug::JSONDocument()` | JSON document handling |

## Performance Considerations

- **Significant overhead:** Enabling all traces produces substantial logging output and impacts performance. Use only for debugging.
- **Log volume:** With all traces enabled, even simple operations generate hundreds of log lines.
- **Disk space:** Extended debugging sessions can consume significant disk space for log files.
- **Memory usage:** TRACE buffers consume additional memory when enabled.
- **Production use:** Do not call `enable_trace()` in production environments.

**Complexity:** O(1) - enables a fixed number of trace points

## Error Handling Strategy

This file contains no error handling as the debug functions are designed to:
- Always succeed (no failure conditions)
- Not throw exceptions
- Modify only global TRACE state

If a debug function is undefined (e.g., due to conditional compilation), the linker will report an error during build.

## Testing Notes

- **Debug builds:** Most useful in Debug or RelWithDebInfo builds
- **Test isolation:** When debugging specific tests, consider enabling only the relevant component traces instead of all traces
- **CI/CD:** Avoid enabling in automated test pipelines unless specifically debugging test failures
- **Unit tests:** Not directly tested; enables debug output for other tests

## Maintenance Notes

When adding new debug functions to the ConfigurationDB module or its dependencies:
1. Add the corresponding header include at the top of the file
2. Add the `debug::*()` call in the appropriate section of `enable_trace()`
3. Maintain the logical grouping (operations, details, options, providers)

### Debug Function Conventions

All debug functions follow these conventions:
- Located in a `debug` namespace within their component namespace
- Named after the component they enable (e.g., `debug::MongoDB()`)
- Take no parameters
- Return void
- Set trace name and levels internally

## Relationship to Other Components

This file serves as an aggregation point for all debug enablement across the ConfigurationDB module. It depends on but does not modify:

- **ConfigurationDB operations** - Enables their trace output
- **Storage providers** - Enables their trace output
- **JsonDocument** - Enables trace for document handling
- **Options classes** - Enables trace for options parsing

## Common Pitfalls

- **Calling in production:** Never call `enable_trace()` in production code; the performance impact is significant
- **Multiple calls:** While harmless, calling multiple times is wasteful; call once at startup
- **Thread safety:** Do not call while other threads are performing database operations
- **Log cleanup:** Remember to clean up or rotate log files after debugging sessions

### Anti-patterns

```cpp
// DON'T do this - calling in production or calling repeatedly:
void performOperation() {
  json::enable_trace();  // Wrong! Don't call on every operation
  // ... operation code ...
}

// DO this instead - call once at startup:
int main() {
  #ifdef DEBUG
  json::enable_trace();  // Conditionally enable only in debug builds
  #endif
  // ... application code ...
}
```

## See Also

- [dispatch_mongodb.cpp](./dispatch_mongodb.cpp.md) - Contains `debug::MongoDB()` implementation
- [dispatch_ucondb.cpp](./dispatch_ucondb.cpp.md) - Contains `debug::UconDB()` implementation
- [dispatch_filedb.cpp](./dispatch_filedb.cpp.md) - Contains `debug::FileSystemDB()` implementation
- [JSONDocument.cpp](../JsonDocument/JSONDocument.cpp.md) - Contains `debug::JSONDocument()` implementation
- [JSONDocumentBuilder.cpp](../JsonDocument/JSONDocumentBuilder.cpp.md) - Contains `debug::JSONDocumentBuilder()` implementation
- [External: TRACE documentation](https://cdcvs.fnal.gov/redmine/projects/trace/wiki) - External TRACE logging documentation

---

**Documentation generated for artdaq-database ConfigurationDB module**
