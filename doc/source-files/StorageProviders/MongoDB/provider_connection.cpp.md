# provider_connection.cpp

## File Overview

Implements MongoDB connection management, including DBConfig and MongoDB class constructors.

**Location**: `/home/user/artdaq-database/artdaq-database/StorageProviders/MongoDB/provider_connection.cpp`

**Purpose**: MongoDB connection initialization and configuration

## Key Components

### getInstance()
```cpp
mongocxx::instance& getInstance();
```

**Purpose**: Singleton pattern for mongocxx::instance (required by driver).

**Thread-Safety**: Uses static initialization for thread-safe singleton.

### DBConfig Constructors

**Default Constructor**: Builds URI from environment or defaults to localhost:27017.

**URI Constructor**: Accepts full MongoDB connection string.

### MongoDB Constructor
```cpp
MongoDB::MongoDB(DBConfig config, PassKeyIdiom const&)
```

**Initialization**:
- Gets singleton mongocxx::instance
- Creates mongocxx::client with URI
- Opens database connection
- Creates SystemMetadata collection if needed

### connection()
```cpp
mongocxx::database& MongoDB::connection();
```

**Returns**: Reference to active database connection.

**First-Use**: Creates SystemMetadata collection and metadata document on first connection.

## Environment Variables

- **ARTDAQ_DATABASE_URI**: Override MongoDB connection URI
  ```bash
  export ARTDAQ_DATABASE_URI="mongodb://prodserver:27017/artdaq_prod"
  ```

---

**Documentation generated for artdaq-database MongoDB provider**
