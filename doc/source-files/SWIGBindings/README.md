# SWIGBindings Module Documentation

## Overview

The SWIGBindings module provides language bindings for the artdaq-database C++ library, enabling access from scripting languages like Python. Using SWIG (Simplified Wrapper and Interface Generator), the module exposes the configuration database API to non-C++ environments.

## Module Location

**Path**: `/home/user/artdaq-database/artdaq-database/SWIGBindings/`

## Purpose

Enable artdaq-database functionality in:
- **Python**: For scripting, automation, and web interfaces
- **Other languages**: Framework supports Java, Perl, Ruby, etc.

Benefits:
- Rapid prototyping and development
- Integration with existing Python tooling
- Web interface development
- Testing and validation scripts
- Configuration management automation

## Module Structure

```
SWIGBindings/
├── CMakeLists.txt           # Build configuration
├── python/                  # Python-specific bindings
│   ├── CMakeLists.txt
│   ├── PythonAddon.cmake
│   └── conftool/           # Configuration tool bindings
│       ├── CMakeLists.txt
│       ├── conftool.h      # C++ interface declarations
│       ├── conftool.cpp    # C++ implementation
│       └── conftool.i      # SWIG interface file
```

## Current Bindings

### Python conftool

**Module Name**: `conftoolp`

**Purpose**: Python interface to configuration database operations

**Documentation**: [conftool-python.md](./conftool-python.md)

**Status**: Production-ready

**Key Features**:
- Document read/write operations
- Configuration management
- Version and entity management
- Import/export functionality
- Database metadata operations
- Optional FHiCL conversion support

---

## SWIG Overview

### What is SWIG?

SWIG (Simplified Wrapper and Interface Generator) automatically generates language bindings for C/C++ code. It:
- Parses C++ headers
- Generates wrapper code
- Handles type conversions
- Manages memory across language boundaries
- Supports multiple target languages

### How SWIG Works

```
┌─────────────────┐
│  C++ Header     │  conftool.h
│  (Interface)    │
└────────┬────────┘
         │
┌────────▼────────┐
│  SWIG Interface │  conftool.i
│  (Directives)   │
└────────┬────────┘
         │
    ┌────▼─────┐
    │   SWIG   │
    │ Generator │
    └────┬─────┘
         │
┌────────▼────────┐
│  Wrapper Code   │  *_wrap.cxx
│  (Generated)    │
└────────┬────────┘
         │
┌────────▼────────┐
│  Compile &      │
│  Link           │
└────────┬────────┘
         │
┌────────▼────────┐
│  Python Module  │  conftoolp.so
│  (Binary)       │
└─────────────────┘
```

### SWIG Benefits

**Automatic Generation**:
- No manual wrapper code
- Consistent interface
- Easy to maintain

**Type Safety**:
- Type checking at compile time
- Automatic type conversions
- Error handling

**Multi-Language**:
- Same interface file → multiple languages
- Consistent API across languages
- Reusable interface definitions

---

## Python Bindings Details

### Installation

After building artdaq-database:

```bash
# Module typically installed to:
/usr/local/lib/pythonX.Y/site-packages/conftoolp.so

# Or in build directory:
artdaq-database/build/lib/conftoolp.so
```

### Python Usage

```python
import conftoolp
import json

# Check module is loaded
print(dir(conftoolp))

# Use functions
query = json.dumps({"collection": "RunHistory"})
success, result = conftoolp.list_collections(query)

if success:
    print("Collections:", result)
else:
    print("Error:", result)
```

### API Categories

| Category | Functions | Purpose |
|----------|-----------|---------|
| **Document Operations** | read_document, write_document, mark_document_* | Basic document CRUD |
| **Version Management** | find_versions, add/remove_version_alias | Version control |
| **Entity Management** | find_entities, add/remove_entity | Entity lifecycle |
| **Configuration Ops** | create/read/write/assign_configuration | Configuration management |
| **Import/Export** | export/import_* | Data transfer |
| **Search** | search_collection | Query operations |
| **Metadata** | list_databases/collections, read_dbinfo | Discovery |
| **Utilities** | enable_trace, fhicl conversion | Support functions |

### Return Value Pattern

All functions return `(bool, str)` tuple:

```python
success, result = conftoolp.some_function(args)

if success:
    # result contains JSON data
    data = json.loads(result)
    process(data)
else:
    # result contains error message
    print(f"Error: {result}")
```

---

## Building Bindings

### Prerequisites

**Required**:
- SWIG 3.0 or later
- Python development headers (`python-dev` or `python3-dev`)
- C++ compiler with C++14 support
- artdaq-database core libraries

**Optional**:
- fhiclcpp (for FHiCL conversion functions)

### CMake Configuration

```cmake
# Find SWIG
find_package(SWIG 3.0 REQUIRED)
include(UseSWIG)

# Find Python
find_package(PythonLibs REQUIRED)
find_package(PythonInterp REQUIRED)

# Set SWIG flags
set(CMAKE_SWIG_FLAGS "-c++" "-py3")

# Add SWIG module
swig_add_library(conftoolp
  TYPE MODULE
  LANGUAGE python
  SOURCES conftool.i conftool.cpp
)

# Link libraries
swig_link_libraries(conftoolp
  artdaq-database_ConfigurationDB
  ${PYTHON_LIBRARIES}
)
```

### Build Process

```bash
# Configure
cd artdaq-database
mkdir build && cd build
cmake .. -DPYTHON_BINDINGS=ON

# Build
make

# Python module created at:
# build/lib/conftoolp.so

# Install (optional)
make install
# Installs to Python site-packages
```

### Testing Installation

```bash
# Set Python path if not installed
export PYTHONPATH=/path/to/build/lib:$PYTHONPATH

# Test import
python3 -c "import conftoolp; print('Success')"

# Test function
python3 -c "import conftoolp; print(dir(conftoolp))"
```

---

## Interface Files

### conftool.h - C++ Interface

**Purpose**: Declares functions to be wrapped

**Structure**:
```cpp
// Type definitions
typedef std::pair<bool, std::string> result_t;

// Function declarations
result_t read_document(std::string const& query_payload);
result_t write_document(std::string const& query_payload,
                        std::string const& json_document);
// ... more functions

// Utilities
void enable_trace();
void set_default_locale();
```

**Design Notes**:
- Simple C-compatible signatures
- No templates (SWIG limitation)
- String-based parameters (JSON)
- Consistent return type
- No exceptions thrown

### conftool.cpp - C++ Implementation

**Purpose**: Implements wrapper functions

**Pattern**:
```cpp
result_t function_name(std::string const& query_payload) {
  set_default_locale();          // Ensure consistent formatting
  auto output = std::string{};   // Prepare output
  return impl::library_function(query_payload, output);
}
```

**Responsibilities**:
- Locale management
- Variable preparation
- Library function calls
- Result forwarding

### conftool.i - SWIG Interface

**Purpose**: Directs SWIG code generation

**Structure**:
```swig
%module conftoolp               // Python module name

%{
#include "conftool.h"           // Include in wrapper
%}

%include "std_pair.i"           // Enable std::pair
%include "std_string.i"         // Enable std::string

%template(result_pair_t) std::pair<bool, std::string>;

%include "conftool.h"           // Process for wrapping
```

**Directives**:
- `%module`: Sets target module name
- `%{ ... %}`: Literal C++ code for wrapper
- `%include`: Includes SWIG library or header
- `%template`: Instantiates C++ templates

---

## Use Cases

### Automation Scripts

```python
#!/usr/bin/env python3
"""Automated configuration backup"""
import conftoolp
import json
from datetime import datetime

# List all configurations
success, configs_json = conftoolp.find_configurations('{}')
configs = json.loads(configs_json)

# Export each configuration
for config in configs['configurations']:
    query = json.dumps({"configuration": config['name']})
    success, export_data = conftoolp.export_configuration(query)

    if success:
        filename = f"backup_{config['name']}_{datetime.now()}.json"
        with open(filename, 'w') as f:
            f.write(export_data)
        print(f"Backed up {config['name']}")
```

### Web Interface Backend

```python
from flask import Flask, request, jsonify
import conftoolp
import json

app = Flask(__name__)

@app.route('/api/documents/<collection>/<entity>')
def get_document(collection, entity):
    query = json.dumps({
        "collection": collection,
        "entity": entity
    })
    success, result = conftoolp.read_document(query)

    if success:
        return jsonify(json.loads(result))
    else:
        return jsonify({"error": result}), 400

@app.route('/api/configurations')
def list_configurations():
    success, result = conftoolp.find_configurations('{}')
    if success:
        return jsonify(json.loads(result))
    else:
        return jsonify({"error": result}), 500
```

### Testing and Validation

```python
import conftoolp
import json
import pytest

def test_document_roundtrip():
    """Test write and read document"""
    query = json.dumps({
        "collection": "TestCollection",
        "entity": "TestEntity",
        "version": "v1"
    })

    document = json.dumps({"test": "data"})

    # Write
    success, msg = conftoolp.write_document(query, document)
    assert success, f"Write failed: {msg}"

    # Read
    success, result = conftoolp.read_document(query)
    assert success, f"Read failed: {result}"

    # Verify
    read_doc = json.loads(result)
    assert read_doc["test"] == "data"
```

### Configuration Migration

```python
def migrate_configurations(source_db, target_db):
    """Migrate configurations between databases"""
    # List configurations
    success, configs = conftoolp.find_configurations('{}')

    for config_name in json.loads(configs)['configurations']:
        # Export from source
        export_query = json.dumps({
            "configuration": config_name,
            "database": source_db
        })
        success, exported = conftoolp.export_configuration(export_query)

        if not success:
            print(f"Failed to export {config_name}")
            continue

        # Import to target
        import_query = json.dumps({
            "configuration": config_name,
            "database": target_db,
            "data": json.loads(exported)
        })
        success, msg = conftoolp.import_configuration(import_query)

        if success:
            print(f"Migrated {config_name}")
        else:
            print(f"Failed to import {config_name}: {msg}")
```

---

## Advanced Topics

### Memory Management

**SWIG handles memory automatically**:
- C++ objects owned by C++
- Python references don't affect C++ lifetime
- Strings copied across boundary
- No manual memory management needed

**Caveats**:
- Large strings copied (performance impact)
- No shared memory between C++ and Python
- Each call creates new string copies

### Thread Safety

**Python GIL**:
- Global Interpreter Lock protects Python state
- Multiple Python threads safe for SWIG calls
- C++ library thread-safety still required

**Recommendations**:
- Check C++ library thread-safety documentation
- Use Python threading locks if needed
- Consider process-based parallelism

### Error Handling

**C++ exceptions caught by wrappers**:
```cpp
try {
  // Library call
} catch (...) {
  return {false, error_message};
}
```

**Python sees only result tuples**:
- No Python exceptions from SWIG layer
- All errors as `(False, error_string)`
- Explicit error checking required

### Performance Optimization

**Minimize calls**:
```python
# Bad: Many small calls
for entity in entities:
    conftoolp.read_document(make_query(entity))

# Better: Batch operation
batch_query = make_batch_query(entities)
conftoolp.search_collection(batch_query)
```

**Cache results**:
```python
class ConfigCache:
    def __init__(self):
        self.cache = {}

    def get_config(self, name):
        if name not in self.cache:
            success, data = conftoolp.read_configuration(
                json.dumps({"configuration": name})
            )
            if success:
                self.cache[name] = json.loads(data)
        return self.cache[name]
```

---

## Troubleshooting

### Import Errors

**Problem**: `ImportError: No module named 'conftoolp'`

**Solutions**:
```bash
# Set Python path
export PYTHONPATH=/path/to/build/lib:$PYTHONPATH

# Or install
cd build
make install

# Or create symlink
ln -s /path/to/build/lib/conftoolp.so \
      ~/.local/lib/pythonX.Y/site-packages/
```

### Symbol Not Found

**Problem**: `undefined symbol: _ZN...`

**Causes**:
- Missing library dependencies
- Incorrect library path
- Version mismatch

**Solutions**:
```bash
# Check dependencies
ldd conftoolp.so

# Set library path
export LD_LIBRARY_PATH=/path/to/artdaq-database/lib:$LD_LIBRARY_PATH

# Verify ABI compatibility
nm conftoolp.so | grep symbol_name
```

### Function Not Available

**Problem**: `AttributeError: module 'conftoolp' has no attribute 'function_name'`

**Causes**:
- Function conditionally compiled (e.g., FHiCL functions)
- SWIG didn't process function
- Wrong module version

**Debug**:
```python
import conftoolp
print(dir(conftoolp))  # List available functions
```

### Type Errors

**Problem**: `TypeError: expected string, got bytes`

**Solution**: Python 3 uses Unicode strings
```python
# Ensure strings, not bytes
query = json.dumps(data)  # Returns str in Python 3
success, result = conftoolp.read_document(query)
```

---

## Future Enhancements

### Potential Improvements

1. **Higher-Level Python API**:
   - Object-oriented wrapper classes
   - Pythonic naming conventions
   - Type hints and documentation

2. **Additional Language Bindings**:
   - Java (for Android/enterprise)
   - Ruby (for DevOps tools)
   - JavaScript/Node.js (for web)

3. **Async Support**:
   - Non-blocking operations
   - Async/await pattern
   - Callback mechanisms

4. **Streaming APIs**:
   - Large document handling
   - Progressive download
   - Memory efficiency

5. **Error Enhancements**:
   - Structured error objects
   - Error codes
   - Exception mapping

---

## Best Practices

### 1. Wrap in Python Classes

```python
class Document:
    """High-level document interface"""

    def __init__(self, collection, entity, version=None):
        self.collection = collection
        self.entity = entity
        self.version = version or "latest"

    def read(self):
        query = self._make_query()
        success, result = conftoolp.read_document(query)
        if not success:
            raise IOError(f"Read failed: {result}")
        return json.loads(result)

    def write(self, data):
        query = self._make_query()
        success, msg = conftoolp.write_document(
            query, json.dumps(data)
        )
        if not success:
            raise IOError(f"Write failed: {msg}")

    def _make_query(self):
        return json.dumps({
            "collection": self.collection,
            "entity": self.entity,
            "version": self.version
        })
```

### 2. Error Handling

```python
class DatabaseError(Exception):
    """Base exception for database errors"""
    pass

def safe_call(func, *args):
    """Safe wrapper for conftoolp calls"""
    success, result = func(*args)
    if not success:
        raise DatabaseError(result)
    return result
```

### 3. Configuration

```python
class DatabaseConfig:
    """Centralized configuration"""

    def __init__(self, uri=None):
        self.uri = uri or os.environ.get('ARTDAQ_DATABASE_URI')
        if not self.uri:
            raise ValueError("Database URI not configured")

    def set_global(self):
        """Set as default for all operations"""
        # Implementation depends on how URI is passed
        pass
```

### 4. Documentation

```python
def read_document(collection: str, entity: str, version: str = "latest") -> dict:
    """Read a document from the database.

    Args:
        collection: Collection name (e.g., "RunHistory")
        entity: Entity name within collection
        version: Version string (default: "latest")

    Returns:
        Document data as dictionary

    Raises:
        DatabaseError: If read operation fails
    """
    query = json.dumps({
        "collection": collection,
        "entity": entity,
        "version": version
    })
    success, result = conftoolp.read_document(query)
    if not success:
        raise DatabaseError(result)
    return json.loads(result)
```

---

## Dependencies

### Build Dependencies
- SWIG 3.0+
- Python development headers
- CMake 3.12+
- C++14 compiler
- artdaq-database core libraries

### Runtime Dependencies
- Python interpreter (2.7 or 3.6+)
- artdaq-database shared libraries
- conftoolp.so module

### Optional Dependencies
- fhiclcpp (for FHiCL conversion)
- pytest (for testing)
- Flask (for web interfaces)

---

## Documentation

### Module Documentation

- **[conftool-python.md](./conftool-python.md)**: Comprehensive Python binding documentation

### External Resources

- **SWIG Documentation**: http://www.swig.org/documentation.html
- **Python/C API**: https://docs.python.org/3/c-api/
- **artdaq-database**: Core library documentation

---

## Summary

The SWIGBindings module provides production-ready Python bindings for artdaq-database through SWIG-generated wrappers. The bindings expose the complete configuration database API with a simple, consistent interface based on JSON payloads and result tuples.

**Key Features**:
- Complete API coverage
- Automatic type conversion
- Memory-safe string handling
- Simple return value pattern
- Optional FHiCL support
- Extensible to other languages

**Common Uses**:
- Automation scripts
- Web interfaces
- Testing frameworks
- Configuration management
- Data migration tools
- Integration with Python ecosystem

**Status**: Production-ready, actively used in artdaq ecosystem
