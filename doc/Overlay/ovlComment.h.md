# ovlComment.h

**Path:** `artdaq-database/Overlay/ovlComment.h`

**Purpose:** Defines the `ovlComment` overlay class for managing configuration comments with line number tracking. Comments are associated with specific line numbers in configuration files, allowing human-readable annotations to be stored alongside configuration data in the database. This enables documentation of specific configuration sections to be preserved through storage and retrieval cycles.


## Key Concepts

### Line-Associated Comments

Each comment is associated with a specific line number, allowing comments to be tied to particular sections of a configuration file. This is useful for preserving context when configurations are stored in the database. For example, a comment explaining why a particular trigger threshold was chosen can be attached to the exact line where that threshold is defined.

### Simple Two-Field Structure

Comments contain only two fields:
- `linenum` - The line number in the source file (integer)
- `value` - The comment text (string)

This minimal structure keeps the JSON representation compact while providing the essential information needed to reconstruct annotated configuration files.

### Direct Comparison

Unlike most overlay classes, `ovlComment`'s comparison operator does not use masking - it always performs a direct comparison of the underlying JSON values. Masking is applied at the list level (`ovlComments`) rather than individual comment level. This design allows fine-grained control over whether comments are included in document comparisons.

## Thread Safety

- **Thread-safe:** No
- **Concurrent access:** Not supported; external synchronization required
- **Locking:** None

All methods access mutable JSON data through references without synchronization. If multiple threads need to access the same `ovlComment` instance, external locking (such as a mutex) must be used.

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/Overlay/common.h` | Module foundation types, constants, result type, and comparison flags |
| `artdaq-database/Overlay/ovlKeyValue.h` | Base class providing JSON key-value access and `value_as<T>()` template |

## Classes/Structures

### `ovlComment`

A type-safe overlay for configuration comments, inheriting from `ovlKeyValue`. Provides accessors for line number and comment text, along with serialization and comparison capabilities. This class wraps a JSON object containing `linenum` and `value` fields.

**Thread Safety:** Not thread-safe; references mutable JSON data.

#### Constructor

##### `ovlComment(object_t::key_type const& key, value_t& comment)`

**Brief:** Constructs an overlay for a comment JSON object with the specified key, enabling type-safe access to line number and text fields.

**Parameters:**
- `key` - The JSON key under which this comment is stored (typically "comment" or an index when in a list)
- `comment` - Reference to the JSON object containing `linenum` and `value` fields

**Preconditions:**
- `comment` must be a valid JSON object or convertible to one
- The JSON object should contain `linenum` (integer) and `value` (string) fields for accessors to work

**Postconditions:**
- Overlay is ready for use with accessors
- The overlay holds a reference to the original JSON; modifications through the overlay affect the underlying JSON

**Throws:**
| Exception | Condition |
|-----------|-----------|
| None | Construction does not throw; issues manifest when accessing missing fields |

**Thread Safety:** Not applicable (construction)

**Example:**
```cpp
#include "artdaq-database/Overlay/ovlComment.h"
#include <iostream>

using namespace artdaq::database::overlay;

void createComment() {
  try {
    // Create JSON structure for a comment
    value_t commentJson = object_t{};
    commentJson["linenum"] = 42;
    commentJson["value"] = "Trigger threshold for production runs";

    // Create overlay
    ovlComment comment("comment", commentJson);

    // Access fields
    std::cout << "Line " << comment.linenum() << ": " << comment.text() << std::endl;
    // Output: Line 42: Trigger threshold for production runs
  } catch (const std::exception& e) {
    std::cerr << "Error creating comment: " << e.what() << std::endl;
  }
}
```

#### Special Member Functions

##### `ovlComment(ovlComment&&) = default`

**Brief:** Default move constructor allowing transfer of overlay ownership. The moved-from object should not be used after the move.

**Thread Safety:** Not applicable (construction)

##### `~ovlComment() = default`

**Brief:** Default destructor; no special cleanup required as the overlay does not own the underlying JSON data.

**Thread Safety:** Not applicable (destruction)

#### Methods

##### `linenum() -> integer&`

**Brief:** Returns a mutable reference to the line number associated with this comment, allowing modification of the line number value.

**Preconditions:**
- The underlying JSON object must contain a `linenum` field of integer type

**Returns:** Reference to the integer line number, allowing modification.

**Postconditions:**
- Modifications through the returned reference affect the underlying JSON

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `std::bad_cast` | If the `linenum` field is not an integer type |
| `std::out_of_range` | If the `linenum` field does not exist |

**Thread Safety:** Unsafe - reads and potentially modifies shared mutable state

**Example:**
```cpp
#include "artdaq-database/Overlay/ovlComment.h"

using namespace artdaq::database::overlay;

void updateLineNumber(ovlComment& comment) {
  try {
    // Read current line number
    std::cout << "Current line: " << comment.linenum() << std::endl;

    // Modify line number
    comment.linenum() = 100;
    std::cout << "Updated line: " << comment.linenum() << std::endl;
  } catch (const std::exception& e) {
    std::cerr << "Error accessing linenum: " << e.what() << std::endl;
  }
}
```

##### `linenum() const -> integer const&`

**Brief:** Returns a const reference to the line number associated with this comment for read-only access.

**Preconditions:**
- The underlying JSON object must contain a `linenum` field of integer type

**Returns:** Const reference to the integer line number.

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `std::bad_cast` | If the `linenum` field is not an integer type |
| `std::out_of_range` | If the `linenum` field does not exist |

**Thread Safety:** Unsafe (reads mutable JSON without synchronization)

##### `text() -> std::string&`

**Brief:** Returns a mutable reference to the comment text content, allowing modification of the comment message.

**Preconditions:**
- The underlying JSON object must contain a `value` field of string type

**Returns:** Reference to the string comment text, allowing modification.

**Postconditions:**
- Modifications through the returned reference affect the underlying JSON

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `std::bad_cast` | If the `value` field is not a string type |
| `std::out_of_range` | If the `value` field does not exist |

**Thread Safety:** Unsafe - reads and potentially modifies shared mutable state

**Example:**
```cpp
#include "artdaq-database/Overlay/ovlComment.h"

using namespace artdaq::database::overlay;

void updateCommentText(ovlComment& comment) {
  try {
    // Append to existing comment
    comment.text() += " - Updated for v2.0";
    std::cout << "New text: " << comment.text() << std::endl;
  } catch (const std::exception& e) {
    std::cerr << "Error accessing text: " << e.what() << std::endl;
  }
}
```

##### `text() const -> std::string const&`

**Brief:** Returns a const reference to the comment text content for read-only access.

**Preconditions:**
- The underlying JSON object must contain a `value` field of string type

**Returns:** Const reference to the string comment text.

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `std::bad_cast` | If the `value` field is not a string type |
| `std::out_of_range` | If the `value` field does not exist |

**Thread Safety:** Unsafe (reads mutable JSON without synchronization)

##### `to_string() const -> std::string` [override]

**Brief:** Serializes the comment to a JSON-formatted string for debugging and logging purposes.

**Preconditions:**
- Both `linenum` and `value` fields must be accessible

**Returns:** JSON representation like `{"linenum": 42, "value": "Configuration comment"}`

**Postconditions:**
- Returns a valid JSON string representation
- Does not modify the underlying JSON

**Throws:**
| Exception | Condition |
|-----------|-----------|
| `std::exception` | If field access fails |

**Thread Safety:** Unsafe (reads mutable JSON)

**Example:**
```cpp
#include "artdaq-database/Overlay/ovlComment.h"
#include <iostream>

using namespace artdaq::database::overlay;

void debugComment(const ovlComment& comment) {
  try {
    std::cout << "Comment JSON: " << comment.to_string() << std::endl;
    // Output: Comment JSON: {"linenum":42,"value":"Some comment"}
  } catch (const std::exception& e) {
    std::cerr << "Error serializing comment: " << e.what() << std::endl;
  }
}
```

##### `operator==(ovlComment const& other) const -> result_t`

**Brief:** Compares this comment with another for equality using direct comparison of underlying JSON values without masking.

**Parameters:**
- `other` - The comment to compare against

**Preconditions:**
- Both comments must have valid underlying JSON data

**Returns:** `result_t` where `first` is `true` if comments are equal, `false` otherwise; `second` contains error details on mismatch.

**Postconditions:**
- Neither comment is modified

**Throws:**
| Exception | Condition |
|-----------|-----------|
| None | Comparison does not throw |

**Thread Safety:** Unsafe (reads mutable JSON from both objects)

**Example:**
```cpp
#include "artdaq-database/Overlay/ovlComment.h"
#include <iostream>

using namespace artdaq::database::overlay;
using namespace artdaq::database::result;

void compareComments() {
  try {
    value_t json1 = object_t{};
    json1["linenum"] = 10;
    json1["value"] = "First comment";
    ovlComment comment1("comment", json1);

    value_t json2 = object_t{};
    json2["linenum"] = 10;
    json2["value"] = "First comment";
    ovlComment comment2("comment", json2);

    auto result = comment1 == comment2;
    if (result.first) {
      std::cout << "Comments are equal" << std::endl;
    } else {
      std::cout << "Comments differ: " << result.second << std::endl;
    }
  } catch (const std::exception& e) {
    std::cerr << "Error comparing comments: " << e.what() << std::endl;
  }
}
```

## Type Aliases

```cpp
using ovlCommentUPtr_t = std::unique_ptr<ovlComment>;
```

Unique pointer type for managing `ovlComment` instances. Used when ownership transfer is needed, such as when adding comments to a comment list.

## Relationship to Other Components

### In the Overlay Hierarchy

```
ovlKeyValue (base)
     ^
     |
ovlComment (this class)
```

`ovlComment` inherits from `ovlKeyValue`, gaining access to the `value_as<T>()` template method and other base class utilities for JSON field access.

### Used By

- **ovlComments** - Defined as `ovlFixedList<ovlComment, DOCUMENT_COMPARE_MUTE_COMMENTS>` in `ovlDatabaseRecord.h`. This list container holds multiple comments for a document.
- **ovlDatabaseRecord** - Contains `ovlComments` as one of its 14 components, providing comment support for database documents.

### JSON Structure

```json
{
  "comments": [
    {"linenum": 10, "value": "Start of DAQ configuration"},
    {"linenum": 25, "value": "Trigger settings section"},
    {"linenum": 50, "value": "End of configuration"}
  ]
}
```

## Notes for Developers

### Usage Example

```cpp
#include "artdaq-database/Overlay/ovlComment.h"
#include <iostream>
#include <vector>

using namespace artdaq::database::overlay;

void workWithComments() {
  try {
    // Create JSON for multiple comments
    std::vector<value_t> commentJsons;

    // First comment
    value_t comment1Json = object_t{};
    comment1Json["linenum"] = 10;
    comment1Json["value"] = "DAQ configuration header";
    commentJsons.push_back(std::move(comment1Json));

    // Second comment
    value_t comment2Json = object_t{};
    comment2Json["linenum"] = 25;
    comment2Json["value"] = "Trigger parameters - DO NOT MODIFY";
    commentJsons.push_back(std::move(comment2Json));

    // Create overlays and work with them
    for (size_t i = 0; i < commentJsons.size(); ++i) {
      ovlComment comment(std::to_string(i), commentJsons[i]);

      // Read values
      std::cout << "Line " << comment.linenum() << ": " << comment.text() << std::endl;

      // Serialize for debugging
      std::cout << "  JSON: " << comment.to_string() << std::endl;
    }
  } catch (const std::exception& e) {
    std::cerr << "Error working with comments: " << e.what() << std::endl;
  }
}
```

### Common Pitfalls

- **Field Names:** Line number is stored in `linenum` field (not "line" or "line_number"); text is stored in `value` field (not "text" or "comment"). Using wrong field names will cause exceptions.
- **No Auto-Initialization:** Unlike some overlays, `ovlComment` does not auto-initialize missing fields; the JSON must contain `linenum` and `value` before accessor methods are called.
- **Masking Level:** Comparison masking (`DOCUMENT_COMPARE_MUTE_COMMENTS`) is applied at the `ovlComments` list level, not at individual comment level. Direct comparison with `operator==` always performs full comparison.
- **Reference Lifetime:** The overlay holds a reference to the original JSON. Ensure the JSON outlives the overlay.

### Anti-patterns

```cpp
// DON'T: Assume comment text field is named "text"
auto& text = comment.value_as<std::string>("text");  // Wrong field name - throws

// DO: Use the text() accessor
auto& text = comment.text();  // Correct - accesses "value" field

// DON'T: Create overlay without required fields
value_t emptyJson = object_t{};
ovlComment comment("comment", emptyJson);
auto line = comment.linenum();  // Throws - field doesn't exist

// DO: Initialize required fields first
value_t json = object_t{};
json["linenum"] = 0;
json["value"] = "";
ovlComment comment("comment", json);
auto line = comment.linenum();  // Works
```

## See Also

- [ovlComment.cpp](./ovlComment.cpp.md) - Implementation file with method details
- [ovlKeyValue.h](./ovlKeyValue.h.md) - Base class providing `value_as<T>()` template
- [ovlFixedList.h](./ovlFixedList.h.md) - Container template used for `ovlComments`
- [ovlDatabaseRecord.h](./ovlDatabaseRecord.h.md) - Uses `ovlComments` as a component
- [common.h](./common.h.md) - Comparison flags and result types
