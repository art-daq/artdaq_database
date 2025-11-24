# shared_types.h

## File Overview

This header file defines a comprehensive set of template-based type aliases and structures for handling hierarchical data with optional metadata. It provides the foundational building blocks for representing configuration data, database documents, and structured data across different formats (JSON, XML, FHiCL, CONF).

The file includes sophisticated template structures for key-value pairs with comments/annotations, collections, tables, and variant types that can hold multiple data types.

**Location**: `/home/user/artdaq-database/artdaq-database/DataFormats/shared_types.h`

## Dependencies

### Standard Libraries
- `<stdexcept>` - For std::out_of_range exceptions
- `<string>` - For std::string type

### Third-Party Libraries

**Boost Libraries**:
- `<boost/optional.hpp>` - Optional value types
- `<boost/variant.hpp>` - Variant (discriminated union) types

## Header Guard

```cpp
#ifndef _ARTDAQ_DATABASE_SHAREDTYPES_H_
#define _ARTDAQ_DATABASE_SHAREDTYPES_H_
...
#endif
```

## Namespace

All types are defined in:
```cpp
artdaq::database::sharedtypes
```

Some types are also in the parent namespace:
```cpp
artdaq::database
```

## Type Aliases

### Basic Numeric Types

```cpp
namespace artdaq::database {
    using decimal = double;
    using integer = int64_t;
}
```

| Type | Underlying Type | Description |
|------|-----------------|-------------|
| `decimal` | `double` | Floating-point numbers |
| `integer` | `int64_t` | 64-bit signed integers |

### Basic Metadata Types

```cpp
namespace artdaq::database::sharedtypes {
    using basic_key_t = std::string;
    using optional_comment_t = boost::optional<std::string>;
    using optional_annotation_t = boost::optional<std::string>;
}
```

| Type | Description |
|------|-------------|
| `basic_key_t` | Standard string type for keys |
| `optional_comment_t` | Optional comment string (may or may not be present) |
| `optional_annotation_t` | Optional annotation string |

## Template Structures

### key_of<KEY, COMMENT>

A template structure representing a key with an optional comment.

**Template Parameters**:
- `KEY` - The key type (typically std::string)
- `COMMENT` - The comment type (typically boost::optional<std::string>)

**Type Aliases**:
```cpp
using key_type = KEY;
using comment_type = COMMENT;
```

**Constructors**:
```cpp
key_of();                                                    // Default
key_of(std::string const& k);                               // Key only
key_of(std::string const& k,                                // Key with comment
       typename comment_type::value_type const& c);
```

**Operators**:
```cpp
bool operator==(key_of const& other) const;               // Equality (compares keys only)
key_of& operator=(key_type const& k);                     // Assignment from key
operator std::string() const;                              // Conversion to string
```

**Members**:
```cpp
key_type key;           // The key value
comment_type comment;   // Optional comment
```

**Usage Example**:
```cpp
using MyKey = key_of<std::string, boost::optional<std::string>>;

MyKey k1("temperature");              // Just a key
MyKey k2("pressure", "in PSI");       // Key with comment

if (k1 == k2) { /* ... */ }           // Compares keys only
std::string str = k1;                  // "temperature, comment="
```

---

### any_key_of<KEY, COMMENT>

A variant type that can hold either a plain KEY or a key_of<KEY, COMMENT>.

```cpp
template <typename KEY, typename COMMENT>
using any_key_of = boost::variant<KEY, key_of<KEY, COMMENT>>;
```

**Purpose**: Allows code to accept either annotated or plain keys.

**Usage Example**:
```cpp
using AnyKey = any_key_of<std::string, optional_comment_t>;

AnyKey k1 = std::string("simple");
AnyKey k2 = key_of<std::string, optional_comment_t>("annotated", "with comment");
```

---

### value_of<VALUE, ANNOTATION>

A template structure representing a value with an optional annotation.

**Template Parameters**:
- `VALUE` - The value type (can be any type)
- `ANNOTATION` - The annotation type (typically boost::optional<std::string>)

**Type Aliases**:
```cpp
using value_type = VALUE;
using annotation_type = ANNOTATION;
```

**Constructors**:
```cpp
value_of();                                                  // Default
value_of(value_type const& v);                              // Value only
value_of(value_type const& v,                               // Value with annotation
         typename annotation_type::value_type const& a);
```

**Operators**:
```cpp
operator value_type();                                       // Conversion to value type
```

**Members**:
```cpp
value_type value;              // The actual value
annotation_type annotation;    // Optional annotation
```

**Usage Example**:
```cpp
using AnnotatedInt = value_of<int, boost::optional<std::string>>;

AnnotatedInt v1(42);                  // Just a value
AnnotatedInt v2(100, "maximum");      // Value with annotation

int plain = v1;                        // Implicit conversion to int
```

---

### any_value_of<VALUE, ANNOTATION>

A variant type that can hold either a plain VALUE or a value_of<VALUE, ANNOTATION>.

```cpp
template <typename VALUE, typename ANNOTATION>
using any_value_of = boost::variant<VALUE, value_of<VALUE, ANNOTATION>>;
```

**Purpose**: Allows code to accept either annotated or plain values.

---

### vector_of<TYPE>

A list-based collection with a std::vector-like interface.

**Template Parameters**:
- `TYPE` - The element type

**Type Aliases**:
```cpp
using value_type = TYPE;
template <typename... Ts>
using container_type = std::list<Ts...>;
using collection_type = container_type<value_type>;
using const_iterator = typename collection_type::const_iterator;
using iterator = typename collection_type::iterator;
using size_type = typename collection_type::size_type;
```

**Note**: Despite the name "vector", this uses `std::list` internally for stable iterators.

**Methods**:
```cpp
bool empty() const;
void swap(vector_of<value_type>& other);
size_type size() const;

// Iterators
iterator begin();
iterator end();
const_iterator begin() const;
const_iterator end() const;

// Element access
value_type& back();
value_type const& back() const;

// Modifiers
void push_back(value_type const& val);
iterator erase(iterator position);
iterator erase(iterator first, iterator last);
iterator insert(iterator position, value_type const& val);

// Direct access to underlying container
collection_type& operator()();
```

**Member**:
```cpp
collection_type values;    // The underlying std::list
```

**Usage Example**:
```cpp
vector_of<int> numbers;
numbers.push_back(1);
numbers.push_back(2);
numbers.push_back(3);

for (auto& num : numbers) {
    std::cout << num << " ";
}
```

---

### kv_pair_of<KEYTYPE, VALUETYPE>

A simple key-value pair structure.

**Template Parameters**:
- `KEYTYPE` - The key type
- `VALUETYPE` - The value type

**Type Aliases**:
```cpp
using key_type = KEYTYPE;
using value_type = VALUETYPE;
```

**Factory Method**:
```cpp
static kv_pair_of make(key_type const& key, value_type const& value);
```

**Members**:
```cpp
key_type key;
value_type value;
```

**Usage Example**:
```cpp
using IntPair = kv_pair_of<std::string, int>;

auto pair = IntPair::make("count", 42);
std::cout << pair.key << ": " << pair.value;  // "count: 42"
```

---

### table_of<KVP>

An ordered associative container (like a map) that maintains insertion order and supports duplicate keys.

**Template Parameters**:
- `KVP` - Key-value pair type (must be kv_pair_of<K,V>)

**Inherits From**: `vector_of<KVP>`

**Type Aliases**:
```cpp
using value_type = KVP;
using key_type = typename value_type::key_type;
using mapped_type = typename value_type::value_type;
using size_type = typename vector_of<KVP>::size_type;
using const_iterator = typename vector_of<KVP>::const_iterator;
using iterator = typename vector_of<KVP>::iterator;
```

**Methods**:

```cpp
// Access (creates element if not found)
mapped_type& operator[](key_type const& key);

// Access (throws if not found)
mapped_type& at(key_type const& key);
mapped_type const& at(key_type const& key) const;

// Search
iterator find(key_type const& key);
const_iterator find(key_type const& key) const;
size_type count(key_type const& key) const;

// Removal
mapped_type delete_at(key_type const& key);
```

**Behavior**:

1. **Ordered**: Maintains insertion order (unlike std::map)
2. **Duplicate keys allowed**: count() can return > 1
3. **operator[]**: Creates entry with default-constructed value if key not found
4. **at()**: Throws std::out_of_range if key not found
5. **find()**: Returns iterator to first matching key, or end()
6. **delete_at()**: Removes first matching key and returns its value

**Usage Example**:
```cpp
using StringTable = table_of<kv_pair_of<std::string, std::string>>;

StringTable config;
config["host"] = "localhost";
config["port"] = "8080";
config["host"] = "backup.local";  // Duplicate key allowed

std::cout << config.count("host");  // Prints: 2

try {
    auto value = config.at("database");
} catch (std::out_of_range& e) {
    // Key not found
}

auto it = config.find("port");
if (it != config.end()) {
    std::cout << it->value;  // "8080"
}

config.delete_at("host");     // Removes first "host" entry
std::cout << config.count("host");  // Prints: 1
```

---

### variant_value_of<TABLE_OF, VECTOR_OF>

A variant type representing a value that can be a table, vector, string, number (decimal/integer), or boolean.

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

**Type Parameters**:
- `TABLE_OF` - The table type (typically table_of<...>)
- `VECTOR_OF` - The vector type (typically vector_of<...>)

**Holds One Of**:
1. `boost::recursive_wrapper<TABLE_OF>` - Nested table (allows recursive structures)
2. `boost::recursive_wrapper<VECTOR_OF>` - Nested vector/array
3. `std::string` - String value
4. `decimal` (double) - Floating-point number
5. `integer` (int64_t) - Integer number
6. `bool` - Boolean value

**Recursive Wrapper**: Allows the variant to contain itself recursively (for nested structures).

**Usage Example**:
```cpp
using MyTable = table_of<kv_pair_of<std::string, int>>;
using MyVector = vector_of<int>;
using MyVariant = variant_value_of<MyTable, MyVector>;

MyVariant v1 = std::string("hello");
MyVariant v2 = 3.14;
MyVariant v3 = 42;
MyVariant v4 = true;

// For nested structures, use recursive_wrapper
MyTable nestedTable;
MyVariant v5 = boost::recursive_wrapper<MyTable>(nestedTable);
```

---

### unwrapper<A>

A helper template for working with variant types, providing convenient access to nested values.

**Template Parameters**:
- `A` - The variant type to unwrap

**Constructor**:
```cpp
unwrapper(A&);
```

**Methods**:

```cpp
template <typename T>
T& value_as();

template <typename T>
T& value_as(std::string const& child);

template <typename O>
auto& value(std::string const& key);

template <typename O, typename T>
auto& value(std::string const& key);  // Creates if not found
```

**Factory Function**:
```cpp
template <typename A>
unwrapper<A> unwrap(A& any);
```

**Usage Example**:
```cpp
using MyVariant = variant_value_of<MyTable, MyVector>;

MyVariant data = /* ... */;

auto wrapper = unwrap(data);

// Access nested table value
auto& tableValue = wrapper.value<MyTable>("config");

// Get typed value with default creation
auto& intValue = wrapper.value<MyTable, int>("count");
```

**Method Details**:

1. **value<O>(key)**: Gets value at key from type O, throws if not found
2. **value<O,T>(key)**: Gets value at key from type O, creates default T if not found

## Design Patterns

### Optional Metadata Pattern

The library provides a consistent pattern for optional metadata:

```cpp
// Keys can have optional comments
key_of<std::string, optional_comment_t> key("name", "user's full name");

// Values can have optional annotations
value_of<int, optional_annotation_t> value(42, "maximum retries");
```

### Variant Access Pattern

When working with variant types, use boost::get:

```cpp
variant_value_of<Table, Vector> var = /* ... */;

if (auto* str = boost::get<std::string>(&var)) {
    // Handle string case
} else if (auto* num = boost::get<decimal>(&var)) {
    // Handle decimal case
} else if (auto* table = boost::get<boost::recursive_wrapper<Table>>(&var)) {
    // Handle nested table case
    auto& actualTable = table->get();  // Unwrap recursive_wrapper
}
```

### Collection Building Pattern

```cpp
using ConfigTable = table_of<kv_pair_of<std::string, std::string>>;

ConfigTable buildConfig() {
    ConfigTable config;
    config["host"] = "localhost";
    config["port"] = "8080";
    config["timeout"] = "30";
    return config;
}
```

## Performance Considerations

1. **std::list vs std::vector**:
   - `vector_of` uses `std::list` internally
   - Provides stable iterators (iterators not invalidated by insertions/deletions)
   - Slower random access than std::vector
   - Better for frequent insertions/deletions in the middle

2. **table_of Lookups**:
   - Linear search O(n) for find/at/count operations
   - Not as efficient as std::map for large tables
   - Maintains insertion order and allows duplicates
   - Suitable for small to medium-sized tables

3. **Variant Access**:
   - boost::get with pointer has negligible overhead
   - Avoid excessive type checking in tight loops

4. **Recursive Wrappers**:
   - Add indirection (pointer) overhead
   - Necessary for recursive variant types
   - Minimal impact for typical use cases

## Usage Across DataFormats

These types are used extensively in all DataFormats submodules:

- **Json**: JSON objects → `table_of`, JSON arrays → `vector_of`, JSON values → `variant_value_of`
- **Xml**: XML elements → `table_of`, XML attributes → `key_of`, XML text → `value_of`
- **FHiCL**: FHiCL tables → `table_of`, FHiCL sequences → `vector_of`, FHiCL parameters → `kv_pair_of`
- **Conf**: Configuration tables → `table_of`, configuration values → `variant_value_of`

## Related Files

- **shared_literals.h** - String constants used as keys in tables
- **common.h** - Includes boost/variant.hpp and sets BOOST_VARIANT_USE_RELAXED_GET_BY_DEFAULT
- **Json/json_types.h** - JSON-specific type definitions using these templates
- **Xml/xml_types.h** - XML-specific type definitions using these templates
- **Fhicl/fhicl_types.h** - FHiCL-specific type definitions using these templates
- **Conf/conf_types.h** - CONF-specific type definitions using these templates

## Notes

- All types are header-only with no separate implementation file
- The use of `std::list` instead of `std::vector` in `vector_of` provides iterator stability
- `table_of` allows duplicate keys, unlike std::map, which is important for certain configuration formats
- The variant types support recursive structures through `boost::recursive_wrapper`
- Template-based design allows type-safe, compile-time polymorphism
- Optional metadata (comments, annotations) is pervasive, supporting self-documenting configurations
