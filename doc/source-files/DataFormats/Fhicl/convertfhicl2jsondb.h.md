# convertfhicl2jsondb.h

## File Overview

Provides bidirectional conversion between FHiCL documents and database JSON format, preserving metadata (comments, annotations).

**Location**: `/home/user/artdaq-database/artdaq-database/DataFormats/Fhicl/convertfhicl2jsondb.h`

## Conversion Functions

```cpp
bool fhicl_to_jsondb(std::string const& fhicl, std::string& json_db);
bool jsondb_to_fhicl(std::string const& json_db, std::string& fhicl);
```

### fhicl_to_jsondb

Converts FHiCL document to database JSON format.

**Features**:
- Preserves comments as metadata
- Preserves annotations
- Maintains structure

### jsondb_to_fhicl

Converts database JSON to FHiCL document.

**Features**:
- Restores comments from metadata
- Restores annotations
- Generates valid FHiCL

## Purpose

Enables storage of FHiCL configurations in database while preserving all metadata.

## Related Files

- **convertfhicl2jsondb.cpp** - Implementation
- **fhicl_reader/writer.h** - FHiCL I/O
- **Json/convertjson2guijson.h** - Metadata handling
