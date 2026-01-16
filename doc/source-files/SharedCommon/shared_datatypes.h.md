# shared_datatypes.h

**Path:** `artdaq-database/SharedCommon/shared_datatypes.h`

**Purpose:** Defines semantic type aliases used throughout the artdaq-database project. Instead of using raw `std::string` everywhere, this file provides meaningful type names like `object_id_t` and `timestamp_t` that make code self-documenting. When you see `object_id_t`, you immediately know it represents a unique identifier, not just any string.


## Key Concepts

### Type Aliases

A *type alias* creates a new name for an existing type using the `using` keyword. This does not create a new type at the language level (the compiler still sees `std::string`), but makes code more readable and maintainable:

```cpp
// Without type aliases - unclear what each string represents
void saveDocument(std::string id, std::string time, std::string location);

// With type aliases - intent is clear
void saveDocument(object_id_t id, timestamp_t time, path_t location);
```

### Why Use Semantic Types?

1. **Self-documentation**: Code reads like documentation; function signatures clearly indicate intent
2. **Refactoring safety**: Change the underlying type in one place if needed in the future
3. **IDE support**: Better autocomplete suggestions and type hints
4. **Code review**: Easier to spot type mismatches and logic errors

### Important Limitation

These are *aliases*, not distinct types. The compiler will not prevent mixing them:

```cpp
object_id_t id = "507f1f77bcf86cd799439011";
timestamp_t time = id;  // Compiles! Semantically wrong but no compiler error
```

The type names are for human readers, not compiler enforcement. Be careful to use the correct type in the correct context.

## Thread Safety

- **Thread-safe:** Yes (for reads)
- **Concurrent access:** All types inherit `std::string`'s thread-safety guarantees: safe for concurrent reads, but concurrent writes to the same object require external synchronization
- **Locking:** No internal locking; use external synchronization for concurrent modifications

## Dependencies

| Include | Purpose |
|---------|---------|
| `<string>` | Provides `std::string` base type for most aliases |
| `<utility>` | Provides `std::pair` for `string_pair_t` |

## Type Aliases

### Summary Table

| Alias | Underlying Type | Purpose |
|-------|-----------------|---------|
| `path_t` | `std::string` | Filesystem paths, directory names, URIs |
| `string_pair_t` | `std::pair<std::string, std::string>` | Key-value pairs, mappings |
| `object_id_t` | `std::string` | Document IDs, unique identifiers (typically 24-char hex) |
| `timestamp_t` | `std::string` | ISO 8601 formatted timestamps |

---

### `path_t`

```cpp
using path_t = std::string;
```

**Brief:** Represents filesystem paths, directory names, and URI paths. Use this type for any string that refers to a location in the filesystem or a network resource.

**Thread Safety:** Thread-safe for concurrent reads; concurrent writes require external synchronization

**Common Uses:**
- File locations on disk
- Directory paths
- MongoDB connection URIs
- Configuration file paths

**Example:**
```cpp
#include "artdaq-database/SharedCommon/shared_datatypes.h"
#include "artdaq-database/SharedCommon/shared_exceptions.h"
#include <iostream>
#include <fstream>

namespace db = artdaq::database;

bool loadConfiguration(db::path_t const& config_path) {
    if (config_path.empty()) {
        throw db::invalid_argument("loadConfiguration")
            << "Configuration path cannot be empty";
    }

    std::ifstream file(config_path);
    if (!file) {
        throw db::runtime_error("loadConfiguration")
            << "Failed to open file: " << config_path;
    }

    std::cout << "Loading configuration from: " << config_path << std::endl;
    // ... load logic
    return true;
}

int main() {
    try {
        db::path_t config_file = "/etc/artdaq/config.json";
        db::path_t database_uri = "mongodb://localhost:27017/test_db";
        db::path_t export_dir = "/tmp/exports/";

        loadConfiguration(config_file);
        return 0;
    } catch (db::exception const& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
```

---

### `string_pair_t`

```cpp
using string_pair_t = std::pair<std::string, std::string>;
```

**Brief:** Represents a pair of related strings, typically key-value pairs or mappings. Use this type when you need to associate two string values together.

**Thread Safety:** Thread-safe for concurrent reads; concurrent writes require external synchronization

**Common Uses:**
- Configuration key-value pairs
- Environment variables
- HTTP headers
- Query parameters

**Example:**
```cpp
#include "artdaq-database/SharedCommon/shared_datatypes.h"
#include <vector>
#include <iostream>

namespace db = artdaq::database;

void processOptions(std::vector<db::string_pair_t> const& options) {
    if (options.empty()) {
        std::cout << "No options provided" << std::endl;
        return;
    }

    for (auto const& [key, value] : options) {  // C++17 structured binding
        std::cout << key << " = " << value << std::endl;
    }
}

int main() {
    // Single pair
    db::string_pair_t config_entry = {"hostname", "localhost"};
    std::cout << "Key: " << config_entry.first
              << ", Value: " << config_entry.second << std::endl;

    // Collection of pairs
    std::vector<db::string_pair_t> options;
    options.push_back({"port", "27017"});
    options.push_back({"database", "test_db"});
    options.push_back({"authSource", "admin"});

    processOptions(options);

    return 0;
}
```

---

### `object_id_t`

```cpp
using object_id_t = std::string;
```

**Brief:** Represents unique object identifiers, similar to MongoDB's ObjectId. Typically 24-character hexadecimal strings generated by `generate_oid()`. Use this type for any string that uniquely identifies a document in the database.

**Thread Safety:** Thread-safe for concurrent reads; concurrent writes require external synchronization

**Format:** 24-character hexadecimal string (e.g., `"507f1f77bcf86cd799439011"`)

**Common Uses:**
- Document unique identifiers in the database
- Cross-reference IDs between related documents
- Tracking document lineage

**Example:**
```cpp
#include "artdaq-database/SharedCommon/shared_datatypes.h"
#include "artdaq-database/SharedCommon/shared_exceptions.h"
#include <iostream>
#include <map>

namespace db = artdaq::database;

// Forward declaration - actual implementation in helper_functions.h
db::object_id_t generate_oid();

struct Document {
    db::object_id_t id;
    std::string name;
    std::string content;
};

void processDocument(db::object_id_t const& id) {
    if (id.empty()) {
        throw db::invalid_argument("processDocument")
            << "Document ID cannot be empty";
    }
    if (id.size() != 24) {
        throw db::invalid_argument("processDocument")
            << "Invalid document ID format: expected 24 characters, got "
            << id.size();
    }
    // ... process document
}

int main() {
    try {
        // Create a document with an ID
        Document doc;
        doc.id = "507f1f77bcf86cd799439011";
        doc.name = "detector_config";
        doc.content = R"({"threshold": 100})";

        // Validate and process
        processDocument(doc.id);

        // Store documents by ID
        std::map<db::object_id_t, Document> documents;
        documents[doc.id] = doc;

        // Retrieve by ID
        if (documents.count(doc.id) > 0) {
            std::cout << "Found document: " << documents[doc.id].name << std::endl;
        }

        return 0;
    } catch (db::exception const& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
```

---

### `timestamp_t`

```cpp
using timestamp_t = std::string;
```

**Brief:** Represents ISO 8601 formatted timestamps with timezone information. Use this type for any string that represents a point in time.

**Thread Safety:** Thread-safe for concurrent reads; concurrent writes require external synchronization

**Format:** `YYYY-MM-DDTHH:MM:SS.sss+HHMM` (e.g., `"2024-01-15T14:30:25.123-0500"`)

**Common Uses:**
- Document creation timestamps
- Modification timestamps
- Audit trails
- Version history

**Example:**
```cpp
#include "artdaq-database/SharedCommon/shared_datatypes.h"
#include "artdaq-database/SharedCommon/shared_exceptions.h"
#include <iostream>

namespace db = artdaq::database;

// Forward declaration - actual implementation in helper_functions.h
db::timestamp_t timestamp();

struct AuditRecord {
    db::object_id_t document_id;
    std::string action;           // "created", "modified", "deleted"
    std::string user;
    db::timestamp_t when;
};

struct VersionedDocument {
    db::object_id_t id;
    std::string name;
    std::string version;
    db::timestamp_t created;
    db::timestamp_t modified;
};

void validateTimestamp(db::timestamp_t const& ts) {
    if (ts.empty()) {
        throw db::invalid_argument("validateTimestamp")
            << "Timestamp cannot be empty";
    }
    // Basic format validation (ISO 8601)
    if (ts.size() < 19) {
        throw db::invalid_argument("validateTimestamp")
            << "Invalid timestamp format: " << ts;
    }
}

int main() {
    try {
        VersionedDocument doc;
        doc.id = "507f1f77bcf86cd799439011";
        doc.name = "detector_config";
        doc.version = "v1.0";
        doc.created = "2024-01-15T14:30:25.123-0500";
        doc.modified = "2024-01-16T09:15:00.000-0500";

        validateTimestamp(doc.created);
        validateTimestamp(doc.modified);

        std::cout << "Document: " << doc.name << std::endl;
        std::cout << "Created: " << doc.created << std::endl;
        std::cout << "Modified: " << doc.modified << std::endl;

        return 0;
    } catch (db::exception const& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
```

## Relationship to Other Components

This file is a foundational header included by many SharedCommon files and throughout the project:

- **helper_functions.h** - Uses `object_id_t` and `timestamp_t` in function signatures for `generate_oid()` and `timestamp()`
- **fileststem_functions.h** - Uses `path_t` for file and directory operations
- **returned_result.h** - Uses these types in result messages
- **ConfigurationDB** - Uses all types extensively for document management
- **StorageProviders** - Uses `path_t` for connection URIs and `object_id_t` for document IDs

## Example

```cpp
#include "artdaq-database/SharedCommon/shared_datatypes.h"
#include "artdaq-database/SharedCommon/shared_exceptions.h"
#include <iostream>

namespace db = artdaq::database;

// A complete document structure using semantic types
struct ConfigurationDocument {
    db::object_id_t   id;           // Unique document identifier
    std::string       name;         // Human-readable name
    std::string       version;      // Semantic version string
    db::path_t        source_file;  // Where this configuration came from
    db::timestamp_t   created;      // When document was created
    db::timestamp_t   modified;     // Last modification time
};

// Function demonstrating type usage
void displayDocument(ConfigurationDocument const& doc) {
    std::cout << "Document ID: " << doc.id << std::endl;
    std::cout << "Name: " << doc.name << std::endl;
    std::cout << "Version: " << doc.version << std::endl;
    std::cout << "Source: " << doc.source_file << std::endl;
    std::cout << "Created: " << doc.created << std::endl;
    std::cout << "Modified: " << doc.modified << std::endl;
}

int main() {
    ConfigurationDocument doc;
    doc.id = "507f1f77bcf86cd799439011";
    doc.name = "detector_config";
    doc.version = "v1.0.0";
    doc.source_file = "/etc/artdaq/detector.fcl";
    doc.created = "2024-01-15T14:30:25.123-0500";
    doc.modified = "2024-01-15T14:30:25.123-0500";

    displayDocument(doc);
    return 0;
}
```

## Notes for Developers

### When to Use Each Type

| Type | Use For |
|------|---------|
| `path_t` | File paths, directory names, URIs, connection strings |
| `string_pair_t` | Key-value pairs, mappings, two related strings |
| `object_id_t` | Document IDs, unique identifiers, foreign keys |
| `timestamp_t` | Dates, times, audit trails, version timestamps |

### Common Pitfalls

- **Pitfall 1:** Mixing up type aliases since the compiler does not enforce type safety:
  ```cpp
  // BAD: Compiler will not catch this mistake
  db::object_id_t id = "2024-01-15T14:30:25.123-0500";  // Actually a timestamp!
  db::timestamp_t time = "507f1f77bcf86cd799439011";   // Actually an ID!

  // GOOD: Use the correct type for the correct purpose
  db::object_id_t id = "507f1f77bcf86cd799439011";
  db::timestamp_t time = "2024-01-15T14:30:25.123-0500";
  ```

- **Pitfall 2:** Forgetting that these are still strings and can be empty:
  ```cpp
  // GOOD: Always validate before use
  void processDocument(db::object_id_t const& id) {
      if (id.empty()) {
          throw db::invalid_argument("processDocument")
              << "Document ID cannot be empty";
      }
      // ... proceed
  }
  ```

- **Pitfall 3:** Assuming format validation is automatic:
  ```cpp
  // BAD: Assuming any string is a valid ID
  db::object_id_t id = "invalid";  // Compiles but not a valid ID

  // GOOD: Validate format when receiving external input
  void setDocumentId(db::object_id_t const& id) {
      if (id.size() != 24) {
          throw db::invalid_argument("setDocumentId")
              << "Invalid ID format: expected 24 hex characters";
      }
      // ... use id
  }
  ```

### Anti-patterns

```cpp
// DON'T use raw std::string when semantic types are available
void saveDocument(std::string id, std::string path);  // Unclear!

// DO use semantic types for clarity
void saveDocument(db::object_id_t id, db::path_t path);  // Clear intent

// DON'T mix up types even though compiler allows it
db::path_t p = doc.id;  // BAD: ID is not a path!

// DO use types consistently
db::path_t p = doc.source_file;  // GOOD: path to path
```

## See Also

- [helper_functions.h](./helper_functions.h.md) - Functions that generate and manipulate these types (`generate_oid()`, `timestamp()`)
- [returned_result.h](./returned_result.h.md) - Result type using string messages
- [configuraion_api_literals.h](./configuraion_api_literals.h.md) - String constants used with these types
