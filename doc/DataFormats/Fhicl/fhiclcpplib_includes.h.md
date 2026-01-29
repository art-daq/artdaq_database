# fhiclcpplib_includes.h

**Path:** `artdaq-database/DataFormats/Fhicl/fhiclcpplib_includes.h`

**Purpose:** Aggregator header that centralizes all includes from the external fhiclcpp library, providing a single point of access to FHiCL parsing and parameter set handling functionality from Fermilab's fhiclcpp library.


## Key Concepts

### FHiCL (Fermilab Hierarchical Configuration Language)

FHiCL is a configuration language developed at Fermilab for use in high-energy physics experiments. It provides:

- **Hierarchical structure**: Configurations organized in nested tables and sequences
- **Prolog sections**: Reusable definitions that can be referenced later (BEGIN_PROLOG/END_PROLOG blocks)
- **Include directives**: Ability to include other FHiCL files
- **Protection levels**: Control over which values can be overridden

### fhiclcpp Library

The fhiclcpp library is Fermilab's official C++ implementation for parsing and manipulating FHiCL documents. This header aggregates its key components:

- **ParameterSet**: The in-memory representation of a parsed FHiCL document
- **extended_value**: Low-level representation of FHiCL values with type and protection information
- **intermediate_table**: The parse tree structure before conversion to ParameterSet
- **parse functions**: Functions to parse FHiCL text into data structures

## Thread Safety

- **Thread-safe:** No (this is a header-only aggregator)
- **Concurrent access:** The underlying fhiclcpp library functions are generally not thread-safe for parsing the same document
- **Locking:** None; callers must provide external synchronization if needed

## Dependencies

| Include | Purpose |
|---------|---------|
| `fhiclcpp/ParameterSet.h` | In-memory representation of parsed FHiCL configuration; provides type-safe access to configuration values |
| `fhiclcpp/Protection.h` | Enumeration defining protection levels (NONE, PROTECT_IGNORE, PROTECT_ERROR) for FHiCL values |
| `fhiclcpp/exception.h` | Exception types thrown during FHiCL parsing and validation |
| `fhiclcpp/extended_value.h` | Low-level value representation with type tag, protection, and prolog membership information |
| `fhiclcpp/intermediate_table.h` | Parse tree structure representing FHiCL document before conversion to ParameterSet |
| `fhiclcpp/make_ParameterSet.h` | Factory functions to create ParameterSet from various input sources |
| `fhiclcpp/parse.h` | Core parsing functions (parse_document, parse_value) for FHiCL text |
| `fhiclcpp/parse_shims_opts.h` | Shim options controlling parsing behavior (snippet mode, strict mode) |

## Usage

### Basic Usage

```cpp
#include "artdaq-database/DataFormats/Fhicl/fhiclcpplib_includes.h"
#include <string>
#include <iostream>

void parseFhiclDocument(std::string const& fhicl_text) {
  try {
    // Enable snippet mode for partial documents
    ::shims::isSnippetMode(true);

    // Parse FHiCL text into intermediate table
    auto table = ::fhicl::parse_document(fhicl_text);

    // Iterate over parsed entries
    for (auto const& entry : table) {
      std::cout << "Key: " << entry.first << "\n";
      std::cout << "In prolog: " << entry.second.in_prolog << "\n";
      std::cout << "Value tag: " << static_cast<int>(entry.second.tag) << "\n";
    }
  } catch (::fhicl::exception const& e) {
    std::cerr << "FHiCL parse error: " << e.what() << "\n";
  }
}
```

### Working with Protection Levels

```cpp
#include "artdaq-database/DataFormats/Fhicl/fhiclcpplib_includes.h"

void checkProtection(::fhicl::extended_value const& value) {
  switch (value.protection) {
    case ::fhicl::Protection::NONE:
      // Value can be freely overridden
      break;
    case ::fhicl::Protection::PROTECT_IGNORE:
      // Override attempts are silently ignored
      break;
    case ::fhicl::Protection::PROTECT_ERROR:
      // Override attempts throw an exception
      break;
  }
}
```

## Relationship to Other Components

This header serves as the interface between artdaq-database and Fermilab's fhiclcpp library. It is used by:

- **fhicl_reader.cpp**: Uses `parse_document()` to parse FHiCL text into an intermediate table
- **helper_functions.h**: Uses `value_tag` and `Protection` enumerations for type conversion
- **convertfhicl2jsondb.h**: Uses `extended_value` to access parsed FHiCL data

The aggregation pattern simplifies dependency management and ensures consistent fhiclcpp usage across the FHiCL module.

## See Also

- [fhicl_reader.h](./fhicl_reader.h.md) - Uses fhiclcpp to parse FHiCL documents
- [helper_functions.h](./helper_functions.h.md) - FHiCL utility functions using fhiclcpp types
- [convertfhicl2jsondb.h](./convertfhicl2jsondb.h.md) - FHiCL to JSON conversion using extended_value
- [External: fhiclcpp documentation](https://cdcvs.fnal.gov/redmine/projects/fhicl-cpp/wiki) - Official fhiclcpp documentation

## Notes for Developers

### Snippet Mode

The `::shims::isSnippetMode(true)` call is essential when parsing partial FHiCL documents that lack required elements. Always enable snippet mode before parsing user-provided FHiCL text.

### Common Pitfalls

- **Pitfall 1:** Forgetting to enable snippet mode when parsing partial documents will cause parse failures on valid partial FHiCL content.
- **Pitfall 2:** The fhiclcpp library throws `::fhicl::exception` for parse errors; always wrap parsing calls in try-catch blocks.
- **Pitfall 3:** The `intermediate_table` is a typedef for `std::map<std::string, extended_value>`, which preserves insertion order only in the standard library's ordered map sense (alphabetical by key), not the original document order.
