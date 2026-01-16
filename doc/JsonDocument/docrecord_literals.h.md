# docrecord_literals.h

**Path:** `artdaq-database/JsonDocument/docrecord_literals.h`

**Purpose:** This header file defines string literals and constants used throughout the JsonDocument module. It provides action name constants for document operations and template strings for document initialization, ensuring consistency and reducing the risk of typos in string literals used for JSON field names and operations.


## Key Concepts

### Compile-Time Constants

All constants use `constexpr auto`:
- **Compile-time evaluation** - No runtime overhead for constant values
- **Type deduction** - Automatically gets correct type (`const char*`)
- **String literals** - Stored in read-only memory segment
- **No initialization order issues** - Constants are guaranteed available

### Centralized String Management

Benefits of centralizing string literals:
- **Compiler catches typos** at compile-time rather than runtime
- **Refactoring requires changes** in one place only
- **Code is more readable** (`actions::markDeleted` vs `"markDeleted"`)
- **IDE autocomplete support** for constant names

### Historical Note

The file contains a disabled `literal` namespace (wrapped in `#if 0`) with field path constants. These are likely disabled because:
- The project now uses literals from `json_common.h` or `configuration_api_literals.h`
- Migration to different literal organization occurred
- Kept for reference documentation of expected document structure

## Thread Safety

- **Thread-safe:** Yes (all constants are `constexpr` and read-only)
- **Concurrent access:** Inherently thread-safe for reading
- **Locking:** No locks needed (compile-time constants)

## Dependencies

This header has no includes - it only defines constants.

## Constants

### namespace actions

**Brief:** Defines string constants for document modification operations. These action names are used in logging, audit trails, and operation dispatch.

#### `addAlias`

**Brief:** Action constant for adding an alias (alternate name) to a document version.

**Type:** `constexpr auto` (const char*)

**Value:** `"addAlias"`

**Usage:** `JSONDocumentBuilder::addAlias()` operation logging

**Related Method:** `JSONDocumentBuilder::addAlias(JSONDocument const&)`

#### `setentity`

**Brief:** Action constant for setting or configuring an entity within a document.

**Type:** `constexpr auto` (const char*)

**Value:** `"setentity"`

**Note:** Uses lowercase (inconsistent with other camelCase names in this namespace)

**Related Method:** `JSONDocumentBuilder::setEntity(JSONDocument const&)`

#### `removeAlias`

**Brief:** Action constant for removing an existing alias from a document.

**Type:** `constexpr auto` (const char*)

**Value:** `"removeAlias"`

**Usage:** `JSONDocumentBuilder::removeAlias()` operation logging

**Related Method:** `JSONDocumentBuilder::removeAlias(JSONDocument const&)`

#### `addToGlobalConfig`

**Brief:** Action constant for adding a configuration to the global configuration list.

**Type:** `constexpr auto` (const char*)

**Value:** `"addToGlobalConfig"`

**Usage:** Logging when configurations are added to global configuration compositions

**Related Method:** `JSONDocumentBuilder::addConfiguration(JSONDocument const&)`

#### `setVersion`

**Brief:** Action constant for setting or updating the version identifier of a document.

**Type:** `constexpr auto` (const char*)

**Value:** `"setVersion"`

**Usage:** `JSONDocumentBuilder::setVersion()` operation logging

**Related Method:** `JSONDocumentBuilder::setVersion(JSONDocument const&)`

#### `markDeleted`

**Brief:** Action constant for marking a document as deleted (soft delete operation).

**Type:** `constexpr auto` (const char*)

**Value:** `"markDeleted"`

**Usage:** `JSONDocumentBuilder::markDeleted()` operation logging

**Behavior:** Sets `bookkeeping.isdeleted` flag to true; document remains in database but is excluded from normal queries

**Related Method:** `JSONDocumentBuilder::markDeleted()`

#### `markReadonly`

**Brief:** Action constant for marking a document as read-only (write-protected).

**Type:** `constexpr auto` (const char*)

**Value:** `"markReadonly"`

**Usage:** `JSONDocumentBuilder::markReadonly()` operation logging

**Behavior:** Sets `bookkeeping.isreadonly` flag to true; subsequent modification attempts throw `readonly_exception`

**Related Method:** `JSONDocumentBuilder::markReadonly()`

### template__empty_document

**Brief:** Provides a minimal valid JSON document template containing only an empty JSON object.

**Type:** `constexpr auto` (const char*)

**Value:**
```cpp
constexpr auto template__empty_document =
    "{\n"
    "}";
```

**Characteristics:**
- Contains only an empty JSON object `{}`
- Includes newline for formatting
- Used as starting point for new documents

**Usage:**
```cpp
// From JSONDocumentBuilder constructor
JSONDocumentBuilder::JSONDocumentBuilder()
    : _document(std::string(template__empty_document)),
      // ...

// From createFromData
_createFromTemplate({std::string{template__empty_document}});
```

### Disabled literal Namespace

**Brief:** A disabled namespace containing field path constants that document the expected JSON document structure.

The file contains commented-out (`#if 0`) field path constants:

```cpp
#if 0
namespace literal {
  // Document structure paths
  constexpr auto document = "document";
  constexpr auto data = "document.data";
  constexpr auto search = "document.search";
  constexpr auto metadata = "document.metadata";
  constexpr auto comments = "document.metadata.comments";

  // Version and ID
  constexpr auto version = "version";
  constexpr auto id = "document._id";

  // Alias management
  constexpr auto alias = "alias";
  constexpr auto aliases = "aliases";
  constexpr auto aliases_active = "aliases.active";
  constexpr auto aliases_history = "aliases.history";

  // Bookkeeping
  constexpr auto bookkeeping = "bookkeeping";
  constexpr auto bookkeeping_updates = "bookkeeping.updates";
  constexpr auto bookkeeping_isdeleted = "bookkeeping.isdeleted";
  constexpr auto bookkeeping_isreadonly = "bookkeeping.isreadonly";

  // Configurations
  constexpr auto configuration = "configuration";
  constexpr auto configurations = "configurations";
  constexpr auto configurations_name = "configurations.name";

  // Entities
  constexpr auto name = "name";
  constexpr auto entities = "entities";
  constexpr auto entities_name = "entities.name";

  // Origin and changelog
  constexpr auto changelog = "changelog";
  constexpr auto origin = "origin";
  constexpr auto source_rawdata = "origin.rawdata";

  constexpr auto document_root = "root";
}
#endif
```

These constants document the expected JSON document structure even though they are disabled in compilation.

## Relationship to Other Components

### Within the JsonDocument Module

- **JSONDocumentBuilder.cpp** - Uses action constants for logging and audit trails
- **JSONDocumentBuilder.h** - Uses `template__empty_document` for initialization
- **common.h** - Includes this header for module-wide availability
- **JSONDocumentMigrator.cpp** - References action semantics during migration

### Field Literals Location

Field path literals have migrated to:
- `artdaq-database/DataFormats/Json/json_common.h` - Core field names
- `artdaq-database/SharedCommon/configuration_api_literals.h` - API-specific literals

The `actions` namespace and `template__empty_document` constant remain active in this file.

## Usage Examples

### Using Action Constants

```cpp
#include "artdaq-database/JsonDocument/docrecord_literals.h"
#include <string>

using namespace artdaq::database::docrecord;

void logAction(std::string const& action) {
  if (action == actions::markReadonly) {
    std::cout << "Document marked as read-only" << std::endl;
  } else if (action == actions::addAlias) {
    std::cout << "Alias added to document" << std::endl;
  } else if (action == actions::markDeleted) {
    std::cout << "Document soft-deleted" << std::endl;
  }
}
```

### Using Template Constant

```cpp
#include "artdaq-database/JsonDocument/docrecord_literals.h"
#include "artdaq-database/JsonDocument/JSONDocument.h"

using namespace artdaq::database::docrecord;

void createEmptyDocument() {
  // Create empty document from template
  JSONDocument doc(std::string(template__empty_document));
  // doc now contains: {}
}
```

### Action Dispatch Pattern

```cpp
#include "artdaq-database/JsonDocument/docrecord_literals.h"

using namespace artdaq::database::docrecord;

void performAction(JSONDocumentBuilder& builder,
                   std::string const& action,
                   JSONDocument const& data) {
  if (action == actions::addAlias) {
    builder.addAlias(data);
  } else if (action == actions::removeAlias) {
    builder.removeAlias(data);
  } else if (action == actions::setVersion) {
    builder.setVersion(data);
  } else if (action == actions::markDeleted) {
    builder.markDeleted();
  } else if (action == actions::markReadonly) {
    builder.markReadonly();
  }
}
```

## Notes for Developers

### Best Practices

1. **Always use constants** instead of string literals:
   ```cpp
   // Good - typo caught at compile time
   if (action == actions::markDeleted)

   // Bad - typo causes runtime bug
   if (action == "markDleted")
   ```

2. **Import namespace selectively** for clarity:
   ```cpp
   using namespace artdaq::database::docrecord::actions;
   // Now can use: markDeleted, addAlias, etc.
   ```

3. **Do not modify action names** - They may be persisted in databases, logs, or audit trails

4. **Use in logging consistently** - Ensures searchable audit trails:
   ```cpp
   TLOG(10) << "Performing action: " << actions::markReadonly;
   ```

### Naming Conventions

| Pattern | Example | Notes |
|---------|---------|-------|
| Action names | `addAlias`, `markDeleted` | camelCase |
| Exception | `setentity` | lowercase (inconsistency) |
| Template constants | `template__empty_document` | double underscore separator |

### The `#if 0` Block

The disabled `literal` namespace code is completely excluded from compilation but serves as documentation for:
- Expected document field structure
- Path naming conventions
- Historical reference for document schema

This may be:
- **Removed** - Once migration is complete
- **Re-enabled** - If module-specific literals are needed again
- **Partially restored** - Cherry-picking specific literals
- **Kept as documentation** - Reference for document structure

## See Also

- [common.h.md](./common.h.md) - Aggregator header that includes this file
- [JSONDocumentBuilder.h.md](./JSONDocumentBuilder.h.md) - Primary user of action constants
- [JSONDocumentBuilder.cpp.md](./JSONDocumentBuilder.cpp.md) - Implementation using these constants
- [docrecord_exceptions.h.md](./docrecord_exceptions.h.md) - Exception types (readonly_exception relates to markReadonly)
- [../DataFormats/Json/json_common.h.md](../DataFormats/Json/json_common.h.md) - Current location of field literals
