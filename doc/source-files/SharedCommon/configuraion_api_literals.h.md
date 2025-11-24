# configuraion_api_literals.h

## File Overview

This header file defines a comprehensive set of string literals and constants used throughout the artdaq-database Configuration API. It serves as a central repository for all API operation names, parameter keys, provider names, filter keys, format types, and other literal values to ensure consistency across the codebase and prevent string literal typos.

**Location**: `/home/user/artdaq-database/artdaq-database/SharedCommon/configuraion_api_literals.h`

## Dependencies

This file has no external dependencies beyond standard C++ (it's a pure header file with only constexpr string literals).

## Namespace Structure

The file organizes literals in a hierarchical namespace structure:

```
artdaq::database::result              - Result messages
artdaq::database::configapi::literal  - Main literal definitions
  ├─ operation                        - Operation names
  ├─ provider                         - Database provider names
  ├─ option                           - Option/parameter keys
  ├─ filter                           - Filter keys for queries
  ├─ format                           - Data format types
  └─ msg                              - User-facing messages
```

## Constants and Literals

### Result Messages (artdaq::database::result)

| Constant | Value | Purpose |
|----------|-------|---------|
| `msg_EmptyFilter` | `"{\"message\":\"empty_filter\"}"` | JSON message for empty filter errors |
| `msg_EmptyDocument` | `"{\"message\":\"Json document is empty\"}"` | JSON message for empty document errors |
| `msg_SystemCallFailed` | Complex message | Error message when system() call fails due to permissions |

### Operation Names (operation namespace)

Configuration operations:
- `writeconfiguration` = `"globalconfstore"` - Store complete configuration
- `readconfiguration` = `"globalconfload"` - Load complete configuration
- `newconfig` = `"newconfig"` - Create new configuration
- `assignconfig` = `"addconfig"` - Assign/add configuration
- `removeconfig` = `"rmconfig"` - Remove configuration

Document operations:
- `overwritedocument` = `"overwritedocument"` - Overwrite existing document
- `writedocument` = `"store"` - Store document
- `readdocument` = `"load"` - Load document

Discovery operations:
- `listcollections` = `"listcollections"` - List available collections
- `listdatabases` = `"listdatabases"` - List available databases
- `readdbinfo` = `"readdbinfo"` - Read database information

Search operations:
- `findconfigs` = `"findconfigs"` - Find configurations
- `findversions` = `"findversions"` - Find versions
- `findentities` = `"findentities"` - Find entities
- `searchcollection` = `"searchcollection"` - Search within collection

Entity management:
- `addentity` = `"addentity"` - Add entity
- `rmentity` = `"rmentity"` - Remove entity

State operations:
- `markdeleted` = `"markdeleted"` - Mark document as deleted
- `markreadonly` = `"markreadonly"` - Mark document as read-only

Composition operations:
- `confcomposition` = `"buildfilter"` - Build configuration filter/composition

Run management:
- `addrun` = `"addrun"` - Add run information

Alias operations:
- `addversionalias` = `"addveralias"` - Add version alias
- `rmversionalias` = `"rmveralias"` - Remove version alias
- `findversionalias` = `"findveralias"` - Find version alias
- `addconfigalias` = `"addconfigalias"` - Add configuration alias
- `rmconfigalias` = `"rmconfigalias"` - Remove configuration alias
- `findconfigalias` = `"findconfigalias"` - Find configuration alias

Import/Export operations:
- `exportdatabase` = `"exportdatabase"` - Export entire database
- `importdatabase` = `"importdatabase"` - Import entire database
- `exportconfig` = `"exportconfig"` - Export configuration
- `importconfig` = `"importconfig"` - Import configuration
- `exportcollection` = `"exportcollection"` - Export collection
- `importcollection` = `"importcollection"` - Import collection

### Provider Names (provider namespace)

| Constant | Value | Purpose |
|----------|-------|---------|
| `mongo` | `"mongo"` | MongoDB database provider |
| `filesystem` | `"filesystem"` | Filesystem-based database provider |
| `ucon` | `"ucon"` | UConDB (Unified Conditions Database) provider |

### Option Keys (option namespace)

Request parameters:
- `path` = `"path"` - File path parameter
- `operation` = `"operation"` - Operation type
- `format` = `"dataformat"` - Data format specification
- `provider` = `"dbprovider"` - Database provider selection
- `result` = `"result"` - Result field
- `source` = `"source"` - Source field

Version/Run parameters:
- `version` = `"version"` - Version identifier
- `version_alias` = `"alias"` - Version alias
- `run` = `"run"` - Run number
- `threads` = `"threads"` - Number of threads
- `debug` = `"debug"` - Debug flag

Search parameters:
- `searchfilter` = `"filter"` - Search filter
- `searchquery` = `"searchquery"` - Search query
- `bulkoperations` = `"bulkoperations"` - Bulk operations flag

Entity parameters:
- `collection` = `"collection"` - Collection name
- `entity` = `"entity"` - Entity name
- `configuration` = `"configuration"` - Configuration name
- `configuration_alias` = `"configurationalias"` - Configuration alias

### Filter Keys (filter namespace)

Query filter keys for searching within documents:
- `entities` = `"entities.name"` - Filter by entity name
- `configurations` = `"configurations.name"` - Filter by configuration name
- `version_aliases` = `"aliases.active.name"` - Filter by active version aliases
- `configuration_aliases` = `"configaliases.active.name"` - Filter by active configuration aliases
- `runs` = `"runs.name"` - Filter by run name
- `version` = `"version"` - Filter by version
- `version_alias` = `"alias"` - Filter by version alias
- `run` = `"run"` - Filter by run
- `configuration_alias` = `"configuration_alias"` - Filter by configuration alias
- `pipeline` = `"pipeline"` - Filter by pipeline

### Format Types (format namespace)

| Constant | Value | Purpose |
|----------|-------|---------|
| `gui` | `"gui"` | GUI-formatted output |
| `json` | `"json"` | JSON-formatted output |

### User Messages (msg namespace)

- `empty_filter` = `"Search filter is empty"` - Error message for empty filter
- `empty_document` = `"Json document is empty"` - Error message for empty document
- `cant_call_system` = Complex message - Error when system() call is unavailable

### General Literals

Common values:
- `name` = `"name"` - Generic "name" key
- `notprovided` = `"notprovided"` - Value indicating not provided
- `apiname` = `"artdaq_database"` - API name identifier
- `whitespace` = `" "` - Single space character
- `nullstring` = `""` - Empty string

JSON templates:
- `empty_json` = `"{ }"` - Empty JSON object
- `empty_search_result` = `"{\"search\":[ ]}"` - Empty search result structure
- `empty_filesystem_index` = Complex JSON - Template for empty filesystem index

Export/Import settings:
- `bzip2base64` = `"ascii.tar.bzip2.base64"` - Export format identifier
- `tmpdirprefix` = `"/tmp/adb"` - Temporary directory prefix
- `dbexport_extension` = `".tar-bzip2-base64"` - Export file extension

Database format:
- `database_format_version` = `3` - Current database format version number
- `database_format_locale` = `"en_US.UTF-8"` - Database locale setting

Timestamp formats:
- `timestamp_format_old` = `"%a %b %d %H:%M:%S %Y"` - Legacy timestamp format
- `timestamp_format` = `"%FT%T.000%z"` - ISO 8601 timestamp format
- `timestamp_faketime` = `"2017-07-18T12:48:10.123-0500"` - Fake time for testing

## Usage Context

This file is included by virtually every source file in the artdaq-database project that needs to:
- Construct API operation requests
- Parse operation names
- Build search queries
- Format output
- Handle database providers
- Export/import data

### Usage Example

```cpp
#include "artdaq-database/SharedCommon/configuraion_api_literals.h"

namespace apiliteral = artdaq::database::configapi::literal;

// Using operation names
std::string operation = apiliteral::operation::writedocument;

// Using option keys
std::string provider_key = apiliteral::option::provider;
std::string mongo_provider = apiliteral::provider::mongo;

// Using filter keys
std::string entity_filter = apiliteral::filter::entities;

// Building temporary directory
std::string tmpdir = apiliteral::tmpdirprefix + "123456";
```

## Design Pattern

This file follows the **String Literal Registry** pattern:
- All string literals are defined as `constexpr` for compile-time evaluation
- Organized in nested namespaces for logical grouping
- Prevents typos and makes refactoring easier
- Provides single source of truth for API strings
- Enables compiler optimization (strings are compile-time constants)

## Notes

- The file name has a typo: "configuraion" should be "configuration" (note the missing 't')
- All literals are `constexpr` meaning they have zero runtime overhead
- The empty_filesystem_index template represents the structure of the filesystem provider's index
- The database_format_version (3) indicates the schema version for compatibility checking
- Timestamp formats support both old legacy format and new ISO 8601 format for backward compatibility
