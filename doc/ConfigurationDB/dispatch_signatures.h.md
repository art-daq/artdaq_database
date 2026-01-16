# dispatch_signatures.h

**Path:** `artdaq-database/ConfigurationDB/dispatch_signatures.h`

**Purpose:** Defines the operation signature types and registration infrastructure for the command-line tool dispatch system. This header provides type aliases for common operation signatures and a template-based registration mechanism for mapping operation names to their implementing functions at runtime.


## Key Concepts

### Operation Registration System

This header implements a registration-based dispatch pattern used primarily by the `conftool` CLI:
1. Operations are registered by name during program initialization
2. Each operation maps to a functor containing a function pointer and bound arguments
3. The dispatch system invokes the appropriate functor based on the operation name from command-line arguments

This pattern enables:
- **Extensibility:** New operations can be added without modifying dispatch logic
- **Decoupling:** Operation implementations are separate from the CLI framework
- **Runtime binding:** Operations are selected at runtime based on user input

### Functor-Based Dispatch

The system uses polymorphic functors (from `operation_dispatch.h`) to store operations with different signatures in a single map. Each functor encapsulates:
- A function pointer to the operation implementation
- Bound arguments (references to input/output strings)
- An `invoke()` method to execute the operation

### Signature Type Aliases

Three signature patterns are defined to handle common operation calling conventions:
- **opsig_str_t:** Operations that take one string (query) and return a result
- **opsig_strstr_t:** Operations that take two strings (query and data) and return a result
- **opsig_str_rstr_t:** Operations that take a query string and modify an output string

## Thread Safety

- **Thread-safe:** No
- **Concurrent access:** Operations map is not thread-safe; registration must complete before concurrent invocations
- **Locking:** No internal locking

**Important:** All operation registrations must be completed during single-threaded initialization before any concurrent operation invocations begin.

## Dependencies

| Include | Purpose |
|---------|---------|
| `<cstring>` | C-style string utilities |
| `<map>` | Container for operation name to functor mapping |
| `<memory>` | `std::unique_ptr` for functor ownership |
| `<string>` | String type for operation names and data |
| `<utility>` | `std::pair` for result type, `std::forward` for perfect forwarding |
| `artdaq-database/ConfigurationDB/operation_dispatch.h` | Functor base class and template implementations (`Functor`, `RA1Functor`, `RA1A2Functor`, `RA1R2Functor`) |

## Type Aliases

### `result_t`

```cpp
using result_t = std::pair<bool, std::string>;
```

**Brief:** Standard result type returned by all registered operations, containing a success flag and a result or error message.

**Components:**
- `first` (bool): Success (`true`) or failure (`false`) status
- `second` (string): Result data on success, or error message on failure

**Example:**
```cpp
result_t success_result = {true, "Operation completed successfully"};
result_t error_result = {false, "Database connection failed"};
```

### `opsig_str_t`

```cpp
using opsig_str_t = RA1Functor<result_t, std::string>;
```

**Brief:** Signature for operations that take one const string reference and return a result.

**Function Signature:** `result_t function(std::string const&)`

**Use case:** Operations that only need a query string, with results returned in the `result_t`. Examples include list operations and simple queries.

### `opsig_strstr_t`

```cpp
using opsig_strstr_t = RA1A2Functor<result_t, std::string, std::string>;
```

**Brief:** Signature for operations that take two const string references and return a result.

**Function Signature:** `result_t function(std::string const&, std::string const&)`

**Use case:** Operations that need both a query and input data, such as write operations where the first argument is options and the second is the document content.

### `opsig_str_rstr_t`

```cpp
using opsig_str_rstr_t = RA1R2Functor<result_t, std::string, std::string>;
```

**Brief:** Signature for operations that take one const string and one non-const string reference for output.

**Function Signature:** `result_t function(std::string const&, std::string&)`

**Use case:** Operations that modify an output buffer, such as read operations where the query is passed in and results are written to the second string.

### `operations_map_t`

```cpp
using operations_map_t = std::map<std::string, std::unique_ptr<Functor<result_t>>>;
```

**Brief:** Map type that stores registered operations, mapping operation names to their polymorphic functor wrappers.

**Key:** Operation name (e.g., "readdocument", "writedocument")

**Value:** Unique pointer to polymorphic `Functor<result_t>` base class

## Functions

### `getOperations() -> operations_map_t&`

**Brief:** Returns a reference to the global singleton map containing all registered operations for the CLI dispatch system.

**Parameters:** None

**Preconditions:** None

**Returns:** Reference to the static `operations_map_t` instance

**Postconditions:**
- Returns the same map instance on every call
- Map is empty on first call, populated via `registerOperation()`

**Throws:** None

**Thread Safety:**
- Initialization is thread-safe (C++11 magic statics)
- Concurrent reads after initialization are safe
- Concurrent writes are NOT safe

**Side Effects:** On first call, allocates the static map

**Example:**
```cpp
#include "artdaq-database/ConfigurationDB/dispatch_signatures.h"

namespace cf = artdaq::database::configuration;

void checkOperationExists(std::string const& opName) {
  auto& ops = cf::getOperations();
  if (ops.count(opName) > 0) {
    std::cout << "Operation '" << opName << "' is registered\n";
  } else {
    std::cout << "Operation '" << opName << "' not found\n";
  }
}

void listAllOperations() {
  auto& ops = cf::getOperations();
  std::cout << "Registered operations:\n";
  for (const auto& [name, functor] : ops) {
    std::cout << "  - " << name << "\n";
  }
}
```

### `registerOperation<SIG, Args...>(std::string name, Args&&... args) -> operations_map_t&`

**Brief:** Registers an operation with the dispatch system by creating a functor and storing it in the operations map under the specified name.

**Template Parameters:**
- `SIG` - Functor signature type (e.g., `opsig_str_t`, `opsig_strstr_t`, `opsig_str_rstr_t`)
- `Args` - Variadic template for constructor arguments (function pointer and bound arguments)

**Parameters:**
- `name` - `std::string` operation name used for lookup during dispatch
- `args` - Forwarded arguments for functor construction (typically function pointer followed by argument references)

**Preconditions:**
- `SIG` must be a valid functor type from `operation_dispatch.h`
- Arguments must match the functor's constructor signature
- Must be called during single-threaded initialization

**Returns:** Reference to the operations map (allows chaining)

**Postconditions:**
- Operation is registered in the global map
- If `name` already exists, previous registration is replaced

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `std::bad_alloc` | Memory allocation failure for functor |

**Thread Safety:** Not thread-safe - must be called during single-threaded initialization

**Side Effects:** Modifies the global operations map

**Example:**
```cpp
#include "artdaq-database/ConfigurationDB/dispatch_signatures.h"

namespace cf = artdaq::database::configuration;

// Example operation function matching opsig_str_rstr_t signature
cf::result_t myReadOperation(std::string const& query, std::string& output) {
  try {
    // Implementation: parse query and populate output
    output = "{ \"result\": \"data from query\" }";
    return {true, "Success"};
  } catch (const std::exception& e) {
    return {false, std::string("Read failed: ") + e.what()};
  }
}

// Example operation function matching opsig_strstr_t signature
cf::result_t myWriteOperation(std::string const& options, std::string const& data) {
  try {
    // Implementation: write data using options
    return {true, "Document written successfully"};
  } catch (const std::exception& e) {
    return {false, std::string("Write failed: ") + e.what()};
  }
}

// Global variables to hold bound arguments (must outlive operations map)
std::string g_query_json;
std::string g_output_buffer;
std::string g_options_json;
std::string g_data_json;

void registerMyOperations() {
  // Register a read operation with opsig_str_rstr_t signature
  cf::registerOperation<cf::opsig_str_rstr_t>(
    "my_read",                              // Operation name
    myReadOperation,                        // Function pointer
    std::cref(g_query_json),                // Bound argument 1 (const ref)
    std::ref(g_output_buffer)               // Bound argument 2 (mutable ref)
  );

  // Register a write operation with opsig_strstr_t signature
  cf::registerOperation<cf::opsig_strstr_t>(
    "my_write",                             // Operation name
    myWriteOperation,                       // Function pointer
    std::cref(g_options_json),              // Bound argument 1 (const ref)
    std::cref(g_data_json)                  // Bound argument 2 (const ref)
  );
}
```

## Relationship to Other Components

```
dispatch_signatures.h (this file)
       |
       +-- Uses: operation_dispatch.h (Functor base class templates)
       |
       +-- Implemented by: dispatch_signatures.cpp (getOperations singleton)
       |
       +-- Used by: conftoolifc.h (CLI command dispatch)
       |
       +-- Used by: dboperation_*.cpp (operation function implementations)
```

- **operation_dispatch.h:** Provides the `Functor` base class and `RA1Functor`, `RA1A2Functor`, `RA1R2Functor` template classes
- **dispatch_signatures.cpp:** Implements `getOperations()` singleton
- **conftoolifc.h:** Uses `registerOperation()` to set up CLI tool command dispatch
- **dboperation_*.cpp:** Contains the operation functions that are registered

## Notes for Developers

### Registration Timing

Operations must be registered before any dispatch attempts:
```cpp
// Correct: Register during initialization
int main() {
  registerAllOperations();  // Registration phase - single threaded

  auto& ops = getOperations();
  // Now safe to invoke operations concurrently
}
```

### Ownership Model

The operations map takes ownership of functors via `unique_ptr`:
- Functors are heap-allocated during registration
- Automatic cleanup when map is destroyed at program exit
- Duplicate registrations delete previous functor

### Name Collisions

Registering an operation with an existing name silently replaces the previous registration:
```cpp
registerOperation<opsig_str_t>("read", funcA, ...);
registerOperation<opsig_str_t>("read", funcB, ...);  // Replaces funcA
```

### Common Pitfalls

- **Pitfall 1:** Dangling references - Ensure bound arguments outlive the operations map. Arguments are stored by reference, not copied.

```cpp
// WRONG: Local variables go out of scope
void badRegistration() {
  std::string query = "...";
  std::string output;
  registerOperation<opsig_str_rstr_t>("op", func, query, output);
}  // query and output destroyed, but references remain in functor!

// CORRECT: Use variables with sufficient lifetime (global or static)
std::string g_query;
std::string g_output;
void goodRegistration() {
  registerOperation<opsig_str_rstr_t>("op", func, g_query, g_output);
}
```

- **Pitfall 2:** Thread-unsafe registration - All registrations must complete before concurrent operation invocations begin.

- **Pitfall 3:** Forgetting to use `std::ref`/`std::cref` - Without these wrappers, arguments may be copied instead of referenced:

```cpp
// WRONG: Without std::ref, a copy might be made
registerOperation<opsig_str_rstr_t>("op", func, query, output);

// CORRECT: Explicitly use reference wrappers
registerOperation<opsig_str_rstr_t>("op", func, std::cref(query), std::ref(output));
```

## See Also

- [operation_dispatch.h](./operation_dispatch.h.md) - Functor base class and templates
- [dispatch_signatures.cpp](./dispatch_signatures.cpp.md) - Implementation of `getOperations()` singleton
- [conftoolifc.h](./conftoolifc.h.md) - CLI tool dispatch interface using this system

---

**Documentation generated for artdaq-database ConfigurationDB module**
