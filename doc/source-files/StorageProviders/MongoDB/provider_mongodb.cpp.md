# provider_mongodb.cpp

## File Overview

Implementation of MongoDB storage provider query operations. Implements template specializations for findConfigurations, findVersions, listCollections, and other database discovery operations using MongoDB C++ driver.

**Location**: `/home/user/artdaq-database/artdaq-database/StorageProviders/MongoDB/provider_mongodb.cpp`

**Purpose**: MongoDB provider query operation implementations

## Key Template Specializations

### findConfigurations
Queries MongoDB collections for all global configurations using aggregation pipeline.

**Implementation**: Uses MongoDB aggregation to group and sort configurations.

### configurationComposition
Retrieves entities that compose a configuration.

**Implementation**: Queries indexed fields for configuration membership.

### findVersions
Finds all versions of entities or configurations.

**Modes**:
- By entity name: Find all versions of an entity
- By configuration: Find all versions in a configuration

### findEntities
Discovers all entities across collections.

**Implementation**: Scans collections, aggregates unique entity names.

### listCollections
Lists all user collections in the database.

**Implementation**: Uses MongoDB `list_collections()` command, filters system collections.

### listDatabases
Lists all databases on MongoDB server.

**Implementation**: Uses MongoDB admin database `listDatabases` command.

### databaseMetadata
Retrieves SystemMetadata documents.

**Implementation**: Queries SystemMetadata collection directly.

---

**Documentation generated for artdaq-database MongoDB provider**
