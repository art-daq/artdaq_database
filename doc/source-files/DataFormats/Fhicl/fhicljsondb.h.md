# fhicljsondb.h

## File Overview

High-level FHiCL configuration database interface functions.

**Location**: `/home/user/artdaq-database/artdaq-database/DataFormats/Fhicl/fhicljsondb.h`

## Functions

```cpp
bool fhicl_to_db(std::string const& fhicl, std::string& db_json);
bool db_to_fhicl(std::string const& db_json, std::string& fhicl);
```

Wrapper functions for database operations with FHiCL configurations.

## Related Files

- **fhicljsondb.cpp** - Implementation
- **convertfhicl2jsondb.h** - Core conversion logic
