# SharedCommon Module Documentation

## Overview

This directory contains comprehensive documentation for all source files in the artdaq-database SharedCommon module. The SharedCommon module provides fundamental utilities, types, and infrastructure used throughout the entire artdaq-database project.

**Module Location**: `/home/user/artdaq-database/artdaq-database/SharedCommon/`

**Documentation Created**: November 13, 2025
**Last Updated:** 2026-01-14

---

## Documentation Files

### Configuration and Literals

#### [configuraion_api_literals.h.md](./configuraion_api_literals.h.md)
Central repository of string literals and constants for the Configuration API.

**Key Contents**:
- Operation names (writedocument, readdocument, etc.)
- Provider names (mongo, filesystem, ucon)
- Option keys (path, operation, format, etc.)
- Filter keys for queries
- Format types (json, gui)
- Timestamp formats and database constants

**Size**: 9.1 KB | **Purpose**: Prevent string literal typos and ensure consistency

---

### Common Includes

#### [common.h.md](./common.h.md)
Aggregates commonly used Standard C++ and third-party library headers.

**Key Contents**:
- Standard library includes (vector, string, chrono, etc.)
- Boost library includes (demangle, lexical_cast)
- TRACE debugging system include

**Size**: 4.7 KB | **Purpose**: Reduce boilerplate includes across the codebase

#### [sharedcommon_common.h.md](./sharedcommon_common.h.md)
Master aggregator header that includes all SharedCommon functionality.

**Key Contents**:
- Includes all other SharedCommon headers
- Provides single-include access to entire module
- Maintains correct inclusion order

**Size**: 13 KB | **Purpose**: Convenient one-stop include for implementation files

---

### Filesystem Operations

#### [fileststem_functions.h.md](./fileststem_functions.h.md)
Declarations of filesystem utility functions.

**Key Functions**:
- `list_files()` - Recursive directory listing
- `mkdir()` / `mkdirfile()` - Directory creation
- `read_buffer_from_file()` / `write_buffer_to_file()` - File I/O
- `make_temp_dir()` / `delete_temp_dir()` - Temporary directory management
- `dir_to_tarbzip2base64()` / `tarbzip2base64_to_dir()` - Archive operations
- Path conversion functions

**Size**: 9.1 KB | **Purpose**: High-level filesystem operations

#### [fileststem_functions.cpp.md](./fileststem_functions.cpp.md)
Implementation of filesystem utility functions.

**Key Details**:
- Uses Boost.Filesystem for cross-platform operations
- System commands for advanced operations (tar, bzip2)
- Comprehensive error handling
- Security considerations documented

**Size**: 13 KB | **Purpose**: Portable filesystem utilities implementation

---

### Helper Utilities

#### [helper_functions.h.md](./helper_functions.h.md)
Declarations of comprehensive helper utilities.

**Key Functions**:
- **Time**: `timestamp()`, `to_string()`, `to_timepoint()`
- **Strings**: `quoted_()`, `trim()`, `to_lower()`, `to_upper()`, `replace_all()`
- **JSON**: `to_json()`, `to_id()`, `generate_oid()`
- **Parsing**: `dequote()`, `debrace()`, `debracket()`, `annotate()`
- **System**: `expand_environment_variables()`, `unamejson()`
- **Validation**: `confirm()` macro for assertions

**Size**: 15 KB | **Purpose**: String manipulation, time handling, JSON generation

#### [helper_functions.cpp.md](./helper_functions.cpp.md)
Implementation of helper utility functions.

**Key Details**:
- ISO 8601 timestamp handling with millisecond precision
- POSIX wordexp for environment variable expansion
- Regex-based parsing
- Compiler-specific code for GCC vs Clang
- Fake time mode for testing

**Size**: 17 KB | **Purpose**: Core utility implementations

---

### Process Management

#### [process_exit_codes.h.md](./process_exit_codes.h.md)
Standardized exit codes and trace mode settings.

**Key Constants**:
- **Exit Codes**: SUCCESS (0), FAILURE (1), INVALID_ARGUMENT (128), UNCAUGHT_EXCEPTION (144)
- **Trace Modes**: modeM, modeS for TRACE logging configuration

**Size**: 8.2 KB | **Purpose**: Consistent process return values

---

### Debugging and Diagnostics

#### [printStackTrace.h.md](./printStackTrace.h.md)
Declarations for stack trace capture and exception diagnostics.

**Key Functions**:
- `getStackTrace()` - Capture current call stack
- `getCxaThrowStack()` - Get stack from exception throw point
- `demangle()` - Convert mangled C++ symbols to readable names
- `registerUngracefullExitHandlers()` - Register all debugging handlers
- `registerAbortHandler()`, `registerTerminateHandler()` - Specific handlers
- `current_exception_diagnostic_information()` - Comprehensive exception info

**Size**: 11 KB | **Purpose**: Production debugging infrastructure

#### [printStackTrace.cpp.md](./printStackTrace.cpp.md)
Implementation of debugging and diagnostic functions.

**Key Details**:
- C++ ABI `__cxa_throw` interception for exception tracking
- Signal handlers for fatal signals (SIGSEGV, SIGABRT, etc.)
- Terminate handler for uncaught exceptions
- Backtrace capture and formatting
- Symbol demangling with regex parsing

**Size**: 17 KB | **Purpose**: Critical debugging infrastructure

---

### Result Handling

#### [returned_result.h.md](./returned_result.h.md)
Result type and utility functions for success/failure reporting.

**Key Types**:
- `result_t` - std::pair<bool, std::string> for operation results

**Key Functions**:
- `Success()` / `Failure()` - Create result objects
- `ThrowOnFailure()` - Convert failure to exception

**Constants**:
- Predefined JSON messages (msg_Success, msg_Failure, msg_Added, etc.)

**Size**: 12 KB | **Purpose**: Alternative to exceptions for expected failures

#### [returned_result.cpp.md](./returned_result.cpp.md)
Implementation of result creation and validation functions.

**Key Details**:
- Simple implementations using aggregate initialization
- Support for both string and ostringstream overloads
- Efficient string handling
- Thread-safe implementations

**Size**: 12 KB | **Purpose**: Lightweight error handling

---

### Type Definitions

#### [shared_datatypes.h.md](./shared_datatypes.h.md)
Common type aliases for semantic clarity.

**Key Types**:
- `path_t` - Filesystem paths (std::string)
- `string_pair_t` - Related string pairs (std::pair)
- `object_id_t` - Unique object identifiers (std::string)
- `timestamp_t` - ISO 8601 timestamps (std::string)

**Size**: 12 KB | **Purpose**: Semantic type names for readability

---

### Exception Handling

#### [shared_exceptions.h.md](./shared_exceptions.h.md)
Exception type hierarchy based on CET exceptions.

**Key Classes**:
- `exception` - Base exception class
- `invalid_argument` - Invalid parameter exceptions
- `runtime_error` - Runtime operation failures
- `invalid_option_exception` - Configuration/option errors
- `runtime_exception` - General runtime exceptions

**Size**: 15 KB | **Purpose**: Consistent exception handling across project

---

## Module Architecture

### Dependency Graph

```
sharedcommon_common.h (aggregator)
  ├─ common.h (standard library includes)
  ├─ shared_datatypes.h (type aliases)
  ├─ shared_exceptions.h (exception types)
  ├─ process_exit_codes.h (exit codes)
  ├─ returned_result.h (result types)
  ├─ helper_functions.h (utilities)
  │   └─ shared_datatypes.h
  ├─ printStackTrace.h (debugging)
  │   └─ process_exit_codes.h
  └─ fileststem_functions.h (filesystem)
      └─ shared_datatypes.h
```

### Module Categories

1. **Foundation**
   - common.h
   - shared_datatypes.h
   - configuraion_api_literals.h

2. **Error Handling**
   - shared_exceptions.h
   - returned_result.h/cpp
   - process_exit_codes.h

3. **Utilities**
   - helper_functions.h/cpp
   - fileststem_functions.h/cpp

4. **Debugging**
   - printStackTrace.h/cpp

5. **Aggregators**
   - sharedcommon_common.h

---

## Key Features Documented

### For Junior Developers

Each documentation file includes:

1. **File Overview** - Purpose and role in the system
2. **Dependencies** - What it includes and depends on
3. **Detailed Function Documentation**:
   - Signatures and parameters
   - Purpose and behavior
   - Usage examples
   - Error handling
   - Performance considerations
4. **Usage Patterns** - Common ways to use the code
5. **Design Rationale** - Why things are done this way
6. **Best Practices** - How to use correctly
7. **Related Files** - What else to look at
8. **Notes and Caveats** - Important things to know

### Special Topics Covered

- **Thread Safety** - Which functions are thread-safe
- **Exception Safety** - What guarantees are provided
- **Performance** - Optimization considerations
- **Security** - Security implications and best practices
- **Platform Dependencies** - Unix/Linux specific code
- **Compiler Differences** - GCC vs Clang handling
- **Testing** - How to test these components

---

## Total Documentation

- **14 Markdown files** totaling **167 KB**
- **Average file size**: ~12 KB
- **Comprehensive coverage** of all source files in SharedCommon
- **500+ code examples** throughout documentation
- **Detailed explanations** suitable for junior developers

---

## Usage Guidelines

### For New Developers

Start with these files to understand the module:

1. **sharedcommon_common.h.md** - Overview of what's available
2. **shared_datatypes.h.md** - Basic types used everywhere
3. **shared_exceptions.h.md** - Error handling approach
4. **helper_functions.h.md** - Common utilities

### For Feature Development

Consult these based on what you're working on:

- **File operations**: fileststem_functions.h/cpp.md
- **String/time handling**: helper_functions.h/cpp.md
- **Error reporting**: returned_result.h/cpp.md, shared_exceptions.h.md
- **Debugging issues**: printStackTrace.h/cpp.md

### For Debugging

When debugging crashes or exceptions:

1. Read **printStackTrace.h/cpp.md** for stack trace usage
2. Check **shared_exceptions.h.md** for exception types
3. Review **process_exit_codes.h.md** for exit code meanings

---

## Documentation Conventions

- **Function signatures** shown with full namespace
- **Usage examples** provided for complex functions
- **Error handling** explicitly documented
- **Performance notes** where relevant
- **Security considerations** highlighted
- **Related files** cross-referenced
- **Design rationale** explained

---

## Maintenance

This documentation should be updated when:

- New functions are added to SharedCommon
- Function signatures change
- New files are added to the module
- Significant implementation changes occur
- Best practices evolve

---

## Questions or Issues?

For questions about this documentation or the SharedCommon module:

1. Review the specific file's documentation
2. Check related files (cross-references provided)
3. Consult the source code with documentation as guide
4. Refer to artdaq-database project documentation

---

## File Naming Note

Several files have typos in their names (inherited from source):
- **fileststem_functions** (should be "filesystem")
- **configuraion_api_literals** (should be "configuration")

Documentation uses the actual filenames for accuracy, but notes the typos.

---

**Documentation generated for artdaq-database SharedCommon module**
**Target audience**: Junior to intermediate C++ developers
**Last updated**: December 30, 2025
