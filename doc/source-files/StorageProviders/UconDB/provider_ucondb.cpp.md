# provider_ucondb.cpp

## File Overview

Implementation of UConDB storage provider query operations using REST API calls.

**Location**: `/home/user/artdaq-database/artdaq-database/StorageProviders/UconDB/provider_ucondb.cpp`

**Purpose**: UConDB provider query operation implementations

## Template Specializations

### findConfigurations
Queries UConDB for all configurations via REST API.

**Implementation**: HTTP GET to `/configurations` endpoint.

### configurationComposition
Retrieves entities in a configuration.

**Implementation**: HTTP GET with configuration filter.

### findVersions
Finds all versions via UConDB version API.

**Implementation**: HTTP GET to `/versions` endpoint.

### findEntities
Discovers entities via UConDB entity API.

**Implementation**: HTTP GET to `/entities` endpoint.

### listCollections
Lists collections from UConDB metadata.

### listDatabases
Lists available databases on UConDB server.

### databaseMetadata
Retrieves database metadata from UConDB.

---

**Documentation generated for artdaq-database UConDB provider**
