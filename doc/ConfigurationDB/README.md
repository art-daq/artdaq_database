# ConfigurationDB Module Documentation

## Module Overview

The **ConfigurationDB** module is the LARGEST and most complex module in artdaq-database, containing **56 source files** with approximately **9,000 lines of code**. It provides the complete infrastructure for managing artdaq configurations, including:

- High-level C++ API for configuration management (`ConfigurationInterface`)
- Database operation implementations for all providers (FileSystemDB, MongoDB, UconDB)
- Multi-threaded bulk operations support via `Multitasker`
- Version control and alias management
- Configuration composition and global configuration support
- Export/import functionality for backups and migration
- Comprehensive provider dispatch mechanism

**Location**: `artdaq-database/ConfigurationDB/`

**Purpose**: Complete configuration database management system with multi-provider support

---

## Module Architecture

### Layer Structure

```
+-------------------------------------------------------------+
|                    User Application Code                     |
+---------------------------+---------------------------------+
                            |
+---------------------------v---------------------------------+
|  ConfigurationInterface (High-Level API)                     |
|  - Template-based type-safe interface                        |
|  - Serialization support (JSON, FHiCL)                       |
|  - Version management                                        |
|  - Global configuration composition                          |
|  Files: configurationdbifc.h, configurationdbifc_base.h     |
+---------------------------+---------------------------------+
                            |
+---------------------------v---------------------------------+
|  Operation Classes (Options Layer)                           |
|  - ManageDocumentOperation                                   |
|  - ManageConfigsOperation                                    |
|  - ManageAliasesOperation                                    |
|  - BulkOperationsOperation                                   |
|  Files: options_operation_*.h/cpp                            |
+---------------------------+---------------------------------+
                            |
+---------------------------v---------------------------------+
|  Database Operations (Function Layer)                        |
|  - JSON-based operation functions                            |
|  - Format conversions                                        |
|  - Query processing                                          |
|  Files: dboperation_*.h/cpp                                  |
+---------------------------+---------------------------------+
                            |
+---------------------------v---------------------------------+
|  Implementation Details (Detail Layer)                       |
|  - Operation-specific logic                                  |
|  - Format handling                                           |
|  - Result processing                                         |
|  Files: detail_*.cpp                                         |
+---------------------------+---------------------------------+
                            |
+---------------------------v---------------------------------+
|  Provider Dispatch (Provider Layer)                          |
|  - FileSystemDB implementation                               |
|  - MongoDB implementation                                    |
|  - UconDB implementation                                     |
|  Files: dispatch_*.h/cpp, operation_dispatch.h              |
+---------------------------+---------------------------------+
                            |
+---------------------------v---------------------------------+
|  Storage Providers (Backend Layer)                           |
|  - Actual database/file I/O                                  |
|  Module: StorageProviders/                                   |
+-------------------------------------------------------------+
```

---

## Key Design Patterns

### 1. Provider Dispatch Pattern

The module routes operations to different storage backends through a unified dispatch interface:

```cpp
// Each provider namespace implements the same functions
namespace filesystem { void writeDocument(...); }
namespace mongo { void writeDocument(...); }
namespace ucon { void writeDocument(...); }

// Detail layer selects provider at runtime
if (opts.provider() == "filesystem") {
    filesystem::writeDocument(opts, doc);
} else if (opts.provider() == "mongo") {
    mongo::writeDocument(opts, doc);
}
```

### 2. Two-Namespace Pattern

Operations are declared in two namespaces for different use cases:
- `opts` - Takes operation objects directly (for C++ applications)
- `json` - Takes JSON string payloads (for REST API compatibility)

```cpp
namespace opts {
    result_t read_document(ManageDocumentOperation const& options);
}
namespace json {
    result_t read_document(std::string const& task_payload);
}
```

### 3. Functor-Based Command Dispatch

The `operation_dispatch.h` defines functor templates that wrap function pointers with bound arguments for deferred execution, enabling the command-line tool dispatch system.

---

## File Organization

### Core Interface Files (6 files)
| File | Purpose |
|------|---------|
| `configurationdbifc.h` | Main ConfigurationInterface class |
| `configurationdbifc_base.h` | Serialization support and base classes |
| `common.h` | Common includes and utilities |
| `configuration_common.h` | Configuration-specific common includes |
| `configurationdb.h` | Master include for all operations |
| `conftoolifc.h` | Command-line tool interface |

### Operation Options Classes (10 files)
| File | Purpose |
|------|---------|
| `options_operation_base.h/cpp` | Base class with common functionality |
| `options_operation_managedocument.h/cpp` | Document operations |
| `options_operation_manageconfigs.h/cpp` | Configuration management |
| `options_operation_managealiases.h/cpp` | Alias management |
| `options_operation_bulkoperations.h/cpp` | Bulk export/import |
| `options_operations.h` | Aggregate header |

### Database Operation Functions (14 files)
| File | Purpose |
|------|---------|
| `dboperation_managedocument.h/cpp` | Document CRUD |
| `dboperation_manageconfigs.h/cpp` | Global configurations |
| `dboperation_managealiases.h/cpp` | Version aliases |
| `dboperation_metadata.h/cpp` | Database metadata |
| `dboperation_searchcollection.h/cpp` | Collection search |
| `dboperation_exportimport.h/cpp` | Export/import |
| `dboperation_findcompositions.h/cpp` | Find compositions |

### Implementation Details (6 files)
| File | Purpose |
|------|---------|
| `detail_managedocument.cpp` | Document operation logic |
| `detail_manageconfigs.cpp` | Configuration logic |
| `detail_managealiases.cpp` | Alias logic |
| `detail_metadata.cpp` | Metadata logic |
| `detail_searchcollection.cpp` | Search logic |
| `detail_exportimport.cpp` | Export/import logic |
| `detail_findcompositions.cpp` | Composition finding logic |

### Provider Dispatch (10 files)
| File | Purpose |
|------|---------|
| `dispatch_common.h` | Aggregates all provider headers |
| `dispatch_signatures.h/cpp` | Function signature definitions |
| `dispatch_filedb.h/cpp` | FileSystemDB provider |
| `dispatch_mongodb.h/cpp` | MongoDB provider |
| `dispatch_ucondb.h/cpp` | UconDB provider |
| `operation_dispatch.h` | Functor templates |

### Utilities (6 files)
| File | Purpose |
|------|---------|
| `configuration_dbproviders.h/cpp` | Provider validation |
| `shared_helper_functions.h/cpp` | Format conversion |
| `Multitasker.h/cpp` | Thread pool |
| `search_filter.h` | Filter interface (legacy) |
| `enable_trace.cpp` | Trace logging |

---

## Supported Operations

### Document Operations
| Operation | Function | Description |
|-----------|----------|-------------|
| `readdocument` | `read_document()` | Read documents by filter |
| `writedocument` | `write_document()` | Write new document version |
| `markreadonly` | `mark_document_readonly()` | Protect from changes |
| `markdeleted` | `mark_document_deleted()` | Soft delete |
| `findversions` | `find_versions()` | List available versions |
| `findentities` | `find_entities()` | List entities |

### Configuration Management
| Operation | Function | Description |
|-----------|----------|-------------|
| `findconfigs` | `find_configurations()` | Search global configs |
| `createconfig` | `create_configuration()` | Create new composition |
| `assignconfig` | `assign_configuration()` | Add to composition |
| `removeconfig` | `remove_configuration()` | Remove from composition |
| `confcomposition` | `configuration_composition()` | Get composition members |

### Alias Management
| Operation | Function | Description |
|-----------|----------|-------------|
| `addversionalias` | `add_version_alias()` | Create alias |
| `rmversionalias` | `remove_version_alias()` | Delete alias |
| `findversionalias` | `find_version_aliases()` | Search aliases |

### Metadata & Search
| Operation | Function | Description |
|-----------|----------|-------------|
| `listdatabases` | `list_databases()` | List databases |
| `listcollections` | `list_collections()` | List collections |
| `readdbinfo` | `read_dbinfo()` | Get database info |
| `searchcollection` | `search_collection()` | Generic search |

### Bulk Operations
| Operation | Function | Description |
|-----------|----------|-------------|
| `exportcollection` | `export_collection()` | Export collection |
| `importcollection` | `import_collection()` | Import collection |
| `exportdatabase` | `export_database()` | Export database |
| `importdatabase` | `import_database()` | Import database |

---

## Data Formats

| Format | Enum Value | Description | Use Case |
|--------|------------|-------------|----------|
| JSON | `json` | Human-readable structured | Web, APIs |
| FHiCL | `fhicl` | Fermilab configuration | Physics apps |
| XML | `xml` | Structured markup | Legacy systems |
| GUI | `gui` | Simplified for display | User interfaces |
| DB | `db` | Internal with metadata | Storage/backup |
| CSV | `csv` | Tabular | Reports |
| Origin | `origin` | Format-agnostic | Pass-through |

---

## Environment Variables

### ARTDAQ_DATABASE_URI

**Purpose**: Specifies the database connection URI

**Format by Provider**:
```bash
# FileSystemDB
export ARTDAQ_DATABASE_URI="filesystemdb:///path/to/database"

# MongoDB
export ARTDAQ_DATABASE_URI="mongodb://host:port/database"

# UconDB
export ARTDAQ_DATABASE_URI="ucondb://host:port"
```

---

## Thread Safety

### Thread-Safe Components
- Individual operation objects (if not shared)
- Multitasker worker threads (internal)
- Database operations through providers

### NOT Thread-Safe
- **ConfigurationInterface**: Create one per thread
- **Operation classes**: Don't share across threads
- **Multitasker instance**: Use only from creating thread

### Best Practice
```cpp
void thread_worker() {
    ConfigurationInterface ifc;  // Thread-local instance
    ManageDocumentOperation opts("worker");
    // Use ifc and opts within this thread only
}
```

---

## Error Handling

### Result Type
```cpp
using result_t = std::pair<bool, std::string>;
// first:  success flag (true = success)
// second: message (success message or error description)
```

### Pattern
```cpp
auto result = operation();
if (result.first) {
    TLOG(TLVL_INFO) << "Success: " << result.second;
} else {
    TLOG(TLVL_ERROR) << "Error: " << result.second;
}
```

---

## Debugging

### Enable Trace Logging
```cpp
// All ConfigurationDB operations
artdaq::database::configuration::json::enable_trace();

// Specific components
artdaq::database::configuration::debug::Multitasker();
artdaq::database::configuration::debug::FileSystemDB();
artdaq::database::configuration::debug::MongoDB();
```

### TRACE Environment Variables
```bash
export TRACE_LVLS="DEBUG"
export TRACE_FILE="/tmp/artdaq_db.trace"
```

---

## Module Statistics

- **Total Files**: 56
- **Header Files**: 26
- **Implementation Files**: 29
- **Build Files**: 1 (CMakeLists.txt)
- **Total Lines**: ~9,000
- **Namespaces**: `artdaq::database::configuration`
- **Main Classes**: 10+
- **Supported Providers**: 3 (FileSystemDB, MongoDB, UconDB)
- **Supported Formats**: 7 (JSON, FHiCL, XML, GUI, DB, CSV, Origin)
- **Operation Categories**: 6 (Document, Config, Alias, Metadata, Search, Bulk)

---

## Getting Started

For a comprehensive guide to using `ConfigurationInterface`, see:

**[TUTORIAL_ConfigurationInterface.md](./TUTORIAL_ConfigurationInterface.md)** - Complete tutorial for junior developers covering:
- Core concepts (versions, entities, global configurations)
- Environment setup and prerequisites
- Step-by-step usage examples
- Making configurations serializable
- Error handling strategies
- Multi-threaded operations
- Best practices and troubleshooting

---

## See Also

- **[TUTORIAL_ConfigurationInterface.md](./TUTORIAL_ConfigurationInterface.md)** - Complete usage tutorial
- **[configurationdbifc.h.md](./configurationdbifc.h.md)** - API reference documentation
- Individual file documentation in this directory
- `StorageProviders/` module for backend implementations
- `JsonDocument/` module for JSON document handling
- `DataFormats/` module for format conversions

---

**Documentation generated for artdaq-database ConfigurationDB module**
