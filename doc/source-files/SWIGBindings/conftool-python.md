# Python conftool SWIG Bindings

## Overview

The Python conftool bindings provide a Python interface to the artdaq-database configuration management library. Using SWIG (Simplified Wrapper and Interface Generator), the C++ configuration API is exposed to Python, enabling Python scripts and applications to interact with the database.

## Files

### conftool.h

**Location**: `/home/user/artdaq-database/artdaq-database/SWIGBindings/python/conftool/conftool.h`

**Type**: C++ header

**Purpose**: Declares C++ functions to be wrapped for Python

### conftool.cpp

**Location**: `/home/user/artdaq-database/artdaq-database/SWIGBindings/python/conftool/conftool.cpp`

**Type**: C++ implementation

**Purpose**: Implements wrapper functions that call library functions

### conftool.i

**Location**: `/home/user/artdaq-database/artdaq-database/SWIGBindings/python/conftool/conftool.i`

**Type**: SWIG interface file

**Purpose**: Defines how to generate Python bindings

---

## Architecture

### Three-Layer Design

```
┌─────────────────────────────────┐
│  Python Application             │
│  (imports conftoolp)            │
└─────────────────┬───────────────┘
                  │
┌─────────────────▼───────────────┐
│  SWIG-Generated Python Module   │
│  (conftoolp.so)                 │
└─────────────────┬───────────────┘
                  │
┌─────────────────▼───────────────┐
│  C++ Wrapper Functions          │
│  (conftool.cpp)                 │
└─────────────────┬───────────────┘
                  │
┌─────────────────▼───────────────┐
│  artdaq-database C++ Library    │
│  (configuration::json::*)       │
└─────────────────────────────────┘
```

### Component Responsibilities

**conftool.h**: Interface declaration
- Function signatures
- Type definitions (`result_t`)
- Conditional compilation (`#ifdef ADFHICLCPP`)

**conftool.cpp**: Implementation
- Locale management
- Library function calls
- Error handling and result passing

**conftool.i**: SWIG directives
- Module name definition
- Type mappings
- Template instantiations

---

## API Functions

### Document Management

#### read_document()
```cpp
result_t read_document(std::string const& query_payload);
```

**Purpose**: Read a document from the database

**Parameters**:
- `query_payload`: JSON string with query parameters

**Returns**: `(success, result_or_error_message)`

**Python Usage**:
```python
import conftoolp

query = '{"collection":"RunHistory","filter":{"entities":"MyEntity"}}'
success, result = conftoolp.read_document(query)

if success:
    print("Document:", result)
else:
    print("Error:", result)
```

#### write_document()
```cpp
result_t write_document(std::string const& query_payload,
                        std::string const& json_document);
```

**Purpose**: Write a document to the database

**Parameters**:
- `query_payload`: JSON query with collection, entity, version
- `json_document`: Document content as JSON

**Returns**: `(success, result_or_error_message)`

**Python Usage**:
```python
query = '{"collection":"RunHistory","entity":"Test","version":"v1"}'
document = '{"data":"value"}'
success, msg = conftoolp.write_document(query, document)
```

#### mark_document_readonly()
```cpp
result_t mark_document_readonly(std::string const& query_payload);
```

**Purpose**: Mark a document as read-only

**Python Usage**:
```python
success, msg = conftoolp.mark_document_readonly(query)
```

#### mark_document_deleted()
```cpp
result_t mark_document_deleted(std::string const& query_payload);
```

**Purpose**: Mark a document as deleted

**Python Usage**:
```python
success, msg = conftoolp.mark_document_deleted(query)
```

### Version Management

#### find_versions()
```cpp
result_t find_versions(std::string const& query_payload);
```

**Purpose**: Find all versions of a document

**Returns**: JSON array of version information

**Python Usage**:
```python
query = '{"collection":"RunHistory","entity":"MyEntity"}'
success, versions_json = conftoolp.find_versions(query)
```

#### add_version_alias()
```cpp
result_t add_version_alias(std::string const& query_payload);
```

**Purpose**: Add an alias to a version

**Python Usage**:
```python
query = '{"entity":"MyEntity","version":"v1","alias":"stable"}'
success, msg = conftoolp.add_version_alias(query)
```

#### remove_version_alias()
```cpp
result_t remove_version_alias(std::string const& query_payload);
```

**Purpose**: Remove a version alias

#### find_version_aliases()
```cpp
result_t find_version_aliases(std::string const& query_payload);
```

**Purpose**: List all version aliases

### Entity Management

#### find_entities()
```cpp
result_t find_entities(std::string const& query_payload);
```

**Purpose**: Find entities in a collection

**Python Usage**:
```python
query = '{"collection":"RunHistory"}'
success, entities = conftoolp.find_entities(query)
```

#### add_entity()
```cpp
result_t add_entity(std::string const& query_payload);
```

**Purpose**: Add a new entity to a collection

#### remove_entity()
```cpp
result_t remove_entity(std::string const& query_payload);
```

**Purpose**: Remove an entity from a collection

### Configuration Management

#### find_configurations()
```cpp
result_t find_configurations(std::string const& query_payload);
```

**Purpose**: List available configurations

**Python Usage**:
```python
success, configs = conftoolp.find_configurations('{}')
```

#### configuration_composition()
```cpp
result_t configuration_composition(std::string const& query_payload);
```

**Purpose**: Get the composition of a configuration

**Returns**: JSON describing all documents in configuration

**Python Usage**:
```python
query = '{"configuration":"Production_v1"}'
success, composition = conftoolp.configuration_composition(query)
```

#### create_configuration()
```cpp
result_t create_configuration(std::string const& query_payload);
```

**Purpose**: Create a new configuration

#### assign_configuration()
```cpp
result_t assign_configuration(std::string const& query_payload);
```

**Purpose**: Assign configuration to a run

#### remove_configuration()
```cpp
result_t remove_configuration(std::string const& query_payload);
```

**Purpose**: Remove a configuration

#### read_configuration()
```cpp
result_t read_configuration(std::string const& query_payload);
```

**Purpose**: Read an entire configuration

#### write_configuration()
```cpp
result_t write_configuration(std::string const& query_payload,
                              std::string const& json_document);
```

**Purpose**: Write an entire configuration

### Import/Export

#### export_configuration()
```cpp
result_t export_configuration(std::string const& query_payload);
```

**Purpose**: Export a configuration to JSON

**Python Usage**:
```python
query = '{"configuration":"MyConfig"}'
success, export_json = conftoolp.export_configuration(query)
```

#### import_configuration()
```cpp
result_t import_configuration(std::string const& query_payload);
```

**Purpose**: Import a configuration from JSON

#### export_database()
```cpp
result_t export_database(std::string const& query_payload);
```

**Purpose**: Export entire database

#### import_database()
```cpp
result_t import_database(std::string const& query_payload);
```

**Purpose**: Import entire database

#### export_collection()
```cpp
result_t export_collection(std::string const& query_payload);
```

**Purpose**: Export a collection

#### import_collection()
```cpp
result_t import_collection(std::string const& query_payload);
```

**Purpose**: Import a collection

### Search

#### search_collection()
```cpp
result_t search_collection(std::string const& query_payload);
```

**Purpose**: Search within a collection

**Python Usage**:
```python
query = '{"collection":"RunHistory","filter":{"someField":"value"}}'
success, results = conftoolp.search_collection(query)
```

### Metadata

#### list_databases()
```cpp
result_t list_databases(std::string const& query_payload);
```

**Purpose**: List available databases

#### read_dbinfo()
```cpp
result_t read_dbinfo(std::string const& query_payload);
```

**Purpose**: Get database information

#### list_collections()
```cpp
result_t list_collections(std::string const& query_payload);
```

**Purpose**: List collections in database

**Python Usage**:
```python
success, collections = conftoolp.list_collections('{}')
```

### FHiCL Utilities

#### fhicl_to_json()
```cpp
#ifdef ADFHICLCPP
result_t fhicl_to_json(std::string const& fcl,
                       std::string const& filename);
#endif
```

**Purpose**: Convert FHiCL to JSON

**Availability**: Only if compiled with FHiCL support

**Python Usage** (if available):
```python
fcl_content = "key: value"
success, json_str = conftoolp.fhicl_to_json(fcl_content, "config.fcl")
```

#### json_to_fhicl()
```cpp
#ifdef ADFHICLCPP
result_t json_to_fhicl(std::string const& jsn,
                       std::string& filename);
#endif
```

**Purpose**: Convert JSON to FHiCL

### Utilities

#### enable_trace()
```cpp
void enable_trace();
```

**Purpose**: Enable debug tracing

**Python Usage**:
```python
conftoolp.enable_trace()
```

#### set_default_locale()
```cpp
void set_default_locale();
```

**Purpose**: Set default locale for number formatting

**Note**: Called automatically by all functions

---

## Type Definitions

### result_t

```cpp
typedef std::pair<bool, std::string> result_t;
```

**Components**:
- `first` (bool): Success flag
- `second` (string): Result data or error message

**Python Type**: Tuple `(bool, str)`

**Python Usage**:
```python
success, message = conftoolp.some_function(args)

if success:
    # message contains result data
    print("Result:", message)
else:
    # message contains error description
    print("Error:", message)
```

---

## SWIG Interface (conftool.i)

### Module Definition

```swig
%module conftoolp
```

Defines the Python module name: `import conftoolp`

### Includes

```swig
%{
    #include "conftool.h"
%}
```

Includes C++ headers in generated wrapper code.

### Type Mappings

```swig
%include "std_pair.i"
%include "std_string.i"
```

Enables automatic conversion:
- C++ `std::string` ↔ Python `str`
- C++ `std::pair` ↔ Python `tuple`

### Template Instantiation

```swig
%template(result_pair_t) std::pair<bool, std::string>;
```

Creates Python type for the result pair.

### Interface Inclusion

```swig
%include "conftool.h"
```

Processes header to generate bindings for all declared functions.

---

## Implementation Details (conftool.cpp)

### Namespace Aliases

```cpp
namespace impl = artdaq::database::configuration::json;
namespace fjlib = artdaq::database::fhicljson;
```

Simplifies access to library functions.

### Locale Management

Every function calls:
```cpp
set_default_locale();
```

Ensures consistent number formatting across C++/Python boundary.

### Library Delegation

Wrapper functions delegate to library:
```cpp
result_t read_document(std::string const& query_payload) {
  set_default_locale();
  auto conf = std::string{};
  return impl::read_document(query_payload, conf);
}
```

**Pattern**:
1. Set locale
2. Prepare output variables
3. Call library function
4. Return result

### Conditional Compilation

FHiCL functions wrapped only if available:
```cpp
#ifdef ADFHICLCPP
result_t fhicl_to_json(...) { ... }
#endif
```

---

## Building Python Bindings

### CMake Configuration

```cmake
find_package(SWIG REQUIRED)
find_package(PythonLibs REQUIRED)

swig_add_library(conftoolp
  TYPE MODULE
  LANGUAGE python
  SOURCES conftool.i conftool.cpp
)
```

### Build Process

1. **SWIG Generation**:
   ```
   conftool.i → SWIG → conftool_wrap.cxx
   ```

2. **Compilation**:
   ```
   conftool.cpp + conftool_wrap.cxx → conftoolp.so
   ```

3. **Python Import**:
   ```python
   import conftoolp  # Loads conftoolp.so
   ```

### Installation

Python module typically installed to:
```
/usr/local/lib/pythonX.Y/site-packages/conftoolp.so
```

Or in development:
```
artdaq-database/build/lib/conftoolp.so
```

---

## Python Usage Examples

### Complete Example

```python
#!/usr/bin/env python3
import conftoolp
import json

# Enable debugging
conftoolp.enable_trace()

# Prepare query
query = {
    "collection": "RunHistory",
    "entity": "TestEntity",
    "version": "v1"
}

# Write document
document = {
    "data": {
        "parameter1": "value1",
        "parameter2": 42
    }
}

success, msg = conftoolp.write_document(
    json.dumps(query),
    json.dumps(document)
)

if not success:
    print(f"Write failed: {msg}")
    exit(1)

print("Write successful")

# Read it back
success, result = conftoolp.read_document(json.dumps(query))

if success:
    doc = json.loads(result)
    print("Document:", json.dumps(doc, indent=2))
else:
    print(f"Read failed: {result}")
```

### Error Handling Pattern

```python
def safe_call(func, *args):
    """Wrapper for conftoolp calls with error handling"""
    success, result = func(*args)
    if not success:
        raise RuntimeError(f"Operation failed: {result}")
    return result

# Usage
try:
    result = safe_call(conftoolp.read_document, query_json)
    print("Success:", result)
except RuntimeError as e:
    print("Error:", e)
```

### Configuration Management

```python
import conftoolp
import json

# List configurations
success, configs_json = conftoolp.find_configurations('{}')
if success:
    configs = json.loads(configs_json)
    print("Available configurations:")
    for config in configs:
        print(f"  - {config}")

# Get configuration composition
query = json.dumps({"configuration": "Production_v1"})
success, comp = conftoolp.configuration_composition(query)
if success:
    composition = json.loads(comp)
    print(f"Configuration has {len(composition)} documents")
```

### Bulk Operations

```python
def bulk_import(documents):
    """Import multiple documents"""
    results = []
    for doc in documents:
        query = json.dumps({
            "collection": doc["collection"],
            "entity": doc["entity"],
            "version": doc["version"]
        })
        success, msg = conftoolp.write_document(
            query,
            json.dumps(doc["data"])
        )
        results.append((doc["entity"], success, msg))
    return results
```

---

## Error Handling

### C++ Exceptions

C++ exceptions are caught and returned as `(false, error_message)`:
- No Python exceptions raised by SWIG layer
- Always check return tuple first element

### Python Error Handling

```python
success, result = conftoolp.some_operation(args)

if not success:
    # Handle error
    logger.error(f"Operation failed: {result}")
    # Decide: retry, abort, fallback, etc.
else:
    # Process result
    data = json.loads(result)
```

### Common Errors

**JSON Parse Errors**:
```python
# Invalid JSON in query
success, error = conftoolp.read_document("not json")
# success = False, error = "Parse error..."
```

**Missing Required Fields**:
```python
# Incomplete query
query = json.dumps({"collection": "Test"})  # Missing entity
success, error = conftoolp.read_document(query)
# success = False, error = "Missing required field: entity"
```

**Database Connection**:
```python
# If database unavailable
success, error = conftoolp.list_databases('{}')
# success = False, error = "Cannot connect to database"
```

---

## Performance Considerations

### String Copying

Each call involves string copies:
```
Python str → C++ std::string → Library → C++ std::string → Python str
```

For large documents, consider:
- Batch operations
- Caching results
- Streaming APIs (if available)

### Locale Setting

Every function calls `set_default_locale()`:
- Overhead is minimal
- Ensures correctness
- Thread-safe

### JSON Parsing

JSON parsing happens in both Python and C++:
- Python: For building queries
- C++: Library parses queries
- Consider pre-serializing frequently-used queries

---

## Thread Safety

### C++ Library

Thread safety depends on underlying library:
- Check ConfigurationDB documentation
- Some operations may require locks

### Python GIL

Python's Global Interpreter Lock (GIL) protects:
- Python data structures
- SWIG-wrapped calls

**Note**: If C++ library has threading issues, Python won't fix them.

---

## Limitations

### No Object-Oriented API

SWIG bindings provide functional interface:
- Functions, not classes
- No Python-native objects
- Consider wrapping in Python classes

### Binary Return Values

All results as JSON strings:
- Must parse JSON in Python
- No typed return objects
- No schema validation

### Error Reporting

Limited error information:
- Bool + string only
- No error codes
- No structured error data

### Callback Support

No callbacks from C++ to Python:
- Synchronous operations only
- No progress reporting
- No async operations

---

## Best Practices

### 1. JSON Validation

Validate before calling:
```python
import jsonschema

query_schema = {
    "type": "object",
    "required": ["collection", "entity"],
    "properties": {
        "collection": {"type": "string"},
        "entity": {"type": "string"}
    }
}

jsonschema.validate(query, query_schema)
success, result = conftoolp.read_document(json.dumps(query))
```

### 2. Wrap in Classes

```python
class DatabaseDocument:
    def __init__(self, collection, entity, version):
        self.collection = collection
        self.entity = entity
        self.version = version

    def read(self):
        query = json.dumps({
            "collection": self.collection,
            "entity": self.entity,
            "version": self.version
        })
        success, result = conftoolp.read_document(query)
        if not success:
            raise RuntimeError(result)
        return json.loads(result)

    def write(self, data):
        query = json.dumps({
            "collection": self.collection,
            "entity": self.entity,
            "version": self.version
        })
        success, msg = conftoolp.write_document(
            query, json.dumps(data)
        )
        if not success:
            raise RuntimeError(msg)
```

### 3. Connection Management

```python
class DatabaseConnection:
    def __enter__(self):
        # Setup connection
        self.test_connection()
        return self

    def __exit__(self, *args):
        # Cleanup
        pass

    def test_connection(self):
        success, _ = conftoolp.list_databases('{}')
        if not success:
            raise ConnectionError("Database unavailable")

# Usage
with DatabaseConnection() as db:
    # Operations
    pass
```

---

## Dependencies

### Build-Time
- SWIG 3.0+
- Python development headers
- C++ compiler

### Runtime
- Python interpreter
- artdaq-database shared libraries
- conftoolp.so module

---

## Related Files

- **conftool.py**: Python script using these bindings
- **ConfigurationDB**: C++ library being wrapped
- **CMakeLists.txt**: Build configuration

---

## Summary

The Python conftool SWIG bindings provide comprehensive access to artdaq-database configuration management from Python. The bindings expose all major database operations through a functional interface, with JSON-based parameters and results. While the API is low-level, it can be wrapped in higher-level Python classes for easier use.

**Key Features**:
- Complete API coverage
- Simple result tuple pattern
- JSON-based communication
- Automatic type conversion
- Conditional FHiCL support

**Best For**:
- Python automation scripts
- Web interfaces
- Configuration management tools
- Testing and validation
- Data migration scripts
