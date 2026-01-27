# provider_filedb_readwrite.cpp

**Path:** `artdaq-database/StorageProviders/FileSystemDB/provider_filedb_readwrite.cpp`

**Implements:** [provider_filedb.h](./provider_filedb.h.md)

**Purpose:** Implements the core document read and write operations for the FileSystemDB storage provider. This file provides template specializations for `readDocument()` and `writeDocument()` that handle actual file I/O with integrated search index management for efficient document retrieval.

## Implementation Overview

This file provides the FileSystemDB-specific implementations for the `StorageProvider` template class. Documents are stored as individual JSON files within collection directories, with a search index maintained in each collection for fast query operations. The read operation queries the index first to find matching document IDs, then reads only those specific files. The write operation stores the document file and updates the search index atomically.

## Key Algorithms

### Document Read Algorithm

**Steps:**
1. Extract the filter document from the input argument
2. Determine the collection name from either the filter or the argument document
3. Construct the collection path by combining the connection path with the collection name
4. Expand any environment variables in the path
5. Create the collection directory if it does not exist
6. Load the SearchIndex from the collection's `index.json` file
7. Query the index using the filter to obtain matching document IDs
8. For each matching ID, read the corresponding `.json` file from disk
9. Return a vector containing all matching documents

**Why this approach:** Using a search index avoids full directory scans, which would be O(n) for n documents. The index provides O(log n) lookup performance for common queries.

### Document Write Algorithm

**Steps:**
1. Extract the user document to be stored
2. Extract the filter document (if present) to check for existing document updates
3. Determine the collection name from the document, filter, or argument
4. Determine the object ID: use existing ID from filter (update) or generate new ID (insert)
5. Build the complete document with metadata using JSONDocumentBuilder
6. Create the collection directory if needed
7. For new documents, check for semantic duplicates (same version + entity)
8. For updates, remove the old document from the search index
9. Write the JSON document to disk as `<oid>.json`
10. Add the document to the search index
11. Return the object ID

**Why this approach:** The semantic duplicate check prevents storing configurations that would be indistinguishable by version and entity name, which would cause confusion during retrieval.

## Internal Functions

### `readDocument(JSONDocument const& arg) -> std::vector<JSONDocument>`

**Brief:** Reads documents from the filesystem that match the specified filter criteria, using the search index for efficient lookups.

**Called by:** `StorageProvider::findDocument()`, `StorageProvider::getDocument()`

**Purpose:** This template specialization implements the FileSystemDB-specific logic for reading documents. It bridges between the generic storage provider interface and the filesystem-based storage.

**Parameters:**
- `arg` - A JSON document containing the filter criteria and collection name

**Preconditions:**
- The argument must contain a valid collection name in either the filter or at the root level
- The database connection must be established

**Returns:** A vector of `JSONDocument` objects matching the filter. Returns an empty vector if no documents match.

**Postconditions:**
- The search index file is not modified
- All returned documents were present on disk at the time of the query

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Collection name is empty after all extraction attempts |
| `runtime_error` | Collection directory cannot be created or accessed |
| `runtime_error` | File read operation fails (permissions, corruption) |

**Thread Safety:** Unsafe for concurrent writes to the same collection. Safe for concurrent reads if no writes are occurring.

**Side Effects:**
- Creates the collection directory if it does not exist
- Reads files from disk

**Complexity:** O(k * log n) for index lookup plus O(m) for reading m matching documents, where n is the total number of indexed documents.

### `writeDocument(JSONDocument const& arg) -> object_id_t`

**Brief:** Writes a document to the filesystem, managing both file storage and search index updates, with duplicate detection for new documents.

**Called by:** `StorageProvider::insertDocument()`, `StorageProvider::updateDocument()`

**Purpose:** This template specialization implements the FileSystemDB-specific logic for persisting documents. It handles both new document insertion and existing document updates.

**Parameters:**
- `arg` - A JSON document containing the document to store, optional filter, and collection name

**Preconditions:**
- The argument must contain a valid document under the `document` key
- A collection name must be determinable from the document, filter, or argument
- For updates, the filter must contain a valid object ID

**Returns:** The object ID (`object_id_t`) of the written document. For new documents this is a generated ID; for updates this is the existing ID.

**Postconditions:**
- The document file exists on disk at `<collection>/<oid>.json`
- The search index contains an entry for this document
- For updates, the old index entry has been replaced

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `runtime_error` | Collection name is empty |
| `runtime_error` | For new documents: a document with the same OID file already exists |
| `runtime_error` | For new documents: a document with the same version and entity already exists (semantic duplicate) |
| `runtime_error` | Collection directory cannot be created |
| `runtime_error` | File write operation fails |

**Thread Safety:** Unsafe. Concurrent writes to the same collection can corrupt the search index or overwrite files.

**Side Effects:**
- Creates a new file or overwrites an existing file on disk
- Creates the collection directory if it does not exist
- Modifies the search index file
- For updates, reads the old document to remove it from the index

**Complexity:** O(1) for file write, O(log n) for index operations where n is the number of indexed documents.

### `filesystem::debug::ReadWrite() -> void`

**Brief:** Enables TRACE-level debug logging for document read and write operations, useful for troubleshooting filesystem storage issues.

**Called by:** Diagnostic utilities, test harnesses

**Purpose:** Activates verbose logging for all read/write operations to help diagnose storage problems.

**Parameters:** None

**Preconditions:** None

**Returns:** Nothing

**Postconditions:**
- TRACE logging is enabled for the `provider_filedb_readwrite.cpp` trace name
- All subsequent read/write operations will produce detailed log output

**Throws:** None

**Thread Safety:** Safe to call from any thread.

**Side Effects:**
- Modifies global TRACE logging state
- Enables debug logging for the index module as well

## File Storage Format

Documents are organized in a hierarchical directory structure:

```
${ARTDAQ_DATABASE_DATADIR}/filesystemdb/<database_name>/
+-- SystemMetadata/
|   |-- index.json
|   +-- <oid>.json
+-- ComponentConfigs/
|   |-- index.json
|   |-- 507f1f77bcf86cd799439011.json
|   +-- 507f1f77bcf86cd799439012.json
+-- RunConfigurations/
    |-- index.json
    +-- ...
```

Each collection directory contains:
- `index.json` - The search index mapping field values to document IDs
- `<oid>.json` - Individual document files named by their object ID

## Performance Considerations

| Operation | Time Complexity | Notes |
|-----------|-----------------|-------|
| Read (index query) | O(k * log n) | k = number of filter terms |
| Read (file I/O) | O(m) | m = number of matching documents |
| Write (file) | O(d) | d = document size |
| Write (index update) | O(a) | a = number of indexed attributes |
| Duplicate check | O(log n) | Index-based lookup |

The index provides significant performance benefits for large collections. Without the index, queries would require O(n) file reads to scan all documents.

## Error Handling Strategy

Errors are detected at multiple levels:

1. **Validation errors**: Empty collection names, missing required fields
2. **Filesystem errors**: Permission denied, disk full, file corruption
3. **Semantic errors**: Duplicate documents with same version/entity

All errors are reported via `runtime_error` exceptions with descriptive messages including the context (e.g., filename, filter used). The error messages always include the `"FileSystemDB"` prefix to identify the storage provider.

## Testing Notes

- **Unit tests:** `test/StorageProviders/FileSystemDB/`
- **Key test cases:**
  - Read with empty filter returns all documents
  - Read with specific filter returns matching subset
  - Write new document creates file and index entry
  - Write update modifies existing document
  - Duplicate detection prevents version/entity conflicts
  - Concurrent access behavior (negative tests)

## Maintenance Notes

### Semantic Duplicate Detection
The duplicate detection logic (lines 155-194) was added to prevent storing multiple configurations with the same version and entity name. This is a business logic constraint, not a technical one. If requirements change, this section may need modification.

### Index Consistency
The current implementation does not provide ACID guarantees. A crash between file write and index update could leave the database inconsistent. Future versions may need to implement write-ahead logging or journaling.

## See Also

- [provider_filedb.h](./provider_filedb.h.md) - Header file with class declarations
- [provider_filedb_index.h](./provider_filedb_index.h.md) - SearchIndex implementation
- [provider_filedb_headers.h](./provider_filedb_headers.h.md) - Aggregated includes
- [JSONDocumentBuilder.h](../../JsonDocument/JSONDocumentBuilder.h.md) - Document construction utilities
- [External: Boost.Filesystem](https://www.boost.org/doc/libs/release/libs/filesystem/) - Cross-platform filesystem operations

---

**Last Updated:** 2026-01-14
