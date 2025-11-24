# provider_filedb_readwrite.cpp

## File Overview

This file implements the core read and write operations for the FileSystemDB provider. It provides the template specializations for `readDocument()` and `writeDocument()` functions that handle actual file I/O and index management.

**Location**: `/home/user/artdaq-database/artdaq-database/StorageProviders/FileSystemDB/provider_filedb_readwrite.cpp`

**Purpose**: Core document read/write operations with filesystem storage

## Template Specialization: readDocument

```cpp
template <>
template <>
std::vector<JSONDocument>
StorageProvider<JSONDocument, FileSystemDB>::readDocument(JSONDocument const& arg);
```

### Purpose

Read documents from filesystem based on search criteria.

### Algorithm

1. **Extract Collection Name**: From filter or arg document
2. **Build Collection Path**: Append collection to database path
3. **Expand Variables**: Process environment variables in path
4. **Load Search Index**: Open `index.json` for the collection
5. **Query Index**: Find matching document IDs
6. **Read Files**: For each ID, read corresponding `.json` file
7. **Return Documents**: Collect all matching documents

### Implementation Details

```cpp
// Extract collection name (try filter first, then arg)
auto collection_name = filter_document.findChild(jsonliteral::collection).value();
if (collection_name.empty()) {
    collection_name = arg.findChild(jsonliteral::collection).value();
}

// Build path
auto collection = _provider->connection() + collection_name;
collection = expand_environment_variables(collection);

// Query index
SearchIndex search_index(index_path);
auto oids = search_index.findDocumentIDs(filter_json);

// Read each document
for (auto const& oid : oids) {
    auto doc_path = boost::filesystem::path(dir_name).append(oid).replace_extension(".json");
    auto json = std::string{};
    db::read_buffer_from_file(json, {doc_path.c_str()});
    returnCollection.emplace_back(json);
}
```

### Error Handling

- Throws `runtime_error` if collection name missing
- Throws if collection directory doesn't exist
- Throws if document files can't be read

---

## Template Specialization: writeDocument

```cpp
template <>
object_id_t
StorageProvider<JSONDocument, FileSystemDB>::writeDocument(JSONDocument const& arg);
```

### Purpose

Write a document to filesystem and update the search index.

### Algorithm

1. **Extract Document**: Get user document from arg
2. **Extract Collection**: Find collection name
3. **Generate/Extract OID**: Generate new or extract existing object ID
4. **Create Collection Directory**: Ensure directory exists
5. **Write Document File**: Save JSON to `<oid>.json`
6. **Update Index**: Add document to search index
7. **Return OID**: Return the document's object ID

### Implementation Details

```cpp
// Extract user document and collection
auto user_document = arg.findChildDocument(jsonliteral::document);
auto collection_name = user_document.findChild(jsonliteral::collection).value();

// Get or generate object ID
auto oid = user_document.findChild(jsonliteral::oid).value();
if (oid.empty()) {
    oid = generate_oid();  // Generate new ID
}

// Write file
auto filename = collection_dir + "/" + oid + ".json";
db::write_buffer_to_file(user_document.to_string(), filename);

// Update index
SearchIndex search_index(index_path);
search_index.addDocument(user_document, oid);
// Auto-saved on search_index destruction
```

### New vs. Update

- **New Document**: If no `_id` field, generates new OID
- **Update Document**: If `_id` field present, updates existing file
- **Index**: Updated in both cases

### Error Handling

- Throws if collection name missing
- Throws if file write fails (permissions, disk space)
- Throws if index update fails

---

## File Naming Convention

Documents are stored as: `<collection>/<object_id>.json`

**Example**:
```
/data/artdaq/configs/
└── ComponentConfigs/
    ├── index.json
    ├── 507f1f77bcf86cd799439011.json
    └── 507f1f77bcf86cd799439012.json
```

---

## Index Integration

Both operations integrate with SearchIndex:
- **Read**: Queries index for matching OIDs
- **Write**: Updates index with new/modified document

Index is automatically saved when SearchIndex object destructs.

---

## Thread Safety

**Not thread-safe**: Concurrent reads/writes to same collection require external synchronization. The index file and document files can be corrupted by simultaneous writes.

**Recommendation**: Use file locking or mutex for production use.

---

## Performance

### Read Operation
- **Index Query**: O(k * log n) where k = criteria, n = documents
- **File Reads**: O(m) where m = matching documents
- **Total**: O(k * log n + m)

### Write Operation
- **File Write**: O(1) per document
- **Index Update**: O(1) per indexed attribute
- **Total**: O(1)

---

## Usage Example

```cpp
// Write document
JSONDocument doc;
doc.setCollection("RunConfigs");
doc.setData("{\"run\": 12345, \"config\": \"test\"}");

auto oid = provider->writeDocument(doc);
std::cout << "Wrote document with ID: " << oid << std::endl;

// Read documents
JSONDocument filter;
filter.setCollection("RunConfigs");
filter.setFilter("{\"run\": 12345}");

auto results = provider->readDocument(filter);
for (auto const& result : results) {
    std::cout << "Found: " << result << std::endl;
}
```

---

## Related Files

- **provider_filedb.h** - Provider class declaration
- **provider_filedb_index.h** - SearchIndex class
- **provider_connection.cpp** - Connection management
- **SharedCommon/fileststem_functions.cpp** - File I/O utilities

---

**Documentation generated for artdaq-database FileSystemDB provider**
