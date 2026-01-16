# configuraion_api_literals.h

**Path:** `artdaq-database/SharedCommon/configuraion_api_literals.h`

**Purpose:** Central repository of string constants for the Configuration API. Using named constants instead of string literals throughout the codebase prevents typos, enables IDE autocomplete, and makes refactoring easier. All constants are `constexpr` for zero runtime overhead.


**Note:** The filename contains a typo ("configuraion" instead of "configuration"). This is preserved for backward compatibility with existing code that includes this header.

## Key Concepts

### Why Use String Constants?

```cpp
// Without constants - error-prone, no IDE help
opts.operation("writedocument");  // Typo? Hard to catch at compile time

// With constants - safe, IDE autocomplete, compile-time verification
opts.operation(apiliteral::operation::writedocument);  // IDE helps, refactor-safe
```

### Namespace Organization

Constants are organized hierarchically for easy discovery:

```
artdaq::database::result              - Result messages (JSON format)
artdaq::database::configapi::literal
    +-- operation                     - API operation names (store, load, etc.)
    +-- provider                      - Storage provider identifiers (mongo, filesystem)
    +-- option                        - Request parameters (version, collection, etc.)
    +-- filter                        - Query filter keys (entities.name, etc.)
    +-- format                        - Output formats (json, gui)
    +-- msg                           - User-facing messages
```

### Usage Pattern

```cpp
namespace apiliteral = artdaq::database::configapi::literal;

// Now use apiliteral:: prefix for all constants
auto op = apiliteral::operation::writedocument;
auto prov = apiliteral::provider::mongo;
auto fmt = apiliteral::format::json;
```

## Thread Safety

- **Thread-safe:** Yes
- **Concurrent access:** All values are `constexpr` constants evaluated at compile time
- **Locking:** No locking needed; constants cannot be modified at runtime

## Dependencies

| Include | Purpose |
|---------|---------|
| (none) | Pure `constexpr` constants - no external dependencies |

## Constants

### Namespace: `artdaq::database::result`

**Brief:** JSON-formatted result messages for API responses. These are used by the `result_t` type functions.

| Constant | Value | Purpose |
|----------|-------|---------|
| `msg_EmptyFilter` | `"{\"message\":\"empty_filter\"}"` | Search filter is empty |
| `msg_EmptyDocument` | `"{\"message\":\"Json document is empty\"}"` | Document content is empty |
| `msg_SystemCallFailed` | (long message) | system() call failed due to setuid/setgid restrictions |

---

### Namespace: `artdaq::database::configapi::literal`

**Brief:** Main namespace for API literal constants. Contains general constants and nested namespaces for specific categories.

#### General Constants

| Constant | Value | Purpose |
|----------|-------|---------|
| `operations` | `"operations"` | Operations key in JSON request |
| `database` | `"database"` | Database key in request/response |
| `name` | `"name"` | Name field identifier |
| `notprovided` | `"notprovided"` | Placeholder for missing/optional values |
| `apiname` | `"artdaq_database"` | API identifier string |
| `whitespace` | `" "` | Single space character |
| `nullstring` | `""` | Empty string constant |
| `empty_json` | `"{ }"` | Empty JSON object |
| `empty_search_result` | `"{\"search\":[ ]}"` | Empty search result JSON |
| `database_format_version` | `3` | Current database schema version (integer) |
| `database_format_locale` | `"en_US.UTF-8"` | Locale for string formatting |

#### Timestamp Constants

| Constant | Value | Purpose |
|----------|-------|---------|
| `timestamp_format` | `"%FT%T.000%z"` | ISO 8601 format string for `strftime` |
| `timestamp_format_old` | `"%a %b %d %H:%M:%S %Y"` | Legacy timestamp format |
| `timestamp_faketime` | `"2017-07-18T12:48:10.123-0500"` | Fixed timestamp for testing |

**Timestamp format codes:**
- `%F` = `%Y-%m-%d` (ISO 8601 date: 2024-01-15)
- `%T` = `%H:%M:%S` (24-hour time: 14:30:25)
- `%z` = Timezone offset (+HHMM or -HHMM: -0500)

Example output: `2024-01-15T14:30:25.000-0500`

#### Export/Import Constants

| Constant | Value | Purpose |
|----------|-------|---------|
| `bzip2base64` | `"ascii.tar.bzip2.base64"` | Archive format identifier |
| `tmpdirprefix` | `"/tmp/adb"` | Temp directory prefix for operations |
| `dbexport_extension` | `".tar-bzip2-base64"` | Export file extension |
| `empty_filesystem_index` | (JSON structure) | Template for empty FileSystemDB index |

---

### Namespace: `operation` - API Operations

**Brief:** API operation identifiers used to specify which action to perform.

#### Document Operations

| Constant | Value | Purpose |
|----------|-------|---------|
| `writedocument` | `"store"` | Store a single document |
| `readdocument` | `"load"` | Load a single document |
| `overwritedocument` | `"overwritedocument"` | Overwrite an existing document |

#### Configuration Operations

| Constant | Value | Purpose |
|----------|-------|---------|
| `writeconfiguration` | `"globalconfstore"` | Store a global configuration |
| `readconfiguration` | `"globalconfload"` | Load a global configuration |
| `newconfig` | `"newconfig"` | Create a new configuration |
| `assignconfig` | `"addconfig"` | Assign configuration to an entity |
| `removeconfig` | `"rmconfig"` | Remove a configuration |
| `confcomposition` | `"buildfilter"` | Build configuration composition filter |

#### Search Operations

| Constant | Value | Purpose |
|----------|-------|---------|
| `findconfigs` | `"findconfigs"` | Find configurations matching criteria |
| `findversions` | `"findversions"` | Find versions of a document |
| `findentities` | `"findentities"` | Find entities in the database |
| `searchcollection` | `"searchcollection"` | Search within a collection |
| `findcompositionscontaining` | `"findcompositionscontaining"` | Find compositions containing an item |

#### Entity Operations

| Constant | Value | Purpose |
|----------|-------|---------|
| `addentity` | `"addentity"` | Add a new entity |
| `rmentity` | `"rmentity"` | Remove an entity |

#### Alias Operations

| Constant | Value | Purpose |
|----------|-------|---------|
| `addversionalias` | `"addveralias"` | Add a version alias |
| `rmversionalias` | `"rmveralias"` | Remove a version alias |
| `findversionalias` | `"findveralias"` | Find version aliases |
| `addconfigalias` | `"addconfigalias"` | Add a configuration alias |
| `rmconfigalias` | `"rmconfigalias"` | Remove a configuration alias |
| `findconfigalias` | `"findconfigalias"` | Find configuration aliases |

#### Database Operations

| Constant | Value | Purpose |
|----------|-------|---------|
| `listcollections` | `"listcollections"` | List all collections in database |
| `listdatabases` | `"listdatabases"` | List all available databases |
| `readdbinfo` | `"readdbinfo"` | Read database information/statistics |

#### Import/Export Operations

| Constant | Value | Purpose |
|----------|-------|---------|
| `exportdatabase` | `"exportdatabase"` | Export entire database |
| `importdatabase` | `"importdatabase"` | Import entire database |
| `exportconfig` | `"exportconfig"` | Export a configuration |
| `importconfig` | `"importconfig"` | Import a configuration |
| `exportcollection` | `"exportcollection"` | Export a collection |
| `importcollection` | `"importcollection"` | Import a collection |

#### Status Operations

| Constant | Value | Purpose |
|----------|-------|---------|
| `markdeleted` | `"markdeleted"` | Mark document as deleted |
| `markreadonly` | `"markreadonly"` | Mark document as read-only |
| `addrun` | `"addrun"` | Add a run number to document |

---

### Namespace: `provider` - Storage Providers

**Brief:** Storage provider identifiers used to select which backend to use.

| Constant | Value | Purpose |
|----------|-------|---------|
| `mongo` | `"mongo"` | MongoDB storage provider |
| `filesystem` | `"filesystem"` | FileSystemDB storage provider (JSON files) |
| `ucon` | `"ucon"` | UConDB conditions database |

---

### Namespace: `option` - Request Parameters

**Brief:** Parameter names for API requests.

| Constant | Value | Purpose |
|----------|-------|---------|
| `path` | `"path"` | File or URI path |
| `operation` | `"operation"` | Operation type to perform |
| `format` | `"dataformat"` | Data format (json, fhicl, xml) |
| `provider` | `"dbprovider"` | Storage provider to use |
| `result` | `"result"` | Result output field |
| `source` | `"source"` | Source document field |
| `version` | `"version"` | Version identifier |
| `version_alias` | `"alias"` | Version alias name |
| `run` | `"run"` | Run number |
| `threads` | `"threads"` | Thread count for parallel operations |
| `debug` | `"debug"` | Debug mode flag |
| `searchfilter` | `"filter"` | Search filter expression |
| `searchquery` | `"searchquery"` | Search query string |
| `bulkoperations` | `"bulkoperations"` | Bulk operations flag |
| `collection` | `"collection"` | Collection name |
| `entity` | `"entity"` | Entity name |
| `configuration` | `"configuration"` | Configuration name |
| `configuration_alias` | `"configurationalias"` | Configuration alias name |

---

### Namespace: `filter` - Query Filter Keys

**Brief:** Keys used in database query filters.

| Constant | Value | Purpose |
|----------|-------|---------|
| `entities` | `"entities.name"` | Filter by entity name |
| `configurations` | `"configurations.name"` | Filter by configuration name |
| `version_aliases` | `"aliases.active.name"` | Filter by active version alias |
| `configuration_aliases` | `"configaliases.active.name"` | Filter by active configuration alias |
| `runs` | `"runs.name"` | Filter by run name |
| `version` | `"version"` | Filter by version |
| `version_alias` | `"alias"` | Version alias filter key |
| `run` | `"run"` | Run filter key |
| `configuration_alias` | `"configuration_alias"` | Configuration alias filter key |
| `pipeline` | `"pipeline"` | MongoDB aggregation pipeline |

---

### Namespace: `format` - Output Formats

**Brief:** Output format identifiers.

| Constant | Value | Purpose |
|----------|-------|---------|
| `json` | `"json"` | Standard JSON format output |
| `gui` | `"gui"` | GUI-friendly format output |

---

### Namespace: `msg` - User Messages

**Brief:** User-facing error and status messages.

| Constant | Value | Purpose |
|----------|-------|---------|
| `empty_filter` | `"Search filter is empty"` | Error when no filter provided |
| `empty_document` | `"Json document is empty"` | Error when document is empty |
| `cant_call_system` | (long message) | Error when system() cannot be called |

## Relationship to Other Components

- **ConfigurationDB** - Uses operation constants for dispatch and validation
- **Utilities** - Uses option constants for CLI argument parsing
- **StorageProviders** - Uses filter constants for query construction
- **helper_functions.h** - Uses timestamp constants for formatting
- Included by virtually every file that interacts with the Configuration API

## Example

```cpp
#include "artdaq-database/SharedCommon/configuraion_api_literals.h"
#include "artdaq-database/SharedCommon/shared_exceptions.h"
#include <iostream>
#include <string>

namespace apiliteral = artdaq::database::configapi::literal;
namespace db = artdaq::database;

// Example: Setting up a request
struct RequestOptions {
    std::string operation;
    std::string provider;
    std::string version;
    std::string collection;
    std::string format;

    void setOperation(std::string const& op) { operation = op; }
    void setProvider(std::string const& prov) { provider = prov; }
    void set(std::string const& key, std::string const& value) {
        if (key == apiliteral::option::version) version = value;
        else if (key == apiliteral::option::collection) collection = value;
    }
    void setFormat(std::string const& fmt) { format = fmt; }
};

void setupStoreRequest(RequestOptions& opts) {
    // Using operation constants
    opts.setOperation(apiliteral::operation::writedocument);

    // Using provider constants
    opts.setProvider(apiliteral::provider::mongo);

    // Using option constants
    opts.set(apiliteral::option::version, "v1.0");
    opts.set(apiliteral::option::collection, "detector_configs");

    // Using format constants
    opts.setFormat(apiliteral::format::json);
}

// Example: Checking operation type
bool handleOperation(std::string const& op) {
    if (op == apiliteral::operation::writedocument) {
        std::cout << "Performing store operation" << std::endl;
        return true;
    } else if (op == apiliteral::operation::readdocument) {
        std::cout << "Performing load operation" << std::endl;
        return true;
    } else if (op == apiliteral::operation::findconfigs) {
        std::cout << "Performing search operation" << std::endl;
        return true;
    }
    throw db::invalid_argument("handleOperation")
        << "Unknown operation: " << op;
}

int main() {
    try {
        RequestOptions opts;
        setupStoreRequest(opts);

        std::cout << "Operation: " << opts.operation << std::endl;
        std::cout << "Provider: " << opts.provider << std::endl;

        handleOperation(opts.operation);
        return 0;
    } catch (db::exception const& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
```

## Notes for Developers

### Common Usage Pattern

```cpp
#include "artdaq-database/SharedCommon/configuraion_api_literals.h"

// Create namespace alias for brevity
namespace apiliteral = artdaq::database::configapi::literal;

// Use the alias throughout the file
void example() {
    auto op = apiliteral::operation::writedocument;
    auto prov = apiliteral::provider::filesystem;
    auto ver_key = apiliteral::option::version;
}
```

### Adding New Constants

When adding new operations or options:
1. Add to the appropriate namespace
2. Use `constexpr auto` for compile-time evaluation
3. Follow existing naming conventions (lowercase, underscores for multi-word)
4. Update this documentation file

```cpp
namespace operation {
    // Existing constants...
    constexpr auto newoperation = "newoperation";  // New operation
}
```

### Common Pitfalls

- **Pitfall 1:** Using string literals instead of constants defeats the purpose:
  ```cpp
  // BAD - no compile-time checking, typo-prone
  opts.operation("stoer");  // Typo will not be caught!

  // GOOD - compile-time verification
  opts.operation(apiliteral::operation::writedocument);
  ```

- **Pitfall 2:** Forgetting the namespace alias makes code verbose:
  ```cpp
  // VERBOSE
  opts.operation(artdaq::database::configapi::literal::operation::writedocument);

  // BETTER - use namespace alias
  namespace apiliteral = artdaq::database::configapi::literal;
  opts.operation(apiliteral::operation::writedocument);
  ```

- **Pitfall 3:** Using the wrong namespace level:
  ```cpp
  // BAD - operation is in nested namespace
  auto op = apiliteral::writedocument;  // Error: not in this namespace

  // GOOD - use correct nested namespace
  auto op = apiliteral::operation::writedocument;
  ```

### Anti-patterns

```cpp
// DON'T hardcode strings that have constants defined
void badExample() {
    std::string op = "store";  // BAD: use apiliteral::operation::writedocument
    std::string prov = "mongo"; // BAD: use apiliteral::provider::mongo
}

// DO use the defined constants
void goodExample() {
    auto op = apiliteral::operation::writedocument;
    auto prov = apiliteral::provider::mongo;
}

// DON'T compare with literal strings
if (operation == "store") { }  // BAD

// DO compare with constants
if (operation == apiliteral::operation::writedocument) { }  // GOOD
```

## See Also

- [helper_functions.h](./helper_functions.h.md) - Functions that use these constants
- [returned_result.h](./returned_result.h.md) - Result constants in `artdaq::database::result` namespace
- [shared_datatypes.h](./shared_datatypes.h.md) - Type aliases used with these constants
