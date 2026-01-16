# shared_types.h

**Path:** `artdaq-database/DataFormats/shared_types.h`

**Purpose:** Defines the foundational template-based type system for representing hierarchical data with optional metadata. This header provides building blocks for key-value pairs, collections, tables, and variant types that form the core data structures used by all DataFormats submodules (JSON, XML, FHiCL, Conf). The types support optional comments and annotations, enabling self-documenting configuration data.


## Key Concepts

### Metadata-Enriched Types

The types in this header support attaching metadata (comments, annotations) to both keys and values. This is essential for configuration formats like FHiCL that allow inline comments, enabling round-trip preservation of documentation.

### Ordered Collections

Unlike `std::map`, the `table_of` type maintains insertion order and allows duplicate keys. This is necessary for configuration formats where order matters or duplicate keys are valid.

### Recursive Variant Types

The `variant_value_of` type uses `boost::recursive_wrapper` to allow values that can contain nested tables or arrays, enabling representation of arbitrarily nested data structures.

### List-Based Storage

The `vector_of` type uses `std::list` internally (despite its name) to provide iterator stability during insertions and deletions.

## Thread Safety

- **Thread-safe:** No
- **Concurrent access:** All types are value types with no internal synchronization. External locking is required for concurrent access to shared instances.
- **Locking:** None provided; user must synchronize access

## Dependencies

| Include | Purpose |
|---------|---------|
| `<stdexcept>` | Provides `std::out_of_range` for table access errors |
| `<string>` | Provides `std::string` for key and value types |
| `<boost/optional.hpp>` | Provides `boost::optional` for optional metadata |
| `<boost/variant.hpp>` | Provides `boost::variant` for polymorphic value types |

## Type Aliases

### Numeric Types (artdaq::database namespace)

#### `decimal`

**Brief:** Type alias for double-precision floating-point numbers.

**Definition:** `using decimal = double;`

**Usage:** Represents floating-point configuration values in variant types.

---

#### `integer`

**Brief:** Type alias for 64-bit signed integers.

**Definition:** `using integer = int64_t;`

**Usage:** Represents integer configuration values in variant types. Uses 64-bit to avoid overflow with large values.

---

### Metadata Types (artdaq::database::sharedtypes namespace)

#### `basic_key_t`

**Brief:** Standard string type for keys in key-value pairs.

**Definition:** `using basic_key_t = std::string;`

---

#### `optional_comment_t`

**Brief:** Optional string for comments associated with keys.

**Definition:** `using optional_comment_t = boost::optional<std::string>;`

**Usage:** Allows keys to have optional inline comments (common in FHiCL format).

---

#### `optional_annotation_t`

**Brief:** Optional string for annotations associated with values.

**Definition:** `using optional_annotation_t = boost::optional<std::string>;`

**Usage:** Allows values to have optional annotations or documentation.

## Classes/Structures

### `key_of<KEY, COMMENT>`

**Brief:** A template structure representing a key with an optional comment. Used when configuration keys need associated documentation.

**Template Parameters:**
- `KEY` - The key type (typically `std::string`)
- `COMMENT` - The comment type (typically `boost::optional<std::string>`)

**Thread Safety:** Not thread-safe; no internal synchronization.

#### Type Aliases

| Alias | Type | Description |
|-------|------|-------------|
| `key_type` | `KEY` | The underlying key type |
| `comment_type` | `COMMENT` | The comment type |

#### Constructors

##### `key_of()`

**Brief:** Default constructor creating an empty key with no comment.

##### `key_of(std::string const& k)`

**Brief:** Constructs a key with the given string value and no comment.

**Parameters:**
- `k` - The key string value

##### `key_of(std::string const& k, typename comment_type::value_type const& c)`

**Brief:** Constructs a key with both a string value and a comment.

**Parameters:**
- `k` - The key string value
- `c` - The comment string

#### Methods

##### `operator==(key_of const& other) const -> bool`

**Brief:** Compares two keys for equality based only on the key value, ignoring comments.

**Parameters:**
- `other` - The key to compare against

**Returns:** `true` if key values match, `false` otherwise

**Note:** Comments are not considered in equality comparison.

##### `operator=(key_type const& k) -> key_of&`

**Brief:** Assigns a new key value, leaving the comment unchanged.

**Parameters:**
- `k` - The new key value

**Returns:** Reference to this key

##### `operator std::string() const`

**Brief:** Converts the key to a string representation including the comment.

**Returns:** String in format `"key, comment=comment_value"`

#### Members

| Member | Type | Description |
|--------|------|-------------|
| `key` | `key_type` | The key value |
| `comment` | `comment_type` | Optional comment |

#### Example

```cpp
#include "artdaq-database/DataFormats/shared_types.h"

using namespace artdaq::database::sharedtypes;

using CommentedKey = key_of<std::string, optional_comment_t>;

// Key without comment
CommentedKey simple("threshold");

// Key with comment
CommentedKey documented("max_events", "Maximum events per run");

// Equality ignores comments
CommentedKey another("threshold", "different comment");
assert(simple == another);  // true - same key value

// String conversion includes comment
std::string str = documented;  // "max_events, comment=Maximum events per run"
```

---

### `any_key_of<KEY, COMMENT>`

**Brief:** A variant type that can hold either a plain key or a key with comment metadata.

**Definition:**
```cpp
template <typename KEY, typename COMMENT>
using any_key_of = boost::variant<KEY, key_of<KEY, COMMENT>>;
```

**Usage:** Allows functions to accept either simple keys or annotated keys, providing flexibility without separate overloads.

---

### `value_of<VALUE, ANNOTATION>`

**Brief:** A template structure representing a value with an optional annotation. Used when configuration values need associated documentation.

**Template Parameters:**
- `VALUE` - The value type (can be any type)
- `ANNOTATION` - The annotation type (typically `boost::optional<std::string>`)

**Thread Safety:** Not thread-safe; no internal synchronization.

#### Type Aliases

| Alias | Type | Description |
|-------|------|-------------|
| `value_type` | `VALUE` | The underlying value type |
| `annotation_type` | `ANNOTATION` | The annotation type |

#### Constructors

##### `value_of()`

**Brief:** Default constructor creating a default-constructed value with no annotation.

##### `value_of(value_type const& v)`

**Brief:** Constructs with the given value and no annotation.

**Parameters:**
- `v` - The value

##### `value_of(value_type const& v, typename annotation_type::value_type const& a)`

**Brief:** Constructs with both a value and an annotation.

**Parameters:**
- `v` - The value
- `a` - The annotation string

#### Methods

##### `operator value_type()`

**Brief:** Implicit conversion to the underlying value type.

**Returns:** The stored value

#### Members

| Member | Type | Description |
|--------|------|-------------|
| `value` | `value_type` | The actual value |
| `annotation` | `annotation_type` | Optional annotation |

#### Example

```cpp
#include "artdaq-database/DataFormats/shared_types.h"

using namespace artdaq::database::sharedtypes;

using AnnotatedInt = value_of<int, optional_annotation_t>;

// Value without annotation
AnnotatedInt threshold(100);

// Value with annotation
AnnotatedInt maxRetries(5, "Number of retry attempts before failure");

// Implicit conversion
int plain = threshold;  // plain = 100
```

---

### `any_value_of<VALUE, ANNOTATION>`

**Brief:** A variant type that can hold either a plain value or a value with annotation metadata.

**Definition:**
```cpp
template <typename VALUE, typename ANNOTATION>
using any_value_of = boost::variant<VALUE, value_of<VALUE, ANNOTATION>>;
```

---

### `vector_of<TYPE>`

**Brief:** A list-based collection with a vector-like interface. Uses `std::list` internally for iterator stability during modifications.

**Template Parameters:**
- `TYPE` - The element type

**Thread Safety:** Not thread-safe; no internal synchronization.

#### Type Aliases

| Alias | Type | Description |
|-------|------|-------------|
| `value_type` | `TYPE` | Element type |
| `container_type<Ts...>` | `std::list<Ts...>` | Underlying container template |
| `collection_type` | `std::list<value_type>` | Actual container type |
| `iterator` | `collection_type::iterator` | Iterator type |
| `const_iterator` | `collection_type::const_iterator` | Const iterator type |
| `size_type` | `collection_type::size_type` | Size type |

#### Methods

##### `empty() const -> bool`

**Brief:** Checks if the collection contains no elements.

**Returns:** `true` if empty, `false` otherwise

##### `size() const -> size_type`

**Brief:** Returns the number of elements in the collection.

**Returns:** Number of elements

##### `swap(vector_of<value_type>& other) -> void`

**Brief:** Exchanges the contents with another vector_of.

**Parameters:**
- `other` - The vector to swap with

##### `begin() -> iterator` / `begin() const -> const_iterator`

**Brief:** Returns an iterator to the first element.

**Returns:** Iterator to the beginning

##### `end() -> iterator` / `end() const -> const_iterator`

**Brief:** Returns an iterator past the last element.

**Returns:** Iterator to the end

##### `back() -> value_type&` / `back() const -> value_type const&`

**Brief:** Returns a reference to the last element.

**Returns:** Reference to the last element

**Preconditions:**
- Collection must not be empty

##### `push_back(value_type const& val) -> void`

**Brief:** Adds an element to the end of the collection.

**Parameters:**
- `val` - The value to add

##### `erase(iterator position) -> iterator`

**Brief:** Removes the element at the given position.

**Parameters:**
- `position` - Iterator to the element to remove

**Returns:** Iterator to the element following the removed one

##### `erase(iterator first, iterator last) -> iterator`

**Brief:** Removes elements in the range [first, last).

**Parameters:**
- `first` - Iterator to the first element to remove
- `last` - Iterator past the last element to remove

**Returns:** Iterator to the element following the last removed one

##### `insert(iterator position, value_type const& val) -> iterator`

**Brief:** Inserts an element before the given position.

**Parameters:**
- `position` - Iterator before which to insert
- `val` - The value to insert

**Returns:** Iterator to the inserted element

##### `operator()() -> collection_type&`

**Brief:** Provides direct access to the underlying std::list.

**Returns:** Reference to the internal collection

#### Members

| Member | Type | Description |
|--------|------|-------------|
| `values` | `collection_type` | The underlying std::list |

#### Example

```cpp
#include "artdaq-database/DataFormats/shared_types.h"

using namespace artdaq::database::sharedtypes;

vector_of<int> numbers;
numbers.push_back(10);
numbers.push_back(20);
numbers.push_back(30);

// Iteration
for (auto& n : numbers) {
    std::cout << n << " ";  // Output: 10 20 30
}

// Insert at beginning
numbers.insert(numbers.begin(), 5);

// Remove last
numbers.erase(--numbers.end());

// Direct access to list
auto& list = numbers();
```

---

### `kv_pair_of<KEYTYPE, VALUETYPE>`

**Brief:** A simple key-value pair structure for use in ordered tables.

**Template Parameters:**
- `KEYTYPE` - The key type
- `VALUETYPE` - The value type

**Thread Safety:** Not thread-safe; value type semantics.

#### Type Aliases

| Alias | Type | Description |
|-------|------|-------------|
| `key_type` | `KEYTYPE` | The key type |
| `value_type` | `VALUETYPE` | The value type |

#### Methods

##### `make(key_type const& key, value_type const& value) -> kv_pair_of` [static]

**Brief:** Factory method to create a key-value pair.

**Parameters:**
- `key` - The key
- `value` - The value

**Returns:** A new kv_pair_of instance

#### Members

| Member | Type | Description |
|--------|------|-------------|
| `key` | `key_type` | The key |
| `value` | `value_type` | The value |

#### Example

```cpp
#include "artdaq-database/DataFormats/shared_types.h"

using namespace artdaq::database::sharedtypes;

using StringPair = kv_pair_of<std::string, std::string>;

auto pair = StringPair::make("host", "localhost");
std::cout << pair.key << " = " << pair.value;  // host = localhost
```

---

### `table_of<KVP>`

**Brief:** An ordered associative container that maintains insertion order and allows duplicate keys. Provides map-like access semantics while preserving order.

**Template Parameters:**
- `KVP` - Key-value pair type (must be `kv_pair_of<K,V>`)

**Inherits From:** `vector_of<KVP>`

**Thread Safety:** Not thread-safe; no internal synchronization.

#### Type Aliases

| Alias | Type | Description |
|-------|------|-------------|
| `value_type` | `KVP` | The key-value pair type |
| `key_type` | `KVP::key_type` | The key type |
| `mapped_type` | `KVP::value_type` | The value type |
| `size_type` | Inherited | Size type |
| `iterator` | Inherited | Iterator type |
| `const_iterator` | Inherited | Const iterator type |

#### Methods

##### `operator[](key_type const& key) -> mapped_type&`

**Brief:** Accesses the value for a key, creating a default entry if the key does not exist.

**Parameters:**
- `key` - The key to look up

**Returns:** Reference to the value associated with the key

**Side Effects:** Creates a new entry with default value `{false}` if key not found

**Complexity:** O(n) linear search

##### `at(key_type const& key) -> mapped_type&` / `at(key_type const& key) const -> mapped_type const&`

**Brief:** Accesses the value for a key, throwing if not found.

**Parameters:**
- `key` - The key to look up

**Returns:** Reference to the value associated with the key

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `std::out_of_range` | Key not found in table |

**Complexity:** O(n) linear search

##### `find(key_type const& key) -> iterator` / `find(key_type const& key) const -> const_iterator`

**Brief:** Searches for a key in the table.

**Parameters:**
- `key` - The key to search for

**Returns:** Iterator to the first matching entry, or `end()` if not found

**Complexity:** O(n) linear search

##### `count(key_type const& key) const -> size_type`

**Brief:** Counts the number of entries with the given key.

**Parameters:**
- `key` - The key to count

**Returns:** Number of entries with the specified key (can be > 1 for duplicate keys)

**Complexity:** O(n) linear search

##### `delete_at(key_type const& key) -> mapped_type`

**Brief:** Removes the first entry with the given key and returns its value.

**Parameters:**
- `key` - The key of the entry to remove

**Returns:** The value that was associated with the key

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `std::out_of_range` | Key not found in table |

**Complexity:** O(n) linear search

#### Example

```cpp
#include "artdaq-database/DataFormats/shared_types.h"

using namespace artdaq::database::sharedtypes;

using ConfigTable = table_of<kv_pair_of<std::string, std::string>>;

ConfigTable config;

// Add entries (order preserved)
config["host"] = "localhost";
config["port"] = "8080";
config["timeout"] = "30";

// Duplicate keys allowed
config["host"] = "backup.local";
std::cout << config.count("host") << "\n";  // Output: 2

// Safe access with at()
try {
    auto& value = config.at("database");  // Throws - key not found
} catch (std::out_of_range& e) {
    std::cerr << "Key not found: " << e.what() << "\n";
}

// Search with find()
auto it = config.find("port");
if (it != config.end()) {
    std::cout << "Found: " << it->value << "\n";  // Output: 8080
}

// Remove first occurrence
auto removed = config.delete_at("host");  // Removes "localhost"
std::cout << config.count("host") << "\n";  // Output: 1
```

---

### `variant_value_of<TABLE_OF, VECTOR_OF>`

**Brief:** A variant type that can hold any of the common value types: nested tables, arrays, strings, decimal numbers, integers, or booleans. Uses recursive wrappers to support nested structures.

**Definition:**
```cpp
template <typename TABLE_OF, typename VECTOR_OF>
using variant_value_of = boost::variant<
    boost::recursive_wrapper<TABLE_OF>,
    boost::recursive_wrapper<VECTOR_OF>,
    std::string,
    decimal,
    integer,
    bool
>;
```

**Holds One Of:**
| Type | Description |
|------|-------------|
| `boost::recursive_wrapper<TABLE_OF>` | Nested table (object) |
| `boost::recursive_wrapper<VECTOR_OF>` | Nested array |
| `std::string` | String value |
| `decimal` (double) | Floating-point number |
| `integer` (int64_t) | Integer number |
| `bool` | Boolean value |

**Example:**
```cpp
#include "artdaq-database/DataFormats/shared_types.h"

using namespace artdaq::database::sharedtypes;
using namespace artdaq::database;

// Forward declare concrete types
struct MyTable;
struct MyVector;

using MyVariant = variant_value_of<MyTable, MyVector>;

MyVariant strVal = std::string("hello");
MyVariant intVal = integer{42};
MyVariant decVal = decimal{3.14};
MyVariant boolVal = true;

// Type checking with relaxed get
if (auto* str = boost::get<std::string>(&strVal)) {
    std::cout << "String: " << *str << "\n";
}
```

---

### `unwrapper<A>`

**Brief:** A helper template for convenient access to nested values in variant types. Provides methods to extract typed values from variant containers.

**Template Parameters:**
- `A` - The variant type to unwrap

**Thread Safety:** Not thread-safe; operates on references.

#### Constructor

##### `unwrapper(A&)`

**Brief:** Constructs an unwrapper for the given variant reference.

**Parameters:**
- Reference to the variant to unwrap

#### Methods

##### `value_as<T>() -> T&`

**Brief:** Extracts a reference to the contained value as type T.

**Template Parameters:**
- `T` - The expected type

**Returns:** Reference to the value as type T

##### `value_as<T>(std::string const& child) -> T&`

**Brief:** Extracts a child value as type T.

**Parameters:**
- `child` - The child key name

**Returns:** Reference to the child value as type T

##### `value<O>(std::string const& key) -> auto&`

**Brief:** Gets the value at the specified key from container type O.

**Template Parameters:**
- `O` - The container type (must have `at()` method)

**Parameters:**
- `key` - The key to look up

**Returns:** Reference to the value

**Throws:** `std::out_of_range` if key not found

##### `value<O, T>(std::string const& key) -> auto&`

**Brief:** Gets the value at the specified key, creating a default T if not found.

**Template Parameters:**
- `O` - The container type
- `T` - The default value type

**Parameters:**
- `key` - The key to look up

**Returns:** Reference to the value (existing or newly created)

#### Members

| Member | Type | Description |
|--------|------|-------------|
| `any` | `A&` | Reference to the wrapped variant |

---

### `unwrap<A>(A& any) -> unwrapper<A>`

**Brief:** Factory function to create an unwrapper for a variant.

**Parameters:**
- `any` - Reference to the variant to unwrap

**Returns:** An unwrapper instance for the variant

**Example:**
```cpp
using MyVariant = variant_value_of<MyTable, MyVector>;

MyVariant data = /* ... */;
auto wrapper = unwrap(data);

// Access nested table value
try {
    auto& tableValue = wrapper.value<MyTable>("config");
} catch (std::out_of_range& e) {
    // Key not found
}
```

## Performance Considerations

### List-Based Storage

`vector_of` uses `std::list` internally:
- **Advantage:** Iterator stability - iterators remain valid during insertions/deletions
- **Disadvantage:** No random access, cache-unfriendly memory layout
- **Best for:** Frequent mid-sequence modifications

### Linear Search in table_of

All lookup operations in `table_of` are O(n):
- **find(), at(), count(), operator[]:** Linear scan through entries
- **Best for:** Small to medium-sized tables (< 100 entries)
- **Consider alternatives:** For large tables with frequent lookups, consider std::unordered_map

### Variant Type Overhead

Using `boost::variant` adds:
- Type tag storage (typically 1-8 bytes)
- Type checking on access
- Recursive wrappers add pointer indirection

## Relationship to Other Components

```
shared_types.h
    |
    +-- Json/json_types.h (defines json_object_t, json_array_t using these templates)
    +-- Xml/xml_types.h (defines xml_element_t using these templates)
    +-- Fhicl/fhicl_types.h (defines fhicl_table_t, fhicl_sequence_t)
    +-- Conf/conf_types.h (defines conf_table_t)
    |
    +-- Used by JsonDocument (document storage)
    +-- Used by Overlay (document manipulation)
```

## See Also

- [shared_literals.h](./shared_literals.h.md) - String constants used as keys in these types
- [common.h](./common.h.md) - Configures boost::variant relaxed access mode
- [Json/json_types.h](./Json/json_types.h.md) - JSON-specific instantiations
- [External: Boost.Variant](https://www.boost.org/doc/libs/release/doc/html/variant.html) - boost::variant documentation
- [External: Boost.Optional](https://www.boost.org/doc/libs/release/libs/optional/doc/html/index.html) - boost::optional documentation

## Notes for Developers

### Common Pitfalls

- **Pitfall 1:** The `vector_of` name is misleading - it uses `std::list` internally, so there is no random access operator[].
- **Pitfall 2:** `table_of::operator[]` creates entries if they do not exist. Use `at()` or `find()` for safe lookups.
- **Pitfall 3:** `key_of` equality ignores comments. Two keys with different comments but the same key value are considered equal.

### Anti-patterns

```cpp
// DON'T assume vector_of has random access:
vector_of<int> nums;
// nums[0];  // Error - no operator[]!

// DO iterate or use std::advance:
auto it = nums.begin();
std::advance(it, 0);
int first = *it;

// DON'T use operator[] for existence check (creates entry):
if (table["key"]) { }  // BAD - creates entry if missing!

// DO use find() for existence check:
if (table.find("key") != table.end()) { }  // GOOD
```

### Best Practices

1. **Use `at()` for safe access:**
   ```cpp
   try {
       auto& value = table.at("key");
   } catch (std::out_of_range&) {
       // Handle missing key
   }
   ```

2. **Use `find()` for conditional access:**
   ```cpp
   auto it = table.find("optional_key");
   if (it != table.end()) {
       process(it->value);
   }
   ```

3. **Use relaxed boost::get for variant access:**
   ```cpp
   if (auto* str = boost::get<std::string>(&variant)) {
       // Handle string case
   }
   ```
