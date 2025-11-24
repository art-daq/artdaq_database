# storage_providers.h

## File Overview

This header file defines the core `StorageProvider` template class that serves as a type-safe interface for all database storage backends in artdaq-database. It implements a generic storage abstraction that can work with different document types and provider implementations using compile-time polymorphism.

**Location**: `/home/user/artdaq-database/artdaq-database/StorageProviders/storage_providers.h`

**Lines of Code**: 81

**Purpose**: Generic storage provider interface using template-based polymorphism

## Dependencies

### Standard Library
- `<memory>` - std::shared_ptr for provider management
- `<string>` - std::string for database URIs and metadata
- `<vector>` - std::vector for returning multiple results

### Project Headers
- `"artdaq-database/SharedCommon/configuraion_api_literals.h"` - Configuration constants
- `"artdaq-database/SharedCommon/shared_datatypes.h"` - Common type definitions (object_id_t)

## Namespace: artdaq::database

All types and functions in this file are declared within the `artdaq::database` namespace.

## Constants

### system_metadata
```cpp
constexpr auto system_metadata = "SystemMetadata";
```

**Purpose**: String literal for the system metadata collection name.

**Usage**: Identifies the special collection that stores database metadata.

---

### ouid_invalid
```cpp
constexpr auto ouid_invalid = "000000000000000000000000";
```

**Purpose**: Represents an invalid or uninitialized object unique identifier (OUID).

**Format**: 24-character hexadecimal string (all zeros)

**Usage**: Used to indicate that an object ID is not valid or has not been assigned.

**Example**:
```cpp
object_id_t id = artdaq::database::ouid_invalid;
if (id == artdaq::database::ouid_invalid) {
    // Handle invalid ID case
}
```

---

## Class: StorageProvider<TYPE, IMPL>

### Template Parameters

- **TYPE**: The storable document type (e.g., JSONDocument)
- **IMPL**: The concrete provider implementation (e.g., FileSystemDBProvider, MongoDBProvider, UconDBProvider)

### Purpose

`StorageProvider` is a template class that provides a unified interface for all storage backend implementations. It uses compile-time polymorphism (templates) rather than runtime polymorphism (virtual functions) for better performance and type safety.

### Type Aliases

```cpp
using StorableType = TYPE;
using Provider = StorageProvider<TYPE, IMPL>;
using ProviderSPtr = std::shared_ptr<Provider>;
```

**Usage**:
```cpp
using FileDBProvider = StorageProvider<JSONDocument, FileSystemDBProviderImpl>;
FileDBProvider::ProviderSPtr provider = ...;
```

---

## Factory Method

### create
```cpp
static ProviderSPtr create(std::shared_ptr<IMPL> const& provider);
```

**Purpose**: Factory method to create a StorageProvider instance.

**Parameters**:
- `provider` - Shared pointer to the concrete provider implementation

**Returns**: Shared pointer to newly created StorageProvider

**Usage Example**:
```cpp
auto impl = std::make_shared<FileSystemDBProviderImpl>();
auto provider = StorageProvider<JSONDocument, FileSystemDBProviderImpl>::create(impl);
```

**Design**: Uses the Pass-Key Idiom to restrict construction to the factory method only.

---

## Pass-Key Idiom

### PassKeyIdiom Class

```cpp
class PassKeyIdiom final {
 private:
  template <typename T, typename I>
  friend ProviderSPtr create(std::shared_ptr<I> const&);

 public:
  PassKeyIdiom() = default;
};
```

**Purpose**: Implements the Pass-Key Idiom design pattern to control object construction.

**Design Pattern**:
- Constructor is public but requires a PassKeyIdiom instance
- Only the `create()` friend function can construct PassKeyIdiom
- Prevents direct construction while allowing std::make_shared

**Benefits**:
- Enforces factory pattern
- Allows use of std::make_shared (better than make_unique for shared_ptr)
- Compile-time enforcement (no runtime overhead)

---

## Constructor

### StorageProvider
```cpp
StorageProvider(std::shared_ptr<IMPL> const& provider, PassKeyIdiom const&);
```

**Purpose**: Construct a storage provider with the given implementation.

**Parameters**:
- `provider` - Shared pointer to concrete implementation
- `PassKeyIdiom const&` - Pass-key to restrict construction

**Access**: Public, but can only be called by the `create()` factory method due to Pass-Key Idiom.

**Implementation**: Stores the provider implementation in `_provider` member.

---

## Document Operations

### readDocument
```cpp
template <typename FILTER>
std::vector<TYPE> readDocument(FILTER const&);
```

**Purpose**: Read one or more documents matching the filter criteria.

**Template Parameters**:
- `FILTER` - Type of filter object (typically contains search criteria)

**Parameters**:
- Filter object specifying which documents to retrieve

**Returns**: Vector of documents matching the filter

**Usage Example**:
```cpp
SearchFilter filter;
filter.collection = "ComponentConfigs";
filter.run = 12345;

auto documents = provider->readDocument(filter);
for (auto const& doc : documents) {
    // Process each document
}
```

**Design Note**: Returns a vector even for single results to maintain consistent interface.

---

### writeDocument
```cpp
object_id_t writeDocument(TYPE const&);
```

**Purpose**: Write a document to storage.

**Parameters**:
- Document to write (TYPE, typically JSONDocument)

**Returns**: Object ID of the written document

**Usage Example**:
```cpp
JSONDocument doc;
doc.setCollection("Configurations");
doc.setData(configData);

object_id_t id = provider->writeDocument(doc);
std::cout << "Wrote document with ID: " << id << std::endl;
```

**Behavior**:
- Creates new document if doesn't exist
- Updates existing document based on provider implementation
- Returns unique identifier for the document

---

## Configuration Management Operations

### findConfigurations
```cpp
template <typename FILTER>
std::vector<FILTER> findConfigurations(FILTER const&);
```

**Purpose**: Search for configurations matching the given criteria.

**Parameters**:
- `FILTER` - Filter specifying search criteria

**Returns**: Vector of filters, each representing a found configuration

**Usage**: Used to discover available configurations in the database.

---

### addConfiguration
```cpp
template <typename FILTER>
std::vector<FILTER> addConfiguration(FILTER const&);
```

**Purpose**: Add a new configuration to the database.

**Parameters**:
- `FILTER` - Filter/specification for the new configuration

**Returns**: Vector containing the added configuration information

**Usage**: Creates new configuration entries in the database.

---

### findVersions
```cpp
template <typename FILTER>
std::vector<FILTER> findVersions(FILTER const&);
```

**Purpose**: Find all versions of a configuration.

**Parameters**:
- `FILTER` - Filter identifying the configuration

**Returns**: Vector of filters, each representing a version

**Usage**: Retrieve version history for a specific configuration.

---

### findEntities
```cpp
template <typename FILTER>
std::vector<FILTER> findEntities(FILTER const&);
```

**Purpose**: Find entities (components, systems) matching criteria.

**Parameters**:
- `FILTER` - Filter specifying search criteria

**Returns**: Vector of filters representing found entities

**Usage**: Discover what entities (hardware components, software modules) are configured.

---

### configurationComposition
```cpp
template <typename FILTER>
std::vector<FILTER> configurationComposition(FILTER const&);
```

**Purpose**: Get the composition (constituent parts) of a configuration.

**Parameters**:
- `FILTER` - Filter identifying the configuration

**Returns**: Vector of filters representing the configuration's components

**Usage**: Understand what makes up a complex configuration.

---

## Database Metadata Operations

### listCollections
```cpp
template <typename FILTER>
std::vector<FILTER> listCollections(FILTER const&);
```

**Purpose**: List all collections in the database.

**Parameters**:
- `FILTER` - Filter that may specify which database to query

**Returns**: Vector of filters, each representing a collection

**Usage Example**:
```cpp
Filter filter;
filter.database = "artdaq_db";

auto collections = provider->listCollections(filter);
for (auto const& col : collections) {
    std::cout << "Collection: " << col.collection << std::endl;
}
```

---

### listDatabases
```cpp
template <typename FILTER>
std::vector<FILTER> listDatabases(FILTER const&);
```

**Purpose**: List all available databases.

**Parameters**:
- `FILTER` - Filter that may specify search criteria

**Returns**: Vector of filters, each representing a database

**Usage**: Discover what databases are available in the storage system.

---

### databaseMetadata
```cpp
template <typename FILTER>
std::vector<FILTER> databaseMetadata(FILTER const&);
```

**Purpose**: Retrieve metadata about a database (creation time, format, etc.).

**Parameters**:
- `FILTER` - Filter identifying the database

**Returns**: Vector containing database metadata

**Usage**: Get information about database creation, format version, etc.

---

### searchCollection
```cpp
template <typename FILTER>
std::vector<FILTER> searchCollection(FILTER const&);
```

**Purpose**: Search within a collection using flexible criteria.

**Parameters**:
- `FILTER` - Filter with search criteria

**Returns**: Vector of matching results

**Usage**: Generic search operation for finding documents in a collection.

---

## Helper Functions

### make_database_metadata
```cpp
std::string make_database_metadata(std::string const& name,
                                   std::string const& uri);
```

**Purpose**: Generate JSON metadata for a new database.

**Parameters**:
- `name` - Database name
- `uri` - Database connection URI

**Returns**: JSON string containing database metadata

**Throws**: Assertion failure if name or uri is empty

**Implementation**: See storage_providers.cpp for details.

**Generated Metadata Includes**:
- Database name and URI
- Locale setting
- Creation timestamp
- Creating user
- System information (uname)
- Database format version

**Usage Example**:
```cpp
std::string metadata = make_database_metadata("artdaq_config_db",
                                              "filesystemdb:///data/configs");
// Store metadata in SystemMetadata collection
```

---

## Private Members

### _provider
```cpp
std::shared_ptr<IMPL> _provider;
```

**Purpose**: Stores the concrete provider implementation.

**Type**: Shared pointer to the template parameter IMPL

**Lifetime**: Shared ownership - provider can be shared across multiple StorageProvider instances.

---

## Design Patterns

### Template-Based Interface

**Advantage over Virtual Functions**:
1. **Performance**: No virtual function call overhead
2. **Type Safety**: Compile-time type checking
3. **Flexibility**: Each provider can have different implementation details
4. **No Runtime Overhead**: Template instantiation at compile time

**Example**:
```cpp
// Instead of:
class BaseProvider {
    virtual void write(Document const&) = 0;  // Runtime polymorphism
};

// We have:
template <typename TYPE, typename IMPL>
class StorageProvider {
    object_id_t writeDocument(TYPE const&);    // Compile-time polymorphism
};
```

### Factory Pattern with Pass-Key Idiom

The combination of factory method and pass-key idiom ensures:
1. All instances are created through controlled factory
2. Instances are properly initialized
3. std::make_shared can still be used
4. No runtime overhead

### Type-Safe Filter Pattern

Operations use template filters rather than fixed types:
```cpp
template <typename FILTER>
std::vector<FILTER> findConfigurations(FILTER const&);
```

This allows different providers to use different filter implementations while maintaining the same interface.

---

## Implementation Requirements

Concrete provider implementations (IMPL) must provide implementations for all the template member functions. Typically this is done through explicit template instantiation in the .cpp file.

**Example Structure**:
```cpp
// In provider_filedb.cpp
template <>
std::vector<JSONDocument>
StorageProvider<JSONDocument, FileSystemDBProviderImpl>::readDocument(
    SearchFilter const& filter) {
    return _provider->read(filter);
}
```

---

## Usage Example: Complete Workflow

```cpp
#include "artdaq-database/StorageProviders/storage_providers.h"
#include "artdaq-database/StorageProviders/FileSystemDB/provider_filedb.h"

// Create provider implementation
auto impl = std::make_shared<FileSystemDBProviderImpl>();
impl->configure("/data/artdaq/configs");

// Create storage provider
auto provider = StorageProvider<JSONDocument, FileSystemDBProviderImpl>::create(impl);

// Write a document
JSONDocument doc;
doc.setCollection("RunConfigs");
doc.setData("{\"run\": 12345, \"config\": \"test\"}");
object_id_t id = provider->writeDocument(doc);

// Read it back
SearchFilter filter;
filter.collection = "RunConfigs";
filter.run = 12345;
auto results = provider->readDocument(filter);

// List collections
auto collections = provider->listCollections(SearchFilter{});
```

---

## Thread Safety

The `StorageProvider` class itself is thread-safe for read operations if the underlying IMPL is thread-safe. Write operations require external synchronization or rely on IMPL thread safety.

Check specific provider documentation:
- FileSystemDB provider thread safety
- MongoDB provider thread safety
- UconDB provider thread safety

---

## Related Files

- **storage_providers.cpp** - Implementation of `make_database_metadata()`
- **FileSystemDB/provider_filedb.h** - FileSystem provider implementation
- **MongoDB/provider_mongodb.h** - MongoDB provider implementation
- **UconDB/provider_ucondb.h** - UconDB provider implementation
- **SharedCommon/shared_datatypes.h** - Type definitions (object_id_t)

---

## Best Practices

### Provider Creation
```cpp
// GOOD: Use factory method
auto provider = StorageProvider<Doc, Impl>::create(impl_ptr);

// BAD: Don't try to construct directly
// auto provider = std::make_shared<StorageProvider<Doc, Impl>>(impl_ptr, ???);
// Won't compile - can't create PassKeyIdiom
```

### Error Handling
```cpp
// Check for invalid IDs
object_id_t id = provider->writeDocument(doc);
if (id == artdaq::database::ouid_invalid) {
    // Handle write failure
}
```

### Filter Usage
```cpp
// Create appropriate filter for the operation
SearchFilter filter;
filter.collection = "Configurations";  // Required
filter.version = "v1.0";               // Optional
auto results = provider->readDocument(filter);
```

---

**Documentation generated for artdaq-database StorageProviders module**
