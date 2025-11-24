# provider_ucondb.h

## File Overview

This header defines the UconDB storage provider that interfaces with the UConDB configuration database service via REST API. UConDB is Fermilab's Unified Configuration Database system for storing experiment configurations.

**Location**: `/home/user/artdaq-database/artdaq-database/StorageProviders/UconDB/provider_ucondb.h`

**Purpose**: UConDB REST API provider interface

## Namespace: artdaq::database::ucon

## Key Components

### DBConfig Struct
```cpp
struct DBConfig final {
    DBConfig();
    DBConfig(const std::string& uri_);
    std::string uri;
    const std::string connectionURI() const;
};
```

**Purpose**: UConDB connection configuration.

**URI Format**: `ucondb://http[s]://hostname:port/database`

**Examples**:
- `ucondb://http://ucondb-dev.fnal.gov:8080/artdaq_db`
- `ucondb://https://ucondb.fnal.gov/production_db`

### UconDB Class
```cpp
class UconDB final {
public:
    static std::shared_ptr<UconDB> create(DBConfig const& config);
    explicit UconDB(DBConfig config, PassKeyIdiom const&);
    std::string& connection();
private:
    DBConfig _config;
    std::string _connection;
};
```

**Purpose**: UConDB REST API connection manager.

**Implementation**: HTTP/HTTPS communication with UConDB service.

### Type Aliases
```cpp
template <typename TYPE>
using UconDBProvider = StorageProvider<TYPE, UconDB>;
```

**Usage**: `UconDBProvider<JSONDocument>`

## Literal Constants

```cpp
namespace literal {
    constexpr auto UCONDBURI = "ucondb://";
    constexpr auto db_name = "test_configuration_db";
}
```

## Debug Functions

```cpp
namespace debug {
    void enable();
    void ReadWrite();
}
```

## Features

- **REST API**: HTTP/HTTPS communication
- **Centralized**: Single source of truth for configurations
- **Versioned**: Full version history tracking
- **Multi-Experiment**: Shared service for multiple experiments
- **Web Interface**: GUI access to configurations

## Usage Example

```cpp
#include "artdaq-database/StorageProviders/UconDB/provider_ucondb.h"

using namespace artdaq::database::ucon;

// Configure UConDB
DBConfig config("ucondb://http://ucondb.fnal.gov:8080/artdaq_db");

// Create connection
auto db = UconDB::create(config);

// Create provider
auto provider = UconDBProvider<JSONDocument>::create(db);

// Use provider
auto results = provider->readDocument(query);
```

---

**Documentation generated for artdaq-database UConDB provider**
