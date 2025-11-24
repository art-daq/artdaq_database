# Utilities Module Documentation

## Overview

The Utilities module contains command-line tools for managing, converting, and maintaining artdaq-database configurations and databases. These tools provide essential functionality for database operations, format conversions, and maintenance tasks.

## Module Location

**Path**: `/home/user/artdaq-database/artdaq-database/Utilities/`

## Available Utilities

### Configuration Database Tools

#### conftool
**Purpose**: Command-line interface to the configuration database

**Key Features**:
- Read/write documents and configurations
- Direct database operations
- Testing and debugging interface

**Documentation**: [conftool.md](./conftool.md)

---

### Bulk Operations

#### bulkloader
**Purpose**: High-performance bulk import of FHiCL files to database

**Key Features**:
- Multi-threaded file loading
- Configurable thread count
- Performance statistics
- Optional verification mode

**Documentation**: [bulkloader.md](./bulkloader.md)

**Typical Use**: Initial database population, configuration migration

#### bulkdownloader
**Purpose**: High-performance bulk export of database configurations to files

**Key Features**:
- Multi-threaded document retrieval
- Automatic directory structure creation
- Configuration composition-based export
- Performance statistics

**Documentation**: [bulkdownloader.md](./bulkdownloader.md)

**Typical Use**: Database backup, configuration export, reverse migration

---

### Database Maintenance

#### migrate_database
**Purpose**: Migrate database to new format with schema updates

**Key Features**:
- Document format migration
- JSONDocumentMigrator integration
- Non-destructive (creates new database)
- Collection-by-collection processing

**Documentation**: [migrate_database.md](./migrate_database.md)

**Typical Use**: Schema upgrades, format migrations

#### rebuild_database_index
**Purpose**: Rebuild search indexes for filesystem databases

**Key Features**:
- Creates fresh indexes from documents
- Fixes corrupted indexes
- Non-destructive (creates new database)
- Auto-rebuild mode

**Documentation**: [rebuild_database_index.md](./rebuild_database_index.md)

**Typical Use**: Index corruption recovery, index format upgrades

---

### Format Conversion Tools

#### fhicl2json
**Purpose**: Convert FHiCL configuration files to JSON format

**Key Features**:
- Full FHiCL syntax support
- Subtree extraction (--main flag)
- Include file resolution
- Clean JSON output to stdout

**Documentation**: [fhicl2json.md](./fhicl2json.md)

**Typical Use**: Format conversion, database preparation, validation

#### readfhicl
**Purpose**: Read and display FHiCL files with various processing options

**Key Features**:
- Display with/without prolog
- Full resolution or snippet mode
- Prune nil values
- Formatted output

**Documentation**: [readfhicl.md](./readfhicl.md)

**Typical Use**: Configuration inspection, debugging, validation

#### refactorfhicl
**Purpose**: Refactor FHiCL files by extracting tables (experimental)

**Key Features**:
- Automatic table extraction
- Version-based deduplication
- Include directive generation
- Modularization support

**Status**: Experimental/incomplete

**Documentation**: [refactorfhicl.md](./refactorfhicl.md)

**Note**: Contains incomplete code; review before use

---

### Testing and Validation

#### readjson
**Purpose**: JSON parser/serializer performance testing

**Key Features**:
- 100-iteration reliability test
- Performance benchmarking
- Round-trip validation
- Progress indication

**Documentation**: [readjson.md](./readjson.md)

**Typical Use**: Performance testing, parser validation

#### fixtestjson
**Purpose**: Convert JSON values to strings for schema compatibility

**Key Features**:
- Recursive type conversion
- In-place file modification
- Numbers/booleans → strings
- Document.data migration

**Documentation**: [fixtestjson.md](./fixtestjson.md)

**Warning**: Overwrites original files

**Typical Use**: Test data preparation, schema migration

---

## Utility Categories

### By Function

| Category | Utilities |
|----------|-----------|
| **Database Operations** | conftool |
| **Bulk Transfer** | bulkloader, bulkdownloader |
| **Maintenance** | migrate_database, rebuild_database_index |
| **Format Conversion** | fhicl2json, readfhicl, refactorfhicl |
| **Testing/Validation** | readjson, fixtestjson |

### By Database Interaction

| Interaction Level | Utilities |
|-------------------|-----------|
| **Direct DB Access** | conftool, bulkloader, bulkdownloader, migrate_database, rebuild_database_index |
| **File Operations** | fhicl2json, readfhicl, refactorfhicl, readjson, fixtestjson |

### By Performance Characteristics

| Performance | Utilities |
|-------------|-----------|
| **Multi-threaded** | bulkloader, bulkdownloader |
| **Single-threaded** | conftool, migrate_database, rebuild_database_index, fhicl2json, readfhicl, refactorfhicl, readjson, fixtestjson |

---

## Common Workflows

### Initial Database Setup

```bash
# 1. Prepare configurations (if needed)
for fcl in *.fcl; do
  fhicl2json -c "$fcl" > "${fcl%.fcl}.json"
done

# 2. Bulk load into database
bulkloader -p /configs -c InitialConfig -r 1 -t 8

# 3. Verify
bulkdownloader -p /verify -c InitialConfig -r 1
diff -r /configs /verify
```

### Database Migration

```bash
# 1. Backup original
cp -r /db/original /db/backup

# 2. Migrate schema
migrate_database -u filesystemdb:///db/original

# 3. Rebuild indexes
rebuild_database_index -u filesystemdb:///db/original_migrated

# 4. Test new database
# ... perform tests ...

# 5. Deploy if successful
mv /db/original /db/old
mv /db/original_migrated_new /db/original
```

### Configuration Backup

```bash
# Export all configurations
for config in $(list_configurations); do
  bulkdownloader -p "/backup/$(date +%Y%m%d)/$config" \
                 -c "$config" \
                 -r latest
done
```

### Format Conversion Pipeline

```bash
# FHiCL → JSON → Database
fhicl2json -c config.fcl | \
  fixtestjson -c /dev/stdin | \
  conftool --import
```

### Configuration Validation

```bash
# Validate FHiCL syntax
readfhicl -c config.fcl -r > /dev/null && echo "Valid"

# Validate JSON structure
readjson -c document.json > /dev/null && echo "Valid"

# Test database round-trip
bulkloader -p /tmp/test -c TestConfig -r 999
bulkdownloader -p /tmp/verify -c TestConfig -r 999
diff -r /tmp/test /tmp/verify
```

---

## Dependencies

### Common Dependencies

All utilities depend on:
- Boost.Program_options (command-line parsing)
- artdaq-database core libraries
- Standard C++ library

### Specific Dependencies

| Utility | Special Dependencies |
|---------|---------------------|
| **bulkloader, bulkdownloader** | Boost.Filesystem, C++ threading |
| **migrate_database, rebuild_database_index** | Boost.Filesystem, JSON document classes |
| **fhicl2json, readfhicl, refactorfhicl** | fhiclcpp, cetlib |
| **readjson, fixtestjson** | JSON library, Boost.Variant |

---

## Environment Variables

### FHICL_FILE_PATH

Used by FHiCL utilities:
- **Utilities**: fhicl2json, readfhicl, refactorfhicl
- **Purpose**: Search path for `#include` files
- **Format**: Colon-separated directory list
- **Default**: Current directory (`.`)

**Example**:
```bash
export FHICL_FILE_PATH="/configs:/configs/common:/configs/prolog"
```

---

## Exit Codes

### Standard Exit Codes

All utilities use consistent exit codes from `process_exit_codes.h`:

| Code | Name | Meaning |
|------|------|---------|
| 0 | SUCCESS | Operation completed successfully |
| 1 | HELP | Help message displayed |
| 2+ | INVALID_ARGUMENT | Missing or invalid arguments |
| Variable | FAILURE | Operation failed |
| Variable | UNCAUGHT_EXCEPTION | Unhandled exception |

---

## Performance Characteristics

### Multi-threaded Utilities

**bulkloader** and **bulkdownloader**:
- Default threads: Hardware concurrency
- Configurable via `-t` flag
- Near-linear speedup up to CPU core count
- Network databases benefit from higher thread counts

### Single-threaded Utilities

**migrate_database** and **rebuild_database_index**:
- Sequential processing ensures consistency
- Simpler error handling
- Suitable for operations requiring order

### File-based Utilities

**Format converters** (fhicl2json, readfhicl, etc.):
- Fast for typical config file sizes
- Memory-bound for large files
- I/O-bound for many files

---

## Safety Considerations

### Destructive Operations

**Utilities that modify files in-place**:
- **fixtestjson**: Overwrites original JSON file

**Recommendation**: Always create backups before running

### Non-Destructive Operations

**Utilities that create new outputs**:
- **migrate_database**: Creates `{uri}_migrated`
- **rebuild_database_index**: Creates `{uri}_new`
- **bulkdownloader**: Creates new directory
- **fhicl2json, readfhicl**: Output to stdout

---

## Troubleshooting

### Common Issues

#### Command Not Found
```bash
# Add to PATH or use full path
export PATH="/path/to/artdaq-database/build/bin:$PATH"
```

#### Permission Denied
```bash
# Check executable permissions
ls -l bulkloader
chmod +x bulkloader
```

#### Database URI Issues
```bash
# Ensure correct format
filesystemdb:///absolute/path/to/database  # Correct
filesystemdb://relative/path               # May fail
```

#### Missing Dependencies
```bash
# Set library path
export LD_LIBRARY_PATH="/path/to/artdaq-database/lib:$LD_LIBRARY_PATH"
```

#### FHICL Include Errors
```bash
# Set include path
export FHICL_FILE_PATH="/configs:/common:."
```

---

## Development Notes

### Building Utilities

Utilities are built as part of the main artdaq-database build:
```bash
cd /path/to/artdaq-database
mkdir build && cd build
cmake ..
make
# Binaries in: build/bin/
```

### Testing Utilities

```bash
# Run individual utility tests
./bulkloader -h
./fhicl2json -h

# Integration tests
cd artdaq-database/test
./run_tests.sh
```

### Adding New Utilities

To add a new utility:
1. Create source file in `artdaq-database/Utilities/`
2. Add to `CMakeLists.txt`
3. Document in this directory
4. Add to README.md
5. Create unit tests

---

## Additional Resources

### Source Code
- **Location**: `/home/user/artdaq-database/artdaq-database/Utilities/`
- **Build System**: CMakeLists.txt

### Related Documentation
- **Python wrapper**: conftool.py (see SWIGBindings documentation)
- **Core libraries**: See ConfigurationDB module documentation
- **Database providers**: See StorageProviders documentation

### External Documentation
- **FHiCL Language**: https://cdcvs.fnal.gov/redmine/projects/fhicl-cpp
- **artdaq**: https://cdcvs.fnal.gov/redmine/projects/artdaq
- **Boost**: https://www.boost.org/doc/

---

## Quick Reference

### Most Common Commands

```bash
# Bulk load configurations
bulkloader -p /configs -c MyConfig -r 1001

# Bulk export configurations
bulkdownloader -p /export -c MyConfig -r 1001

# Convert FHiCL to JSON
fhicl2json -c config.fcl > config.json

# Rebuild database indexes
rebuild_database_index -u filesystemdb:///var/lib/artdaq/db

# Migrate database schema
migrate_database -u filesystemdb:///var/lib/artdaq/db

# View FHiCL resolved
readfhicl -c config.fcl -r

# Direct database operation
conftool --operation readdocument --entity MyEntity
```

### Getting Help

All utilities support `--help`:
```bash
bulkloader --help
fhicl2json --help
migrate_database --help
```

---

## Maintenance Status

| Utility | Status | Notes |
|---------|--------|-------|
| conftool | Stable | Production-ready |
| bulkloader | Stable | Production-ready |
| bulkdownloader | Stable | Production-ready |
| migrate_database | Stable | Production-ready |
| rebuild_database_index | Stable | Production-ready |
| fhicl2json | Stable | Production-ready |
| readfhicl | Stable | Production-ready |
| readjson | Stable | Testing tool |
| fixtestjson | Stable | Use with caution (destructive) |
| refactorfhicl | Experimental | Incomplete implementation |

---

## Version History

See individual utility documentation for detailed change history and implementation notes.

---

## Contact and Support

For questions, issues, or contributions related to the Utilities module, please refer to the main artdaq-database project documentation and issue tracking system.
