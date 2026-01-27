# dispatch_common.h

**Path:** `artdaq-database/ConfigurationDB/dispatch_common.h`

**Purpose:** Convenience aggregation header that includes all storage provider dispatch headers (FileSystemDB, MongoDB, UconDB) in a single include. This header simplifies code that needs access to all provider dispatch functions without explicitly including each provider header individually.


## Key Concepts

### Provider Aggregation Pattern

This header implements a common C++ pattern where related headers are grouped into a single "umbrella" or "aggregation" header. This provides:
- **Convenience:** Single include for all provider interfaces
- **Consistency:** Ensures all providers are always available together
- **Maintenance:** Adding a new provider only requires modifying this file

### Storage Provider Dispatch Architecture

The ConfigurationDB module uses a dispatch pattern where:
1. Operations are defined with a generic interface in `dboperation_*.h`
2. Detail-level implementations in `detail_*.cpp` select providers at runtime
3. Provider-specific dispatch headers declare the actual storage operations
4. Provider implementations handle the actual data persistence

```
User API -> detail_*.cpp -> dispatch_*.h -> StorageProviders/*
                  |
                  +-- Provider selection based on ARTDAQ_DATABASE_URI
```

## Thread Safety

- **Thread-safe:** Not applicable (header only)
- **Concurrent access:** Thread safety determined by included headers
- **Locking:** No locking at this level

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/ConfigurationDB/dispatch_filedb.h` | FileSystemDB provider dispatch functions for file-based storage |
| `artdaq-database/ConfigurationDB/dispatch_mongodb.h` | MongoDB provider dispatch functions for document database storage |
| `artdaq-database/ConfigurationDB/dispatch_ucondb.h` | UconDB provider dispatch functions for REST API-based storage |

## Namespaces Provided

Including this header provides access to three provider namespaces:

### `artdaq::database::configuration::filesystem`

FileSystemDB provider functions for file-based storage. Ideal for:
- Development and testing environments
- Standalone deployments without MongoDB
- Human-readable configuration storage
- Debugging and inspection of stored configurations

### `artdaq::database::configuration::mongo`

MongoDB provider functions for document database storage. Ideal for:
- Production deployments requiring scalability
- Multi-user concurrent access
- Complex query requirements
- Distributed DAQ systems

### `artdaq::database::configuration::ucon`

UconDB provider functions for REST API-based storage. Ideal for:
- Integration with existing UconDB infrastructure
- Remote configuration management
- Web-accessible configuration storage

## Relationship to Other Components

```
dispatch_common.h (this file)
       |
       +-- dispatch_filedb.h --> StorageProviders/FileSystemDB/
       |
       +-- dispatch_mongodb.h --> StorageProviders/MongoDB/
       |
       +-- dispatch_ucondb.h --> StorageProviders/UconDB/
```

- **dispatch_filedb.h:** FileSystemDB provider - stores documents as JSON files in directories
- **dispatch_mongodb.h:** MongoDB provider - stores documents in MongoDB collections
- **dispatch_ucondb.h:** UconDB provider - stores documents via UconDB REST API
- **detail_*.cpp:** Implementation files that use this header to access all providers
- **configuration_dbproviders.h:** Provider validation and name resolution

## Usage Example

```cpp
#include "artdaq-database/ConfigurationDB/dispatch_common.h"
#include "artdaq-database/ConfigurationDB/options_operations.h"
#include "artdaq-database/JsonDocument/JSONDocument.h"

namespace cf = artdaq::database::configuration;
using artdaq::database::docrecord::JSONDocument;

// Function that dispatches to the appropriate provider based on options
JSONDocument readFromProvider(cf::ManageDocumentOperation const& opts,
                              JSONDocument const& query) {
  auto const& provider = opts.provider();

  try {
    if (provider == "filesystem") {
      return cf::filesystem::readDocument(opts, query);
    } else if (provider == "mongo") {
      return cf::mongo::readDocument(opts, query);
    } else if (provider == "ucon") {
      return cf::ucon::readDocument(opts, query);
    }
    throw std::runtime_error("Unknown provider: " + provider);
  } catch (const std::exception& e) {
    std::cerr << "Failed to read document: " << e.what() << "\n";
    throw;
  }
}
```

## Notes for Developers

### When to Use This Header

- **Use this header** when you need access to multiple providers or want to dispatch operations based on runtime configuration
- **Use individual headers** (e.g., `dispatch_filedb.h`) when you only need one specific provider

### Provider Selection

The provider is typically selected based on the `ARTDAQ_DATABASE_URI` environment variable:
- `filesystemdb://path/to/database` selects FileSystemDB
- `mongodb://host:port/database` selects MongoDB
- `ucondb://host:port/database` selects UconDB

### Compile-Time Considerations

Including this header brings in all provider headers, which may include additional dependencies:
- FileSystemDB: Boost.Filesystem
- MongoDB: mongocxx driver headers
- UconDB: HTTP client headers

If build time is a concern and you only need one provider, include that provider's header directly.

### Common Pitfalls

- **Pitfall 1:** Assuming provider availability - Not all providers may be compiled into a given build. Check build configuration if operations fail with "unknown provider" errors.
- **Pitfall 2:** Mixing provider data - Data stored with one provider cannot be directly accessed by another. Migration utilities must be used to transfer data between providers.

### Anti-patterns

```cpp
// DON'T do this: Hard-coding provider selection
#include "artdaq-database/ConfigurationDB/dispatch_filedb.h"  // Only FileSystemDB

// DO this instead: Use dispatch_common.h for runtime flexibility
#include "artdaq-database/ConfigurationDB/dispatch_common.h"
// Provider selected at runtime from ARTDAQ_DATABASE_URI
```

## See Also

- [dispatch_filedb.h](./dispatch_filedb.h.md) - FileSystemDB provider dispatch interface
- [dispatch_mongodb.h](./dispatch_mongodb.h.md) - MongoDB provider dispatch interface
- [dispatch_ucondb.h](./dispatch_ucondb.h.md) - UconDB provider dispatch interface
- [configuration_dbproviders.h](./configuration_dbproviders.h.md) - Provider validation utilities
- [StorageProviders/README.md](../StorageProviders/README.md) - Storage provider module overview

---

**Documentation generated for artdaq-database ConfigurationDB module**
