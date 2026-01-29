# schema.fcl

**Path:** `artdaq-database/Utilities/schema.fcl`

**Purpose:** Configuration schema file that defines how FHiCL configuration files are mapped to database collections during import and export operations. This file uses regex patterns to match filenames and determine which collection each file belongs to, enabling automatic organization of configuration data.

## Key Concepts

### Collection Mapping

The schema maps configuration files to database collections based on filename patterns. This allows the import/export tools to automatically organize files into appropriate collections without manual specification.

### Regex Pattern Matching

Each schema entry uses a regex pattern with capturing groups:
- **Group 1:** Directory path (not used for entity naming)
- **Group 2:** Entity name (filename without extension, used as document identifier)
- **Group 3:** File extension (.fcl)

The pattern must have at least 3 capturing groups for validation to pass.

### Schema Sections

The schema defines four required sequences:
- `artdaq_processes` - DAQ process configurations (BoardReaders, EventBuilders, etc.)
- `artdaq_includes` - Shared include files (can be empty)
- `run_history` - Run-specific configuration for archival
- `system_layout` - System-wide configuration (schema itself, flags)

## Thread Safety

- **Thread Safety:** Configuration file (read-only at runtime)
- **Concurrent Access:** Safe for concurrent reads by multiple processes

## Required Structure

```
artdaq_processes: [
  { collection: "CollectionName", pattern: "regex" },
  ...
]

artdaq_includes: [
  { collection: "CollectionName", pattern: "regex" },
  ...
]

run_history: [
  { collection: "CollectionName", pattern: "regex" },
  ...
]

system_layout: [
  { collection: "CollectionName", pattern: "regex" },
  ...
]
```

### Entry Fields

| Field | Required | Description |
|-------|----------|-------------|
| `collection` | Yes | Target database collection name |
| `pattern` | Yes | Regex pattern with at least 3 capturing groups |
| `entity` | No | Python expression for custom entity naming |

## Pattern Syntax

### Basic Pattern Structure

```
'(directory-pattern)(entity-name-pattern)(extension-pattern)'
```

### Common Pattern Examples

```
# Simple numeric suffix
'(.*/)(boardreader\d+)(\.fcl$)'
# Matches: path/to/boardreader01.fcl -> entity: boardreader01

# With optional prefix
'(.*/)(EventBuilder(\d+|_standard))(\.fcl$)'
# Matches: EventBuilder01.fcl, EventBuilder_standard.fcl

# Multiple component patterns
'(.*/)((we|ww|ew|ee)\d+(b|t|m)?)(\.fcl$)'
# Matches: we01.fcl, ww02b.fcl, ee05m.fcl

# Complex detector naming
'(.*/)((|icarus)pmt((|we|ww|ew|ee)(|top|bot)\d+|_standard))(\.fcl$)'
# Matches: pmt01.fcl, icaruspmt_standard.fcl, pmtwe_top01.fcl
```

### Entity Expression Syntax

When the default entity name (group 2) needs transformation:

```
{
  collection: TPCs
  pattern: '(.*/)(tpc(we|ww|ew|ee)(\d+))(\.fcl$)'
  entity: 'match.group(2).upper()'
}
# tpcwe01.fcl -> entity: TPCWE01

{
  collection: CombinedEntities
  pattern: '(.*/)(component(\d+)_sub(\d+))(\.fcl$)'
  entity: '"component_" + match.group(3) + "_" + match.group(4)'
}
# component01_sub02.fcl -> entity: component_01_02
```

## Collection Types

The example schema defines collections for ICARUS detector components:

| Collection | Description | Example Files |
|------------|-------------|---------------|
| `GPSs` | GPS timing configurations | gps01.fcl, gps_standard.fcl |
| `Triggers` | Trigger system configurations | icarustrigger.fcl |
| `WRs` | White Rabbit timing configurations | wr01.fcl, wr_pull.fcl |
| `XARAs` | XARA readout configurations | xara01.fcl |
| `MBBs` | Mini Backplane Board configurations | mbb01.fcl |
| `WIBs` | Warm Interface Board configurations | wib01.fcl |
| `PTBs` | Penn Trigger Board configurations | ptb01.fcl |
| `CRTs` | Cosmic Ray Tagger configurations | crt01.fcl, icarus_crt_feb01.fcl |
| `PMTs` | Photomultiplier Tube configurations | pmt01.fcl, icaruspmt_standard.fcl |
| `TPCs` | Time Projection Chamber configurations | tpc01.fcl, icarustpc_standard.fcl |
| `NevisTPCs` | Nevis TPC driver configurations | NevisTPC2StreamCALIB_driver.fcl |
| `EventBuilders` | Event builder process configurations | EventBuilder01.fcl |
| `DataLoggers` | Data logger process configurations | DataLogger01.fcl |
| `Dispatchers` | Dispatcher process configurations | Dispatcher01.fcl |
| `RoutingManager` | Routing manager configurations | RoutingManager01.fcl |
| `Boot` | Boot configuration | boot.fcl |
| `UserSettings` | User settings | user_setings.fcl |
| `KnownBoardreaders` | Known boardreaders list | known_boardreaders_list.fcl |
| `CriticalProcesses` | Critical process list | critical_process_list.fcl |
| `DataflowConfiguration` | Dataflow configuration | DataflowConfiguration.fcl |
| `Hashes` | Configuration hashes | hashes.fcl |
| `RunHistory` | Run history documents | (various, for archival) |
| `SystemLayout` | System layout | schema.fcl |
| `Flags` | Configuration flags | flags.fcl |

## Usage Examples

### Minimal Schema

```
artdaq_processes: [{
  collection: BoardReaders
  pattern: '(.*/)(boardreader\d+)(\.fcl$)'
}, {
  collection: EventBuilders
  pattern: '(.*/)(EventBuilder\d+)(\.fcl$)'
}]

artdaq_includes: []

run_history: [{
  collection: RunHistory
  pattern: '(.*/)(.*)(\.fcl$)'
}]

system_layout: [{
  collection: SystemLayout
  pattern: '(.*)(schema)(\.fcl$)'
}]
```

### Production Schema with Custom Entity Names

```
artdaq_processes: [{
  collection: TPCs
  pattern: '(.*/)(((east|west)_tpc)(\d+)(top|bot)?)(\.fcl$)'
  entity: 'match.group(3) + match.group(4) + (match.group(5) or "")'
}]
```

### Validating a Schema

When conftool.py imports a configuration, it validates the schema:

```bash
# If validation fails, you'll see errors like:
# Error: Schema rule artdaq_processes:[{collection:X pattern:Y}]
#   has an invalid regular expression in the "pattern" key-value pair.
#   Regular expressions must have at least three capturing groups...
```

## Validation Rules

The schema validator checks:

1. **Required Sequences:** All four sequences must exist (`artdaq_processes`, `artdaq_includes`, `run_history`, `system_layout`)
2. **Sequence Type:** Each must be a list/sequence type
3. **Entry Type:** Each entry must be a table/dictionary
4. **Required Fields:** Each entry must have `collection` and `pattern`
5. **Pattern Validity:** Regex must compile successfully
6. **Capturing Groups:** Pattern must have at least 3 capturing groups
7. **Entity Expression:** If `entity` is provided, it must be a valid Python expression

## Common Pitfalls

- **Fewer Than 3 Groups:** Patterns like `(.*)(filename\.fcl)` will fail validation
- **Invalid Regex:** Unescaped special characters (e.g., `.` instead of `\.`)
- **Missing Collection:** Forgetting the `collection` field
- **Python Syntax Errors:** Invalid expressions in `entity` field
- **Quote Style:** Use single quotes for regex patterns to avoid escape issues

## File Locations

| Context | Location |
|---------|----------|
| During import | Must be in current working directory |
| Default location | `$ARTDAQ_DATABASE_CONFDIR/schema.fcl` |
| Fallback | Copied from artdaq-database installation |

When running `conftool.py importConfiguration`:
1. Looks for `schema.fcl` in current directory
2. If not found, copies from `$ARTDAQ_DATABASE_CONFDIR/schema.fcl`
3. If that doesn't exist, copies from package installation directory

## artdaq_processes vs run_history

Both sections often have similar patterns but serve different purposes:

- **artdaq_processes:** Used during normal configuration import/export
  - Files go to specific collection types (BoardReaders, EventBuilders, etc.)
  - Enables organized browsing and management

- **run_history:** Used during run archival
  - All files typically go to a single `RunHistory` collection
  - Associates configurations with specific run numbers

## Relationship to Other Components

- **conftool.py:** Primary consumer that reads and validates the schema
- **conftoolp C++ module:** Uses schema patterns for file matching
- **bulkloader/bulkdownloader:** May use schema for bulk operations

## See Also

- [conftool_py.md](./conftool_py.md) - Python tool that uses this schema
- [conftool.cc.md](./conftool.cc.md) - C++ tool documentation
- [configurationdbifc.h.md](../ConfigurationDB/configurationdbifc.h.md) - Database interface
