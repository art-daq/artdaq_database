# dispatch_signatures.cpp

**Path:** `artdaq-database/ConfigurationDB/dispatch_signatures.cpp`

**Implements:** [dispatch_signatures.h](./dispatch_signatures.h.md)

**Purpose:** Implements the global operations map singleton used by the command-line tool dispatch system. This minimal implementation file provides the `getOperations()` function that returns a reference to the static operations map where all CLI operations are registered.

## Implementation Overview

This file contains a single function implementation that manages a static singleton map using the Meyers Singleton pattern. The function-local static variable is initialized on first use and destroyed at program exit.

Key implementation details:
- Static local variable ensures single instance across all translation units
- Lazy initialization on first call to `getOperations()`
- Automatic cleanup when program terminates
- Thread-safe initialization (guaranteed by C++11 standard)

## Thread Safety

- **Thread-safe:** Partial
- **Concurrent access:**
  - Initialization is thread-safe (C++11 guarantees)
  - Concurrent reads after initialization are safe
  - Concurrent writes (registrations) are NOT safe
- **Locking:** Relies on compiler-provided magic statics for initialization

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/ConfigurationDB/dispatch_signatures.h` | Type definitions for `operations_map_t` and function declaration |

## Internal Functions

### `getOperations() -> operations_map_t&`

**Brief:** Returns a reference to the global singleton map that stores all registered CLI operations, using the Meyers Singleton pattern for safe initialization.

**Called by:**
- `registerOperation()` template function during initialization
- Operation dispatch code during runtime

**Purpose:** Provide a single, globally-accessible registry for CLI operations without requiring explicit initialization or causing static initialization order problems.

**Parameters:** None

**Preconditions:** None

**Returns:** Reference to the static `operations_map_t` instance

**Postconditions:**
- A valid reference is always returned
- The same map instance is returned on every call
- Map is empty on first access (before any registrations)

**Throws:** None (noexcept in practice, though not declared)

**Thread Safety:**
- First call: Thread-safe (C++11 magic statics guarantee)
- Subsequent calls: Thread-safe for reads
- Registration (map modification): Not thread-safe (caller must synchronize)

**Side Effects:** On first call, allocates the static map on the heap via default map allocator

**Complexity:** O(1) for subsequent calls after initialization

## Key Algorithms

### Meyers Singleton Pattern

```cpp
operations_map_t& getOperations() {
  static auto _operations_map = operations_map_t{};
  return _operations_map;
}
```

This pattern provides:

1. **Lazy initialization:** Map is only created when first needed, not at program startup
2. **Single instance:** Function-local static guarantees exactly one instance
3. **Thread-safe initialization:** C++11 standard guarantees thread-safe initialization of function-local statics
4. **Automatic cleanup:** Destructor is called at program exit during static destruction phase

**Why this approach:**
- Avoids the Static Initialization Order Fiasco (SIOF) - no dependencies on other static variables
- Simpler than explicit singleton class pattern with manual locking
- No manual memory management required
- Works correctly with dynamic library loading
- No need for "double-checked locking" or other complex patterns

### Initialization Sequence

```
First call to getOperations():
  1. Compiler-generated code acquires internal lock
  2. Checks if already initialized (via hidden flag)
  3. If not initialized:
     a. Calls operations_map_t default constructor
     b. Sets initialized flag
  4. Releases lock
  5. Returns reference to _operations_map

Subsequent calls:
  1. Checks initialized flag (typically a single branch)
  2. Returns reference to _operations_map
```

## Performance Considerations

- **First call:** Small overhead for initialization check and potential lock acquisition
- **Subsequent calls:** Minimal overhead (typically a single branch, often optimized away)
- **Memory:** Map allocated once on heap, lives until program exit
- **Optimization:** Modern compilers may inline the function and optimize the initialization check

## Error Handling Strategy

This function cannot fail under normal circumstances:
- Static initialization is guaranteed to succeed or terminate the program
- No exceptions are thrown from the default map constructor
- Reference return eliminates null pointer concerns
- No external resources are acquired that could fail

## Testing Notes

- **Unit tests:** Implicitly tested by any test that registers and invokes operations
- **Key test cases:**
  - Multiple calls return same instance
  - Registration persists across calls
  - Operations survive across translation units
  - Thread-safe initialization under concurrent access

## Maintenance Notes

### Static Lifetime

The operations map has static lifetime, meaning:
- It persists for the entire program execution
- Registered functors (stored as `unique_ptr`) are destroyed at program exit
- Order of destruction relative to other statics is unspecified

**Warning:** Do not access the operations map from destructors of other static objects, as the map may already be destroyed.

### Dynamic Libraries

When used across dynamic library boundaries:
- Each library may have its own copy if symbol is not properly exported
- On Linux/macOS, ensure the symbol is exported from a single shared library
- Consider using `__attribute__((visibility("default")))` on Linux
- On Windows, use `__declspec(dllexport)` / `__declspec(dllimport)`

### Alternative Implementations

If more control is needed in the future, consider:
- Explicit singleton class with controlled initialization order
- Thread-safe map wrapper (e.g., `std::shared_mutex`) for concurrent registration
- Custom allocator for memory tracking
- Explicit `initialize()` and `shutdown()` functions for deterministic lifecycle

### Code Simplicity

The implementation is intentionally minimal (10 lines of code). This simplicity:
- Reduces maintenance burden
- Minimizes potential for bugs
- Makes behavior easy to understand and verify
- Follows the principle of least surprise

## See Also

- [dispatch_signatures.h](./dispatch_signatures.h.md) - Header with type definitions and `registerOperation()` template
- [operation_dispatch.h](./operation_dispatch.h.md) - Functor base classes stored in the map
- [conftoolifc.h](./conftoolifc.h.md) - Uses this map for CLI command dispatch

---

**Documentation generated for artdaq-database ConfigurationDB module**
