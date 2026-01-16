# conftool.h

**Path:** `artdaq-database/SWIGBindings/python/conftool/conftool.h`

**Purpose:** This header file declares the C++ functions that are exposed to Python through SWIG bindings. It defines a simplified interface for interacting with the artdaq-database configuration management system, using JSON-based payloads for all operations. The interface is designed for easy wrapping with SWIG by avoiding templates and using a consistent return type pattern.


## Key Concepts

### SWIG Bindings
SWIG (Simplified Wrapper and Interface Generator) automatically generates Python bindings from C++ header files. This header is specifically designed for SWIG compatibility:
- Uses simple C-compatible function signatures (no templates in the interface)
- Consistent return type (`result_t`) across all functions
- String-based parameters for JSON payloads
- No exceptions thrown (errors returned via result tuple)

### Python/C++ Interface
The interface uses JSON strings for communication between Python and C++:
- All queries are passed as JSON-encoded strings
- Results are returned as `std::pair<bool, std::string>` which maps to Python tuples
- The boolean indicates success/failure; the string contains data or error message

### Python Module Name
The generated Python module is named `conftoolp` (with 'p' suffix for Python). Import it as:
```python
import conftoolp
```

### conftool Functionality
Provides access to the complete artdaq-database API for:
- Document CRUD operations (read, write, mark as readonly/deleted)
- Version and entity management
- Configuration lifecycle management
- Import/export operations for data migration
- Database metadata and discovery operations

## Thread Safety

- **Thread-safe:** Conditional
- **Concurrent access:** Each function call is independent; multiple Python threads can call these functions concurrently
- **Locking:** Internal locking is managed by the underlying ConfigurationDB library
- **Notes:** The Python GIL provides serialization of Python-side calls, but C++ operations may run concurrently if using multi-threading from C++

## Dependencies

| Include | Purpose |
|---------|---------|
| `<string>` | Provides `std::string` for JSON payload parameters |
| `<utility>` | Provides `std::pair` for the `result_t` return type |

## Type Definitions

### `result_t`

```cpp
typedef std::pair<bool, std::string> result_t;
```

**Brief:** Standard return type for all API functions, representing success/failure status and result data or error message.

**Components:**
- `first` (bool): `true` if operation succeeded, `false` otherwise
- `second` (string): JSON result data on success, or error message on failure

**Python Equivalent:** `tuple[bool, str]`

**Example:**
```python
import conftoolp
import json

success, result = conftoolp.read_document('{"collection":"test"}')
if success:
    data = json.loads(result)
    print("Document:", data)
else:
    print(f"Error: {result}")
```

---

## Functions

### `set_default_locale() -> void`

**Brief:** Sets the default locale for consistent number formatting across the C++/Python boundary.

**Preconditions:**
- None

**Postconditions:**
- System locale is set to ensure decimal points use `.` not `,`

**Thread Safety:** safe

**Side Effects:**
- Modifies the global locale settings

**Example:**
```python
import conftoolp

conftoolp.set_default_locale()  # Usually not needed; called automatically by all other functions
```

---

### `enable_trace() -> void`

**Brief:** Enables debug tracing output for troubleshooting database operations.

**Preconditions:**
- None

**Postconditions:**
- Subsequent API calls will produce debug output to the trace facility

**Thread Safety:** safe

**Side Effects:**
- Enables TRACE macros in the underlying library

**Example:**
```python
import conftoolp

conftoolp.enable_trace()
# Subsequent calls will produce debug output
success, result = conftoolp.list_databases('{}')
```

---

### `read_document(query_payload) -> result_t`

**Brief:** Reads a document from the database matching the specified query criteria.

**Parameters:**
- `query_payload` - JSON string containing query parameters (collection, entity, version, filter criteria)

**Preconditions:**
- `query_payload` must be valid JSON
- Must contain required fields: collection, entity (or filter)

**Returns:** `(true, document_json)` on success, `(false, error_message)` on failure

**Postconditions:**
- On success, the result contains the document as a JSON string

**Throws:**
| Exception | Condition |
|-----------|-----------|
| None | Errors are returned via `result_t` |

**Thread Safety:** safe

**Side Effects:**
- Network/disk I/O to read from database

**Example:**
```python
import conftoolp
import json

query = json.dumps({
    "collection": "RunHistory",
    "filter": {"entities": "MyEntity"}
})
success, result = conftoolp.read_document(query)

if success:
    document = json.loads(result)
    print("Document:", document)
else:
    print("Error:", result)
```

---

### `write_document(query_payload, json_document) -> result_t`

**Brief:** Writes a document to the database at the location specified by the query.

**Parameters:**
- `query_payload` - JSON string with collection, entity, and version identifiers
- `json_document` - The document content as a JSON string

**Preconditions:**
- `query_payload` must be valid JSON with collection, entity, version
- `json_document` must be valid JSON

**Returns:** `(true, success_message)` on success, `(false, error_message)` on failure

**Postconditions:**
- On success, the document is stored in the database

**Throws:**
| Exception | Condition |
|-----------|-----------|
| None | Errors are returned via `result_t` |

**Thread Safety:** safe

**Side Effects:**
- Network/disk I/O to write to database
- Creates or updates database records

**Example:**
```python
import conftoolp
import json

query = json.dumps({
    "collection": "RunHistory",
    "entity": "TestEntity",
    "version": "v1"
})
document = json.dumps({"data": {"key": "value", "threshold": 100}})
success, msg = conftoolp.write_document(query, document)

if success:
    print("Document written successfully")
else:
    print(f"Write failed: {msg}")
```

---

### `mark_document_readonly(query_payload) -> result_t`

**Brief:** Marks a document as read-only to prevent future modifications.

**Parameters:**
- `query_payload` - JSON string identifying the document (collection, entity, version)

**Preconditions:**
- Document must exist
- Document must not already be marked deleted

**Returns:** `(true, success_message)` on success, `(false, error_message)` on failure

**Postconditions:**
- Document is marked read-only in the database

**Throws:**
| Exception | Condition |
|-----------|-----------|
| None | Errors are returned via `result_t` |

**Thread Safety:** safe

**Side Effects:**
- Modifies document metadata in database

**Example:**
```python
import conftoolp
import json

query = json.dumps({
    "collection": "RunHistory",
    "entity": "MyEntity",
    "version": "v1"
})
success, msg = conftoolp.mark_document_readonly(query)
```

---

### `mark_document_deleted(query_payload) -> result_t`

**Brief:** Marks a document as deleted (soft delete), hiding it from normal queries.

**Parameters:**
- `query_payload` - JSON string identifying the document

**Preconditions:**
- Document must exist

**Returns:** `(true, success_message)` on success, `(false, error_message)` on failure

**Postconditions:**
- Document is marked deleted but still exists in database

**Throws:**
| Exception | Condition |
|-----------|-----------|
| None | Errors are returned via `result_t` |

**Thread Safety:** safe

**Side Effects:**
- Modifies document metadata in database

**Example:**
```python
import conftoolp
import json

query = json.dumps({
    "collection": "RunHistory",
    "entity": "OldEntity",
    "version": "v1"
})
success, msg = conftoolp.mark_document_deleted(query)
```

---

### `find_versions(query_payload) -> result_t`

**Brief:** Finds all versions of a document matching the query criteria.

**Parameters:**
- `query_payload` - JSON string with collection and entity identifiers

**Preconditions:**
- `query_payload` must contain collection and entity

**Returns:** `(true, versions_json)` on success with JSON array of version information, `(false, error_message)` on failure

**Postconditions:**
- None

**Throws:**
| Exception | Condition |
|-----------|-----------|
| None | Errors are returned via `result_t` |

**Thread Safety:** safe

**Example:**
```python
import conftoolp
import json

query = json.dumps({"collection": "RunHistory", "entity": "MyEntity"})
success, versions_json = conftoolp.find_versions(query)

if success:
    versions = json.loads(versions_json)
    for v in versions:
        print(f"Version: {v}")
```

---

### `find_version_aliases(query_payload) -> result_t`

**Brief:** Lists all version aliases defined for an entity.

**Parameters:**
- `query_payload` - JSON string with collection and entity identifiers

**Preconditions:**
- `query_payload` must contain collection and entity

**Returns:** `(true, aliases_json)` on success with JSON array of alias information, `(false, error_message)` on failure

**Postconditions:**
- None

**Throws:**
| Exception | Condition |
|-----------|-----------|
| None | Errors are returned via `result_t` |

**Thread Safety:** safe

**Example:**
```python
import conftoolp
import json

query = json.dumps({"collection": "RunHistory", "entity": "MyEntity"})
success, aliases = conftoolp.find_version_aliases(query)
```

---

### `add_version_alias(query_payload) -> result_t`

**Brief:** Adds a human-readable alias to a specific version of a document.

**Parameters:**
- `query_payload` - JSON string containing entity, version, and alias name

**Preconditions:**
- The target version must exist
- The alias name must not already be in use for this entity

**Returns:** `(true, success_message)` on success, `(false, error_message)` on failure

**Postconditions:**
- The alias points to the specified version

**Throws:**
| Exception | Condition |
|-----------|-----------|
| None | Errors are returned via `result_t` |

**Thread Safety:** safe

**Side Effects:**
- Creates alias record in database

**Example:**
```python
import conftoolp
import json

query = json.dumps({
    "entity": "MyEntity",
    "version": "v1",
    "alias": "stable"
})
success, msg = conftoolp.add_version_alias(query)
```

---

### `remove_version_alias(query_payload) -> result_t`

**Brief:** Removes a version alias from an entity.

**Parameters:**
- `query_payload` - JSON string containing entity and alias name

**Preconditions:**
- The alias must exist

**Returns:** `(true, success_message)` on success, `(false, error_message)` on failure

**Postconditions:**
- The alias no longer exists

**Throws:**
| Exception | Condition |
|-----------|-----------|
| None | Errors are returned via `result_t` |

**Thread Safety:** safe

**Side Effects:**
- Removes alias record from database

**Example:**
```python
import conftoolp
import json

query = json.dumps({"entity": "MyEntity", "alias": "old_alias"})
success, msg = conftoolp.remove_version_alias(query)
```

---

### `find_entities(query_payload) -> result_t`

**Brief:** Finds all entities in a collection matching the query criteria.

**Parameters:**
- `query_payload` - JSON string with collection name and optional filter criteria

**Preconditions:**
- `query_payload` must contain collection

**Returns:** `(true, entities_json)` on success with JSON array of entity names, `(false, error_message)` on failure

**Postconditions:**
- None

**Throws:**
| Exception | Condition |
|-----------|-----------|
| None | Errors are returned via `result_t` |

**Thread Safety:** safe

**Example:**
```python
import conftoolp
import json

query = json.dumps({"collection": "RunHistory"})
success, entities = conftoolp.find_entities(query)

if success:
    entity_list = json.loads(entities)
    print(f"Found {len(entity_list)} entities")
```

---

### `add_entity(query_payload) -> result_t`

**Brief:** Adds a new entity to a collection.

**Parameters:**
- `query_payload` - JSON string with collection and entity name

**Preconditions:**
- Collection must exist
- Entity name must not already exist in collection

**Returns:** `(true, success_message)` on success, `(false, error_message)` on failure

**Postconditions:**
- Entity is added to the collection

**Throws:**
| Exception | Condition |
|-----------|-----------|
| None | Errors are returned via `result_t` |

**Thread Safety:** safe

**Side Effects:**
- Creates entity record in database

**Example:**
```python
import conftoolp
import json

query = json.dumps({"collection": "RunHistory", "entity": "NewEntity"})
success, msg = conftoolp.add_entity(query)
```

---

### `remove_entity(query_payload) -> result_t`

**Brief:** Removes an entity from a collection.

**Parameters:**
- `query_payload` - JSON string with collection and entity name

**Preconditions:**
- Entity must exist

**Returns:** `(true, success_message)` on success, `(false, error_message)` on failure

**Postconditions:**
- Entity is removed from the collection

**Throws:**
| Exception | Condition |
|-----------|-----------|
| None | Errors are returned via `result_t` |

**Thread Safety:** safe

**Side Effects:**
- Removes entity record from database

**Example:**
```python
import conftoolp
import json

query = json.dumps({"collection": "RunHistory", "entity": "OldEntity"})
success, msg = conftoolp.remove_entity(query)
```

---

### `find_configurations(query_payload) -> result_t`

**Brief:** Lists all available configurations matching the query criteria.

**Parameters:**
- `query_payload` - JSON string with optional filter criteria (can be empty `{}`)

**Preconditions:**
- None

**Returns:** `(true, configurations_json)` on success with JSON array of configuration names, `(false, error_message)` on failure

**Postconditions:**
- None

**Throws:**
| Exception | Condition |
|-----------|-----------|
| None | Errors are returned via `result_t` |

**Thread Safety:** safe

**Example:**
```python
import conftoolp
import json

success, configs = conftoolp.find_configurations('{}')

if success:
    configurations = json.loads(configs)
    for config in configurations:
        print(f"Configuration: {config}")
```

---

### `configuration_composition(query_payload) -> result_t`

**Brief:** Gets the composition of a configuration, listing all documents that make up the configuration.

**Parameters:**
- `query_payload` - JSON string with configuration name

**Preconditions:**
- Configuration must exist

**Returns:** `(true, composition_json)` on success with JSON describing all documents in the configuration, `(false, error_message)` on failure

**Postconditions:**
- None

**Throws:**
| Exception | Condition |
|-----------|-----------|
| None | Errors are returned via `result_t` |

**Thread Safety:** safe

**Example:**
```python
import conftoolp
import json

query = json.dumps({"configuration": "Production_v1"})
success, composition = conftoolp.configuration_composition(query)

if success:
    docs = json.loads(composition)
    print(f"Configuration contains {len(docs)} documents")
```

---

### `create_configuration(query_payload) -> result_t`

**Brief:** Creates a new named configuration.

**Parameters:**
- `query_payload` - JSON string with configuration details

**Preconditions:**
- Configuration name must not already exist

**Returns:** `(true, success_message)` on success, `(false, error_message)` on failure

**Postconditions:**
- New configuration is created in the database

**Throws:**
| Exception | Condition |
|-----------|-----------|
| None | Errors are returned via `result_t` |

**Thread Safety:** safe

**Side Effects:**
- Creates configuration record in database

**Example:**
```python
import conftoolp
import json

query = json.dumps({"configuration": "NewConfig_v1"})
success, msg = conftoolp.create_configuration(query)
```

---

### `assign_configuration(query_payload) -> result_t`

**Brief:** Assigns a configuration to a run or context.

**Parameters:**
- `query_payload` - JSON string with configuration and assignment details

**Preconditions:**
- Configuration must exist

**Returns:** `(true, success_message)` on success, `(false, error_message)` on failure

**Postconditions:**
- Configuration is assigned as specified

**Throws:**
| Exception | Condition |
|-----------|-----------|
| None | Errors are returned via `result_t` |

**Thread Safety:** safe

**Side Effects:**
- Modifies assignment records in database

**Example:**
```python
import conftoolp
import json

query = json.dumps({
    "configuration": "Production_v1",
    "run": 12345
})
success, msg = conftoolp.assign_configuration(query)
```

---

### `remove_configuration(query_payload) -> result_t`

**Brief:** Removes a configuration from the database.

**Parameters:**
- `query_payload` - JSON string with configuration name

**Preconditions:**
- Configuration must exist

**Returns:** `(true, success_message)` on success, `(false, error_message)` on failure

**Postconditions:**
- Configuration is removed from the database

**Throws:**
| Exception | Condition |
|-----------|-----------|
| None | Errors are returned via `result_t` |

**Thread Safety:** safe

**Side Effects:**
- Removes configuration record from database

**Example:**
```python
import conftoolp
import json

query = json.dumps({"configuration": "OldConfig"})
success, msg = conftoolp.remove_configuration(query)
```

---

### `read_configuration(query_payload) -> result_t`

**Brief:** Reads an entire configuration, returning all documents that comprise it.

**Parameters:**
- `query_payload` - JSON string with configuration name

**Preconditions:**
- Configuration must exist

**Returns:** `(true, configuration_json)` on success, `(false, error_message)` on failure

**Postconditions:**
- None

**Throws:**
| Exception | Condition |
|-----------|-----------|
| None | Errors are returned via `result_t` |

**Thread Safety:** safe

**Side Effects:**
- Network/disk I/O to read from database

**Example:**
```python
import conftoolp
import json

query = json.dumps({"configuration": "Production_v1"})
success, config_data = conftoolp.read_configuration(query)
```

---

### `write_configuration(query_payload, json_document) -> result_t`

**Brief:** Writes an entire configuration to the database.

**Parameters:**
- `query_payload` - JSON string with configuration metadata
- `json_document` - The complete configuration data as JSON

**Preconditions:**
- `query_payload` must be valid JSON
- `json_document` must be valid JSON

**Returns:** `(true, success_message)` on success, `(false, error_message)` on failure

**Postconditions:**
- Configuration is stored in the database

**Throws:**
| Exception | Condition |
|-----------|-----------|
| None | Errors are returned via `result_t` |

**Thread Safety:** safe

**Side Effects:**
- Network/disk I/O to write to database

**Example:**
```python
import conftoolp
import json

query = json.dumps({"configuration": "NewConfig_v1"})
config_data = json.dumps({"entities": [...]})
success, msg = conftoolp.write_configuration(query, config_data)
```

---

### `export_configuration(query_payload) -> result_t`

**Brief:** Exports a configuration to JSON format for backup or transfer to another system.

**Parameters:**
- `query_payload` - JSON string with configuration name

**Preconditions:**
- Configuration must exist

**Returns:** `(true, export_json)` on success with complete configuration data, `(false, error_message)` on failure

**Postconditions:**
- None

**Throws:**
| Exception | Condition |
|-----------|-----------|
| None | Errors are returned via `result_t` |

**Thread Safety:** safe

**Side Effects:**
- Network/disk I/O to read from database

**Example:**
```python
import conftoolp
import json

query = json.dumps({"configuration": "MyConfig"})
success, export_json = conftoolp.export_configuration(query)

if success:
    with open("backup.json", "w") as f:
        f.write(export_json)
    print("Configuration exported successfully")
```

---

### `import_configuration(query_payload) -> result_t`

**Brief:** Imports a configuration from JSON data.

**Parameters:**
- `query_payload` - JSON string containing configuration data to import

**Preconditions:**
- `query_payload` must contain valid configuration data

**Returns:** `(true, success_message)` on success, `(false, error_message)` on failure

**Postconditions:**
- Configuration is imported into the database

**Throws:**
| Exception | Condition |
|-----------|-----------|
| None | Errors are returned via `result_t` |

**Thread Safety:** safe

**Side Effects:**
- Network/disk I/O to write to database

**Example:**
```python
import conftoolp
import json

with open("backup.json", "r") as f:
    config_data = f.read()

success, msg = conftoolp.import_configuration(config_data)
```

---

### `export_database(query_payload) -> result_t`

**Brief:** Exports the entire database contents to JSON format.

**Parameters:**
- `query_payload` - JSON string with export options

**Preconditions:**
- Database must be accessible

**Returns:** `(true, database_json)` on success, `(false, error_message)` on failure

**Postconditions:**
- None

**Throws:**
| Exception | Condition |
|-----------|-----------|
| None | Errors are returned via `result_t` |

**Thread Safety:** safe

**Side Effects:**
- Network/disk I/O to read from database

**Example:**
```python
import conftoolp

success, db_export = conftoolp.export_database('{}')
```

---

### `import_database(query_payload) -> result_t`

**Brief:** Imports entire database contents from JSON data.

**Parameters:**
- `query_payload` - JSON string containing database data to import

**Preconditions:**
- `query_payload` must contain valid database export data

**Returns:** `(true, success_message)` on success, `(false, error_message)` on failure

**Postconditions:**
- Database contents are imported

**Throws:**
| Exception | Condition |
|-----------|-----------|
| None | Errors are returned via `result_t` |

**Thread Safety:** safe

**Side Effects:**
- Network/disk I/O to write to database
- May overwrite existing data

**Example:**
```python
import conftoolp

with open("database_backup.json", "r") as f:
    db_data = f.read()

success, msg = conftoolp.import_database(db_data)
```

---

### `export_collection(query_payload) -> result_t`

**Brief:** Exports a specific collection to JSON format.

**Parameters:**
- `query_payload` - JSON string with collection name

**Preconditions:**
- Collection must exist

**Returns:** `(true, collection_json)` on success, `(false, error_message)` on failure

**Postconditions:**
- None

**Throws:**
| Exception | Condition |
|-----------|-----------|
| None | Errors are returned via `result_t` |

**Thread Safety:** safe

**Side Effects:**
- Network/disk I/O to read from database

**Example:**
```python
import conftoolp
import json

query = json.dumps({"collection": "RunHistory"})
success, collection_data = conftoolp.export_collection(query)
```

---

### `import_collection(query_payload) -> result_t`

**Brief:** Imports a collection from JSON data.

**Parameters:**
- `query_payload` - JSON string containing collection data to import

**Preconditions:**
- `query_payload` must contain valid collection data

**Returns:** `(true, success_message)` on success, `(false, error_message)` on failure

**Postconditions:**
- Collection is imported into the database

**Throws:**
| Exception | Condition |
|-----------|-----------|
| None | Errors are returned via `result_t` |

**Thread Safety:** safe

**Side Effects:**
- Network/disk I/O to write to database

**Example:**
```python
import conftoolp

with open("collection_backup.json", "r") as f:
    collection_data = f.read()

success, msg = conftoolp.import_collection(collection_data)
```

---

### `search_collection(query_payload) -> result_t`

**Brief:** Searches within a collection using query filters to find matching documents.

**Parameters:**
- `query_payload` - JSON string with collection name and filter criteria

**Preconditions:**
- `query_payload` must contain collection and valid filter

**Returns:** `(true, results_json)` on success with JSON array of matching documents, `(false, error_message)` on failure

**Postconditions:**
- None

**Throws:**
| Exception | Condition |
|-----------|-----------|
| None | Errors are returned via `result_t` |

**Thread Safety:** safe

**Example:**
```python
import conftoolp
import json

query = json.dumps({
    "collection": "RunHistory",
    "filter": {"someField": "value"}
})
success, results = conftoolp.search_collection(query)

if success:
    matches = json.loads(results)
    print(f"Found {len(matches)} matching documents")
```

---

### `list_databases(query_payload) -> result_t`

**Brief:** Lists all available databases accessible by the configuration system.

**Parameters:**
- `query_payload` - JSON string with optional filter criteria (can be empty `{}`)

**Preconditions:**
- None

**Returns:** `(true, databases_json)` on success with JSON array of database names, `(false, error_message)` on failure

**Postconditions:**
- None

**Throws:**
| Exception | Condition |
|-----------|-----------|
| None | Errors are returned via `result_t` |

**Thread Safety:** safe

**Example:**
```python
import conftoolp
import json

success, dbs = conftoolp.list_databases('{}')

if success:
    databases = json.loads(dbs)
    print("Available databases:", databases)
```

---

### `read_dbinfo(query_payload) -> result_t`

**Brief:** Reads database information and metadata such as version, size, and connection status.

**Parameters:**
- `query_payload` - JSON string with optional database name

**Preconditions:**
- None

**Returns:** `(true, dbinfo_json)` on success with database metadata, `(false, error_message)` on failure

**Postconditions:**
- None

**Throws:**
| Exception | Condition |
|-----------|-----------|
| None | Errors are returned via `result_t` |

**Thread Safety:** safe

**Example:**
```python
import conftoolp
import json

success, info = conftoolp.read_dbinfo('{}')

if success:
    db_info = json.loads(info)
    print("Database info:", db_info)
```

---

### `list_collections(query_payload) -> result_t`

**Brief:** Lists all collections in the database.

**Parameters:**
- `query_payload` - JSON string with optional database name (can be empty `{}`)

**Preconditions:**
- None

**Returns:** `(true, collections_json)` on success with JSON array of collection names, `(false, error_message)` on failure

**Postconditions:**
- None

**Throws:**
| Exception | Condition |
|-----------|-----------|
| None | Errors are returned via `result_t` |

**Thread Safety:** safe

**Example:**
```python
import conftoolp
import json

success, collections = conftoolp.list_collections('{}')

if success:
    collection_list = json.loads(collections)
    print("Collections:", collection_list)
```

---

## Conditional Functions (FHiCL Support)

These functions are only available when compiled with FHiCL support (`ADFHICLCPP` macro defined).

### `fhicl_to_json(fcl, filename) -> result_t`

**Brief:** Converts FHiCL (Fermilab Hierarchical Configuration Language) content to JSON format.

**Parameters:**
- `fcl` - FHiCL content as a string
- `filename` - Original filename (used for error reporting and context)

**Preconditions:**
- `fcl` must be valid FHiCL syntax
- Only available if compiled with `ADFHICLCPP`

**Returns:** `(true, json_string)` on success, `(false, error_message)` on failure

**Postconditions:**
- None

**Throws:**
| Exception | Condition |
|-----------|-----------|
| None | Errors are returned via `result_t` |

**Thread Safety:** safe

**Example:**
```python
import conftoolp
import json

# Check if FHiCL support is available
if hasattr(conftoolp, 'fhicl_to_json'):
    fcl_content = """
    process_name: MyProcess
    source: {
      module_type: RootInput
      fileNames: ["input.root"]
    }
    """
    success, json_str = conftoolp.fhicl_to_json(fcl_content, "config.fcl")

    if success:
        config = json.loads(json_str)
        print("Converted to JSON:", config)
else:
    print("FHiCL support not available")
```

---

### `json_to_fhicl(jsn, filename) -> result_t`

**Brief:** Converts JSON content to FHiCL (Fermilab Hierarchical Configuration Language) format.

**Parameters:**
- `jsn` - JSON content as a string
- `filename` - Output filename reference

**Preconditions:**
- `jsn` must be valid JSON
- Only available if compiled with `ADFHICLCPP`

**Returns:** `(true, fhicl_string)` on success, `(false, error_message)` on failure

**Postconditions:**
- None

**Throws:**
| Exception | Condition |
|-----------|-----------|
| None | Errors are returned via `result_t` |

**Thread Safety:** safe

**Example:**
```python
import conftoolp
import json

if hasattr(conftoolp, 'json_to_fhicl'):
    json_content = json.dumps({
        "process_name": "MyProcess",
        "threshold": 100
    })
    success, fhicl_str = conftoolp.json_to_fhicl(json_content, "output.fcl")

    if success:
        print("FHiCL output:", fhicl_str)
```

---

## Relationship to Other Components

### Connection to ConfigurationDB

This header provides a simplified facade over the ConfigurationDB module:

```
conftool.h (Simplified API)
    |
    v
configurationdb.h (Aggregate header)
    |
    +-- dboperation_managedocument.h  (Document CRUD)
    +-- dboperation_managealiases.h   (Version aliases)
    +-- dboperation_manageconfigs.h   (Configuration management)
    +-- dboperation_exportimport.h    (Import/Export)
    +-- dboperation_metadata.h        (Database metadata)
    +-- dboperation_searchcollection.h (Search operations)
```

### Data Flow

```
Python Application
    |
    | import conftoolp
    v
SWIG Wrapper (conftoolp.so)
    |
    | std::string parameters
    v
conftool.h/cpp (Wrapper layer)
    |
    | Delegates to
    v
artdaq::database::configuration::json::* (Core library)
    |
    v
MongoDB/FileDB Backend
```

## See Also

- [conftool.cpp.md](./conftool.cpp.md) - Implementation of wrapper functions
- [conftool.i.md](./conftool.i.md) - SWIG interface file
- [conftool-python.md](./conftool-python.md) - Python API reference
- [ConfigurationDB/configurationdb.h.md](../ConfigurationDB/configurationdb.h.md) - Core library header

## Notes for Developers

### SWIG Considerations

1. **No Templates in Interface**: SWIG has limited template support. The `result_t` typedef wraps `std::pair<bool, std::string>` for SWIG compatibility.

2. **String-Based Communication**: All parameters are strings (JSON) to avoid complex type mappings.

3. **Consistent Return Pattern**: Every function returns `result_t`, simplifying error handling in generated wrappers.

4. **Conditional Compilation**: FHiCL functions use `#ifdef ADFHICLCPP` - check availability before use:
   ```python
   if hasattr(conftoolp, 'fhicl_to_json'):
       # FHiCL support available
   ```

### Common Pitfalls

- **Invalid JSON**: All query payloads must be valid JSON strings. Use `json.dumps()` in Python to ensure proper formatting.
- **Missing Required Fields**: Each function expects specific fields in the query payload. Check documentation for required fields.
- **Assuming FHiCL Support**: Always check `hasattr()` before using FHiCL functions.

### Anti-patterns

```python
# DON'T do this - raw string may not be valid JSON:
success, result = conftoolp.read_document("{collection: 'test'}")

# DO this instead - use json.dumps for proper JSON:
import json
query = json.dumps({"collection": "test"})
success, result = conftoolp.read_document(query)
```
