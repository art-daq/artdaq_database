# Utilities Module Documentation

## Overview

The Utilities module contains command-line tools, scripts, and configuration files for managing, converting, and maintaining artdaq-database configurations and databases. These tools provide essential functionality for database operations, format conversions, infrastructure management, and maintenance tasks.

## Module Location

**Path**: `artdaq-database/Utilities/`

---

## Command-Line Tools (C++)

### Configuration Database Tools

#### conftool
**Purpose**: Command-line interface to the configuration database

**Key Features**:
- Read/write documents and configurations
- Direct database operations
- Testing and debugging interface

**Documentation**: [conftool.cc.md](./conftool.cc.md)

---

### Bulk Operations

#### bulkloader
**Purpose**: High-performance bulk import of FHiCL files to database

**Usage**:
```bash
bulkloader -p /path/to/configs -c MyConfig -r 1001 -t 8
```

**Key Features**:
- Multi-threaded file loading
- Configurable thread count
- Performance statistics
- Optional verification mode

**Documentation**: [bulkloader.cc.md](./bulkloader.cc.md)

#### bulkdownloader
**Purpose**: High-performance bulk export of database configurations to files

**Usage**:
```bash
bulkdownloader -p /path/to/export -c MyConfig -r 1001
```

**Key Features**:
- Multi-threaded document retrieval
- Automatic directory structure creation
- Configuration composition-based export
- Performance statistics

**Documentation**: [bulkdownloader.cc.md](./bulkdownloader.cc.md)

---

### Database Maintenance

#### migrate_database
**Purpose**: Migrate database to new format with schema updates

**Usage**:
```bash
migrate_database -u filesystemdb:///path/to/database
```

**Key Features**:
- Document format migration
- JSONDocumentMigrator integration
- Non-destructive (creates new database)
- Collection-by-collection processing

**Documentation**: [migrate_database.cc.md](./migrate_database.cc.md)

#### rebuild_database_index
**Purpose**: Rebuild search indexes for filesystem databases

**Usage**:
```bash
rebuild_database_index -u filesystemdb:///path/to/database
```

**Key Features**:
- Creates fresh indexes from documents
- Fixes corrupted indexes
- Non-destructive (creates new database)
- Auto-rebuild mode

**Documentation**: [rebuild_database_index.cc.md](./rebuild_database_index.cc.md)

---

### Format Conversion Tools

#### fhicl2json
**Purpose**: Convert FHiCL configuration files to JSON format

**Usage**:
```bash
fhicl2json -c config.fcl > config.json
fhicl2json -c config.fcl -m  # Extract main subtree only
```

**Key Features**:
- Full FHiCL syntax support
- Subtree extraction (--main flag)
- Include file resolution
- Clean JSON output to stdout

**Documentation**: [fhicl2json.cc.md](./fhicl2json.cc.md)

#### readfhicl
**Purpose**: Read and display FHiCL files with various processing options

**Usage**:
```bash
readfhicl -c config.fcl -r -p  # Resolve and prune
readfhicl -c config.fcl -s     # Show prolog
```

**Key Features**:
- Display with/without prolog
- Full resolution or snippet mode
- Prune nil values
- Formatted output

**Documentation**: [readfhicl.cc.md](./readfhicl.cc.md)

#### refactorfhicl
**Purpose**: Refactor FHiCL files by extracting tables (experimental)

**Status**: Experimental/incomplete

**Documentation**: [refactorfhicl.cc.md](./refactorfhicl.cc.md)

---

### Testing and Validation

#### readjson
**Purpose**: JSON parser/serializer performance testing

**Documentation**: [readjson.cc.md](./readjson.cc.md)

#### fixtestjson
**Purpose**: Convert JSON values to strings for schema compatibility

**Warning**: Overwrites original files

**Documentation**: [fixtestjson.cc.md](./fixtestjson.cc.md)

---

## Scripts and Wrappers

### conftool.py (Python)
**Purpose**: High-level Python wrapper for configuration management

**Usage**:
```bash
conftool.py importConfiguration demo_safemode
conftool.py exportConfiguration demo_safemode00003
conftool.py archiveRunConfiguration production 12345
conftool.py getListOfAvailableRunConfigurations
```

**Key Features**:
- Configuration import/export with versioning
- Run archival operations
- Flag-based configuration filtering
- Schema validation
- Bulk operations via SSH

**Documentation**: [conftool_py.md](./conftool_py.md)

### readwrite.js (JavaScript)
**Purpose**: Node.js utility for FHiCL/JSON round-trip testing

**Usage**:
```bash
node readwrite.js config.fcl
```

**Documentation**: [readwrite_js.md](./readwrite_js.md)

---

## Configuration Files

### .database.bash.rc
**Path**: `bashrc/.database.bash.rc`

**Purpose**: Shell environment setup for artdaq-database operations

**Key Features**:
- UPS product setup
- Environment variable configuration
- Web GUI server management functions
- SSH tunnel utilities

**Documentation**: [database_bash_rc.md](./database_bash_rc.md)

### mongod.conf
**Path**: `config/mongod.conf`

**Purpose**: Template MongoDB configuration file

**Documentation**: [mongod_conf.md](./mongod_conf.md)

### schema.fcl
**Path**: `schema.fcl`

**Purpose**: Configuration schema defining file-to-collection mappings

**Key Features**:
- Regex patterns for file matching
- Collection assignment rules
- Entity name extraction
- Support for artdaq processes, includes, run history, and system layout

**Documentation**: [schema_fcl.md](./schema_fcl.md)

---

## Systemd Services and Infrastructure

**Path**: `systemd/`

### Service Unit Files

| Service | Purpose |
|---------|---------|
| `mongodbserver@.service` | MongoDB server instances |
| `mongodbarbiter@.service` | MongoDB arbiter for replica sets |
| `webconfigeditor@.service` | Web configuration editor |
| `ssh-tunnel@.service` | Persistent SSH tunnels |

### Control Scripts

| Script | Purpose |
|--------|---------|
| `mongod-ctrl.sh` | MongoDB lifecycle management |
| `webconfigeditor-ctrl.sh` | Web editor lifecycle management |
| `backup_artdaq_database.sh` | Database backup operations |
| `setup_database.sh` | Interactive environment setup |
| `ssh-tunnel-ctr.sh` | SSH tunnel creation |

### Support Files

| File | Purpose |
|------|---------|
| `initd_functions` | Shell functions for service management |
| `start.sh` / `stop.sh` / `status.sh` | Simple wrapper scripts |
| `redeploy.sh` | Redeployment helper |

**Documentation**: [systemd_services.md](./systemd_services.md)

---

## Quick Reference

### Most Common Operations

```bash
# Import configuration to database
conftool.py importConfiguration myconfig_prefix

# Export configuration from database
conftool.py exportConfiguration myconfig00001

# Bulk load FHiCL files
bulkloader -p /configs -c MyConfig -r 1001

# Bulk export to files
bulkdownloader -p /export -c MyConfig -r 1001

# Convert FHiCL to JSON
fhicl2json -c config.fcl > config.json

# View resolved FHiCL
readfhicl -c config.fcl -r

# Archive run configuration
conftool.py archiveRunConfiguration production 12345

# List available configurations
conftool.py getListOfAvailableRunConfigurations
```

### Environment Setup

```bash
# Required environment variable
export ARTDAQ_DATABASE_URI="mongodb://localhost:27017/mydb"
# or
export ARTDAQ_DATABASE_URI="filesystemdb:///path/to/database"

# Optional for FHiCL tools
export FHICL_FILE_PATH="/configs:/common:."

# Optional for remote operations
export ARTDAQ_DATABASE_REMOTEHOST="remote-host.fnal.gov"
```

---

## Utility Categories

### By Function

| Category | Utilities |
|----------|-----------|
| **Database Operations** | conftool, conftool.py |
| **Bulk Transfer** | bulkloader, bulkdownloader |
| **Maintenance** | migrate_database, rebuild_database_index |
| **Format Conversion** | fhicl2json, readfhicl, refactorfhicl |
| **Testing/Validation** | readjson, fixtestjson, readwrite.js |
| **Infrastructure** | mongod-ctrl.sh, webconfigeditor-ctrl.sh, backup_artdaq_database.sh |

### By Language

| Language | Utilities |
|----------|-----------|
| **C++** | conftool, bulkloader, bulkdownloader, migrate_database, rebuild_database_index, fhicl2json, readfhicl, refactorfhicl, readjson, fixtestjson |
| **Python** | conftool.py |
| **JavaScript** | readwrite.js |
| **Bash** | mongod-ctrl.sh, webconfigeditor-ctrl.sh, backup_artdaq_database.sh, setup_database.sh, ssh-tunnel-ctr.sh |

### By Database Interaction

| Interaction | Utilities |
|-------------|-----------|
| **Direct DB Access** | conftool, conftool.py, bulkloader, bulkdownloader, migrate_database, rebuild_database_index |
| **File Operations** | fhicl2json, readfhicl, refactorfhicl, readjson, fixtestjson |
| **Infrastructure** | mongod-ctrl.sh, webconfigeditor-ctrl.sh, backup scripts |

---

## Dependencies

### C++ Utilities
- Boost.Program_options, Boost.Filesystem
- artdaq-database core libraries
- fhiclcpp, cetlib (for FHiCL tools)

### Python Wrapper
- Python 2.7 or 3.x
- conftoolp (C++ bindings module)

### JavaScript Utility
- Node.js
- fhicljson native module

### Infrastructure Scripts
- MongoDB (via UPS)
- artdaq_database, artdaq_node_server (via UPS)
- Standard Unix utilities

---

## Exit Codes

All C++ utilities use consistent exit codes:

| Code | Name | Meaning |
|------|------|---------|
| 0 | SUCCESS | Operation completed successfully |
| 1 | HELP | Help message displayed |
| 2+ | INVALID_ARGUMENT | Missing or invalid arguments |
| Variable | FAILURE | Operation failed |
| Variable | UNCAUGHT_EXCEPTION | Unhandled exception |

---

## Safety Considerations

### Destructive Operations
- **fixtestjson**: Overwrites original JSON file
- **backup_artdaq_database.sh**: Creates backups but verify storage

### Non-Destructive Operations
- **migrate_database**: Creates `{uri}_migrated`
- **rebuild_database_index**: Creates `{uri}_new`
- **bulkdownloader**: Creates new directory
- **fhicl2json, readfhicl**: Output to stdout

**Recommendation**: Always create backups before running maintenance operations

---

## Maintenance Status

| Component | Status | Notes |
|-----------|--------|-------|
| conftool | Stable | Production-ready |
| conftool.py | Stable | Production-ready |
| bulkloader | Stable | Production-ready |
| bulkdownloader | Stable | Production-ready |
| migrate_database | Stable | Production-ready |
| rebuild_database_index | Stable | Production-ready |
| fhicl2json | Stable | Production-ready |
| readfhicl | Stable | Production-ready |
| readjson | Stable | Testing tool |
| fixtestjson | Stable | Use with caution |
| refactorfhicl | Experimental | Incomplete |
| systemd services | Stable | Production-ready |
| backup scripts | Stable | Production-ready |

---

## Documentation Index

### C++ Utilities
- [bulkdownloader.cc.md](./bulkdownloader.cc.md)
- [bulkloader.cc.md](./bulkloader.cc.md)
- [conftool.cc.md](./conftool.cc.md)
- [fhicl2json.cc.md](./fhicl2json.cc.md)
- [fixtestjson.cc.md](./fixtestjson.cc.md)
- [migrate_database.cc.md](./migrate_database.cc.md)
- [readfhicl.cc.md](./readfhicl.cc.md)
- [readjson.cc.md](./readjson.cc.md)
- [rebuild_database_index.cc.md](./rebuild_database_index.cc.md)
- [refactorfhicl.cc.md](./refactorfhicl.cc.md)

### Scripts and Configuration
- [conftool_py.md](./conftool_py.md) - Python wrapper
- [readwrite_js.md](./readwrite_js.md) - JavaScript utility
- [database_bash_rc.md](./database_bash_rc.md) - Shell configuration
- [mongod_conf.md](./mongod_conf.md) - MongoDB configuration
- [schema_fcl.md](./schema_fcl.md) - Schema definition

### Infrastructure
- [systemd_services.md](./systemd_services.md) - Services and control scripts

---

## Contact and Support

For questions, issues, or contributions related to the Utilities module, please refer to the main artdaq-database project documentation and issue tracking system.
