# shared_datatypes.h

## File Overview

This header file defines common type aliases used throughout the artdaq-database project. It establishes consistent naming for fundamental data types, improving code readability and providing a single point to change underlying type implementations if needed.

**Location**: `/home/user/artdaq-database/artdaq-database/SharedCommon/shared_datatypes.h`

## Dependencies

### Standard Library
- `<string>` - String class
- `<utility>` - std::pair

## Header Guard

```cpp
#ifndef _ARTDAQ_DATABASE_SHARED_DATATYPES_H_
#define _ARTDAQ_DATABASE_SHARED_DATATYPES_H_
```

## Namespace: artdaq::database

All type aliases are defined in the `artdaq::database` namespace.

## Type Aliases

### path_t

```cpp
using path_t = std::string;
```

**Purpose**: Represents a filesystem path or URL path.

**Underlying Type**: std::string

**Usage Context**:
- File paths: `/path/to/config.json`
- Directory paths: `/data/configurations/`
- URL paths: May be used for URI paths in some contexts

**Usage Example**:
```cpp
path_t config_path = "/etc/artdaq/database/config.json";
path_t data_dir = "/var/lib/artdaq/database/";

bool file_exists(path_t const& path) {
    // ... implementation ...
}
```

**Design Rationale**:
- Semantic clarity: path_t clearly indicates "this string is a path"
- Future flexibility: Could change to std::filesystem::path without changing signatures
- Type safety: Functions expecting paths are self-documenting

---

### string_pair_t

```cpp
using string_pair_t = std::pair<std::string, std::string>;
```

**Purpose**: Represents a pair of related strings (key-value, name-value, etc.).

**Underlying Type**: std::pair<std::string, std::string>

**Common Uses**:
- Key-value pairs
- Name-value associations
- Attribute-value pairs
- Before-after comparisons

**Usage Example**:
```cpp
// Configuration option
string_pair_t option{"database_provider", "mongodb"};

// Using in a function
string_pair_t parse_assignment(std::string const& input) {
    // Parse "key=value" format
    auto pos = input.find('=');
    return {input.substr(0, pos), input.substr(pos + 1)};
}

// Accessing elements
std::cout << "Key: " << option.first << std::endl;
std::cout << "Value: " << option.second << std::endl;
```

**Design Rationale**:
- Common pattern deserves a named type
- More readable than bare std::pair
- Could be replaced with custom struct without changing much code
- Interoperable with std::map, std::unordered_map

---

### object_id_t

```cpp
using object_id_t = std::string;
```

**Purpose**: Represents a unique object identifier (typically MongoDB ObjectId or UUID).

**Underlying Type**: std::string

**Format Examples**:
- MongoDB ObjectId: `"507f1f77bcf86cd799439011"` (24 hex characters)
- UUID: `"550e8400-e29b-41d4-a716-446655440000"` (with or without hyphens)

**Usage Context**:
- Document IDs in database operations
- Unique identifiers for configurations
- Version identifiers
- Entity identifiers

**Usage Example**:
```cpp
object_id_t generate_new_id() {
    // ... generate unique ID ...
    return "507f1f77bcf86cd799439011";
}

object_id_t doc_id = generate_new_id();

bool delete_document(object_id_t const& id) {
    // ... delete document with given ID ...
}

// Extracting from JSON
object_id_t extract_id(std::string const& json) {
    // Parse JSON and extract ID field
}
```

**Design Rationale**:
- String representation is universal and serializable
- Compatible with multiple ID formats (MongoDB, UUID, custom)
- Easy to log and debug
- Could be replaced with custom ID class if needed

---

### timestamp_t

```cpp
using timestamp_t = std::string;
```

**Purpose**: Represents a timestamp as a string (typically ISO 8601 format).

**Underlying Type**: std::string

**Format**: ISO 8601 with milliseconds: `"2017-07-18T12:48:10.123-0500"`

**Components**:
- Date: `YYYY-MM-DD`
- Time: `HH:MM:SS.mmm`
- Timezone: `±HHMM`

**Usage Context**:
- Document creation/modification times
- Operation timestamps
- Version timestamps
- Log entry timestamps

**Usage Example**:
```cpp
timestamp_t current_time() {
    // Returns current time in ISO 8601 format
    return "2025-11-13T10:30:45.123-0600";
}

timestamp_t created_at = current_time();
timestamp_t modified_at = current_time();

struct Document {
    object_id_t id;
    timestamp_t created;
    timestamp_t modified;
    std::string content;
};
```

**Design Rationale**:
- String format is portable and human-readable
- ISO 8601 is standard and sortable
- Easy to serialize to JSON/XML
- Compatible with database timestamp formats
- Could be replaced with std::chrono::time_point if needed

---

## Usage Patterns

### Function Signatures

These type aliases make function signatures more self-documenting:

```cpp
// Clear intent - expects a path
bool load_config(path_t const& config_path);

// Clear intent - returns an object ID
object_id_t create_document(std::string const& content);

// Clear intent - expects timestamp
bool is_expired(timestamp_t const& expiry_time);

// Clear intent - returns key-value pair
string_pair_t parse_option(std::string const& option_string);
```

Compare with using raw types:
```cpp
// Less clear - what kind of string?
bool load_config(std::string const& config_path);

// Less clear - what is the returned string?
std::string create_document(std::string const& content);
```

### Structured Data

```cpp
struct ConfigurationMetadata {
    object_id_t id;
    std::string name;
    timestamp_t created;
    timestamp_t modified;
    path_t storage_path;
    std::vector<string_pair_t> attributes;
};
```

### Collections

```cpp
std::vector<object_id_t> document_ids;
std::map<object_id_t, timestamp_t> document_timestamps;
std::vector<string_pair_t> configuration_options;
std::set<path_t> search_paths;
```

### Parameter Objects

```cpp
struct DatabaseQuery {
    object_id_t collection_id;
    timestamp_t start_time;
    timestamp_t end_time;
    std::vector<string_pair_t> filters;
};
```

## Design Philosophy

### Semantic Types

These aliases create **semantic types** - the underlying type is less important than the semantic meaning:

- `path_t` means "this is a path" (not just any string)
- `object_id_t` means "this is an identifier" (not just any string)
- `timestamp_t` means "this is a timestamp" (not just any string)
- `string_pair_t` means "these strings are related" (not just any pair)

### Benefits

1. **Self-Documentation**: Code is more readable
   ```cpp
   void process(path_t path);           // Clear: expects a path
   void process(std::string path);      // Unclear: what kind of string?
   ```

2. **Refactoring**: Can change underlying type in one place
   ```cpp
   // Today: using path_t = std::string;
   // Tomorrow: using path_t = std::filesystem::path;
   // All code using path_t automatically updated
   ```

3. **Type Safety**: Catch errors at compile time (with careful design)
   ```cpp
   void process_id(object_id_t id);
   void process_timestamp(timestamp_t ts);

   object_id_t id = "507f1f77bcf86cd799439011";
   timestamp_t ts = "2025-11-13T10:30:45.123-0600";

   // These work
   process_id(id);
   process_timestamp(ts);

   // These would work too (same underlying type) but semantically wrong
   // Strong typing would prevent this
   process_id(ts);  // Compiles but semantically wrong
   ```

4. **Consistency**: Ensures same type used everywhere for same purpose
   ```cpp
   // All functions use same type for paths
   path_t get_config_path();
   bool validate_path(path_t const&);
   void set_path(path_t const&);
   ```

### Limitations

Since these are type aliases (not strong types), they don't provide actual type safety:

```cpp
object_id_t id = "123";
path_t path = "123";
// These are interchangeable (both std::string)
id = path;  // Compiles, but semantically wrong
```

To get true type safety, would need strong typedef or custom classes:
```cpp
struct object_id_t {
    std::string value;
    explicit object_id_t(std::string s) : value(std::move(s)) {}
};
// Now id = path; would be a compile error
```

## Migration Path

If stronger typing is needed in the future:

### Step 1: Define Strong Types
```cpp
class ObjectId {
    std::string value_;
public:
    explicit ObjectId(std::string s) : value_(std::move(s)) {}
    std::string const& str() const { return value_; }
    // ... validation, comparison operators, etc. ...
};

using object_id_t = ObjectId;  // Update alias
```

### Step 2: Update Code Gradually
```cpp
// Old code (still works with string)
object_id_t id = "123";  // Now requires explicit ObjectId("123")

// New code
object_id_t id = ObjectId("123");
std::string str = id.str();
```

The type alias provides the flexibility for this migration.

## Comparison with Alternatives

### Raw Types
```cpp
std::string path;           // What kind of string?
std::string id;             // What kind of string?
std::pair<std::string, std::string> kv;  // What's the relationship?
```
**Pros**: Simple, no learning curve
**Cons**: No semantic meaning, harder to refactor

### Strong Typedefs (C++11)
```cpp
enum class path_tag {};
using path_t = strong_typedef<std::string, path_tag>;
```
**Pros**: Type safety, prevents mixing
**Cons**: More complex, harder to use with existing APIs

### Custom Classes
```cpp
class Path {
    std::string value_;
public:
    explicit Path(std::string s);
    // ... methods ...
};
```
**Pros**: Maximum control, validation
**Cons**: Most complex, requires lots of boilerplate

### Type Aliases (Current Approach)
```cpp
using path_t = std::string;
```
**Pros**: Simple, flexible, good documentation
**Cons**: No actual type safety

**Decision**: Type aliases strike the best balance for artdaq-database needs.

## Related Files

- **helper_functions.h** - Uses object_id_t and timestamp_t extensively
- **fileststem_functions.h** - Uses path_t for file operations
- **returned_result.h** - Could use string_pair_t for key-value results
- All database operation files - Use these types in their interfaces

## Best Practices

1. **Use Semantic Types in Signatures**: Makes intent clear
   ```cpp
   void store(object_id_t id, path_t path, timestamp_t time);
   ```

2. **Use Appropriate Conversions**: Be explicit when converting
   ```cpp
   path_t path = get_path();
   std::string str = path;  // Implicit conversion OK
   ```

3. **Document Formats**: Especially for string-based types
   ```cpp
   /// @brief Get document creation time
   /// @return ISO 8601 timestamp: "YYYY-MM-DDTHH:MM:SS.mmm±ZZZZ"
   timestamp_t get_created_time();
   ```

4. **Validate Input**: Even though types are aliases, validate values
   ```cpp
   bool is_valid_object_id(object_id_t const& id) {
       return id.length() == 24 && std::all_of(id.begin(), id.end(), ::isxdigit);
   }
   ```

5. **Consider Strong Types**: For critical code paths or when type safety is paramount
   ```cpp
   // If needed in future:
   class ObjectId { /* ... */ };
   using object_id_t = ObjectId;
   ```

## Notes

- All types are string-based for simplicity and serialization compatibility
- ISO 8601 timestamp format is assumed throughout the codebase
- Object IDs are typically 24-character hex strings (MongoDB format)
- The aliases provide flexibility for future refactoring without breaking existing code
- Consider adding validation functions for each type
- string_pair_t is particularly useful with std::map and configuration parsing
