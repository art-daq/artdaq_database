# operation_dispatch.h

**Path:** `artdaq-database/ConfigurationDB/operation_dispatch.h`

**Purpose:** Defines template-based functor classes that implement the operation dispatch pattern for the ConfigurationDB module. These functors wrap function pointers with their bound arguments, enabling deferred invocation through a uniform polymorphic interface. This pattern allows operations to be registered, stored, and invoked later without knowing the specific function signature at the call site.


## Key Concepts

### Functor Pattern

The functor classes in this header implement a command pattern variant that provides:

1. **Deferred Execution**: Store a function pointer and its arguments for later invocation
2. **Uniform Interface**: All functors expose the same `invoke()` method regardless of function signature
3. **Type Safety**: Template parameters ensure compile-time type checking of function signatures and arguments
4. **Polymorphism**: Base class `Functor<R>` enables runtime dispatch through virtual methods

### Why Use This Pattern?

In the ConfigurationDB module, different operations (read, write, find, etc.) have different signatures but need to be:
- Registered in a dispatch table
- Selected at runtime based on operation name
- Invoked uniformly through a common interface

The functor pattern enables this by wrapping heterogeneous function pointers in a homogeneous interface.

### Class Hierarchy

```
Functor<R>                        (abstract base - pure virtual invoke())
    |
    +-- RFunctor<R>               (wraps: R function())
    +-- RA1Functor<R, A1>         (wraps: R function(A1 const&))
    +-- RA1A2Functor<R, A1, A2>   (wraps: R function(A1 const&, A2 const&))
    +-- RA1R2Functor<R, A1, R2>   (wraps: R function(A1 const&, R2&))
    +-- VA1R2Functor<A1, R2>      (wraps: void function(A1 const&, R2&))
```

### Naming Convention

The class names encode the function signature:
- `R` = Return type (first position)
- `V` = Void return (first position)
- `A1`, `A2` = Const reference arguments
- `R2` = Non-const reference argument (output parameter)

## Thread Safety

- **Thread-safe:** No
- **Concurrent access:** Functors should not be shared across threads without external synchronization
- **Locking:** None - functors store references to external objects

**Important:** The functor classes store references to their arguments, not copies. The referenced objects must remain valid until `invoke()` is called.

## Dependencies

No external dependencies. Uses only standard C++ features.

| Include | Purpose |
|---------|---------|
| (none) | Header-only, no external dependencies |

## Classes

### `Functor<R>` (Base Class)

**Brief:** Abstract base class defining the polymorphic interface for all functor types. Provides the virtual `invoke()` method that derived classes implement.

```cpp
template <typename R>
struct Functor {
  virtual R invoke() = 0;
  virtual ~Functor() {}
};
```

**Template Parameters:**
- `R` - Return type of the wrapped function

**Thread Safety:** Not thread-safe

#### Methods

##### `invoke() -> R` [pure virtual]

**Brief:** Invokes the wrapped function with its bound arguments and returns the result.

**Returns:** The return value from the wrapped function call.

**Throws:** Any exceptions thrown by the wrapped function propagate through.

**Thread Safety:** Not thread-safe

---

##### `~Functor()` [virtual]

**Brief:** Virtual destructor enabling proper cleanup of derived functor objects through base class pointers.

**Thread Safety:** Not thread-safe

---

### `RFunctor<R>`

**Brief:** Functor wrapper for functions taking no arguments and returning type R.

```cpp
template <typename R>
class RFunctor : public Functor<R> {
public:
  typedef R (*FP)();
  RFunctor(FP fp);
  R invoke() override final;
private:
  FP _fp;
};
```

**Template Parameters:**
- `R` - Return type of the wrapped function

**Thread Safety:** Not thread-safe

#### Constructor

##### `RFunctor(FP fp)`

**Brief:** Constructs a functor wrapping a no-argument function.

**Parameters:**
- `fp` - Function pointer of type `R (*)()`

**Preconditions:**
- `fp` must be a valid, non-null function pointer

**Throws:**

| Exception | Condition |
|-----------|-----------|
| (undefined behavior) | If `fp` is null |

**Example:**
```cpp
#include "artdaq-database/ConfigurationDB/operation_dispatch.h"
#include <iostream>

namespace dispatch = artdaq::configuration::dispatch;

std::string getStatus() {
  return "OK";
}

void example() {
  try {
    auto functor = dispatch::RFunctor<std::string>(&getStatus);
    std::string result = functor.invoke();
    std::cout << "Status: " << result << "\n";  // Output: Status: OK
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << "\n";
  }
}
```

---

### `RA1Functor<R, A1>`

**Brief:** Functor wrapper for functions taking one const reference argument and returning type R.

```cpp
template <typename R, typename A1>
class RA1Functor : public Functor<R> {
public:
  typedef R (*FP)(A1 const&);
  RA1Functor(FP fp, A1 const& a1);
  R invoke() override final;
private:
  FP _fp;
  A1 const& _a1;
};
```

**Template Parameters:**
- `R` - Return type of the wrapped function
- `A1` - Type of the first (const reference) argument

**Thread Safety:** Not thread-safe

#### Constructor

##### `RA1Functor(FP fp, A1 const& a1)`

**Brief:** Constructs a functor wrapping a single-argument function with a bound argument.

**Parameters:**
- `fp` - Function pointer of type `R (*)(A1 const&)`
- `a1` - Const reference to the argument (must remain valid until `invoke()`)

**Preconditions:**
- `fp` must be a valid, non-null function pointer
- `a1` must remain valid until `invoke()` is called

**Throws:**

| Exception | Condition |
|-----------|-----------|
| (undefined behavior) | If `fp` is null or `a1` becomes invalid |

**Example:**
```cpp
#include "artdaq-database/ConfigurationDB/operation_dispatch.h"
#include <iostream>

namespace dispatch = artdaq::configuration::dispatch;

int processQuery(std::string const& query) {
  return static_cast<int>(query.length());
}

void example() {
  std::string query = "SELECT * FROM configs";
  try {
    auto functor = dispatch::RA1Functor<int, std::string>(&processQuery, query);
    int result = functor.invoke();
    std::cout << "Query length: " << result << "\n";  // Output: Query length: 23
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << "\n";
  }
}
```

---

### `RA1A2Functor<R, A1, A2>`

**Brief:** Functor wrapper for functions taking two const reference arguments and returning type R.

```cpp
template <typename R, typename A1, typename A2>
class RA1A2Functor : public Functor<R> {
public:
  typedef R (*FP)(A1 const&, A2 const&);
  RA1A2Functor(FP fp, A1 const& a1, A2 const& a2);
  R invoke() override final;
private:
  FP _fp;
  A1 const& _a1;
  A2 const& _a2;
};
```

**Template Parameters:**
- `R` - Return type of the wrapped function
- `A1` - Type of the first (const reference) argument
- `A2` - Type of the second (const reference) argument

**Thread Safety:** Not thread-safe

#### Constructor

##### `RA1A2Functor(FP fp, A1 const& a1, A2 const& a2)`

**Brief:** Constructs a functor wrapping a two-argument function with bound arguments.

**Parameters:**
- `fp` - Function pointer of type `R (*)(A1 const&, A2 const&)`
- `a1` - Const reference to the first argument
- `a2` - Const reference to the second argument

**Preconditions:**
- `fp` must be a valid, non-null function pointer
- Both `a1` and `a2` must remain valid until `invoke()` is called

**Throws:**

| Exception | Condition |
|-----------|-----------|
| (undefined behavior) | If `fp` is null or arguments become invalid |

**Example:**
```cpp
#include "artdaq-database/ConfigurationDB/operation_dispatch.h"
#include "artdaq-database/JsonDocument/JSONDocument.h"
#include <iostream>

namespace dispatch = artdaq::configuration::dispatch;
using JSONDocument = artdaq::database::docrecord::JSONDocument;

JSONDocument combineDocuments(JSONDocument const& a, JSONDocument const& b) {
  // Combine logic...
  return JSONDocument{"{\"combined\": true}"};
}

void example() {
  JSONDocument doc1{"{\"id\": 1}"};
  JSONDocument doc2{"{\"id\": 2}"};
  try {
    auto functor = dispatch::RA1A2Functor<JSONDocument, JSONDocument, JSONDocument>(
        &combineDocuments, doc1, doc2);
    auto result = functor.invoke();
    std::cout << "Combined: " << result << "\n";
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << "\n";
  }
}
```

---

### `RA1R2Functor<R, A1, R2>`

**Brief:** Functor wrapper for functions taking one const reference argument and one non-const reference output argument, returning type R.

```cpp
template <typename R, typename A1, typename R2>
class RA1R2Functor : public Functor<R> {
public:
  typedef R (*FP)(A1 const&, R2&);
  RA1R2Functor(FP fp, A1 const& a1, R2& r2);
  R invoke() override final;
private:
  FP _fp;
  A1 const& _a1;
  R2& _r2;
};
```

**Template Parameters:**
- `R` - Return type of the wrapped function
- `A1` - Type of the first (const reference) argument
- `R2` - Type of the second (non-const reference) output argument

**Thread Safety:** Not thread-safe

#### Constructor

##### `RA1R2Functor(FP fp, A1 const& a1, R2& r2)`

**Brief:** Constructs a functor wrapping a function with one input and one output parameter.

**Parameters:**
- `fp` - Function pointer of type `R (*)(A1 const&, R2&)`
- `a1` - Const reference to the input argument
- `r2` - Non-const reference to the output argument (will be modified by `invoke()`)

**Preconditions:**
- `fp` must be a valid, non-null function pointer
- Both `a1` and `r2` must remain valid until `invoke()` is called

**Postconditions:**
- After `invoke()`, `r2` may contain modified output from the wrapped function

**Throws:**

| Exception | Condition |
|-----------|-----------|
| (undefined behavior) | If `fp` is null or arguments become invalid |

**Example:**
```cpp
#include "artdaq-database/ConfigurationDB/operation_dispatch.h"
#include <iostream>

namespace dispatch = artdaq::configuration::dispatch;

struct Result { bool success; std::string message; };

Result processWithOutput(std::string const& input, std::string& output) {
  output = "Processed: " + input;
  return {true, "OK"};
}

void example() {
  std::string input = "test data";
  std::string output;
  try {
    auto functor = dispatch::RA1R2Functor<Result, std::string, std::string>(
        &processWithOutput, input, output);
    Result result = functor.invoke();
    if (result.success) {
      std::cout << "Output: " << output << "\n";  // Output: Processed: test data
    }
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << "\n";
  }
}
```

---

### `VA1R2Functor<A1, R2>`

**Brief:** Functor wrapper for void functions taking one const reference argument and one non-const reference output argument.

```cpp
template <typename A1, typename R2>
class VA1R2Functor : public Functor<void> {
public:
  typedef void (*FP)(A1 const&, R2&);
  VA1R2Functor(FP fp, A1 const& a1, R2& r2);
  void invoke() override final;
private:
  FP _fp;
  A1 const& _a1;
  R2& _r2;
};
```

**Template Parameters:**
- `A1` - Type of the first (const reference) argument
- `R2` - Type of the second (non-const reference) output argument

**Thread Safety:** Not thread-safe

#### Constructor

##### `VA1R2Functor(FP fp, A1 const& a1, R2& r2)`

**Brief:** Constructs a functor wrapping a void function with input and output parameters.

**Parameters:**
- `fp` - Function pointer of type `void (*)(A1 const&, R2&)`
- `a1` - Const reference to the input argument
- `r2` - Non-const reference to the output argument

**Preconditions:**
- `fp` must be a valid, non-null function pointer
- Both `a1` and `r2` must remain valid until `invoke()` is called

**Postconditions:**
- After `invoke()`, `r2` may contain modified output from the wrapped function

**Throws:**

| Exception | Condition |
|-----------|-----------|
| (undefined behavior) | If `fp` is null or arguments become invalid |

**Example:**
```cpp
#include "artdaq-database/ConfigurationDB/operation_dispatch.h"
#include <vector>
#include <iostream>

namespace dispatch = artdaq::configuration::dispatch;

void transformData(std::string const& input, std::vector<int>& output) {
  output.push_back(static_cast<int>(input.length()));
}

void example() {
  std::string input = "hello";
  std::vector<int> output;
  try {
    auto functor = dispatch::VA1R2Functor<std::string, std::vector<int>>(
        &transformData, input, output);
    functor.invoke();
    std::cout << "Output size: " << output.size() << "\n";  // Output size: 1
    std::cout << "First element: " << output[0] << "\n";    // First element: 5
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << "\n";
  }
}
```

---

## Relationship to Other Components

- **dispatch_signatures.h** - Uses these functors to register operation handlers
- **conftoolifc.h** - Creates functors for each registered CLI operation
- **options_operations.h** - Operation options classes often passed as arguments to wrapped functions
- **Command-line tools** - Use the dispatch system to route operations by name

## Usage Example

```cpp
#include "artdaq-database/ConfigurationDB/operation_dispatch.h"
#include <memory>
#include <map>
#include <iostream>

namespace dispatch = artdaq::configuration::dispatch;

// Define result type
struct OperationResult {
  bool success;
  std::string message;
};

// Define operation functions
OperationResult readOperation(std::string const& query, std::string& output) {
  output = "Read result for: " + query;
  return {true, "success"};
}

OperationResult writeOperation(std::string const& query, std::string& output) {
  output = "Write completed";
  return {true, "success"};
}

void dispatchExample() {
  // Create dispatch table
  using FunctorPtr = std::unique_ptr<dispatch::Functor<OperationResult>>;
  std::map<std::string, FunctorPtr> operations;

  // Arguments must outlive functors
  std::string query = "SELECT * FROM configs";
  std::string output;

  // Register operations
  operations["read"] = std::make_unique<
      dispatch::RA1R2Functor<OperationResult, std::string, std::string>>(
      &readOperation, query, output);

  operations["write"] = std::make_unique<
      dispatch::RA1R2Functor<OperationResult, std::string, std::string>>(
      &writeOperation, query, output);

  // Dispatch by operation name with error handling
  std::string opName = "read";
  try {
    if (operations.count(opName)) {
      auto result = operations[opName]->invoke();
      if (result.success) {
        std::cout << "Output: " << output << "\n";
      } else {
        std::cerr << "Operation failed: " << result.message << "\n";
      }
    } else {
      std::cerr << "Unknown operation: " << opName << "\n";
    }
  } catch (const std::exception& e) {
    std::cerr << "Exception during operation: " << e.what() << "\n";
  }
}
```

## Notes for Developers

### Reference Lifetime

**Critical:** The functor classes store references, not copies, of their arguments. The referenced objects must remain valid until `invoke()` is called. Violating this leads to undefined behavior.

```cpp
// WRONG - dangling reference
dispatch::RA1Functor<int, std::string>* createFunctor() {
  std::string local = "temp";  // Local variable
  return new dispatch::RA1Functor<int, std::string>(&process, local);
  // 'local' is destroyed here, functor has dangling reference!
}

// CORRECT - ensure argument outlives functor
std::string persistent = "temp";
auto functor = dispatch::RA1Functor<int, std::string>(&process, persistent);
// 'persistent' remains valid
int result = functor.invoke();
```

### Move Semantics

Functors are not movable or copyable due to their reference members. Always use pointers (preferably smart pointers) when storing functors:

```cpp
// Use unique_ptr for ownership
auto functor = std::make_unique<dispatch::RFunctor<int>>(&myFunction);

// Store in containers via pointers
std::map<std::string, std::unique_ptr<dispatch::Functor<Result>>> operations;
```

### Final Override

The `invoke()` method is marked `override final` in all derived classes, preventing further overriding. This is intentional - extend the pattern by creating new functor classes, not by overriding existing ones.

### Exception Safety

The functor classes themselves do not throw exceptions. However, any exceptions thrown by the wrapped function will propagate through `invoke()`. Callers should handle exceptions appropriately:

```cpp
try {
  auto result = functor->invoke();
  // Process result
} catch (const std::runtime_error& e) {
  // Handle runtime errors from wrapped function
} catch (const std::exception& e) {
  // Handle other exceptions
}
```

## Common Pitfalls

- **Dangling references:** Always ensure arguments outlive the functor
- **Thread sharing:** Do not share functor instances across threads without synchronization
- **Null function pointers:** Validate function pointers before constructing functors
- **Forgetting virtual destructor:** Always delete through base class pointer to avoid leaks

### Anti-patterns

```cpp
// DON'T: Pass temporary objects
auto bad = dispatch::RA1Functor<int, std::string>(&process, std::string("temp"));
// The temporary is destroyed immediately, leaving a dangling reference!

// DO: Use persistent objects
std::string persistent = "temp";
auto good = dispatch::RA1Functor<int, std::string>(&process, persistent);
```

```cpp
// DON'T: Store functor by value (won't compile due to reference members)
// std::vector<dispatch::RA1Functor<int, std::string>> functors;  // ERROR

// DO: Store via pointers
std::vector<std::unique_ptr<dispatch::Functor<int>>> functors;
```

## See Also

- [dispatch_signatures.h](./dispatch_signatures.h.md) - Operation signature definitions
- [dispatch_signatures.cpp](./dispatch_signatures.cpp.md) - Operation signature implementations
- [conftoolifc.h](./conftoolifc.h.md) - CLI operation interface
- [options_operations.h](./options_operations.h.md) - Operation options classes

---

**Documentation generated for artdaq-database ConfigurationDB module**
