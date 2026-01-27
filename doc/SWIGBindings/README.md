# SWIGBindings Module

## Overview

The SWIGBindings module provides language bindings for the artdaq-database C++ library, enabling access from scripting languages like Python. Using SWIG (Simplified Wrapper and Interface Generator), the module exposes the configuration database API to non-C++ environments.

This module is essential for:
- **Automation scripts**: Programmatic configuration management
- **Web interfaces**: Backend services for configuration GUIs
- **Testing and validation**: Python-based test frameworks
- **Integration**: Connecting artdaq-database with Python tooling

## Architecture

### Three-Layer Design

```
+----------------------------------+
|   Python Application             |
|   (imports conftoolp)            |
+------------------+---------------+
                   |
+------------------v---------------+
|   SWIG-Generated Python Module   |
|   (conftoolp.so)                 |
+------------------+---------------+
                   |
+------------------v---------------+
|   C++ Wrapper Functions          |
|   (conftool.cpp)                 |
+------------------+---------------+
                   |
+------------------v---------------+
|   artdaq-database C++ Library    |
|   (configuration::json::*)       |
+----------------------------------+
```

### Data Flow

All API calls follow a JSON-based pattern:

1. Python application creates JSON query string
2. SWIG converts Python `str` to C++ `std::string`
3. Wrapper function sets locale and delegates to library
4. Library returns `std::pair<bool, std::string>`
5. SWIG converts to Python `tuple(bool, str)`
6. Application parses JSON result

## Key Classes

| Class/Type | Purpose |
|------------|---------|
| `result_t` | Type alias for `std::pair<bool, std::string>` - standard return type for all API functions |

## Key Concepts

### SWIG (Simplified Wrapper and Interface Generator)

SWIG automatically generates language bindings from C++ code:
- Parses C++ headers
- Generates wrapper code
- Handles type conversions
- Manages memory across language boundaries

### JSON-Based API

The conftool API uses JSON strings for all communication:
- **Input**: JSON query payloads specifying operation parameters
- **Output**: JSON result data or error messages
- **Advantages**: Simple type mapping, language-agnostic, human-readable

### Result Tuple Pattern

Every API function returns `(bool, str)`:
- `True, data` - Operation succeeded, data contains JSON result
- `False, error` - Operation failed, error contains error message

```python
success, result = conftoolp.read_document(query)
if success:
    data = json.loads(result)
else:
    print(f"Error: {result}")
```

## Dependencies

### Internal Dependencies
- **ConfigurationDB**: Core configuration database operations
- **DataFormats/Fhicl**: FHiCL conversion (optional)

### External Dependencies
- **SWIG 3.0+**: Wrapper code generation
- **Python 3.x**: Target language runtime
- **Python development headers**: For building the module

## Thread Safety

- **Module-level:** The conftoolp module is thread-safe for independent operations
- **Function-level:** Each function call is atomic from Python's perspective
- **GIL:** Python's Global Interpreter Lock provides serialization
- **Notes:** Thread safety of underlying database operations depends on ConfigurationDB implementation

## Files

| File | Description |
|------|-------------|
| `python/conftool/conftool.h` | C++ header declaring all API functions and the `result_t` type |
| `python/conftool/conftool.cpp` | Implementation that delegates to ConfigurationDB library |
| `python/conftool/conftool.i` | SWIG interface file driving Python wrapper generation |

### Documentation Files

| File | Description |
|------|-------------|
| [conftool.h.md](./conftool.h.md) | API documentation for the C++ header |
| [conftool.cpp.md](./conftool.cpp.md) | Implementation details and patterns |
| [conftool.i.md](./conftool.i.md) | SWIG interface file documentation |
| [conftool-python.md](./conftool-python.md) | Comprehensive Python API reference |

## Usage Examples

### Basic Usage

```python
import conftoolp
import json

# List collections
success, result = conftoolp.list_collections('{}')
if success:
    collections = json.loads(result)
    print("Collections:", collections)
```

### Document Operations

```python
import conftoolp
import json

# Read a document
query = json.dumps({
    "collection": "RunHistory",
    "entity": "MyEntity",
    "version": "v1"
})
success, result = conftoolp.read_document(query)

if success:
    document = json.loads(result)
    print("Document:", document)
else:
    print("Error:", result)

# Write a document
document = json.dumps({"threshold": 100, "enabled": True})
success, msg = conftoolp.write_document(query, document)
```

### Configuration Management

```python
import conftoolp
import json

# List configurations
success, configs = conftoolp.find_configurations('{}')
if success:
    for config in json.loads(configs):
        print(f"Configuration: {config}")

# Export configuration for backup
query = json.dumps({"configuration": "Production_v1"})
success, export_data = conftoolp.export_configuration(query)

if success:
    with open("backup.json", "w") as f:
        f.write(export_data)
```

### Error Handling Pattern

```python
import conftoolp
import json

def safe_database_call(func, *args):
    """Wrapper for conftoolp calls with proper error handling."""
    try:
        success, result = func(*args)
        if success:
            return json.loads(result) if result else None
        else:
            raise RuntimeError(f"Database operation failed: {result}")
    except json.JSONDecodeError as e:
        raise RuntimeError(f"Invalid JSON response: {e}")

# Usage
try:
    collections = safe_database_call(conftoolp.list_collections, '{}')
    print("Collections:", collections)
except RuntimeError as e:
    print(f"Error: {e}")
```

## Building

### Prerequisites

```bash
# Required packages
apt-get install swig python3-dev

# Or with yum
yum install swig python3-devel
```

### CMake Configuration

```cmake
# Enable Python bindings
cmake -DPYTHON_BINDINGS=ON ..

# With FHiCL support
cmake -DPYTHON_BINDINGS=ON -DUSE_FHICLCPP=ON ..
```

### Build and Install

```bash
cd build
make
make install

# Module installed to Python site-packages
```

### Testing Installation

```python
import conftoolp
print(dir(conftoolp))  # List available functions

# Test connection
success, result = conftoolp.list_databases('{}')
print(f"Connected: {success}")
```

## API Categories

### Document Operations
- `read_document` - Read a document from the database
- `write_document` - Write a document to the database
- `mark_document_readonly` - Mark document as read-only
- `mark_document_deleted` - Soft delete a document

### Version Management
- `find_versions` - Find all versions of a document
- `find_version_aliases` - List version aliases
- `add_version_alias` - Add alias to a version
- `remove_version_alias` - Remove a version alias

### Entity Management
- `find_entities` - Find entities in a collection
- `add_entity` - Add a new entity
- `remove_entity` - Remove an entity

### Configuration Operations
- `find_configurations` - List configurations
- `configuration_composition` - Get configuration contents
- `create_configuration` - Create new configuration
- `assign_configuration` - Assign configuration to run
- `remove_configuration` - Remove a configuration
- `read_configuration` - Read entire configuration
- `write_configuration` - Write entire configuration

### Import/Export
- `export_configuration` - Export configuration to JSON
- `import_configuration` - Import configuration from JSON
- `export_database` - Export entire database
- `import_database` - Import entire database
- `export_collection` - Export a collection
- `import_collection` - Import a collection

### Search and Metadata
- `search_collection` - Search within a collection
- `list_databases` - List available databases
- `read_dbinfo` - Get database information
- `list_collections` - List collections

### Utilities
- `set_default_locale` - Set locale for number formatting
- `enable_trace` - Enable debug tracing

### FHiCL (Conditional)
- `fhicl_to_json` - Convert FHiCL to JSON
- `json_to_fhicl` - Convert JSON to FHiCL

## Troubleshooting

### Import Errors

**Problem:** `ImportError: No module named 'conftoolp'`

**Solution:**
```bash
# Set Python path
export PYTHONPATH=/path/to/build/lib:$PYTHONPATH

# Or install system-wide
make install
```

### Symbol Not Found

**Problem:** `undefined symbol: _ZN...`

**Solution:**
```bash
# Check dependencies
ldd conftoolp.so

# Set library path
export LD_LIBRARY_PATH=/path/to/artdaq-database/lib:$LD_LIBRARY_PATH
```

### FHiCL Functions Not Available

**Problem:** `AttributeError: module 'conftoolp' has no attribute 'fhicl_to_json'`

**Solution:** FHiCL functions require building with `-DUSE_FHICLCPP=ON`

```python
# Check availability
if hasattr(conftoolp, 'fhicl_to_json'):
    # FHiCL support available
else:
    # FHiCL support not compiled
```

## See Also

- [ConfigurationDB/README.md](../ConfigurationDB/README.md) - Core database module
- [DataFormats/README.md](../DataFormats/README.md) - Data format converters
- [External: SWIG Documentation](http://www.swig.org/documentation.html)
- [External: Python/C API](https://docs.python.org/3/c-api/)
