# ConfigurationDB Module Documentation

## Module Overview

The **ConfigurationDB** module is the LARGEST and most complex module in artdaq-database, containing **54 source files** with **~8,850 lines of code**. It provides the complete infrastructure for managing artdaq configurations, including:

- High-level C++ API for configuration management (`ConfigurationInterface`)
- Database operation implementations for all providers (FileSystemDB, MongoDB, UconDB)
- Multi-threaded bulk operations support
- Version control and alias management
- Configuration composition and global configuration support
- Export/import functionality
- Comprehensive provider dispatch mechanism

**Location**: `/home/user/artdaq-database/artdaq-database/ConfigurationDB/`

**Purpose**: Complete configuration database management system with multi-provider support

---

## Module Architecture

### Layer Structure

```
┌─────────────────────────────────────────────────────────────┐
│                    User Application Code                     │
└──────────────────────────┬──────────────────────────────────┘
                           │
┌──────────────────────────▼──────────────────────────────────┐
│  ConfigurationInterface (High-Level API)                     │
│  - Template-based type-safe interface                        │
│  - Serialization support (JSON, FHiCL)                       │
│  - Version management                                        │
│  - Global configuration composition                          │
│  Files: configurationdbifc.h, configurationdbifc_base.h     │
└──────────────────────────┬──────────────────────────────────┘
                           │
┌──────────────────────────▼──────────────────────────────────┐
│  Operation Classes (Options Layer)                           │
│  - ManageDocumentOperation                                   │
│  - ManageConfigsOperation                                    │
│  - ManageAliasesOperation                                    │
│  - BulkOperationsOperation                                   │
│  Files: options_operation_*.h/cpp                            │
└──────────────────────────┬──────────────────────────────────┘
                           │
┌──────────────────────────▼──────────────────────────────────┐
│  Database Operations (Function Layer)                        │
│  - JSON-based operation functions                            │
│  - Format conversions                                        │
│  - Query processing                                          │
│  Files: dboperation_*.h/cpp                                  │
└──────────────────────────┬──────────────────────────────────┘
                           │
┌──────────────────────────▼──────────────────────────────────┐
│  Implementation Details (Detail Layer)                       │
│  - Operation-specific logic                                  │
│  - Format handling                                           │
│  - Result processing                                         │
│  Files: detail_*.cpp                                         │
└──────────────────────────┬──────────────────────────────────┘
                           │
┌──────────────────────────▼──────────────────────────────────┐
│  Provider Dispatch (Provider Layer)                          │
│  - FileSystemDB implementation                               │
│  - MongoDB implementation                                    │
│  - UconDB implementation                                     │
│  Files: dispatch_*.h/cpp, operation_dispatch.h              │
└──────────────────────────┬──────────────────────────────────┘
                           │
┌──────────────────────────▼──────────────────────────────────┐
│  Storage Providers (Backend Layer)                           │
│  - Actual database/file I/O                                  │
│  Module: StorageProviders/                                   │
└─────────────────────────────────────────────────────────────┘
```

---

## File Organization

### Core Interface Files (3 files)
Primary user-facing API:
- **configurationdbifc.h** - Main ConfigurationInterface class
- **configurationdbifc_base.h** - Serialization support and base classes
- **Multitasker.h / .cpp** - Thread pool for parallel operations

### Operation Base Classes (3 files)
Foundation for all operation types:
- **options_operation_base.h / .cpp** - Base class with common functionality
- **options_operations.h** - Aggregate header for all operation classes

### Operation Implementations (8 files)
Specific operation configuration classes:
- **options_operation_managedocument.h / .cpp** - Document operations (read, write, versions)
- **options_operation_manageconfigs.h / .cpp** - Configuration management
- **options_operation_managealiases.h / .cpp** - Alias management
- **options_operation_bulkoperations.h / .cpp** - Bulk export/import

### Database Operation Functions (12 files)
JSON-based operation entry points:
- **dboperation_managedocument.h / .cpp** - Document operation functions
- **dboperation_manageconfigs.h / .cpp** - Configuration operation functions
- **dboperation_managealiases.h / .cpp** - Alias operation functions
- **dboperation_metadata.h / .cpp** - Metadata operation functions
- **dboperation_searchcollection.h / .cpp** - Search operation functions
- **dboperation_exportimport.h / .cpp** - Export/import operation functions

### Implementation Details (6 files)
Core business logic:
- **detail_managedocument.cpp** - Document operation implementations
- **detail_manageconfigs.cpp** - Configuration operation implementations
- **detail_managealiases.cpp** - Alias operation implementations
- **detail_metadata.cpp** - Metadata operation implementations
- **detail_searchcollection.cpp** - Search operation implementations
- **detail_exportimport.cpp** - Export/import implementations

### Provider Dispatch (9 files)
Database provider routing:
- **dispatch_common.h** - Common dispatch includes
- **dispatch_signatures.h / .cpp** - Function signature definitions
- **dispatch_filedb.h / .cpp** - FileSystemDB provider dispatch
- **dispatch_mongodb.h / .cpp** - MongoDB provider dispatch
- **dispatch_ucondb.h / .cpp** - UconDB provider dispatch
- **operation_dispatch.h** - Functor templates for dispatch

### Utility Files (11 files)
Supporting functionality:
- **common.h** - Common includes and utilities
- **configuration_common.h** - Configuration-specific common includes
- **configurationdb.h** - Master include for all operations
- **configuration_dbproviders.h / .cpp** - Provider configuration
- **conftoolifc.h** - Configuration tool interface
- **shared_helper_functions.h / .cpp** - Format conversion utilities
- **search_filter.h** - Filter interface (legacy)
- **enable_trace.cpp** - Trace logging enablement
- **CMakeLists.txt** - Build configuration

---

## Key Components

### 1. ConfigurationInterface

**Purpose**: High-level template-based API for configuration management

**Key Features**:
- Type-safe configuration serialization
- Support for JSON and FHiCL formats
- Version management (store, load, overwrite, mark read-only)
- Global configuration composition
- Multi-threaded bulk operations
- Automatic collection name derivation

**Usage Pattern**:
```cpp
ConfigurationInterface ifc;

// Store a configuration version
MyConfig config;
auto result = ifc.storeVersion<MyConfig*, JsonData>(
    &config, "v1.0", "Component1"
);

// Load a configuration version
MyConfig loaded;
result = ifc.loadVersion<MyConfig*, JsonData>(
    &loaded, "v1.0", "Component1"
);

// Get available versions
auto versions = ifc.getVersions(&config, "Component1");

// Manage global configurations
auto composition = ifc.loadGlobalConfiguration("Run12345");
```

---

### 2. Operation Classes

**Purpose**: Configuration objects for database operations

**Class Hierarchy**:
```
OperationBase
├── ManageDocumentOperation (document CRUD + versioning)
├── ManageConfigsOperation (global config management)
├── ManageAliasesOperation (alias management)
└── BulkOperationsOperation (import/export)
```

**Common Features**:
- Provider selection (filesystem/mongodb/ucondb)
- Operation type specification
- Data format selection
- Collection/query specification
- JSON serialization/deserialization
- Command-line option parsing

**Usage Pattern**:
```cpp
ManageDocumentOperation opts("MyApp");
opts.operation("readdocument");
opts.collection("ComponentConfigs");
opts.version("v1.0");
opts.format(data_format_t::json);

// Convert to JSON for dispatch
JsonData json = opts.to_JsonData();
```

---

### 3. Database Operation Functions

**Purpose**: JSON-based operation entry points for all database operations

**Naming Convention**: `<category>_<operation>(JsonData const&)` or with output parameter

**Categories**:
- **Document**: read_document, write_document, find_versions, mark_document_readonly
- **Config**: find_configurations, create_configuration, configuration_composition
- **Alias**: find_aliases, add_alias, remove_alias
- **Metadata**: list_databases, list_collections, database_metadata
- **Search**: search_collection
- **Bulk**: export_configuration, import_configuration

**Usage Pattern**:
```cpp
namespace cf = artdaq::database::configuration;

// Create options
cf::ManageDocumentOperation opts("app");
opts.operation("readdocument");
// ... configure opts ...

// Call operation function
auto result = cf::json::read_document(opts.to_JsonData());

if (result.first) {
    std::cout << "Success: " << result.second << "\n";
} else {
    std::cerr << "Error: " << result.second << "\n";
}
```

---

### 4. Provider Dispatch System

**Purpose**: Route operations to appropriate database provider implementation

**Dispatch Flow**:
1. Operation function receives JSON request
2. Extracts provider name from request
3. Creates appropriate functor for operation
4. Dispatches to provider-specific implementation
5. Returns result to caller

**Providers**:
- **FileSystemDB**: File-based storage with directory structure
- **MongoDB**: MongoDB database backend
- **UconDB**: Microservice Configuration Database (REST API)

**Implementation Pattern**:
```cpp
// In dispatch_filedb.cpp
result_t filedb::read_document(JsonData const& filter, JsonData& results) {
    // FileSystemDB-specific implementation
    // Uses StorageProvider<JSONDocument, FileSystemDBProviderImpl>
    return {success, message};
}

// Similar in dispatch_mongodb.cpp and dispatch_ucondb.cpp
```

---

### 5. Multitasker (Thread Pool)

**Purpose**: Parallel execution of database operations

**Features**:
- Configurable thread pool size
- Task queue with FIFO ordering
- Result collection and merging
- Exception handling for tasks
- Progress monitoring
- Thread-safe operation

**Usage Pattern**:
```cpp
Multitasker mt(4);  // 4 worker threads

// Submit tasks
for (auto const& config : configs) {
    mt.addTask([config]() {
        return performDatabaseOperation(config);
    });
}

// Wait for completion
mt.waitForResults();

// Get merged results
auto result = mt.getMergedResults();
if (result.first) {
    std::cout << "All operations succeeded\n";
}
```

---

## Data Flow Example

### Storing a Configuration Version

```
User Code:
  ConfigurationInterface::storeVersion<MyConfig*, JsonData>(&config, "v1.0", "Entity")
    │
    ├─ Serializes configuration using MakeSerializable<MyConfig*>
    │
    ▼
  ManageDocumentOperation setup:
    - operation: "writedocument"
    - collection: "MyConfigurations"
    - version: "v1.0"
    - entity: "Entity"
    - format: json
    │
    ▼
  impl::write_document(opts, data):
    - Converts options to JSON
    - Calls json::write_document()
    │
    ▼
  json::write_document(JsonData const& query):
    - Parses JSON to determine operation details
    - Calls detail layer function
    │
    ▼
  detail::write_document_impl(JSONDocument&):
    - Processes document metadata
    - Determines provider from environment/config
    - Creates dispatch functor
    │
    ▼
  Provider Dispatch:
    - Determines provider (filesystem/mongodb/ucondb)
    - Calls provider-specific implementation
    │
    ▼
  filedb::write_document() [or mongodb/ucondb equivalent]:
    - Creates StorageProvider instance
    - Calls provider->writeDocument()
    │
    ▼
  Storage Provider:
    - Actually writes to disk/database
    - Returns object ID
    │
    ▼
  Result propagates back through layers
```

---

## Supported Operations

### Document Operations
- **read_document**: Read document(s) by filter criteria
- **write_document**: Write new document version
- **overwrite_document**: Update existing document
- **mark_document_readonly**: Protect document from changes
- **find_versions**: List available versions
- **list_collections**: List all collections

### Configuration Management
- **find_configurations**: Search for global configurations
- **create_configuration**: Create new global configuration
- **remove_configuration**: Delete global configuration
- **configuration_composition**: Get config makeup
- **assign_configuration**: Link version to global config

### Alias Management
- **find_aliases**: Search for aliases
- **add_alias**: Create new alias
- **remove_alias**: Delete alias
- **assign_alias**: Link alias to global config

### Metadata Operations
- **list_databases**: List available databases
- **list_collections**: List collections in database
- **database_metadata**: Get database info
- **search_collection**: Generic search operation

### Bulk Operations
- **export_configuration**: Export configurations to archive
- **import_configuration**: Import from archive

---

## Data Formats

The module supports multiple data formats for flexibility:

### Format Types
- **JSON**: Human-readable, widely supported, web-friendly
- **FHiCL**: Fermilab's configuration language for physics applications
- **XML**: Structured markup format
- **GUI**: Optimized format for graphical interfaces
- **DB**: Internal database format with full metadata
- **CSV**: Tabular data format
- **Origin**: Format-agnostic original format

### Format Usage
Different formats serve different purposes:
- **Storage**: DB format (includes all metadata)
- **User Input**: JSON or FHiCL (human-readable)
- **GUI Display**: GUI format (simplified structure)
- **Export**: JSON or XML (portable)
- **Reports**: CSV (spreadsheet-compatible)

---

## Thread Safety

### Thread-Safe Components
- Individual operation objects (if not shared)
- Database operations (provider-dependent)
- Multitasker worker threads (internal)

### Not Thread-Safe
- **ConfigurationInterface**: Create one per thread
- **Operation classes**: Don't share across threads
- **Multitasker instance**: Must be used from creating thread only

### Best Practice
```cpp
// GOOD: Each thread has own instances
void thread_worker() {
    ConfigurationInterface ifc;
    ManageDocumentOperation opts("worker");
    // Use ifc and opts...
}

// BAD: Sharing instances
ConfigurationInterface global_ifc;  // Don't do this!
void thread_worker() {
    global_ifc.storeVersion(...);  // NOT THREAD-SAFE!
}
```

---

## Error Handling

### Result Type
All operations return `result_t`:
```cpp
using result_t = std::pair<bool, std::string>;
// first:  success flag (true = success, false = error)
// second: message (success message or error description)
```

### Exception Handling
- **ConfigurationInterface**: Methods are `noexcept`, return error in result
- **Operation functions**: May throw `invalid_option_exception` or `runtime_exception`
- **Provider dispatch**: Catches and converts exceptions to result_t

### Best Practice
```cpp
// Check results from noexcept methods
auto result = ifc.storeVersion(...);
if (!result.first) {
    TLOG(TLVL_ERROR) << "Error: " << result.second;
    return false;
}

// Use try/catch for throwing methods
try {
    auto versions = ifc.getVersions(...);
    // Process versions...
} catch (artdaq::database::exception const& e) {
    TLOG(TLVL_ERROR) << "Database error: " << e.what();
    return false;
}
```

---

## Environment Variables

### ARTDAQ_DATABASE_URI
**Purpose**: Specifies the database connection URI

**Format**:
- FileSystemDB: `filesystemdb:///path/to/database`
- MongoDB: `mongodb://host:port/database`
- UconDB: `ucondb://host:port`

**Usage**:
```bash
# FileSystemDB
export ARTDAQ_DATABASE_URI="filesystemdb:///data/artdaq/configs"

# MongoDB
export ARTDAQ_DATABASE_URI="mongodb://localhost:27017/artdaq_db"

# UconDB
export ARTDAQ_DATABASE_URI="ucondb://config-server:8080"
```

**Provider Selection**: The provider is automatically determined from the URI prefix.

---

## Performance Considerations

### Multi-Threading
- Use `storeGlobalConfiguration_mt()` for bulk operations
- Use Multitasker for parallel database operations
- Thread count typically: `hardware_concurrency / 2`

### Caching
- Operations don't cache - always hit database
- Application should implement caching if needed
- Version lists should be cached by caller

### Batch Operations
- Use bulk operations for import/export
- Group related operations to minimize roundtrips
- Consider using Multitasker for independent operations

---

## Dependencies

### External Libraries
- **Boost.Program_Options**: Command-line parsing
- **Boost.Variant**: Type-safe unions in JSON handling
- **MongoDB C++ Driver**: MongoDB support (optional)
- **libcurl**: UconDB REST API communication (optional)

### Internal Modules
- **SharedCommon**: Common utilities and exceptions
- **BasicTypes**: JsonData, FhiclData types
- **DataFormats**: JSON reader/writer
- **JsonDocument**: JSON document class
- **StorageProviders**: Database backend implementations

---

## Building

ConfigurationDB is built as part of the artdaq-database project:

```bash
cd artdaq-database
mkdir build && cd build
cmake ..
make
```

**CMake Options**:
- `-DWITH_MONGODB=ON`: Enable MongoDB support
- `-DWITH_UCONDB=ON`: Enable UconDB support
- `-DCMAKE_BUILD_TYPE=Debug`: Enable debug logging

---

## Testing

### Unit Tests
Located in `test/ConfigurationDB/` (if exists)

### Integration Tests
Test with actual providers:
```cpp
// Test FileSystemDB
export ARTDAQ_DATABASE_URI="filesystemdb:///tmp/test_db"
// Run tests...

// Test MongoDB
export ARTDAQ_DATABASE_URI="mongodb://localhost:27017/test_db"
// Run tests...
```

### Debugging
Enable trace logging:
```cpp
artdaq::database::configuration::json::enable_trace();
// All operations will now log at maximum verbosity
```

Or for specific components:
```cpp
artdaq::database::configuration::debug::Multitasker();
artdaq::database::configuration::debug::options::OperationBase();
```

---

## Common Use Cases

### 1. Store and Load Configuration

```cpp
ConfigurationInterface ifc;

// Define your configuration
MyDAQConfig config;
config.name = "Run12345";
// ... populate config ...

// Store it
auto result = ifc.storeVersion<MyDAQConfig*, JsonData>(
    &config, "v1.0", "DAQComponent1"
);

if (!result.first) {
    std::cerr << "Failed to store: " << result.second << "\n";
    return 1;
}

// Later, load it back
MyDAQConfig loaded;
result = ifc.loadVersion<MyDAQConfig*, JsonData>(
    &loaded, "v1.0", "DAQComponent1"
);
```

### 2. Create Global Configuration

```cpp
ConfigurationInterface ifc;

// Build composition
ConfigurationInterface::VersionInfoList_t composition;
composition.push_back({"ComponentConfigs", "v1.0", "DAQ1"});
composition.push_back({"ComponentConfigs", "v1.0", "DAQ2"});
composition.push_back({"BoardConfigs", "v2.3", "Board1"});

// Store as global configuration
auto result = ifc.storeGlobalConfiguration(composition, "Run12345");

if (result.first) {
    std::cout << "Created global configuration Run12345\n";
}
```

### 3. List Available Versions

```cpp
ConfigurationInterface ifc;
MyConfig config;  // Used to determine collection name

try {
    auto versions = ifc.getVersions(&config, "DAQComponent1");

    std::cout << "Available versions for DAQComponent1:\n";
    for (auto const& ver : versions) {
        std::cout << "  - " << ver << "\n";
    }
} catch (std::exception const& e) {
    std::cerr << "Error: " << e.what() << "\n";
}
```

### 4. Bulk Operations with Multitasker

```cpp
Multitasker mt;

std::vector<Configuration> configs = loadAllConfigs();

for (auto const& cfg : configs) {
    mt.addTask([&ifc, cfg]() {
        return ifc.storeVersion<Configuration*, JsonData>(
            &cfg, cfg.version, cfg.entity
        );
    });
}

mt.waitForResults();
auto merged = mt.getMergedResults();

if (merged.first) {
    std::cout << "All configurations stored successfully\n";
} else {
    std::cerr << "Some operations failed:\n" << merged.second << "\n";
}
```

---

## Migration Guide

### From Direct Provider API

**Old Approach** (direct provider usage):
```cpp
auto provider = makeProvider_filedb();
JSONDocument doc;
// ... setup doc ...
auto id = provider->writeDocument(doc);
```

**New Approach** (ConfigurationInterface):
```cpp
ConfigurationInterface ifc;
MyConfig config;
// ... setup config ...
auto result = ifc.storeVersion<MyConfig*, JsonData>(
    &config, "v1.0", "entity"
);
```

**Benefits**: Type safety, automatic serialization, cleaner API

---

## Troubleshooting

### Common Issues

**Issue**: "Unable to connect to database"
- **Solution**: Check ARTDAQ_DATABASE_URI environment variable
- Verify provider is running (MongoDB/UconDB)
- Check network connectivity and firewall

**Issue**: "Collection not found"
- **Solution**: Verify collection name matches configuration type
- Check configurationNameImpl() in your serializer
- List available collections: `ifc.listCollections("")`

**Issue**: "Serialization failed"
- **Solution**: Verify MakeSerializable specialization exists
- Check writeDocumentImpl/readDocumentImpl implementations
- Enable trace logging to see detailed errors

**Issue**: "Version already exists"
- **Solution**: Use overwriteVersion() instead of storeVersion()
- Or use a different version string
- Check if document is marked read-only

### Debug Logging

Enable comprehensive logging:
```cpp
// All ConfigurationDB operations
artdaq::database::configuration::json::enable_trace();

// Specific components
artdaq::database::configuration::debug::Multitasker();
artdaq::database::configuration::debug::options::OperationBase();
```

Set TRACE environment variables:
```bash
export TRACE_LVLS="DEBUG"
export TRACE_FILE="/tmp/artdaq_db.trace"
```

---

## Future Enhancements

Potential areas for improvement:
- **Caching Layer**: Add configurable result caching
- **Async Operations**: Support for async/await patterns
- **Transaction Support**: Multi-operation transactions
- **Versioning Improvements**: Semantic versioning support
- **Query Language**: More powerful query capabilities
- **Schema Validation**: Validate configurations against schemas
- **Audit Logging**: Track all configuration changes

---

## Related Modules

- **StorageProviders**: Actual database/file I/O implementations
- **BasicTypes**: Core type definitions
- **DataFormats**: JSON/XML parsing and generation
- **JsonDocument**: JSON document abstraction
- **SharedCommon**: Common utilities, exceptions, logging

---

## Additional Documentation

See individual file documentation:
- Core interface: `configurationdbifc.h.md`, `configurationdbifc_base.h.md`
- Threading: `Multitasker.h.md`
- Operations: `options_operation_*.h.md`
- Dispatch: `dispatch_*.h.md`

---

## Module Statistics

- **Total Files**: 54
- **Header Files**: 26
- **Implementation Files**: 27
- **Build Files**: 1 (CMakeLists.txt)
- **Total Lines**: ~8,850
- **Namespaces**: artdaq::database::configuration
- **Main Classes**: 10+
- **Supported Providers**: 3 (FileSystemDB, MongoDB, UconDB)
- **Supported Formats**: 7 (JSON, FHiCL, XML, GUI, DB, CSV, Origin)
- **Operation Categories**: 6 (Document, Config, Alias, Metadata, Search, Bulk)

---

**Documentation generated for artdaq-database ConfigurationDB module**
