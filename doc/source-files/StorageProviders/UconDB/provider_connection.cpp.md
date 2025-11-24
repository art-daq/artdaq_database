# provider_connection.cpp

## File Overview

Implements UConDB connection management and REST API client initialization.

**Location**: `/home/user/artdaq-database/artdaq-database/StorageProviders/UconDB/provider_connection.cpp`

**Purpose**: UConDB connection initialization

## Key Components

### DBConfig Constructors

**Default Constructor**: Builds URI from environment or defaults.

**URI Constructor**: Accepts UConDB REST API endpoint URI.

### UconDB Constructor
```cpp
UconDB::UconDB(DBConfig config, PassKeyIdiom const&)
```

**Initialization**:
- Parses connection URI
- Extracts hostname, port, database name
- Initializes HTTP client
- Validates connection

### connection()
```cpp
std::string& UconDB::connection();
```

**Returns**: Reference to connection string (API endpoint).

## URI Format

```
ucondb://http://hostname:port/database
ucondb://https://hostname:port/database
```

**Components**:
- Protocol: http or https
- Hostname: UConDB server address
- Port: API port (typically 8080)
- Database: Database name

## Environment Variables

- **ARTDAQ_DATABASE_URI**: Override UConDB connection URI
  ```bash
  export ARTDAQ_DATABASE_URI="ucondb://https://ucondb.fnal.gov:8080/production"
  ```

---

**Documentation generated for artdaq-database UConDB provider**
