# configuration_dbproviders.h

**Path:** `artdaq-database/ConfigurationDB/configuration_dbproviders.h`

**Purpose:** Aggregation header that includes all database provider dispatch headers and declares the provider name validation function. This file serves as the central point for accessing all supported storage provider implementations (FileSystemDB, MongoDB, UconDB).


## Key Concepts

### Storage Provider Architecture

The ConfigurationDB module supports multiple storage backends through a unified dispatch interface. Each provider implements the same set of operations, allowing the system to switch between backends based on configuration without changing application code.

### Supported Providers

| Provider | Literal Name | Description |
|----------|--------------|-------------|
| FileSystemDB | `"filesystem"` | File-based storage using directory structures and JSON files |
| MongoDB | `"mongo"` | Document database backend for scalable storage |
| UconDB | `"ucon"` | Microservice Configuration Database (REST API client) |

### Provider Selection

The active provider is determined by:
1. The `ARTDAQ_DATABASE_URI` environment variable (primary method)
2. Explicit provider setting in operation options

## Thread Safety

- **Thread-safe:** Yes
- **Concurrent access:** The validation function is stateless and thread-safe
- **Locking:** No internal locking required

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/ConfigurationDB/dispatch_filedb.h` | FileSystemDB provider dispatch functions |
| `artdaq-database/ConfigurationDB/dispatch_mongodb.h` | MongoDB provider dispatch functions |
| `artdaq-database/ConfigurationDB/dispatch_ucondb.h` | UconDB provider dispatch functions |

## Functions

### `validate_dbprovider_name(std::string const& provider) -> void`

**Brief:** Validates that a provider name is one of the supported providers. This function is called before dispatching operations to ensure the target provider is valid.

**Parameters:**
- `provider` - The provider name string to validate. Must be one of: "filesystem", "mongo", "ucon"

**Preconditions:**
- `provider` must not be empty

**Returns:** void (no return value on success)

**Postconditions:**
- If the function returns normally, the provider name is guaranteed to be valid

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `artdaq::database::runtime_error` | When `provider` is not one of the valid provider names |
| `std::logic_error` | When `provider` is empty (via `confirm()` assertion in debug builds) |

**Thread Safety:** Safe - this function is stateless and can be called from any thread

**Side Effects:**
- Logs errors at TRACE level 10 before re-throwing exceptions

**Complexity:** O(1) - constant time string comparisons

**Example:**
```cpp
#include "artdaq-database/ConfigurationDB/configuration_dbproviders.h"
#include <iostream>

namespace cf = artdaq::database::configuration;

void setupProvider(std::string const& providerName) {
  try {
    // Validate the provider name before use
    cf::validate_dbprovider_name(providerName);
    std::cout << "Provider '" << providerName << "' is valid" << std::endl;
  } catch (artdaq::database::runtime_error const& e) {
    std::cerr << "Invalid provider: " << e.what() << std::endl;
    throw;  // Re-throw or handle appropriately
  }
}

int main() {
  // Valid provider names
  setupProvider("filesystem");  // OK
  setupProvider("mongo");       // OK
  setupProvider("ucon");        // OK

  // Invalid provider names - will throw
  try {
    setupProvider("mongodb");   // WRONG - should be "mongo"
  } catch (...) {
    std::cerr << "Caught expected exception for 'mongodb'" << std::endl;
  }

  try {
    setupProvider("file");      // WRONG - should be "filesystem"
  } catch (...) {
    std::cerr << "Caught expected exception for 'file'" << std::endl;
  }

  return 0;
}
```

## Dispatch Functions Provided by Included Headers

Each dispatch header provides the same set of operations for its provider:

### Document Operations

| Function | Description |
|----------|-------------|
| `writeDocument()` | Write a document to the database |
| `readDocument()` | Read a single document from the database |
| `readDocuments()` | Read multiple documents matching a query |
| `markDocumentReadonly()` | Mark a document as read-only |
| `markDocumentDeleted()` | Mark a document as deleted |
| `findVersions()` | Find all versions of a document |
| `findEntities()` | Find all entities in a collection |

### Alias Operations

| Function | Description |
|----------|-------------|
| `addVersionAlias()` | Add a new version alias |
| `removeVersionAlias()` | Remove a version alias |
| `findVersionAliases()` | Find all version aliases |
| `addConfigurationAlias()` | Add a configuration alias |
| `removeConfigurationAlias()` | Remove a configuration alias |

### Configuration Operations

| Function | Description |
|----------|-------------|
| `findConfigurations()` | Find all global configurations |
| `configurationComposition()` | Get the composition of a configuration |
| `assignConfiguration()` | Assign a version to a configuration |
| `removeConfiguration()` | Remove from a configuration |
| `findCompositionsContaining()` | Find configurations containing a component |

### Metadata Operations

| Function | Description |
|----------|-------------|
| `listCollections()` | List all collections |
| `listDatabases()` | List all databases |
| `readDbInfo()` | Get database information |
| `searchCollection()` | Search within a collection |

## Relationship to Other Components

```
configuration_dbproviders.h (this file)
           |
           +-- dispatch_filedb.h
           |       |
           |       +-- filesystem namespace functions
           |
           +-- dispatch_mongodb.h
           |       |
           |       +-- mongo namespace functions
           |
           +-- dispatch_ucondb.h
                   |
                   +-- ucon namespace functions
```

- **dispatch_filedb.h**: Provides dispatch functions for FileSystemDB operations
- **dispatch_mongodb.h**: Provides dispatch functions for MongoDB operations
- **dispatch_ucondb.h**: Provides dispatch functions for UconDB operations
- **options_operation_base.h**: Uses validation when setting provider
- **detail_*.cpp files**: Call `validate_dbprovider_name()` before dispatch

## See Also

- [dispatch_filedb.h](./dispatch_filedb.h.md) - FileSystemDB dispatch functions
- [dispatch_mongodb.h](./dispatch_mongodb.h.md) - MongoDB dispatch functions
- [dispatch_ucondb.h](./dispatch_ucondb.h.md) - UconDB dispatch functions
- [configuration_dbproviders.cpp](./configuration_dbproviders.cpp.md) - Implementation of validation
- [provider_filedb.h](../StorageProviders/FileSystemDB/provider_filedb.h.md) - FileSystemDB storage provider
- [provider_mongodb.h](../StorageProviders/MongoDB/provider_mongodb.h.md) - MongoDB storage provider

## Notes for Developers

### Provider Selection via URI

The provider is typically determined from the `ARTDAQ_DATABASE_URI` environment variable:

| URI Prefix | Provider | Example |
|------------|----------|---------|
| `filesystemdb://` | filesystem | `filesystemdb:///path/to/database` |
| `mongodb://` | mongo | `mongodb://localhost:27017/mydb` |
| `ucondb://` | ucon | `ucondb://host:port` |

### Adding a New Provider

To add a new storage provider:

1. Create `dispatch_newprovider.h` and `dispatch_newprovider.cpp` implementing dispatch functions
2. Add include to this header: `#include "artdaq-database/ConfigurationDB/dispatch_newprovider.h"`
3. Update `validate_dbprovider_name()` in the .cpp file to accept the new provider name
4. Add provider literal to `configuraion_api_literals.h`
5. Implement the storage provider in `StorageProviders/NewProvider/`

### Common Pitfalls

- **Pitfall 1:** Using "mongodb" instead of "mongo" - the provider name is "mongo", not "mongodb"
- **Pitfall 2:** Using "file" or "fs" instead of "filesystem" - the full name is required
- **Pitfall 3:** Provider names are case-sensitive - "Mongo" will not work

### Anti-patterns

```cpp
// DON'T do this: Assuming provider name without validation
void dispatch(std::string const& provider) {
  if (provider == "mongo") {
    // dispatch to mongo
  }
  // Missing validation means invalid providers slip through!
}

// DO this instead: Always validate first
void dispatch(std::string const& provider) {
  cf::validate_dbprovider_name(provider);  // Throws if invalid
  if (provider == "mongo") {
    // dispatch to mongo
  }
  // Now we know provider is one of the valid options
}
```

---

**Documentation generated for artdaq-database ConfigurationDB module**
