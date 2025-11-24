# provider_filedb_index.cpp

## File Overview

This implementation file provides the SearchIndex class functionality for fast document lookup in the FileSystemDB provider. It implements an in-memory inverted index with persistent storage in JSON format, enabling efficient queries without file scanning.

**Location**: `/home/user/artdaq-database/artdaq-database/StorageProviders/FileSystemDB/provider_filedb_index.cpp`

**Purpose**: Implementation of searchable index for filesystem document storage

## Key Functions Implemented

### findDocumentIDs

Searches the index using multi-criteria AND logic. Implements sophisticated matching with:
- Support for multiple search criteria (version, entity, configuration, run, etc.)
- Automatic intersection of results (AND logic)
- Efficient duplicate removal
- Special handling for single vs. multiple criteria

**Algorithm**:
1. Parse JSON search criteria
2. For each criterion, call appropriate `_match*()` function
3. Collect all matching document IDs
4. For multiple criteria: keep only IDs that match ALL criteria (intersection)
5. Sort and deduplicate results

### findVersionsByGlobalConfigName

Performs an inner join between versions and configurations to find all version/configuration pairs.

### findVersionsByEntityName

Similar join between versions and entities.

### addDocument / removeDocument

Updates the index when documents are added or removed:
- Extracts indexable attributes from document
- Calls private `_add*()` or `_remove*()` methods for each attribute
- Marks index as dirty for auto-save

## Index Persistence

- **Auto-save**: Index saved to disk on destruction if modified (`_isDirty` flag)
- **Lazy loading**: Index loaded from file on construction
- **JSON format**: Human-readable and editable
- **Atomic writes**: New index written to temp file, then renamed

## Performance

- **In-memory**: All queries operate on in-memory JSON structure
- **O(log n)**: Hash map lookups for indexed attributes
- **Intersection**: Efficient multi-criteria search using count-based filtering

## Related Files

- **provider_filedb_index.h** - Class declaration
- **provider_filedb.cpp** - Uses SearchIndex for queries
- **provider_filedb_readwrite.cpp** - Updates index on writes

---

**Documentation generated for artdaq-database FileSystemDB provider**
