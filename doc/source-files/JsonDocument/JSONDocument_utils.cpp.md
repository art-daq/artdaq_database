# JSONDocument_utils.cpp

## File Overview

This file implements utility functions and supplementary methods for the `JSONDocument` and `JSONDocumentBuilder` classes. It provides JSON serialization/deserialization, value matching, path splitting, file I/O operations, and the critical `createFromData` method that wraps user data in proper database structure.

**Location**: `/home/user/artdaq-database/artdaq-database/JsonDocument/JSONDocument_utils.cpp`

## Dependencies

```cpp
#include "artdaq-database/BasicTypes/data_json.h"
#include "artdaq-database/JsonDocument/JSONDocument.h"
#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"
#include "artdaq-database/JsonDocument/common.h"
#include <boost/filesystem.hpp>
#include <utility>
```

**Key Dependencies**:
- **data_json.h** - JSON data type definitions
- **JSONDocument.h** - Document class
- **JSONDocumentBuilder.h** - Builder class
- **common.h** - Module utilities
- **boost/filesystem.hpp** - File operations

## TRACE Configuration

```cpp
#ifdef TRACE_NAME
#undef TRACE_NAME
#endif
#define TRACE_NAME "JSONDocument_utils.cpp"
```

## Using Declarations

```cpp
using artdaq::database::json::array_t;
using artdaq::database::json::object_t;
using artdaq::database::json::type_t;
using artdaq::database::json::value_t;
using artdaq::database::json::JsonReader;
using artdaq::database::json::JsonWriter;
using artdaq::database::Failure;
using artdaq::database::result_t;
using artdaq::database::Success;
using artdaq::database::docrecord::JSONDocument;
using artdaq::database::docrecord::JSONDocumentBuilder;
using artdaq::database::sharedtypes::unwrap;

namespace db = artdaq::database;
namespace utl = db::docrecord;
namespace dbdr = artdaq::database::docrecord;
namespace jsonliteral = artdaq::database::dataformats::literal;
```

## Visitor Functions

### print_visitor

```cpp
std::string print_visitor(value_t const& value) {
  return boost::apply_visitor(jsn::print_visitor(), value);
}
```

**Purpose**: Converts a JSON value to human-readable string for debugging.

**Uses Visitor Pattern**: Applies `print_visitor` to the variant type.

**Use Case**: Logging and debugging JSON values.

### tostring_visitor

```cpp
std::string tostring_visitor(value_t const& value) {
  return boost::apply_visitor(jsn::tostring_visitor(), value);
}
```

**Purpose**: Converts a JSON value to its string representation.

**Difference from print_visitor**: Provides raw string value rather than formatted output.

## Value Matching Function

### matches

```cpp
bool matches(value_t const& left, value_t const& right)
```

**Purpose**: Recursively compares two JSON values for equality, supporting partial matching for objects.

**Algorithm**:

#### Step 1: Type Check

```cpp
if (left.type() != right.type()) {
  return false;
}
```

Ensures both values have the same JSON type.

#### Step 2: Object Matching

```cpp
if (type(left) == type_t::OBJECT) {
  auto const& leftObj = unwrap(left).value_as<const object_t>();
  auto const& rightObj = unwrap(right).value_as<const object_t>();

  // Partial match support - sizes don't need to match
  if (leftObj.empty() || rightObj.empty()) {
    return false;
  }

  auto const& tempateObj = leftObj.size() >= rightObj.size() ? leftObj : rightObj;
  auto const& candidateObj = leftObj.size() >= rightObj.size() ? rightObj : leftObj;

  for (auto const& templateKVP : tempateObj) {
    if (candidateObj.count(templateKVP.key) != 1) {
      continue;  // Skip keys not in candidate
    }

    auto const& candidateVal = candidateObj.at(templateKVP.key);
    auto const& templateVal = templateKVP.value;

    if (!matches(templateVal, candidateVal)) {
      TLOG(20) << "matches() objects are different at key=<" << templateKVP.key << ">";
      return false;
    }
  }

  return true;
}
```

**Object Matching Characteristics**:
- **Partial Matching**: Template can have more fields than candidate
- **Empty Check**: Rejects empty objects
- **Recursive Comparison**: Recursively matches nested values
- **Skips Missing Keys**: If key not in candidate, continues (allows partial match)

**Use Case**: Finding objects in arrays where you only care about certain fields.

**Example**:
```cpp
// Template: {"name": "test"}
// Candidate: {"name": "test", "value": 42}
// Result: true (partial match)
```

#### Step 3: Array Matching

```cpp
if (type(left) == type_t::ARRAY) {
  auto const& leftObj = unwrap(left).value_as<const array_t>();
  auto const& rightObj = unwrap(left).value_as<const array_t>();  // Note: Bug here!

  if (leftObj.size() != rightObj.size()) {
    return false;
  }

  if (leftObj.empty() || rightObj.empty()) {
    return false;
  }

  auto elementCount = leftObj.size();
  auto leftObjIter = leftObj.begin();
  auto rightObjIter = rightObj.begin();

  while ((elementCount--) != 0u) {
    if (!matches(*leftObjIter, *rightObjIter)) {
      return false;
    }
    std::advance(leftObjIter, 1);
    std::advance(rightObjIter, 1);
  }
  return true;
}
```

**Array Matching Characteristics**:
- **Exact Size**: Arrays must have same number of elements
- **Element-wise**: Compares elements at same positions
- **Recursive**: Uses `matches` for each element

**Bug Alert**: Line 79 uses `unwrap(left)` twice instead of `unwrap(right)`, which is likely a bug that would cause incorrect comparisons.

#### Step 4: Primitive Matching

```cpp
auto leftObj = tostring_visitor(left);
auto rightObj = tostring_visitor(right);
return leftObj == rightObj;
```

For primitives (string, number, boolean, null), converts both to strings and compares.

## Path Utilities

### split_path

```cpp
std::vector<std::string> utl::split_path(std::string const& path)
```

**Purpose**: Splits a dot-notation path into individual components.

**Algorithm**:
```cpp
auto tmp = std::string{path};

// Replace dots with spaces
std::replace(tmp.begin(), tmp.end(), '.', ' ');

// Use string stream to split on spaces
std::istringstream iss(tmp);
std::vector<std::string> tokens{
    std::istream_iterator<std::string>{iss},
    std::istream_iterator<std::string>{}
};

// Optional debug logging (commented out in actual code)
if (!tokens.empty()) {
  std::ostringstream oss;
  for (auto const& token : tokens) {
    oss << "\"" << token << "\",";
  }
}

return tokens;
```

**Example**:
- Input: `"document.data.field"`
- Output: `["document", "data", "field"]`

**TRACE Level**: None (debug code commented out)

## Stream Output Operator

### operator<<

```cpp
std::ostream& utl::operator<<(std::ostream& os, JSONDocument const& document) {
  os << document.to_string();
  return os;
}
```

**Purpose**: Enables streaming `JSONDocument` to output streams.

**Usage**:
```cpp
std::cout << doc << std::endl;
std::ofstream file("output.txt");
file << doc;
```

## JSON Serialization/Deserialization

### readJson

```cpp
value_t JSONDocument::readJson(std::string const& json)
```

**Purpose**: Parses JSON string into internal `value_t` representation.

**Implementation**:
```cpp
if (json.empty()) {
  throw invalid_argument("JSONDocument")
      << "Failed reading JSON: Empty JSON buffer";
}

auto tmpObject = object_t{};

if (JsonReader().read(json, tmpObject)) {
  return {tmpObject};
}

throw invalid_argument("JSONDocument")
    << "Failed reading JSON: Invalid json; json_buffer=" << json;
```

**Process**:
1. Validate JSON string is not empty
2. Create temporary object
3. Use `JsonReader` to parse JSON
4. Return parsed object as `value_t`
5. Throw if parsing fails

**Throws**: `invalid_argument` if JSON is empty or malformed

### writeJson

```cpp
std::string JSONDocument::writeJson() const
```

**Purpose**: Serializes internal `value_t` to JSON string.

**Implementation**:
```cpp
if (type(_value) != type_t::OBJECT) {
  throw invalid_argument("JSONDocument")
      << "Failed writing JSON: Wrong value type: type(_value) != type_t::OBJECT";
}

auto const& tmpObject = boost::get<object_t>(_value);

if (tmpObject.empty()) {
  return jsonliteral::empty_json;
}

auto json = std::string{};

if (!JsonWriter().write(tmpObject, json)) {
  throw invalid_argument("JSONDocument")
      << "Failed writing JSON: JSONDocument::_value has invalid AST";
}

return json;
```

**Process**:
1. Verify value is an object type
2. Extract object from variant
3. Return empty JSON if object is empty
4. Use `JsonWriter` to serialize
5. Throw if serialization fails

**Returns**: JSON string or empty JSON literal

**Throws**: `invalid_argument` if type is wrong or serialization fails

## Public JSONDocument Methods

### to_string

```cpp
std::string JSONDocument::to_string() const
```

**Purpose**: Returns JSON string with caching for performance.

**Implementation**:
```cpp
if (_isDirty) {
  _cached_json_buffer = writeJson();
  _isDirty = false;
}
return _cached_json_buffer;
```

**Lazy Evaluation**:
- Only serializes if dirty flag is set
- Caches result for future calls
- Clears dirty flag after serialization

**Performance**: Avoids redundant serialization for read-heavy operations.

### operator std::string()

```cpp
JSONDocument::operator std::string() const
```

**Purpose**: Implicit conversion to string.

**Implementation**: Identical to `to_string()` with caching.

**Usage**:
```cpp
JSONDocument doc = /* ... */;
std::string json = doc;  // Implicit conversion
```

### empty

```cpp
bool JSONDocument::empty() const {
  return _value.empty();
}
```

**Purpose**: Checks if document has no value.

## JSONDocument Constructors (Implementations)

### Constructor from JsonData

```cpp
JSONDocument::JSONDocument(JsonData const& data)
    : _value{readJson(data)},
      _cached_json_buffer(jsonliteral::empty_json),
      _isDirty(true) {}
```

### Constructor from string

```cpp
JSONDocument::JSONDocument(std::string const& json)
    : _value{readJson(json)},
      _cached_json_buffer(jsonliteral::empty_json),
      _isDirty(true) {}
```

### Constructor from value_t

```cpp
JSONDocument::JSONDocument(value_t value)
    : _value{std::move(value)},
      _cached_json_buffer(jsonliteral::empty_json),
      _isDirty(true) {}
```

### Default Constructor

```cpp
JSONDocument::JSONDocument()
    : _value{object_t{}},
      _cached_json_buffer(jsonliteral::empty_json),
      _isDirty(true) {}
```

**Pattern**: All constructors mark document as dirty and initialize cache to empty.

## Helper Methods

### cached_json_buffer

```cpp
std::string const& JSONDocument::cached_json_buffer() const {
  return _cached_json_buffer;
}
```

**Purpose**: Provides access to cached JSON string without triggering serialization.

### getPayloadValueForKey

```cpp
value_t const& JSONDocument::getPayloadValueForKey(object_t::key_type const& key) const
```

**Purpose**: Extracts payload value from document, handling different document formats.

**Algorithm**:
```cpp
confirm(!key.empty());

TLOG(21) << "getPayloadValueForKey() document=<" << cached_json_buffer() << ">";

if (unwrap(_value).value_as<const object_t>().count("payload") == 1) {
  auto const& value = unwrap(_value).value<const object_t>("payload");

  if (type(value) == type_t::OBJECT &&
      unwrap(value).value_as<const object_t>().count(key) == 1) {
    return unwrap(value).value<const object_t>(key);
  }
  return value;
} else if (unwrap(_value).value_as<const object_t>().size() == 1) {
  return unwrap(_value).value_as<const object_t>().begin()->value;
}

return _value;
```

**Logic**:
1. If document has "payload" key:
   - If payload is object with requested key: return that value
   - Otherwise: return entire payload
2. Else if document has exactly one key:
   - Return that key's value
3. Otherwise: return entire document value

**Use Case**: Flexible extraction supporting different document wrapper formats.

**TRACE Level**: 21

### equals

```cpp
bool JSONDocument::equals(JSONDocument const& other) const
```

**Purpose**: Compares two documents for equality.

**Implementation**:
```cpp
auto result = jsn::operator==(_value, other._value);

TLOG(22) << "matches() JSON buffers are "
         << (result.first ? "equal." : "not equal.");

if (result.first) {
  return true;
}

TLOG(23) << "matches() Error message=<" << result.second << ">";

return false;
```

**Returns**: `true` if documents are equal, `false` otherwise.

**TRACE Levels**:
- 22: Comparison result
- 23: Error message if not equal

## File I/O Operations

### loadFromFile

```cpp
JSONDocument JSONDocument::loadFromFile(std::string const& fileName)
```

**Purpose**: Loads JSON document from file.

**Implementation**:
```cpp
try {
  auto json_buffer = std::string{};

  if (!db::read_buffer_from_file(json_buffer, fileName)) {
    throw invalid_argument("JSONDocument")
        << "Failed calling loadFromFile(): Failed opening a JSON file=" << fileName;
  }

  return {json_buffer};
} catch (std::exception& ex) {
  throw runtime_error("JSONDocument")
      << "Failed calling loadFromFile(): Caught exception:" << ex.what();
}
```

**Process**:
1. Read file content into string buffer
2. Throw if file read fails
3. Create document from JSON string
4. Re-throw exceptions as `runtime_error`

**Throws**:
- `invalid_argument` if file cannot be opened
- `runtime_error` for other exceptions

### saveToFile

```cpp
bool JSONDocument::saveToFile(std::string const& fileName)
```

**Purpose**: Saves JSON document to file with backup.

**Implementation**:
```cpp
try {
  if (fileName.empty()) {
    throw invalid_argument("JSONDocument")
        << "Failed calling saveToFile(): File name is empty.";
  }

  if (boost::filesystem::exists(fileName)) {
    boost::filesystem::copy_file(
        fileName,
        std::string{fileName} + ".bak",
        boost::filesystem::copy_options::overwrite_existing);
  }

  auto buffer = to_string();

  return db::write_buffer_to_file(buffer, fileName);
} catch (std::exception& ex) {
  throw runtime_error("JSONDocument")
      << "Failed calling saveToFile(): Caught exception:" << ex.what();
}
```

**Features**:
1. Validates filename is not empty
2. Creates `.bak` backup if file exists
3. Serializes document to string
4. Writes to file
5. Returns success/failure

**Backup Strategy**: Overwrites existing backup with current file before saving new version.

**Throws**: `runtime_error` for failures

## Static Utility Methods

### value (static)

```cpp
std::string JSONDocument::value(JSONDocument const& document)
```

**Purpose**: Extracts string value from document.

**Implementation**:
```cpp
TLOG(39) << "value() document=<" << document.cached_json_buffer() << ">";

auto docValue = document.getPayloadValueForKey("null");

if (type(docValue) == type_t::OBJECT) {
  return JSONDocument{docValue}.to_string();
}
{ return tostring_visitor(docValue); }
```

**Logic**:
- Extract payload using "null" as key (gets first/only value)
- If value is object: create document and serialize
- Otherwise: convert to string

**TRACE Level**: 39

### value_at (static)

```cpp
std::string JSONDocument::value_at(JSONDocument const& document, std::size_t index)
```

**Purpose**: Extracts value at specific index from document containing an array.

**Implementation**:
```cpp
try {
  TLOG(40) << "value_at() begin json=<" << document.cached_json_buffer() << ">";
  TLOG(41) << "value_at() begin index=<" << index << ">";

  auto docValue = document.getPayloadValueForKey("0");

  auto const& valueArray = unwrap(docValue).value_as<const array_t>();

  if (valueArray.empty()) {
    throw runtime_error("JSONDocument")
        << "Failed calling value_at(): valueArray is empty, document=<"
        << document.cached_json_buffer() << ">";
  }

  if (valueArray.size() < index) {
    throw runtime_error("JSONDocument")
        << "Failed to call value_at(); not enough elements, document=<"
        << document.cached_json_buffer() << ">";
  }

  auto pos = valueArray.begin();
  std::advance(pos, index);

  if (type(*pos) == type_t::OBJECT) {
    return JSONDocument{*pos}.to_string();
  }
  { return tostring_visitor(*pos); }
} catch (std::exception& ex) {
  throw runtime_error("JSONDocument")
      << "Failed calling value_at(): Caught exception:" << ex.what();
}
```

**Process**:
1. Extract array from document
2. Validate array is not empty
3. Validate index is within bounds
4. Advance iterator to index
5. Return value as string (serialize if object)

**TRACE Levels**: 40-41

**Throws**: `runtime_error` if array empty, index out of bounds, or other errors

### findChildValue (non-const wrapper)

```cpp
value_t& JSONDocument::findChildValue(path_t const& path)
```

**Purpose**: Non-const version that wraps const version.

**Implementation**:
```cpp
try {
  auto const& myslef = self();  // Note: typo "myslef"
  return const_cast<value_t&>(myslef.findChildValue(path));
} catch (std::exception& ex) {
  TLOG(42) << "findChildValue() Search failed; Error:" << ex.what();
  throw;
}
```

**Pattern**: const_cast delegation to const version.

**TRACE Level**: 42 (errors only)

## JSONDocumentBuilder Methods

### createFromData

```cpp
JSONDocumentBuilder& JSONDocumentBuilder::createFromData(JSONDocument doc)
```

**Purpose**: Critical method that wraps user data in proper database document structure.

**Implementation**:
```cpp
_overlay.reset(nullptr);

auto const document = std::move(doc);

TLOG(24) << "createFrom() begin args";

#ifdef EXTRA_TRACES
TLOG(24) << "createFrom() document=<" << document << ">";
#endif

_createFromTemplate({std::string{template__empty_document}});

{  // create a new document template using overlay classes
  auto ovl = std::make_unique<ovlDatabaseRecord>(_document._value);
  std::swap(_overlay, ovl);
}

_importUserData(document);

{  // refresh overlays
  auto ovl = std::make_unique<ovlDatabaseRecord>(_document._value);
  std::swap(_overlay, ovl);
}

TLOG(24) << "createFrom() end";

return self();
```

**Process**:
1. Reset overlay
2. Move user document
3. Create empty template
4. Create initial overlay
5. Import user data
6. Refresh overlay
7. Return self

**TRACE Level**: 24

### _importUserData (private)

```cpp
void JSONDocumentBuilder::_importUserData(JSONDocument const& document)
```

**Purpose**: Imports various optional user data fields into the document structure.

**Implementation**: Tries to import multiple optional fields:

#### 1. Document Metadata

```cpp
try {
  auto path = ""s + jsonliteral::document + jsonliteral::dot + jsonliteral::metadata;
  auto metadata = document.findChild(path);

  TLOG(26) << "_importUserData() Found document.metadata=<" << metadata << ">";

  _document.replaceChild(metadata, path);

} catch (notfound_exception const&) {
  TLOG(27) << "_importUserData() No document.metadata";
}
```

#### 2. Changelog

```cpp
try {
  auto data = document.findChild(jsonliteral::changelog);
  TLOG(28) << "_importUserData() Found converted.changelog=<" << data << ">";
  _document.replaceChild(data, jsonliteral::changelog);
} catch (notfound_exception const&) {
  TLOG(29) << "_importUserData() No converted.changelog";
}
```

#### 3. Origin

```cpp
try {
  auto data = document.findChild(jsonliteral::origin);
  TLOG(30) << "_importUserData() Found origin=<" << data << ">";
  _document.replaceChild(data, jsonliteral::origin);
} catch (notfound_exception const&) {
  TLOG(31) << "_importUserData() No origin";
}
```

#### 4. Collection

```cpp
try {
  auto collection = document.findChild(jsonliteral::collection);
  TLOG(32) << "_importUserData() Found origin=<" << collection << ">";  // Note: says "origin" but is collection
  _document.replaceChild(collection, jsonliteral::collection);
} catch (notfound_exception const&) {
  TLOG(33) << "_importUserData() No collection";
}
```

#### 5. Attachments

```cpp
try {
  auto attachments = document.findChild(jsonliteral::attachments);
  TLOG(34) << "_importUserData() Found origin=<" << attachments << ">";  // Note: says "origin" but is attachments
  _document.replaceChild(attachments, jsonliteral::attachments);
} catch (notfound_exception const&) {
  TLOG(35) << "_importUserData() No attachments";
}
```

#### 6. Document Data

```cpp
try {
  auto path = ""s + jsonliteral::document + jsonliteral::dot + jsonliteral::data;
  auto data = document.findChild(path);
  TLOG(36) << "_importUserData() Found document.data=<" << data << ">";
  _document.replaceChild(data, path);
  return;
} catch (notfound_exception const&) {
  TLOG(37) << "_importUserData() No document.data";
}
```

#### 7. Fallback: Entire Document as Data

```cpp
try {
  auto path = ""s + jsonliteral::document + jsonliteral::dot + jsonliteral::data;
  _document.replaceChild(document, path);
} catch (notfound_exception const&) {
  TLOG(38) << "_importUserData() No document.data";
}

TLOG(25) << "_importUserData() end";
```

**Strategy**:
- Try to import each optional field
- Silently skip if not found (catches `notfound_exception`)
- Log what was found or not found
- If no specific data field, treat entire document as data

**TRACE Levels**: 25-38

## Undo Mechanism

### SaveUndo

```cpp
result_t JSONDocumentBuilder::SaveUndo() {
  return Success();
}
```

**Current Implementation**: Placeholder - always returns success.

### CallUndo

```cpp
result_t JSONDocumentBuilder::CallUndo() noexcept try {
  return Success();
} catch (...) {
  return Failure();
}
```

**Current Implementation**: Placeholder - returns success unless exception thrown.

## Debug Function

### debug::JSONDocumentUtils

```cpp
void dbdr::debug::JSONDocumentUtils() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TLOG(43) << "artdaq::database::JSONDocument trace_enable";
}
```

**Purpose**: Enables maximum TRACE logging for utilities.

**TRACE Level**: 43

## Template Specializations

### toJSONDocument<string_pair_t>

```cpp
namespace artdaq::database::docrecord {
template <>
JSONDocument toJSONDocument<string_pair_t>(string_pair_t const& pair) {
  std::ostringstream oss;
  oss << '{';
  oss << db::quoted_(pair.first) << ":" << db::quoted_(pair.second);
  oss << '}';

  return {oss.str()};
}
}
```

**Purpose**: Converts a string pair to JSON document.

**Format**: `{"first": "second"}`

**Example**:
```cpp
string_pair_t pair("name", "value");
auto doc = toJSONDocument(pair);
// Result: {"name":"value"}
```

## Key Features and Patterns

### Lazy Serialization

Uses `_isDirty` flag and `_cached_json_buffer` for performance:
- Only serializes when dirty
- Caches result
- Invalidates cache on modification

### Partial Matching

`matches()` function supports partial object matching:
- Useful for finding objects in arrays
- Only requires subset of fields to match
- Flexible search capability

### Flexible Data Import

`_importUserData` handles multiple optional fields:
- Doesn't require specific structure
- Imports what's available
- Falls back to treating entire document as data

### Error Recovery

Most functions catch exceptions and:
- Log the error
- Re-throw with context
- Provide detailed error messages

## Performance Considerations

1. **Caching**: Lazy serialization reduces redundant JSON generation
2. **Move Semantics**: Used throughout for efficiency
3. **Reference Returns**: Avoids copies where possible
4. **Visitor Pattern**: Efficient type-safe variant access

## Thread Safety

Not thread-safe:
- Modifies mutable state
- No synchronization
- Intended for single-threaded use

## Known Issues

1. **Array Matching Bug**: Line 79 uses `unwrap(left)` twice instead of `unwrap(right)`
2. **TRACE Message Typos**: Some TRACE messages say "origin" when referring to other fields
3. **Variable Name Typo**: "myslef" instead of "myself"

## Related Files

- **JSONDocument.h/cpp** - Main document class
- **JSONDocumentBuilder.h/cpp** - Builder class
- **data_json.h** - JSON data types

## Best Practices

1. **Use caching**: Call `to_string()` multiple times without performance penalty
2. **Enable TRACE**: Use debug functions when troubleshooting
3. **Handle exceptions**: Wrap file I/O in try-catch blocks
4. **Partial matching**: Leverage for flexible array searches
5. **Import user data**: Use `createFromData` for proper document structure

## Notes

- File contains mix of member function implementations and free functions
- Extensive TRACE logging for debugging
- Supports flexible document formats through `getPayloadValueForKey`
- Backup functionality in `saveToFile` prevents data loss
- Path splitting enables dot-notation navigation
- Visitor pattern used extensively for type-safe variant access
