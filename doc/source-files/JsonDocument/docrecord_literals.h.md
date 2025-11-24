# docrecord_literals.h

## File Overview

This header file defines string literals and constants used throughout the JsonDocument module. It provides a centralized location for action names, field names, and template strings that are used for JSON document operations, ensuring consistency and reducing the risk of typos in string literals.

**Location**: `/home/user/artdaq-database/artdaq-database/JsonDocument/docrecord_literals.h`

## Header Guard

```cpp
#ifndef _ARTDAQ_DATABASE_DOCRECORD_DOCUMENT_LITERALS_H_
#define _ARTDAQ_DATABASE_DOCRECORD_DOCUMENT_LITERALS_H_
...
#endif
```

## Namespace Structure

```cpp
namespace artdaq {
namespace database {
namespace docrecord {
  // Literals and constants defined here
}
}
}
```

All constants are defined in the `artdaq::database::docrecord` namespace.

## Contents

The file contains two main sections:
1. **Commented-out literal namespace** - Legacy field path definitions (disabled)
2. **Actions namespace** - Active action name constants
3. **Template constants** - JSON document templates

## 1. Literal Namespace (Disabled)

The file contains an extensive set of commented-out string literals (wrapped in `#if 0 ... #endif`):

```cpp
#if 0
namespace literal {
  constexpr auto document = "document";
  constexpr auto data = "document.data";
  constexpr auto search = "document.search";
  // ... many more ...
}
#endif
```

### Why Disabled?

These literals are likely disabled because:
- The project now uses literals from `artdaq-database/DataFormats/Json/json_common.h` instead
- These were part of an older API design
- They're kept for reference or potential future use
- Migration to a different literal organization system

### Disabled Literal Categories

Even though disabled, these literals document the JSON document structure:

**Document Structure**:
- `document` - Root document object
- `data` - Document data path ("document.data")
- `search` - Search metadata ("document.search")
- `metadata` - Document metadata ("document.metadata")
- `comments` - Metadata comments ("document.metadata.comments")

**Document Identity**:
- `id` - Document ID field ("document._id")
- `version` - Version field
- `alias` - Single alias field
- `name` - Name field

**Collections and Arrays**:
- `aliases` - Aliases array
- `aliases_active` - Active aliases
- `aliases_history` - Alias history
- `configurations` - Configurations array
- `configurations_name` - Configuration names
- `entities` - Entities array
- `entities_name` - Entity names

**Bookkeeping**:
- `bookkeeping` - Bookkeeping metadata
- `bookkeeping_updates` - Update tracking
- `bookkeeping_isdeleted` - Deletion flag
- `bookkeeping_isreadonly` - Read-only flag

**Other Fields**:
- `changelog` - Change history
- `origin` - Origin metadata
- `source_rawdata` - Raw data source
- `configuration` - Single configuration
- `document_root` - Root identifier

## 2. Actions Namespace (Active)

The `actions` namespace defines string constants for document modification operations:

```cpp
namespace actions {
  constexpr auto addAlias = "addAlias";
  constexpr auto setentity = "setentity";
  constexpr auto removeAlias = "removeAlias";
  constexpr auto addToGlobalConfig = "addToGlobalConfig";
  constexpr auto setVersion = "setVersion";
  constexpr auto markDeleted = "markDeleted";
  constexpr auto markReadonly = "markReadonly";
}
```

### Action Descriptions

#### addAlias
**Purpose**: Add an alias (alternate name) to a document
**Usage**: `JSONDocumentBuilder::addAlias()`
**Example**: Adding "latest_stable" as an alias for a specific configuration version

#### setentity
**Purpose**: Set or configure an entity within a document
**Usage**: Entity configuration operations
**Example**: Defining a component entity in the system configuration
**Note**: Uses lowercase naming convention (different from other actions)

#### removeAlias
**Purpose**: Remove an existing alias from a document
**Usage**: `JSONDocumentBuilder::removeAlias()`
**Example**: Removing an obsolete alias when a new version is promoted

#### addToGlobalConfig
**Purpose**: Add a configuration to the global configuration list
**Usage**: Global configuration management operations
**Example**: Registering a new detector configuration in the global registry

#### setVersion
**Purpose**: Set or update the version identifier of a document
**Usage**: `JSONDocumentBuilder::setVersion()`
**Example**: Updating document version to "v2.1.0"

#### markDeleted
**Purpose**: Mark a document as deleted (soft delete)
**Usage**: `JSONDocumentBuilder::markDeleted()`
**Example**: Marking an obsolete configuration as deleted without physically removing it
**Behavior**: Sets bookkeeping flag, document remains in database

#### markReadonly
**Purpose**: Mark a document as read-only (write-protected)
**Usage**: `JSONDocumentBuilder::markReadonly()`
**Example**: Protecting a production configuration from accidental modification
**Behavior**: Prevents further modifications, throws `readonly_exception` on write attempts

### Usage Pattern

These action constants are typically used for:
1. **Operation Logging** - Recording what operation was performed
2. **Change Tracking** - Building changelog entries
3. **API Consistency** - Ensuring operation names are consistent
4. **String Comparison** - Comparing operation types safely

Example usage:
```cpp
// In logging or changelog
TLOG(10) << "Performing action: " << actions::addAlias;

// In operation dispatch
if (operation == actions::markReadonly) {
  builder.markReadonly();
}
```

## 3. Template Constants

### template__empty_document

```cpp
constexpr auto template__empty_document =
    "{\n"
    "}";
```

**Purpose**: Provides a minimal valid JSON document template

**Characteristics**:
- Contains only an empty JSON object
- Includes newline for formatting
- Used as starting point for new documents

**Usage**:
```cpp
// From JSONDocumentBuilder.cpp
JSONDocumentBuilder::JSONDocumentBuilder()
    : _document(std::string(template__empty_document)),
      _overlay(std::make_unique<ovlDatabaseRecord>(_document._value)),
      _initOK(init()) {}
```

**Why This Pattern?**
- Ensures all documents start with valid JSON
- Provides consistent initialization
- Overlay classes can add structure to this empty foundation
- Simpler than hard-coding "{}" in multiple places

## Design Rationale

### Using constexpr

All constants use `constexpr auto`:
- **Compile-time evaluation** - No runtime overhead
- **Type deduction** - Automatically gets correct type
- **String literals** - Stored in read-only memory
- **No initialization order issues** - Guaranteed available

### Centralized Literals

Benefits of centralizing these strings:
1. **Typo Prevention** - Compiler catches typos at compile-time
2. **Refactoring Safety** - Change string value in one place
3. **Code Readability** - `actions::markDeleted` is clearer than `"markDeleted"`
4. **Autocomplete** - IDEs can suggest available actions
5. **Documentation** - Single source of truth for operation names

### Namespace Organization

Using nested namespaces (`actions`, formerly `literal`):
- Groups related constants logically
- Prevents naming conflicts
- Provides context (e.g., `actions::addAlias` vs just `addAlias`)
- Allows selective imports via `using namespace actions`

## Migration Notes

The commented-out `literal` namespace suggests this file is in transition:

**Old Approach** (disabled):
```cpp
namespace literal {
  constexpr auto data = "document.data";
}
```

**New Approach** (likely):
Use literals from `artdaq-database/DataFormats/Json/json_common.h` or related headers, such as `configuration_api_literals.h`.

**Why Migrate?**
- Centralize literals across multiple modules
- Reduce duplication between JsonDocument and other modules
- Use a more standardized literal organization system
- Keep module-specific actions here, common paths elsewhere

## Related Files

- **json_common.h** - Provides JSON data format literals (likely replacement for disabled literals)
- **configuration_api_literals.h** - Contains configuration-related string constants
- **JSONDocumentBuilder.cpp** - Primary consumer of action constants
- **JSONDocument.cpp** - Uses path literals for navigation

## Usage Examples

### Using Action Constants

```cpp
#include "artdaq-database/JsonDocument/docrecord_literals.h"

using namespace artdaq::database::docrecord;

// In a function
void performAction(std::string const& action) {
  if (action == actions::markReadonly) {
    builder.markReadonly();
  } else if (action == actions::addAlias) {
    builder.addAlias(alias_doc);
  }
}
```

### Using Template Constant

```cpp
#include "artdaq-database/JsonDocument/docrecord_literals.h"

using namespace artdaq::database::docrecord;

// Create empty document
JSONDocument doc(std::string(template__empty_document));
```

## Best Practices

1. **Always use these constants** instead of string literals in code
   ```cpp
   // Good
   if (action == actions::markDeleted)

   // Bad
   if (action == "markDeleted")
   ```

2. **Import namespace selectively** for clarity
   ```cpp
   using namespace artdaq::database::docrecord::actions;
   // Now can use: markDeleted, addAlias, etc.
   ```

3. **Don't modify action names** - They may be persisted in databases or logs

4. **Use in switch statements** when appropriate
   ```cpp
   // Note: Can't use strings in switch, but can use in if-else chains
   if (action == actions::addAlias) { /* ... */ }
   else if (action == actions::removeAlias) { /* ... */ }
   ```

## Future Considerations

The disabled `literal` namespace may be:
- **Removed** - Once migration to new literal system is complete
- **Re-enabled** - If module-specific literals are needed again
- **Partially restored** - Cherry-pick specific literals that don't conflict
- **Archived** - Kept as reference documentation

## Notes

- The double underscore in `template__empty_document` follows C++ convention for avoiding name conflicts
- Action names are camelCase except `setentity` (all lowercase)
- The template string includes actual newline (`\n`) for readable JSON output
- These constants have internal linkage when used as `constexpr` in header-only contexts
- The `#if 0` block is a C/C++ preprocessor directive that completely excludes code from compilation
