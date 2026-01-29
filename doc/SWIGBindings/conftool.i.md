# conftool.i

**Path:** `artdaq-database/SWIGBindings/python/conftool/conftool.i`

**Purpose:** This SWIG interface file defines how the C++ conftool API is wrapped and exposed to Python. It specifies the module name, includes necessary SWIG library files for standard C++ type handling, instantiates the required template for the result type, and includes the main header file for automatic wrapper generation. This compact 12-line file drives the entire Python binding generation process.


## Key Concepts

### SWIG Interface Files
SWIG interface files (`.i` extension) contain directives that control wrapper code generation:
- Module naming for the target language
- Type mappings and conversions between C++ and Python
- Template instantiations for C++ templates
- Header file processing for function wrapping

### Python Module Name
The module is named `conftoolp` (with 'p' suffix for Python). This distinguishes the Python module from the underlying C++ library. Import it as:
```python
import conftoolp
```

### Python/C++ Type Mapping
SWIG provides automatic type conversion between C++ and Python:
- `std::string` becomes Python `str`
- `std::pair<T1, T2>` becomes Python `tuple`
- Pointers and references are handled automatically
- Memory management is handled by SWIG

### Wrapper Generation Process
The SWIG tool reads this interface file and generates:
- `conftool_wrap.cxx`: C++ wrapper implementation with Python API glue code
- `conftoolp.py`: Python module interface (optional, for complex cases)
- The compiled result is `conftoolp.so` (shared library importable by Python)

## Thread Safety

- **Thread-safe:** Yes (file is processed at build time only)
- **Concurrent access:** N/A - this is a build-time configuration file
- **Locking:** N/A

## Dependencies

| Include | Purpose |
|---------|---------|
| `std_pair.i` | SWIG library file enabling `std::pair` to Python tuple conversion |
| `std_string.i` | SWIG library file enabling `std::string` to Python str conversion |
| `conftool.h` | The C++ header file containing function declarations to wrap |

## Interface Directives

### `%module conftoolp`

**Brief:** Defines the Python module name for the generated bindings.

**Syntax:**
```swig
%module conftoolp
```

**Effect:**
- The generated shared library will be `_conftoolp.so` (or `conftoolp.so` depending on SWIG version)
- Python import statement: `import conftoolp`
- All wrapped functions become accessible as `conftoolp.function_name()`

**Example Python Usage:**
```python
import conftoolp
print(conftoolp.__doc__)
print(dir(conftoolp))
```

---

### C++ Header Inclusion Block `%{ ... %}`

**Brief:** Includes C++ headers in the generated wrapper code without triggering SWIG processing.

**Syntax:**
```swig
%{
    #include "conftool.h"
%}
```

**Purpose:**
- The `%{ ... %}` block is copied verbatim into the generated `.cxx` wrapper file
- This ensures the wrapper code can access the C++ declarations at compile time
- Does NOT cause SWIG to process the header for wrapping (that's done separately with `%include`)

**Generated Code:**
```cpp
// In conftool_wrap.cxx:
#include "conftool.h"
```

---

### Standard Library Type Support `%include`

**Brief:** Includes SWIG's standard library interface files to enable automatic type conversions.

**Syntax:**
```swig
%include "std_pair.i"
%include "std_string.i"
```

**std_pair.i:**
- Enables wrapping of `std::pair<T1, T2>` templates
- Provides automatic conversion to Python tuples
- Must be included before using `%template` for pair types

**std_string.i:**
- Enables automatic `std::string` to Python `str` conversion
- Handles encoding (Python 3 uses Unicode strings)
- Manages memory for string copies across the language boundary

**Python Result:**
```python
# C++ std::string becomes Python str
query = "some string"  # Python str passed to C++
success, result = conftoolp.read_document(query)  # Returns Python tuple

# C++ std::pair<bool, std::string> becomes Python tuple(bool, str)
type(success)  # <class 'bool'>
type(result)   # <class 'str'>
```

---

### Template Instantiation `%template`

**Brief:** Instantiates the C++ template `std::pair<bool, std::string>` for Python wrapping.

**Syntax:**
```swig
%template(result_pair_t) std::pair<bool,std::string>;
```

**Parameters:**
- `result_pair_t` - The name given to this specific template instantiation
- `std::pair<bool,std::string>` - The C++ template to instantiate

**Why It's Needed:**
- SWIG cannot automatically wrap C++ templates (they need explicit instantiation)
- This matches the `result_t` typedef in `conftool.h`
- Without this, SWIG wouldn't know how to handle `result_t` return values
- The instantiation creates Python-accessible type conversions

**Python Type:**
```python
result = conftoolp.read_document(query)
type(result)  # <class 'tuple'>
len(result)   # 2
type(result[0])  # <class 'bool'>
type(result[1])  # <class 'str'>
```

---

### Header Processing `%include "conftool.h"`

**Brief:** Processes the C++ header file for wrapper generation, creating Python bindings for all declared functions.

**Syntax:**
```swig
%include "conftool.h"
```

**Purpose:**
- SWIG parses `conftool.h` and generates wrappers for all declared functions
- Different from the `%{ #include %}` block (which only includes in generated code)
- This directive causes actual wrapper code generation

**What Gets Wrapped:**
- `typedef std::pair<bool, std::string> result_t;` - Type definition
- `void set_default_locale();` - Utility function
- `result_t read_document(...);` - Document operations
- `result_t write_document(...);` - Document operations
- All other function declarations in the header
- Conditionally compiled functions (`#ifdef ADFHICLCPP`) are included/excluded based on compile flags

---

## Complete Interface File

```swig
%module conftoolp

%{
    #include "conftool.h"
%}

%include "std_pair.i"
%include "std_string.i"
%template(result_pair_t) std::pair<bool,std::string>;

%include "conftool.h"
```

This compact 12-line file generates a complete Python binding for the conftool API with:
- 30+ wrapped functions
- Automatic type conversion
- Memory-safe string handling
- Support for the `result_t` return type pattern

---

## Generated Python Bindings

The interface file generates bindings for all functions declared in `conftool.h`:

### Document Management
- `read_document(query_payload) -> (bool, str)`
- `write_document(query_payload, json_document) -> (bool, str)`
- `mark_document_readonly(query_payload) -> (bool, str)`
- `mark_document_deleted(query_payload) -> (bool, str)`

### Version Management
- `find_versions(query_payload) -> (bool, str)`
- `find_version_aliases(query_payload) -> (bool, str)`
- `add_version_alias(query_payload) -> (bool, str)`
- `remove_version_alias(query_payload) -> (bool, str)`

### Entity Management
- `find_entities(query_payload) -> (bool, str)`
- `add_entity(query_payload) -> (bool, str)`
- `remove_entity(query_payload) -> (bool, str)`

### Configuration Management
- `find_configurations(query_payload) -> (bool, str)`
- `configuration_composition(query_payload) -> (bool, str)`
- `create_configuration(query_payload) -> (bool, str)`
- `assign_configuration(query_payload) -> (bool, str)`
- `remove_configuration(query_payload) -> (bool, str)`
- `read_configuration(query_payload) -> (bool, str)`
- `write_configuration(query_payload, json_document) -> (bool, str)`

### Import/Export
- `export_configuration(query_payload) -> (bool, str)`
- `import_configuration(query_payload) -> (bool, str)`
- `export_database(query_payload) -> (bool, str)`
- `import_database(query_payload) -> (bool, str)`
- `export_collection(query_payload) -> (bool, str)`
- `import_collection(query_payload) -> (bool, str)`

### Search and Metadata
- `search_collection(query_payload) -> (bool, str)`
- `list_databases(query_payload) -> (bool, str)`
- `read_dbinfo(query_payload) -> (bool, str)`
- `list_collections(query_payload) -> (bool, str)`

### Utilities
- `set_default_locale() -> None`
- `enable_trace() -> None`

### FHiCL (Conditional)
- `fhicl_to_json(fcl, filename) -> (bool, str)` (if `ADFHICLCPP` defined)
- `json_to_fhicl(jsn, filename) -> (bool, str)` (if `ADFHICLCPP` defined)

---

## Relationship to Other Components

### Build Process Flow

```
conftool.i (This file)
    |
    | SWIG processes
    v
conftool_wrap.cxx (Generated wrapper code)
    |
    | C++ compiler
    v
conftool_wrap.o + conftool.o
    |
    | Linker
    v
conftoolp.so (Python module)
    |
    | Python imports
    v
import conftoolp
```

### CMake Integration

The CMake build system uses this interface file:

```cmake
# In CMakeLists.txt:
if(USE_FHICLCPP)
  set_property(SOURCE conftool.i PROPERTY COMPILE_DEFINITIONS ADFHICLCPP)
endif()

create_python_addon(ADDON_NAME "conftoolp"
                    LIBRARIES artdaq-database::ConfigurationDB)
```

**Key Points:**
- Conditional compilation flags are set via CMake
- The `create_python_addon` macro handles SWIG invocation
- Libraries are linked to provide implementation

### File Relationships

```
conftool.i (SWIG directives)
    |
    +-- includes --> conftool.h (C++ declarations)
    |                    |
    |                    +-- implemented by --> conftool.cpp
    |
    +-- generates --> conftool_wrap.cxx (SWIG-generated)
    |
    +-- produces --> conftoolp.so (Python module)
```

---

## See Also

- [conftool.h.md](./conftool.h.md) - C++ header with function declarations
- [conftool.cpp.md](./conftool.cpp.md) - C++ implementation of wrapper functions
- [conftool-python.md](./conftool-python.md) - Python API reference
- [External: SWIG Documentation](http://www.swig.org/documentation.html)

## Notes for Developers

### SWIG Directive Reference

| Directive | Purpose |
|-----------|---------|
| `%module name` | Set Python module name |
| `%{ code %}` | Insert literal code in wrapper |
| `%include "file.i"` | Include SWIG library file |
| `%include "file.h"` | Process header for wrapping |
| `%template(name) template<T>` | Instantiate C++ template |
| `%rename(py_name) cpp_name` | Rename function in Python |
| `%ignore function_name` | Exclude function from wrapping |

### Common Modifications

**Adding a New Type:**
```swig
// If a new type needs wrapping:
%include "std_vector.i"
%template(StringVector) std::vector<std::string>;
```

**Renaming Functions:**
```swig
// If Python name should differ from C++:
%rename(python_name) cpp_function_name;
```

**Ignoring Functions:**
```swig
// To exclude a function from wrapping:
%ignore function_to_skip;
```

**Adding Python Code:**
```swig
// To add Python helper code:
%pythoncode %{
def helper_function():
    pass
%}
```

### Conditional Compilation

The FHiCL functions are conditionally compiled:

1. CMake sets the `ADFHICLCPP` definition on `conftool.i`
2. When SWIG processes `conftool.h`, it respects `#ifdef ADFHICLCPP`
3. FHiCL functions are only wrapped when the flag is set

**Checking in Python:**
```python
import conftoolp
if hasattr(conftoolp, 'fhicl_to_json'):
    print("FHiCL support available")
else:
    print("FHiCL support not compiled")
```

### Debugging SWIG Generation

**View generated code:**
```bash
# Generate wrapper with verbose output
swig -python -c++ -v conftool.i

# Examine generated files
cat conftool_wrap.cxx | head -100
```

**Check module contents:**
```python
import conftoolp
print(dir(conftoolp))
help(conftoolp)
```

### Version Compatibility

**SWIG Version:**
- Requires SWIG 3.0 or later
- SWIG 4.x recommended for Python 3.x

**Python Version:**
- Python 2.7: Use `-py2` flag (deprecated)
- Python 3.x: Default behavior

**C++ Standard:**
- SWIG supports C++11/14/17 features
- Use `-std=c++14` or higher when compiling wrapper

### Common Pitfalls

- **Missing Template Instantiation:** If a new template type is added to `conftool.h`, add a corresponding `%template` directive
- **Include Order:** `%include` for SWIG library files must come before `%template` directives that use them
- **Conditional Compilation:** Remember to set compile definitions via CMake for conditional code

### Anti-patterns

```swig
// DON'T do this - wrong order:
%template(result_pair_t) std::pair<bool,std::string>;
%include "std_pair.i"  // Too late! Template directive already processed

// DO this instead - correct order:
%include "std_pair.i"
%template(result_pair_t) std::pair<bool,std::string>;
```
