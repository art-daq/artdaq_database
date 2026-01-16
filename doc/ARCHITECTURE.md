# artdaq-database Architecture

**Version:** 3.00.00
**Last Updated:** 2026-01-14

This document provides a comprehensive overview of the artdaq-database system architecture, designed for developers who need to understand how the system works internally.

---

## Table of Contents

1. [System Overview](#system-overview)
2. [Module Dependency Diagram](#module-dependency-diagram)
3. [Data Flow](#data-flow)
4. [Key Design Patterns](#key-design-patterns)
5. [Module Details](#module-details)
6. [Storage Provider Architecture](#storage-provider-architecture)
7. [Document Structure](#document-structure)
8. [Threading Model](#threading-model)
9. [Error Handling Strategy](#error-handling-strategy)
10. [Configuration Format Pipeline](#configuration-format-pipeline)

---

## System Overview

artdaq-database is a configuration management system designed for high-energy physics data acquisition (DAQ) systems. It provides:

- **Multi-format support**: JSON, FHiCL, XML, and Conf configuration formats
- **Multi-backend storage**: FileSystemDB, MongoDB, and UconDB
- **Version control**: Full version history with aliases for human-readable references
- **Bulk operations**: Multi-threaded import/export for large configuration sets
- **Type-safe API**: Template-based C++ interface with compile-time type checking

### High-Level Architecture

```
┌─────────────────────────────────────────────────────────────────────────┐
│                           User Applications                              │
│                    (Physics Experiments, DAQ Systems)                    │
└────────────────────────────────┬────────────────────────────────────────┘
                                 │
         ┌───────────────────────┼───────────────────────┐
         │                       │                       │
         ▼                       ▼                       ▼
┌─────────────────┐   ┌─────────────────┐   ┌─────────────────┐
│    C++ API      │   │    CLI Tools    │   │   Python API    │
│ Configuration   │   │    conftool     │   │   conftoolp     │
│   Interface     │   │   bulkloader    │   │                 │
└────────┬────────┘   └────────┬────────┘   └────────┬────────┘
         │                     │                     │
         └─────────────────────┼─────────────────────┘
                               │
                               ▼
┌─────────────────────────────────────────────────────────────────────────┐
│                         ConfigurationDB Module                           │
│                                                                          │
│  ┌──────────────────────────────────────────────────────────────────┐   │
│  │                    Operation Layer                                │   │
│  │  ManageDocument │ ManageConfigs │ ManageAliases │ BulkOperations │   │
│  └───────────────────────────────┬──────────────────────────────────┘   │
│                                  │                                       │
│  ┌───────────────────────────────▼──────────────────────────────────┐   │
│  │                    Provider Dispatch                              │   │
│  │     dispatch_filedb │ dispatch_mongodb │ dispatch_ucondb         │   │
│  └───────────────────────────────┬──────────────────────────────────┘   │
└──────────────────────────────────┼──────────────────────────────────────┘
                                   │
┌──────────────────────────────────▼──────────────────────────────────────┐
│                        StorageProviders Module                           │
│                                                                          │
│  ┌─────────────────┐  ┌─────────────────┐  ┌─────────────────┐          │
│  │   FileSystemDB  │  │    MongoDB      │  │    UconDB       │          │
│  │   (JSON files)  │  │ (Document DB)   │  │  (REST API)     │          │
│  │                 │  │                 │  │                 │          │
│  │  ┌───────────┐  │  │  ┌───────────┐  │  │  ┌───────────┐  │          │
│  │  │   Index   │  │  │  │  mongocxx │  │  │  │  libcurl  │  │          │
│  │  │  (JSON)   │  │  │  │  driver   │  │  │  │   HTTP    │  │          │
│  │  └───────────┘  │  │  └───────────┘  │  │  └───────────┘  │          │
│  └─────────────────┘  └─────────────────┘  └─────────────────┘          │
└─────────────────────────────────────────────────────────────────────────┘
```

---

## Module Dependency Diagram

The modules are organized in a layered architecture where higher layers depend on lower layers:

```
Layer 5: Application
┌─────────────────────────────────────────────────────────────────────────┐
│                              Utilities                                   │
│              conftool, bulkloader, bulkdownloader, etc.                 │
└─────────────────────────────────┬───────────────────────────────────────┘
                                  │
Layer 4: API                      │
┌─────────────────────────────────▼───────────────────────────────────────┐
│                           ConfigurationDB                                │
│                     ConfigurationInterface<T>                            │
│              Database Operations, Options, Dispatch                      │
└───────────┬─────────────────────┬───────────────────────┬───────────────┘
            │                     │                       │
Layer 3: Storage                  │                       │
┌───────────▼─────────────────────┼───────────────────────┼───────────────┐
│                          StorageProviders                                │
│         FileSystemDB          MongoDB              UconDB                │
└───────────┬─────────────────────┬───────────────────────┬───────────────┘
            │                     │                       │
Layer 2: Document Model           │                       │
┌───────────▼─────────────────────▼───────────────────────▼───────────────┐
│  ┌────────────────────────┐  ┌────────────────────────────────────────┐ │
│  │      JsonDocument      │  │                Overlay                  │ │
│  │  JSONDocument          │  │  ovlDocument, ovlBookkeeping, etc.     │ │
│  │  JSONDocumentBuilder   │  │  Type-safe JSON field access           │ │
│  └───────────┬────────────┘  └───────────────────┬────────────────────┘ │
│              │                                   │                       │
│  ┌───────────▼───────────────────────────────────▼────────────────────┐ │
│  │                          DataFormats                                │ │
│  │         Json  │  Fhicl  │  Xml  │  Conf                            │ │
│  │        Parsers and serializers for each format                      │ │
│  └───────────────────────────────┬────────────────────────────────────┘ │
└──────────────────────────────────┼──────────────────────────────────────┘
                                   │
Layer 1: Types                     │
┌──────────────────────────────────▼──────────────────────────────────────┐
│                             BasicTypes                                   │
│              JsonData, FhiclData, XmlData, Base64                       │
└──────────────────────────────────┬──────────────────────────────────────┘
                                   │
Layer 0: Foundation                │
┌──────────────────────────────────▼──────────────────────────────────────┐
│                            SharedCommon                                  │
│     Exceptions, Helper Functions, Result Types, Filesystem Utils        │
└─────────────────────────────────────────────────────────────────────────┘
```

### Dependency Rules

1. **Downward only**: Modules can only depend on modules in lower layers
2. **No cycles**: No circular dependencies between modules
3. **SharedCommon**: Foundation module with no internal dependencies
4. **Isolation**: Storage providers don't depend on each other

---

## Data Flow

### Store Configuration Flow

```
User Input                 Format Conversion            Document Building
    │                            │                            │
    ▼                            ▼                            ▼
┌─────────┐               ┌─────────────┐              ┌──────────────┐
│ FHiCL   │──────────────▶│  DataFormats │─────────────▶│ JSONDocument │
│ or JSON │               │  (parsing)   │              │   Builder    │
│ or XML  │               └─────────────┘              └──────┬───────┘
└─────────┘                                                   │
                                                              ▼
                                                   ┌──────────────────┐
                                                   │ Add Metadata     │
                                                   │ - version        │
                                                   │ - timestamp      │
                                                   │ - origin         │
                                                   │ - bookkeeping    │
                                                   └────────┬─────────┘
                                                            │
Storage                   Provider Dispatch                 │
    │                            │                          │
    ▼                            ▼                          ▼
┌─────────┐               ┌─────────────┐         ┌────────────────┐
│ Disk or │◀──────────────│  Storage    │◀────────│ ConfigurationDB│
│ MongoDB │               │  Provider   │         │   Operations   │
└─────────┘               └─────────────┘         └────────────────┘
```

### Retrieve Configuration Flow

```
Query                    Provider Selection           Document Retrieval
    │                            │                            │
    ▼                            ▼                            ▼
┌─────────────┐           ┌─────────────┐            ┌──────────────┐
│ Filter:     │──────────▶│ Dispatch to │───────────▶│ Storage      │
│ - entity    │           │ FileSystemDB│            │ Provider     │
│ - version   │           │ or MongoDB  │            │ Query        │
│ - collection│           │ or UconDB   │            └──────┬───────┘
└─────────────┘           └─────────────┘                   │
                                                            ▼
                                                   ┌──────────────────┐
                                                   │ JSONDocument     │
                                                   │ with Overlays    │
                                                   └────────┬─────────┘
                                                            │
Format Conversion         Extract User Data                 │
    │                            │                          │
    ▼                            ▼                          ▼
┌─────────┐               ┌─────────────┐         ┌────────────────┐
│ FHiCL   │◀──────────────│ DataFormats │◀────────│ Strip Metadata │
│ or JSON │               │ (serialize) │         │ Return Data    │
│ or XML  │               └─────────────┘         └────────────────┘
└─────────┘
```

---

## Key Design Patterns

### 1. Strategy Pattern (Storage Providers)

Different storage backends implement the same interface:

```cpp
template <typename TYPE, typename IMPL>
class StorageProvider {
public:
    std::vector<TYPE> readDocument(Filter const&);
    object_id_t writeDocument(TYPE const&);
    std::vector<TYPE> findConfigurations(Filter const&);
    // ... common interface
};

// Specializations for each backend
using FileSystemDBProvider = StorageProvider<JSONDocument, FileSystemDB>;
using MongoDBProvider = StorageProvider<JSONDocument, MongoDB>;
using UconDBProvider = StorageProvider<JSONDocument, UconDB>;
```

### 2. Builder Pattern (Document Construction)

Documents are constructed using a fluent builder interface:

```cpp
JSONDocumentBuilder builder;
auto document = builder
    .setCollection("Configurations")
    .setEntity("detector_01")
    .setVersion("v1.0.0")
    .setData(jsonData)
    .addBookkeeping()
    .addOrigin("fhicl", "/path/to/file.fcl")
    .build();
```

### 3. Overlay Pattern (Type-Safe Access)

Overlays provide type-safe access to JSON fields:

```cpp
// Without overlay (error-prone)
auto version = document["metadata"]["version"].as_string();

// With overlay (type-safe)
ovlDocument overlay(document);
auto version = overlay.version();      // Returns std::string
auto timestamp = overlay.timestamp();  // Returns time_point
```

### 4. Two-Namespace Pattern (Dual API)

Operations are exposed in two namespaces for different use cases:

```cpp
namespace opts {
    // Direct C++ API - takes strongly-typed options
    result_t read_document(ManageDocumentOperation const& options);
}

namespace json {
    // JSON API - takes JSON string payload (for REST/CLI)
    result_t read_document(std::string const& task_payload);
}
```

### 5. Factory Pattern (Provider Creation)

Providers are created through factory methods with the PassKey idiom:

```cpp
class FileSystemDB {
private:
    struct PassKey { explicit PassKey() = default; };
public:
    static std::shared_ptr<FileSystemDB> create(DBConfig const& config);
    FileSystemDB(PassKey, DBConfig const& config); // Only callable via create()
};
```

### 6. Command Pattern (Operation Dispatch)

Operations are packaged as functors for deferred execution:

```cpp
// In operation_dispatch.h
template <typename FUNCTION, typename... ARGS>
struct operation_functor {
    result_t operator()() { return function_(args_...); }
};

// Usage in CLI dispatch
auto functor = make_operation_functor(read_document, options);
auto result = functor();
```

---

## Module Details

### SharedCommon (Layer 0)

**Purpose**: Foundation utilities with no internal dependencies.

| Component | Description |
|-----------|-------------|
| `shared_exceptions.h` | Exception hierarchy (`database_error`, `invalid_argument`) |
| `returned_result.h` | `result_t` type for operation results |
| `helper_functions.h` | String manipulation, timestamp formatting |
| `fileststem_functions.h` | Path manipulation, file operations |
| `process_exit_codes.h` | Standardized exit codes |
| `configuraion_api_literals.h` | API string constants |

### BasicTypes (Layer 1)

**Purpose**: Type-safe wrappers for configuration data formats.

```cpp
struct JsonData {
    std::string json_buffer;  // JSON content

    explicit operator bool() const;
    friend std::ostream& operator<<(std::ostream&, JsonData const&);
};

struct FhiclData {
    std::string fhicl_buffer;  // FHiCL content
    std::string file_name;     // Source filename

    JsonData toJson() const;   // Convert to JSON
};

struct XmlData {
    std::string xml_buffer;    // XML content

    JsonData toJson() const;   // Convert to JSON
};
```

### DataFormats (Layer 2)

**Purpose**: Bidirectional conversion between formats.

```
            ┌─────────────────────────────────────────┐
            │              DataFormats                 │
            │                                          │
 Input      │  ┌─────────┐     ┌─────────────┐        │   Output
 ──────────▶│  │ Reader  │────▶│   JSON      │────────│──────────▶
            │  │ (parse) │     │ (internal)  │        │
            │  └─────────┘     └──────┬──────┘        │
            │                         │               │
            │                  ┌──────▼──────┐        │
            │                  │   Writer    │        │
            │                  │ (serialize) │        │
            │                  └─────────────┘        │
            └─────────────────────────────────────────┘

Supported format pairs:
  FHiCL ←──→ JSON
  XML   ←──→ JSON
  Conf  ←──→ JSON
  JSON  ←──→ GUI JSON (simplified for display)
```

### JsonDocument (Layer 2)

**Purpose**: Core document model and manipulation.

```cpp
class JSONDocument {
public:
    // Path-based access
    value_t& operator[](std::string const& path);
    value_t const& at(std::string const& path) const;

    // Modification
    void insert(std::string const& path, value_t const& value);
    void erase(std::string const& path);

    // Serialization
    std::string to_string() const;
    static JSONDocument from_string(std::string const& json);
};
```

### Overlay (Layer 2)

**Purpose**: Type-safe structured access to document fields.

```
JSONDocument (raw JSON)
       │
       ▼
┌──────────────────────────────────────────────────┐
│                   ovlDocument                     │
│  ┌──────────────────────────────────────────┐    │
│  │              ovlDatabaseRecord            │    │
│  │  ┌─────────────┐  ┌─────────────────┐    │    │
│  │  │   ovlId     │  │ ovlBookkeeping  │    │    │
│  │  │ (document   │  │ (created, mod-  │    │    │
│  │  │  identifier)│  │  ified, etc.)   │    │    │
│  │  └─────────────┘  └─────────────────┘    │    │
│  │  ┌─────────────┐  ┌─────────────────┐    │    │
│  │  │  ovlOrigin  │  │  ovlChangeLog   │    │    │
│  │  │ (source     │  │ (modification   │    │    │
│  │  │  format)    │  │  history)       │    │    │
│  │  └─────────────┘  └─────────────────┘    │    │
│  └──────────────────────────────────────────┘    │
└──────────────────────────────────────────────────┘
```

### StorageProviders (Layer 3)

**Purpose**: Backend-agnostic storage interface.

See [Storage Provider Architecture](#storage-provider-architecture) for details.

### ConfigurationDB (Layer 4)

**Purpose**: Main API and operation orchestration.

```
                        ConfigurationInterface<T>
                                  │
          ┌───────────────────────┼───────────────────────┐
          │                       │                       │
          ▼                       ▼                       ▼
┌─────────────────┐   ┌─────────────────┐   ┌─────────────────┐
│ ManageDocument  │   │ ManageConfigs   │   │ ManageAliases   │
│ Operations      │   │ Operations      │   │ Operations      │
│                 │   │                 │   │                 │
│ - read          │   │ - find          │   │ - add           │
│ - write         │   │ - create        │   │ - remove        │
│ - delete        │   │ - assign        │   │ - find          │
│ - find versions │   │ - remove        │   │                 │
└────────┬────────┘   └────────┬────────┘   └────────┬────────┘
         │                     │                     │
         └─────────────────────┼─────────────────────┘
                               │
                               ▼
                    ┌─────────────────────┐
                    │  Provider Dispatch  │
                    │  (selects backend)  │
                    └─────────────────────┘
```

### Utilities (Layer 5)

**Purpose**: End-user command-line tools.

| Tool | Purpose |
|------|---------|
| `conftool` | Interactive configuration management |
| `bulkloader` | Multi-threaded batch import |
| `bulkdownloader` | Multi-threaded batch export |
| `migrate_database` | Cross-provider migration |
| `fhicl2json` | Format conversion utility |
| `rebuild_database_index` | FileSystemDB index repair |

---

## Storage Provider Architecture

### Provider Comparison

| Feature | FileSystemDB | MongoDB | UconDB |
|---------|-------------|---------|--------|
| **Storage** | JSON files | BSON documents | REST API |
| **Index** | JSON file | Native | N/A |
| **Transactions** | No | Yes | No |
| **Scalability** | < 10K docs | Billions | Moderate |
| **Thread Safety** | No | Yes | Yes |
| **Query Language** | Custom filter | MongoDB query | REST params |
| **Best For** | Development | Production | Fermilab |

### FileSystemDB Structure

```
${ARTDAQ_DATABASE_URI}/
├── collection_1/
│   ├── entity_1/
│   │   ├── document_v1.json
│   │   ├── document_v2.json
│   │   └── ...
│   ├── entity_2/
│   │   └── ...
│   └── index.json          # Search index
├── collection_2/
│   └── ...
└── database_metadata.json
```

### MongoDB Schema

```javascript
// Document structure in MongoDB
{
    "_id": ObjectId("..."),
    "document": {
        "data": { /* user configuration */ },
        "metadata": {
            "version": "v1.0.0",
            "collection": "Configurations",
            "entity": "detector_01"
        }
    },
    "bookkeeping": {
        "created": ISODate("..."),
        "modified": ISODate("..."),
        "creator": "user@host"
    },
    "origin": {
        "format": "fhicl",
        "source": "/path/to/file.fcl"
    }
}
```

### UconDB REST Interface

```
Base URL: ${UCONDB_URL}

Endpoints:
  GET  /api/configurations/{collection}/{entity}?version={ver}
  POST /api/configurations/{collection}/{entity}
  GET  /api/configurations/{collection}?filter={json}
```

---

## Document Structure

### Complete Document Record

```json
{
    "document": {
        "data": {
            // User's configuration data
            "threshold": 100,
            "channels": [1, 2, 3],
            "settings": { "enabled": true }
        },
        "metadata": {
            "version": "2024-01-15T10:30:00_v1",
            "collection": "detector_configurations",
            "entity": "board_reader_01",
            "configurable_entity": "BoardReader"
        }
    },
    "bookkeeping": {
        "created": "2024-01-15T10:30:00.000Z",
        "modified": "2024-01-15T10:30:00.000Z",
        "creator": "user@hostname",
        "modifier": "user@hostname"
    },
    "origin": {
        "format": "fhicl",
        "source": "/path/to/detector_config.fcl",
        "timestamp": "2024-01-15T10:30:00.000Z"
    },
    "changelog": [
        {
            "timestamp": "2024-01-15T10:30:00.000Z",
            "user": "user@hostname",
            "action": "created",
            "comment": "Initial configuration"
        }
    ],
    "aliases": ["production", "latest"]
}
```

### Overlay Hierarchy

```
JSONDocument
    │
    └── ovlDocument
            │
            ├── ovlDatabaseRecord
            │       ├── ovlId (collection, entity, version)
            │       ├── ovlData (user configuration)
            │       └── ovlMetadata
            │
            ├── ovlBookkeeping
            │       ├── ovlTimeStamp (created)
            │       ├── ovlTimeStamp (modified)
            │       └── ovlKeyValue (creator, modifier)
            │
            ├── ovlOrigin
            │       ├── ovlKeyValue (format)
            │       ├── ovlKeyValue (source)
            │       └── ovlTimeStamp
            │
            └── ovlChangeLog
                    └── ovlMovableList<ovlUpdate>
                            ├── ovlTimeStamp
                            ├── ovlKeyValue (user)
                            ├── ovlKeyValue (action)
                            └── ovlComment
```

---

## Threading Model

### Thread Safety Summary

| Component | Thread-Safe | Recommendation |
|-----------|-------------|----------------|
| `ConfigurationInterface` | No | One instance per thread |
| `ManageDocumentOperation` | No | Don't share across threads |
| `FileSystemDB` | No | Single-threaded access |
| `MongoDB` | Yes | Connection pooling |
| `UconDB` | Yes | Stateless HTTP |
| `Multitasker` | Internal | Use from single thread |
| `JSONDocument` | No | Copy for thread transfer |

### Multitasker Architecture

```
Main Thread                    Worker Threads
     │                              │
     ▼                              │
┌─────────────┐                     │
│ Multitasker │                     │
│   .submit() │                     │
└──────┬──────┘                     │
       │                            │
       ▼                            ▼
┌──────────────────────────────────────┐
│            Task Queue                 │
│  ┌─────┐ ┌─────┐ ┌─────┐ ┌─────┐    │
│  │Task1│ │Task2│ │Task3│ │Task4│    │
│  └─────┘ └─────┘ └─────┘ └─────┘    │
└──────────────────────────────────────┘
       │         │         │         │
       ▼         ▼         ▼         ▼
   ┌───────┐ ┌───────┐ ┌───────┐ ┌───────┐
   │Worker1│ │Worker2│ │Worker3│ │Worker4│
   └───────┘ └───────┘ └───────┘ └───────┘
       │         │         │         │
       └─────────┴─────────┴─────────┘
                       │
                       ▼
                 ┌──────────┐
                 │ Results  │
                 │ Callback │
                 └──────────┘
```

### Safe Threading Pattern

```cpp
// CORRECT: Thread-local instances
void worker_thread() {
    // Each thread creates its own interface
    ConfigurationInterface<JsonData> db;
    ManageDocumentOperation opts("worker");

    // Thread-local operations
    auto result = db.readDocument(opts);
}

// INCORRECT: Shared instance
ConfigurationInterface<JsonData> shared_db;  // DON'T DO THIS

void worker_thread() {
    // Race condition!
    auto result = shared_db.readDocument(opts);
}
```

---

## Error Handling Strategy

### Result Type Pattern

```cpp
using result_t = std::pair<bool, std::string>;

result_t performOperation() {
    if (success) {
        return {true, "Operation completed: " + details};
    } else {
        return {false, "Error: " + error_message};
    }
}

// Usage
auto [success, message] = performOperation();
if (success) {
    TLOG(TLVL_INFO) << message;
} else {
    TLOG(TLVL_ERROR) << message;
}
```

### Exception Hierarchy

```
std::exception
    │
    └── artdaq::database::exception
            │
            ├── database_error
            │       │
            │       ├── connection_error
            │       ├── query_error
            │       └── storage_error
            │
            ├── invalid_argument
            │
            ├── notfound_exception
            │
            └── readonly_exception
```

### Error Propagation Flow

```
Storage Provider           ConfigurationDB            User Code
       │                         │                        │
       │   storage_error         │                        │
       ├────────────────────────▶│                        │
       │                         │   result_t{false, msg} │
       │                         ├────────────────────────▶│
       │                         │                        │
       │   OR (critical)         │                        │
       │   throw database_error  │   propagate exception  │
       ├────────────────────────▶├────────────────────────▶│
       │                         │                        │
```

---

## Configuration Format Pipeline

### Format Conversion Architecture

```
                        ┌──────────────────────┐
                        │    JSON (Internal)   │
                        │   (Pivot Format)     │
                        └──────────┬───────────┘
                                   │
       ┌───────────────────────────┼───────────────────────────┐
       │                           │                           │
       ▼                           ▼                           ▼
┌─────────────┐           ┌─────────────┐           ┌─────────────┐
│   FHiCL     │           │    XML      │           │   Conf      │
│             │           │             │           │             │
│ ┌─────────┐ │           │ ┌─────────┐ │           │ ┌─────────┐ │
│ │ Reader  │ │           │ │ Reader  │ │           │ │ Reader  │ │
│ └────┬────┘ │           │ └────┬────┘ │           │ └────┬────┘ │
│      │      │           │      │      │           │      │      │
│      ▼      │           │      ▼      │           │      ▼      │
│ ┌─────────┐ │           │ ┌─────────┐ │           │ ┌─────────┐ │
│ │ Writer  │ │           │ │ Writer  │ │           │ │ Writer  │ │
│ └─────────┘ │           │ └─────────┘ │           │ └─────────┘ │
└─────────────┘           └─────────────┘           └─────────────┘
```

### FHiCL Processing Detail

```
FHiCL Input                     JSON Output
    │                               │
    ▼                               ▼
┌───────────────────┐    ┌─────────────────────────────┐
│ physics: {        │    │ {                           │
│   analyzers: {    │    │   "physics": {              │
│     myAna: {      │────▶│     "analyzers": {         │
│       threshold:50│    │       "myAna": {            │
│     }             │    │         "threshold": 50     │
│   }               │    │       }                     │
│ }                 │    │     }                       │
└───────────────────┘    │   }                         │
                         │ }                           │
                         └─────────────────────────────┘

Key transformations:
1. Colons → JSON object structure
2. No quotes on keys → Quoted keys
3. No commas → JSON commas
4. #include → Resolved inline
5. @local::var → Variable substitution
```

---

## See Also

- [INDEX.md](./INDEX.md) - Complete module and file index
- [GLOSSARY.md](./GLOSSARY.md) - Standard terminology
- [ConfigurationDB/README.md](./ConfigurationDB/README.md) - Main API documentation
- [StorageProviders/README.md](./StorageProviders/README.md) - Storage backend details

---

*Architecture documentation for artdaq-database v3.00.00*
