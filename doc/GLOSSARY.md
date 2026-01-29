# artdaq-database Glossary

This glossary defines standard terminology for the artdaq-database project. **Use these terms consistently** throughout all documentation to ensure clarity and avoid confusion.

## How to Use This Glossary

1. **Always use the preferred term** - Do not use synonyms or abbreviations listed in "Avoid Using"
2. **Link to this glossary** when first introducing a term in documentation
3. **Add new terms** as you encounter domain-specific concepts during documentation
4. **Maintain alphabetical order** within each category

---

## Core Concepts

### alias

**Definition:** A human-readable name that points to a specific version of a configuration. Aliases provide stable references that can be updated to point to different versions over time.

**Example:** `"production"` alias pointing to version `"v2.3.1"` of a detector configuration.

**Avoid Using:** tag, label, pointer, reference

---

### collection

**Definition:** A group of related documents stored together in the database. In MongoDB, this is a native concept; in FileSystemDB, it maps to a directory structure.

**Example:** `"run_configurations"` collection containing all run-related configurations.

**Avoid Using:** table, bucket, folder, group

---

### configuration

**Definition:** A complete set of parameters that define the behavior of a system component. Configurations are stored as documents in the database and can have multiple versions.

**Example:** A detector threshold configuration with parameters like `{"threshold": 100, "enabled": true}`.

**Avoid Using:** config, conf, cfg, settings, parameters

---

### document

**Definition:** A JSON structure stored in the database. Documents are the fundamental unit of storage and contain both user data and system metadata.

**Example:**
```json
{
  "data": {"threshold": 100},
  "metadata": {"version": "1.0", "created": "2024-01-15"}
}
```

**Avoid Using:** record, entry, item, object

---

### document record

**Definition:** The internal representation of a document that includes both the user's data and all associated metadata (version, timestamps, origin, etc.). This is the complete structure managed by the database system.

**Avoid Using:** doc record, docrecord, internal document, full document

---

### entity

**Definition:** A named component in the configuration system that can have associated configurations. Entities typically represent hardware components, software processes, or logical groupings.

**Example:** `"board_reader_01"`, `"event_builder"`, `"trigger_system"`

**Avoid Using:** object, item, component (when referring to configuration entities)

---

### global configuration

**Definition:** A configuration that applies to multiple entities or the entire system. Global configurations define system-wide parameters and behaviors.

**Example:** A run control configuration that affects all subsystems.

**Avoid Using:** global config, system config, master config, root config

---

### version

**Definition:** A specific iteration of a configuration, identified by a unique identifier. Versions are immutable once created and provide a complete history of configuration changes.

**Example:** Version `"2024-01-15T10:30:00_v1"` of a detector configuration.

**Avoid Using:** revision, iteration, snapshot, checkpoint

---

## Data Formats

### FHiCL

**Definition:** Fermilab Hierarchical Configuration Language. A human-readable configuration format used extensively in the art framework and ARTDAQ ecosystem. FHiCL supports includes, variable substitution, and hierarchical structure.

**Example:**
```
physics: {
  analyzers: {
    myAnalyzer: {
      module_type: MyAnalyzer
      threshold: 100
    }
  }
}
```

**Avoid Using:** fhicl, FHICL, Fhicl (use consistent capitalization: FHiCL)

**See Also:** [FHiCL Reference](https://cdcvs.fnal.gov/redmine/projects/fhicl-cpp/wiki)

---

### JSON

**Definition:** JavaScript Object Notation. A lightweight data interchange format used as the internal representation for all configurations in artdaq-database, regardless of the original format.

**Avoid Using:** json, Json, j-son

---

### XML

**Definition:** Extensible Markup Language. A markup format supported for configuration import/export, converted to/from JSON internally.

**Avoid Using:** xml, Xml

---

## Architecture Components

### overlay

**Definition:** A type-safe wrapper class that provides structured access to JSON document fields. Overlays allow C++ code to interact with JSON data using strongly-typed methods rather than string-based key access.

**Example:** `ovlDocument` provides methods like `getVersion()`, `getTimestamp()` instead of `doc["version"]`.

**Avoid Using:** wrapper, accessor, view, facade

**See Also:** [Overlay Module](./Overlay/README.md)

---

### storage provider

**Definition:** A backend implementation responsible for persisting and retrieving documents. artdaq-database supports multiple storage providers: FileSystemDB (JSON files), MongoDB (document database), and UconDB (REST API).

**Avoid Using:** database provider, backend, store, persistence layer, data store

**See Also:** [StorageProviders Module](./StorageProviders/README.md)

---

### document builder

**Definition:** A class that constructs JSON documents using the builder pattern. The builder ensures documents have correct structure and required fields.

**Avoid Using:** doc builder, JSON builder, constructor

**See Also:** [JSONDocumentBuilder](./JsonDocument/JSONDocumentBuilder.h.md)

---

## Storage Providers

### FileSystemDB

**Definition:** A storage provider that stores documents as JSON files on the local filesystem. Uses a directory structure to organize collections and maintains a search index for queries.

**Directory Structure:**
```
databases/filesystemdb/
├── collection_name/
│   ├── document_001.json
│   ├── document_002.json
│   └── index.json
```

**Avoid Using:** filesystem DB, file system database, file-based storage, JSON storage

---

### MongoDB

**Definition:** A document-oriented NoSQL database used as a storage provider. Provides native support for JSON-like documents (BSON), indexing, and queries.

**Avoid Using:** Mongo, mongo, mongoDB

---

### UconDB

**Definition:** A REST API-based storage provider that interfaces with the UconDB web service. Used for remote configuration storage and retrieval.

**Avoid Using:** UCONDB, Ucon DB, ucon-db

---

## Operations

### bulk operation

**Definition:** An operation that processes multiple documents or configurations in a single transaction or batch. Includes bulk import, bulk export, and bulk update operations.

**Avoid Using:** batch operation, mass operation, multi-document operation

---

### export

**Definition:** The operation of retrieving configurations from the database and writing them to files in a specified format (JSON, FHiCL, XML).

**Avoid Using:** download, dump, extract, save

---

### import

**Definition:** The operation of reading configurations from files and storing them in the database.

**Avoid Using:** upload, load, ingest, insert

---

### migration

**Definition:** The process of transferring configurations between different storage providers or database versions while preserving data integrity.

**Avoid Using:** transfer, copy, move (when referring to cross-provider operations)

---

## Metadata Fields

### bookkeeping

**Definition:** System-managed metadata that tracks document lifecycle information including creation time, modification history, and administrative flags.

**Fields:** `created`, `modified`, `creator`, `modifier`

**Avoid Using:** admin data, system metadata, tracking info

---

### changelog

**Definition:** A record of modifications made to a document over time. Each entry includes timestamp, user, and description of changes.

**Avoid Using:** history, audit log, modification log, change history

---

### origin

**Definition:** Metadata identifying the source of a configuration, including the format it was imported from, source file path, and import timestamp.

**Fields:** `format`, `source`, `timestamp`

**Avoid Using:** source info, provenance, import metadata

---

### timestamp

**Definition:** A date-time value in ISO 8601 format used to record when events occurred (creation, modification, etc.).

**Format:** `YYYY-MM-DDTHH:MM:SS.sssZ`

**Example:** `"2024-01-15T10:30:00.000Z"`

**Avoid Using:** datetime, date, time (when referring to combined date-time values)

---

## Error Handling

### database_error

**Definition:** An exception type thrown when database operations fail due to connection issues, query failures, or data integrity problems.

**Avoid Using:** db_error, DBError, DatabaseException

---

### invalid_argument

**Definition:** An exception type thrown when function parameters are invalid, out of range, or violate preconditions.

**Avoid Using:** bad_argument, argument_error, InvalidArgument

---

### returned_result

**Definition:** A result wrapper type that encapsulates either a successful return value or an error condition. Used for operations that can fail without throwing exceptions.

**Example:**
```cpp
auto result = db.findConfiguration("name");
if (result) {
  auto config = result.value();
} else {
  auto error = result.error();
}
```

**Avoid Using:** result_type, maybe, optional (when referring to this specific error-handling pattern)

---

## Tools

### conftool

**Definition:** The primary command-line interface for interacting with the configuration database. Provides commands for storing, retrieving, listing, and managing configurations.

**Avoid Using:** conf_tool, config tool, configuration tool

---

### bulkloader

**Definition:** A utility for importing multiple configuration files into the database in a single operation.

**Avoid Using:** bulk_loader, bulk importer, batch loader

---

### bulkdownloader

**Definition:** A utility for exporting multiple configurations from the database to files in a single operation.

**Avoid Using:** bulk_downloader, bulk exporter, batch downloader

---

## Adding New Terms

When you encounter a new domain-specific term during documentation:

1. **Check if it exists** - Search this glossary first
2. **Determine the category** - Core Concepts, Architecture, Operations, etc.
3. **Write a clear definition** - 1-3 sentences explaining the term
4. **Provide an example** - If applicable, show concrete usage
5. **List terms to avoid** - Synonyms or abbreviations that should not be used
6. **Add cross-references** - Link to related documentation
7. **Maintain alphabetical order** - Within the appropriate category

**Template for new entries:**

```markdown
### term_name

**Definition:** Clear, concise explanation of what this term means.

**Example:** Concrete example showing the term in use.

**Avoid Using:** synonym1, synonym2, abbreviation

**See Also:** [Related Documentation](./path/to/doc.md)
```

---

## Cross-Reference Index

| Term | Primary Module | Related Terms |
|------|----------------|---------------|
| alias | ConfigurationDB | version, configuration |
| collection | StorageProviders | document |
| configuration | ConfigurationDB | document, version, entity |
| document | JsonDocument | document record, overlay |
| document builder | JsonDocument | document |
| document record | JsonDocument | document, bookkeeping |
| entity | ConfigurationDB | configuration |
| FHiCL | DataFormats | JSON, XML |
| FileSystemDB | StorageProviders | MongoDB, UconDB |
| global configuration | ConfigurationDB | configuration, entity |
| JSON | BasicTypes | FHiCL, XML |
| MongoDB | StorageProviders | FileSystemDB, UconDB |
| overlay | Overlay | document, document record |
| storage provider | StorageProviders | FileSystemDB, MongoDB, UconDB |
| UconDB | StorageProviders | FileSystemDB, MongoDB |
| version | ConfigurationDB | alias, configuration |
