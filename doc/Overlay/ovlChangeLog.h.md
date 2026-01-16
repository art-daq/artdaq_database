# ovlChangeLog.h

**Path:** `artdaq-database/Overlay/ovlChangeLog.h`

**Purpose:** This header defines the `ovlChangeLog` overlay class, which provides specialized methods for accessing and appending human-readable changelog strings. It complements the structured update history in bookkeeping with free-form text descriptions, extending `ovlStringKeyValue` with convenient buffer access methods.


## Key Concepts

### Human-Readable Audit Trail

While bookkeeping provides structured update tracking with timestamps and values (machine-readable), the changelog provides a human-readable narrative of changes. Key differences:

| Aspect | Bookkeeping Updates | Changelog |
|--------|---------------------|-----------|
| Format | Structured JSON | Free-form text |
| Automatic | Yes (via postUpdate) | No (manual entry) |
| Queryable | Yes (by event/timestamp) | Limited (text search) |
| Human-readable | Limited | Yes |
| Growth | One entry per operation | Controlled by application |

### String Buffer Pattern

The changelog is stored as a single string that can be:
- **Read** via `buffer()` const method
- **Replaced** via `buffer(newValue)` setter
- **Appended** via `append(text)` method

This is simpler than maintaining an array of entries and allows applications to control their own formatting.

### Comparison Masking

Inherits masking from `ovlStringKeyValue<DOCUMENT_COMPARE_MUTE_CHANGELOG>`. When the `DOCUMENT_COMPARE_MUTE_CHANGELOG` flag is set in the global comparison mask, changelog differences are ignored during equality comparisons. This is useful in testing scenarios where changelog content should not affect comparison results.

## Thread Safety

- **Thread-safe:** No
- **Concurrent access:** References mutable JSON string
- **Locking:** None - callers must ensure single-threaded access

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/Overlay/common.h` | Module foundation types, constants, and comparison flags |
| `artdaq-database/Overlay/ovlKeyValue.h` | Base class for all overlay objects |
| `artdaq-database/Overlay/ovlStringKeyValue.h` | String-specialized template base with masking support |

## Classes/Structures

### `ovlChangeLog`

**Brief:** Overlay class for managing human-readable changelog strings, providing buffer access and append functionality with optional comparison masking.

**Thread Safety:** Not thread-safe

#### Inheritance

```
ovlKeyValue (base)
     ^
     |
ovlStringKeyValue<DOCUMENT_COMPARE_MUTE_CHANGELOG>
     ^
     |
ovlChangeLog (this class)
```

#### Constructor

##### `ovlChangeLog(object_t::key_type const& key, value_t& changelog)`

**Brief:** Constructs a changelog overlay wrapping a JSON string value. If the field does not exist or is empty, the base class handles default initialization.

**Parameters:**
- `key` - The JSON key (typically `"changelog"`)
- `changelog` - Reference to the JSON string value

**Preconditions:**
- `changelog` must be a JSON string (or convertible to string)

**Postconditions:**
- Overlay wraps the changelog string
- If string is empty or missing, base class provides default value (`"notprovided"`)

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `std::bad_cast` | If changelog is not a string type |

**Thread Safety:** Unsafe

**Example:**
```cpp
#include "artdaq-database/Overlay/ovlChangeLog.h"
#include <iostream>

using namespace artdaq::database::overlay;

void accessChangelog(value_t& recordJson) {
  try {
    auto changelog = overlay<ovlChangeLog, std::string>(recordJson, "changelog");
    std::cout << "Current changelog:\n" << changelog->buffer() << "\n";
  } catch (const std::bad_cast& e) {
    std::cerr << "Changelog is not a string: " << e.what() << "\n";
  } catch (const std::runtime_error& e) {
    std::cerr << "Failed to access changelog: " << e.what() << "\n";
  }
}
```

#### Special Member Functions

##### `ovlChangeLog(ovlChangeLog&&) = default`

**Brief:** Move constructor, allowing changelog overlays to be moved efficiently.

##### `~ovlChangeLog() = default`

**Brief:** Destructor. Non-virtual because this is a final class.

#### Methods

##### `buffer() -> std::string&`

**Brief:** Returns a mutable reference to the changelog string, allowing direct modification.

**Preconditions:**
- None

**Returns:** Reference to the underlying changelog string

**Postconditions:**
- None (returns reference to existing data)

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `std::bad_cast` | If the underlying value is not a string |

**Thread Safety:** Unsafe

**Example:**
```cpp
#include "artdaq-database/Overlay/ovlChangeLog.h"

using namespace artdaq::database::overlay;

void directModification(ovlChangeLog& changelog) {
  // Read current content
  std::cout << changelog.buffer();

  // Direct modification (replaces entire content)
  changelog.buffer() = "New changelog content\n";
}
```

---

##### `buffer() const -> std::string const&`

**Brief:** Returns a const reference to the changelog string for read-only access.

**Preconditions:**
- None

**Returns:** Const reference to the underlying changelog string

**Postconditions:**
- None

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `std::bad_cast` | If the underlying value is not a string |

**Thread Safety:** Unsafe

---

##### `buffer(std::string const& changelog) -> std::string&`

**Brief:** Replaces the entire changelog content with the given string and returns a reference to the new content. Use this to set the complete changelog at once.

**Parameters:**
- `changelog` - The new changelog content to set

**Preconditions:**
- None

**Returns:** Reference to the updated changelog string

**Postconditions:**
- Changelog content is replaced with the provided string

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `std::bad_cast` | If the underlying value is not a string |

**Thread Safety:** Unsafe

**Example:**
```cpp
#include "artdaq-database/Overlay/ovlChangeLog.h"

using namespace artdaq::database::overlay;

void replaceChangelog(ovlChangeLog& changelog) {
  // Replace entire changelog
  changelog.buffer("Initial version created.\nUpdated by admin.\n");
}
```

---

##### `append(std::string const& changelog) -> std::string&`

**Brief:** Appends text to the existing changelog string and returns a reference to the result. This is the most common operation for adding changelog entries.

**Parameters:**
- `changelog` - Text to append to the existing changelog

**Preconditions:**
- None

**Returns:** Reference to the updated changelog string (after append)

**Postconditions:**
- The provided text is concatenated to the existing changelog

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `std::bad_cast` | If the underlying value is not a string |

**Thread Safety:** Unsafe

**Side Effects:**
- Modifies the underlying JSON string value

**Example:**
```cpp
#include "artdaq-database/Overlay/ovlChangeLog.h"
#include <iostream>

using namespace artdaq::database::overlay;

void addChangelogEntries(ovlChangeLog& changelog) {
  try {
    // Append new entries (remember to add newlines)
    changelog.append("2025-01-15: Updated trigger configuration.\n");
    changelog.append("2025-01-16: Added new board reader entity.\n");

    std::cout << "Updated changelog:\n" << changelog.buffer() << "\n";
  } catch (const std::bad_cast& e) {
    std::cerr << "Type error: " << e.what() << "\n";
  }
}
```

---

##### `operator==(ovlChangeLog const& other) const -> result_t`

**Brief:** Compares two changelog overlays for equality, respecting the comparison mask.

**Parameters:**
- `other` - The changelog overlay to compare against

**Preconditions:**
- None

**Returns:** `result_t` pair where:
- `first` is `true` if equal (or masked), `false` otherwise
- `second` contains difference details if not equal

**Postconditions:**
- None (read-only comparison)

**Throws:**

| Exception | Condition |
|-----------|-----------|
| None | Comparison does not throw |

**Thread Safety:** Unsafe

**Comparison Logic:**
- If `DOCUMENT_COMPARE_MUTE_CHANGELOG` flag is set, returns success immediately
- Otherwise, delegates to base class string comparison

**Example:**
```cpp
#include "artdaq-database/Overlay/ovlChangeLog.h"
#include <iostream>

using namespace artdaq::database::overlay;

void compareChangelogs(ovlChangeLog const& log1, ovlChangeLog const& log2) {
  auto result = log1 == log2;
  if (result.first) {
    std::cout << "Changelogs are equal (or comparison was masked)\n";
  } else {
    std::cout << "Changelogs differ: " << result.second << "\n";
  }
}
```

## Type Aliases

### `ovlChangeLogUPtr_t`

```cpp
using ovlChangeLogUPtr_t = std::unique_ptr<ovlChangeLog>;
```

**Brief:** Convenience type alias for unique pointer to ovlChangeLog.

## Relationship to Other Components

### In the Overlay Hierarchy

```
ovlKeyValue (base)
     ^
     |
ovlStringKeyValue<DOCUMENT_COMPARE_MUTE_CHANGELOG>
     ^
     |
ovlChangeLog (this class)
```

### Used By

- **ovlDatabaseRecord** - Contains changelog as one of its 14 components
- **Configuration management tools** - Write user-visible change notes

### JSON Structure

```json
{
  "changelog": "Initial creation by admin.\nUpdated by operator on 2025-01-15.\nConfiguration modified.\n"
}
```

Note: The changelog is a single string, not an array. Applications control the formatting.

## See Also

- [ovlChangeLog.cpp](./ovlChangeLog.cpp.md) - Implementation file
- [ovlStringKeyValue.h](./ovlStringKeyValue.h.md) - Base template class
- [ovlKeyValue.h](./ovlKeyValue.h.md) - Root base class
- [ovlBookkeeping.h](./ovlBookkeeping.h.md) - Structured update tracking (complements changelog)
- [ovlDatabaseRecord.h](./ovlDatabaseRecord.h.md) - Parent document record class

## Notes for Developers

### Complete Usage Example

```cpp
#include "artdaq-database/Overlay/JSONDocumentOverlay.h"
#include "artdaq-database/SharedCommon/helper_functions.h"
#include <sstream>
#include <iostream>

using namespace artdaq::database::overlay;

void updateChangelog(value_t& recordJson) {
  try {
    // Create changelog overlay (creates field if missing)
    auto changelog = overlay<ovlChangeLog, std::string>(recordJson, "changelog");

    // Access current content
    std::cout << "Current changelog:\n" << changelog->buffer() << "\n";

    // Replace entire changelog
    changelog->buffer("Initial version created.\n");

    // Append entries with proper formatting
    changelog->append("Updated trigger configuration.\n");
    changelog->append("Added new board reader entity.\n");

    std::cout << "Updated changelog:\n" << changelog->buffer() << "\n";
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << "\n";
  }
}
```

### Recommended Entry Format

```cpp
#include "artdaq-database/Overlay/ovlChangeLog.h"
#include "artdaq-database/SharedCommon/helper_functions.h"
#include <sstream>

using namespace artdaq::database::overlay;

void addChangelogEntry(ovlChangeLog& changelog,
                       const std::string& username,
                       const std::string& description) {
  std::ostringstream entry;
  entry << "[" << artdaq::database::timestamp() << "] "
        << "User: " << username << " - "
        << description << "\n";
  changelog.append(entry.str());
}

// Usage:
void recordChange(ovlChangeLog& changelog) {
  try {
    addChangelogEntry(changelog, "admin", "Added configuration: DAQConfig");
    addChangelogEntry(changelog, "operator", "Modified trigger threshold to 100");
  } catch (const std::exception& e) {
    std::cerr << "Failed to record change: " << e.what() << "\n";
  }
}
```

### Performance Considerations

- **buffer() access:** O(1) - direct reference return
- **buffer(string) setter:** O(n) - string copy/assignment
- **append():** O(n+m) - string concatenation where n=existing, m=appended
- **Comparison:** O(n) - string comparison when not masked, O(1) when masked

### Common Pitfalls

- **No Automatic Formatting:** The `append()` method does simple concatenation. You must add newlines, timestamps, etc. yourself.
- **No Structured Access:** Unlike bookkeeping updates, you cannot query individual entries programmatically.
- **String Growth:** Large changelogs consume memory; consider periodic archiving for long-lived document records.
- **Missing Newlines:** Forgetting to add newlines results in unreadable changelog text.

### Anti-patterns

```cpp
// DON'T: Append without newline separator
changelog->append("First entry");
changelog->append("Second entry");  // Results in: "First entrySecond entry"

// DO: Include newlines
changelog->append("First entry\n");
changelog->append("Second entry\n");  // Results in: "First entry\nSecond entry\n"

// DON'T: Use buffer() when append() is appropriate
std::string current = changelog->buffer();
changelog->buffer(current + "New entry\n");  // Inefficient!

// DO: Use append() directly
changelog->append("New entry\n");  // More efficient

// DON'T: Forget error handling
auto& buf = changelog->buffer();  // May throw std::bad_cast!

// DO: Handle potential type errors
try {
  auto& buf = changelog->buffer();
  // use buf
} catch (const std::bad_cast& e) {
  // handle error
}
```

### Best Practices

1. **Add newlines** when appending entries for readability
2. **Include timestamps** in changelog entries for temporal tracking
3. **Be concise** - summary of changes, not complete technical details
4. **Format consistently** - use a standard entry format across the application
5. **Use append()** rather than replacing buffer to preserve history
6. **Consider bookkeeping** for machine-readable audit trail in addition to changelog
7. **Handle exceptions** when accessing buffer to catch type mismatches

### Important Notes

- **Final class** - Cannot be derived from
- **Human-readable format** - Not structured JSON, plain text
- **No automatic formatting** - Applications control timestamps, usernames, etc.
- **Inherits default value** - Empty string if field is missing becomes `"notprovided"` (from ovlStringKeyValue)
- **Mask-aware comparison** - Use `DOCUMENT_COMPARE_MUTE_CHANGELOG` to ignore in tests
- **Simple concatenation** - `append()` does `+=`, no separator added
- **Return chaining** - Both `buffer(string)` and `append()` return references for method chaining:
  ```cpp
  changelog->buffer("Initial").append(" - more").append(" - end\n");
  ```
