# provider_mongodb.h

## File Overview

This header defines the MongoDB storage provider that interfaces with MongoDB database servers using the mongocxx C++ driver. It provides production-grade, scalable document storage with full MongoDB query capabilities.

**Location**: `/home/user/artdaq-database/artdaq-database/StorageProviders/MongoDB/provider_mongodb.h`

**Purpose**: MongoDB database provider interface

## Dependencies

- `<mongocxx/client.hpp>` - MongoDB C++ driver client
- `<mongocxx/instance.hpp>` - MongoDB driver instance

## Namespace: artdaq::database::mongo

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

**Purpose**: MongoDB connection configuration.

**URI Format**: `mongodb://[username:password@]host:port/database`

**Examples**:
- `mongodb://localhost:27017/artdaq_db`
- `mongodb://user:pass@dbserver.fnal.gov:27017/production_db`

### MongoDB Class
```cpp
class MongoDB final {
public:
    static std::shared_ptr<MongoDB> create(DBConfig const& config);
    explicit MongoDB(DBConfig config, PassKeyIdiom const&);
    mongocxx::database& connection();
    mongocxx::cursor list_databases();
private:
    DBConfig _config;
    mongocxx::instance& _instance;
    mongocxx::client _client;
    mongocxx::database _connection;
};
```

**Purpose**: MongoDB database connection manager.

**Key Methods**:
- `create()` - Factory method
- `connection()` - Get database connection
- `list_databases()` - List available databases

### Type Aliases
```cpp
template <typename TYPE>
using MongoDBProvider = StorageProvider<TYPE, MongoDB>;
```

**Usage**: `MongoDBProvider<JSONDocument>`

## Literal Constants

```cpp
namespace literal {
    constexpr auto MONGOURI = "mongodb://";
    constexpr auto hostname = "127.0.0.1";
    constexpr auto port = 27017;
    constexpr auto db_name = "test_configuration_db";
}
```

## Debug Functions

```cpp
namespace debug {
    void enable();       // Enable all debugging
    void connection();   // Enable connection debugging
    void ReadWrite();    // Enable I/O debugging
}
```

## Usage Example

```cpp
#include "artdaq-database/StorageProviders/MongoDB/provider_mongodb.h"

using namespace artdaq::database::mongo;

// Configure MongoDB
DBConfig config("mongodb://localhost:27017/artdaq_configs");

// Create connection
auto db = MongoDB::create(config);

// Create provider
auto provider = MongoDBProvider<JSONDocument>::create(db);

// Use provider for queries
auto results = provider->readDocument(query);
```

## Features

- **Scalability**: Handles millions of documents
- **Performance**: Indexed queries, aggregation pipeline
- **Reliability**: Replication, failover support
- **Query Power**: Full MongoDB query language
- **Transactions**: ACID transactions (MongoDB 4.0+)

---

**Documentation generated for artdaq-database MongoDB provider**
