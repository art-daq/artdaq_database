# conftool.py

**Path:** `artdaq-database/Utilities/conftool.py`

**Purpose:** High-level Python command-line interface for artdaq-database configuration management. This script provides user-friendly operations for importing, exporting, archiving, and managing DAQ run configurations. It wraps the C++ `conftoolp` module and handles complex multi-step operations including configuration versioning, schema validation, hash-based integrity checking, remote bulk operations via SSH, and configuration filtering by flags.

## Key Concepts

### Configuration Naming Convention

Configurations follow a strict naming pattern with a 5-digit version suffix:
- Format: `{prefix}{5-digit-number}` (e.g., `demo_safemode00001`)
- The prefix identifies the configuration family (e.g., `demo_safemode`)
- Version numbers auto-increment when creating new configurations

### Schema-Based File Mapping

The `schema.fcl` file defines how FHiCL configuration files map to database collections using regex patterns. Each pattern entry specifies:
- `collection` - Target database collection name
- `pattern` - Regex with at least 3 capturing groups
- `entity` (optional) - Python expression for custom entity naming

### Configuration Composition

A configuration consists of multiple entities (documents) spread across collections. The script manages the complete composition, ensuring all required entities are imported or exported together.

### Archive Database

Run configurations can be archived to a separate database (with `_archive` suffix) for historical preservation. Archives are organized by run number.

## Thread Safety

- **Thread Safety:** Not thread-safe (designed for single-user CLI operation)
- **Concurrent Access:** Multiple instances may cause race conditions during import/export
- **Locking:** No internal locking; relies on database-level consistency

## Dependencies

### Python Requirements

- Python 2.7 or Python 3.x compatible
- `conftoolp` - C++ Python bindings module (SWIG-generated)

### Standard Library Modules

| Module | Purpose |
|--------|---------|
| `sys` | System parameters and exit codes |
| `inspect` | Function introspection for dynamic dispatch |
| `json` | JSON parsing and serialization |
| `re` | Regular expression matching for patterns |
| `fnmatch` | Filename pattern matching |
| `os` | Environment variables and path operations |
| `shutil` | File copy operations for schema handling |
| `time` | Timestamps for backup naming |
| `subprocess` | Executing external commands (bulkloader) |
| `socket` | Hostname resolution for remote operations |
| `hashlib` | MD5 hashing for configuration integrity |

### Required Environment Variables

| Variable | Required | Description |
|----------|----------|-------------|
| `ARTDAQ_DATABASE_URI` | Yes | Database connection string (mongodb:// or filesystemdb://) |
| `ARTDAQ_DATABASE_CONFDIR` | No | Directory containing default schema.fcl |
| `ARTDAQ_DATABASE_REMOTEHOST` | No | Remote host for bulk operations via SSH |
| `ARTDAQ_DATABASE_ALLOW_INCOMPLETE_CONFIGURATIONS` | No | Set to "true" to allow importing incomplete configurations |

### Required Files

| File | Description |
|------|-------------|
| `schema.fcl` | Configuration schema defining file-to-collection mappings (copied to working directory if missing) |
| `flags.fcl` | Optional file defining configuration flags for filtering |

## Command-Line Interface

### Synopsis

```bash
conftool.py <operation> [arguments...]
conftool.py --help
```

### Configuration Management Operations

#### `importConfiguration(configNameOrPrefix)`

**Brief:** Imports FHiCL configuration files from the current directory into the database, creating a new configuration version.

**Parameters:**
- `configNameOrPrefix` - Configuration name prefix (e.g., "demo_safemode"); version is auto-incremented

**Example:**
```bash
conftool.py importConfiguration demo_safemode
# Creates: demo_safemode00001 (or next available version)
```

#### `exportConfiguration(configNamePrefix)`

**Brief:** Exports a configuration from the database to FHiCL files in the current directory.

**Parameters:**
- `configNamePrefix` - Configuration name or prefix to export

**Example:**
```bash
conftool.py exportConfiguration demo_safemode00003
# Creates: schema.fcl, flags.fcl, demo_safemode/*.fcl files
```

#### `updateConfigurationFlags(configName)`

**Brief:** Updates only the flags entity of an existing configuration without changing other entities.

**Parameters:**
- `configName` - Full configuration name with version number

**Example:**
```bash
conftool.py updateConfigurationFlags demo_safemode00003
```

### Listing Operations

#### `getListOfAvailableRunConfigurations(searchString='*')`

**Brief:** Returns a list of all configuration names matching the search pattern.

**Parameters:**
- `searchString` - Wildcard pattern to match (default: '*' for all)

**Example:**
```bash
conftool.py getListOfAvailableRunConfigurations demo_*
```

#### `getListOfAvailableRunConfigurationPrefixes(searchString='*')`

**Brief:** Returns unique configuration prefixes (names without version numbers).

**Example:**
```bash
conftool.py getListOfAvailableRunConfigurationPrefixes
```

#### `getLatestConfiguration(configNamePrefix)`

**Brief:** Returns a dictionary mapping entity names to their content for the latest version of a configuration.

**Parameters:**
- `configNamePrefix` - Configuration name or prefix

### Archive Operations

#### `archiveRunConfiguration(config, run_number, update=False)`

**Brief:** Archives a configuration with a specific run number to the archive database.

**Parameters:**
- `config` - Configuration name
- `run_number` - Run number to associate with the archive

**Example:**
```bash
conftool.py archiveRunConfiguration demo_safemode00003 12345
```

#### `updateArchivedRunConfiguration(config, run_number)`

**Brief:** Updates an already archived configuration with new content.

**Example:**
```bash
conftool.py updateArchivedRunConfiguration demo_safemode00003 12345
```

#### `getListOfArchivedRunConfigurations(searchString='*')`

**Brief:** Lists configurations in the archive database.

**Example:**
```bash
conftool.py getListOfArchivedRunConfigurations 12345
```

#### `exportArchivedRunConfiguration(config)`

**Brief:** Exports an archived configuration to the current directory.

**Parameters:**
- `config` - Full archive path including run number (e.g., "12345/demo_safemode00003")

**Example:**
```bash
conftool.py exportArchivedRunConfiguration 23/demo_safemode00003
```

### Flag-Based Filtering (MongoDB Only)

#### `getListOfMaskedRunConfigurations(filename='flags.fcl')`

**Brief:** Returns configurations whose flags match those in the specified file.

**Parameters:**
- `filename` - Path to flags file (default: flags.fcl)

**Example:**
```bash
# Create flags.fcl with desired criteria
echo "flag_detector_on: true" > flags.fcl
conftool.py getListOfMaskedRunConfigurations flags.fcl
```

#### `getListOfAvailableRunConfigurationsSubtractMasked(filename='flags.fcl')`

**Brief:** Returns configurations that do NOT match the flags in the specified file.

### Database Operations

#### `listDatabases()`

**Brief:** Lists all available databases.

#### `listCollections()`

**Brief:** Lists all collections in the current database.

#### `listVersions(collection_name)`

**Brief:** Lists all document versions in a specific collection.

#### `readDatabaseInfo()`

**Brief:** Returns database metadata as JSON.

#### `exportDatabase()`

**Brief:** Exports the entire database to the current directory.

#### `importDatabase()`

**Brief:** Imports database content from the current directory.

### Format Conversion

#### `fhicl_to_json(filename)`

**Brief:** Converts a FHiCL file to JSON format, writing output to `{filename}.json`.

#### `json_to_fhicl(filename)`

**Brief:** Converts a JSON file to FHiCL format, overwriting the original file.

## Usage Examples

### Complete Import Workflow

```bash
# 1. Set up environment
export ARTDAQ_DATABASE_URI="mongodb://localhost:27017/mydb"

# 2. Navigate to configuration directory with FHiCL files
cd /path/to/my_configs

# 3. Ensure schema.fcl exists (will be copied from default if missing)
ls schema.fcl || echo "Will be copied automatically"

# 4. Import configuration
conftool.py importConfiguration myexperiment

# Output:
# New configuration myexperiment00001
# Imported (BoardReaders, boardreader01, ./boardreader01.fcl)
# Imported (EventBuilders, EventBuilder01, ./EventBuilder01.fcl)
# ...
```

### Complete Export Workflow

```bash
# 1. Create and navigate to export directory
mkdir -p /export/configs && cd /export/configs

# 2. Export a specific configuration version
conftool.py exportConfiguration myexperiment00003

# Files created:
# ./schema.fcl
# ./flags.fcl (if exists)
# ./myexperiment/boardreader01.fcl
# ./myexperiment/EventBuilder01.fcl
# ...
```

### Archive for Production Run

```bash
# 1. Prepare configuration files in current directory
# 2. Archive with run number
conftool.py archiveRunConfiguration production_config00005 12345

# 3. Verify archival
conftool.py getListOfArchivedRunConfigurations 12345
# Output: 12345/production_config00005
```

### Flag-Based Configuration Selection

```bash
# Create flags filter file
cat > flags.fcl << EOF
flag_detector_on: true
flag_trigger_enabled: true
EOF

# Get configurations matching these flags
conftool.py getListOfMaskedRunConfigurations flags.fcl

# Get configurations NOT matching these flags
conftool.py getListOfAvailableRunConfigurationsSubtractMasked flags.fcl
```

## Schema File Structure

The `schema.fcl` file must define four sequences:

```
artdaq_processes: [
  {
    collection: BoardReaders
    pattern: '(.*/)(boardreader\d+)(\.fcl$)'
  },
  # ... more process types
]

artdaq_includes: [
  # Common include files (optional)
]

run_history: [
  # Run history patterns (similar to artdaq_processes)
]

system_layout: [
  {
    collection: SystemLayout
    pattern: '(.*)(schema)(\.fcl$)'
  },
  {
    collection: Flags
    pattern: '(.*)(flags)(\.fcl$)'
  }
]
```

### Pattern Requirements

- Patterns must have at least 3 capturing groups
- Group 1: Directory path
- Group 2: Entity name (used as default)
- Group 3: File extension (.fcl)

### Custom Entity Naming

Use the optional `entity` field with a Python expression:

```
{
  collection: TPCs
  pattern: '(.*/)(tpc(we|ww|ew|ee)\d+)(\.fcl$)'
  entity: 'match.group(2).upper()'  # Transforms entity name
}
```

## Internal Functions

### `__hashConfiguration(entity_userdata_map) -> bool`

**Brief:** Generates MD5 hashes for each entity in a configuration and creates an overall configuration hash for integrity verification. Adds a 'hashes' entry to the map.

### `__composition_reader(subsets, layout, files) -> generator`

**Brief:** Generator that yields (collection, entity_name, filepath) tuples by matching files against schema patterns.

### `__validate_schema(schema)`

**Brief:** Validates schema.fcl structure, checking for required sequences and valid regex patterns. Exits with error if validation fails.

### `__archiveConfigurationWithBulkloader(config, run_number, update) -> bool`

**Brief:** Uses SSH to run bulkloader on a remote host for bulk archive operations. Transfers files via SCP and executes the bulk import remotely.

## Error Handling

### Common Error Messages

| Message | Cause | Solution |
|---------|-------|----------|
| "ARTDAQ_DATABASE_URI is not set" | Missing environment variable | Set ARTDAQ_DATABASE_URI before running |
| "Invalid schema.fcl" | Pattern has fewer than 3 capturing groups | Fix regex pattern in schema.fcl |
| "Incomplete configurations not allowed" | Files don't match schema patterns | Set ARTDAQ_DATABASE_ALLOW_INCOMPLETE_CONFIGURATIONS=true or fix schema.fcl |
| "Configuration X is not found" | Update requested for non-existent configuration | Use importConfiguration for new configurations |

## Common Pitfalls

- **Working Directory:** Import/export operations work relative to the current directory
- **Schema Location:** schema.fcl must be in the current directory or will be copied from default
- **Version Numbers:** Configuration names must end with 5 digits for update operations
- **MongoDB Required:** Flag-based filtering operations require MongoDB (not FileSystemDB)
- **Remote Operations:** Require SSH key authentication and ARTDAQ_DATABASE_REMOTEHOST setting

## Relationship to Other Components

- **conftoolp Module:** Underlying C++ implementation wrapped by this Python interface
- **conftool.cc:** C++ command-line tool with similar but lower-level functionality
- **schema.fcl:** Defines file-to-collection mappings used by import/export
- **bulkloader/bulkdownloader:** Called for high-performance bulk operations

## See Also

- [conftool.cc.md](./conftool.cc.md) - C++ command-line tool
- [schema_fcl.md](./schema_fcl.md) - Schema file documentation
- [conftool.h.md](../SWIGBindings/conftool.h.md) - Python bindings header
