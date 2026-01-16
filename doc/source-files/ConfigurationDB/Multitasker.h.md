# Multitasker.h

**Path:** `artdaq-database/ConfigurationDB/Multitasker.h`

**Purpose:** Defines the `Multitasker` class, a thread pool implementation designed for parallel execution of database operations. This component enables efficient bulk configuration operations by distributing work across multiple worker threads, collecting results, and providing aggregated output.


## Key Concepts

### Thread Pool Pattern

The `Multitasker` implements a classic thread pool pattern:
- A fixed number of worker threads are created at construction
- Tasks are submitted to a shared queue
- Workers pull tasks from the queue and execute them
- Results are collected for later retrieval

This pattern avoids the overhead of creating/destroying threads for each task and provides controlled parallelism.

### Single-Owner Design

**Critical:** The `Multitasker` is designed for single-threaded ownership. All public methods (except `isBusy()`) must be called from the same thread that created the instance. This simplifies the API and prevents common concurrency bugs.

```cpp
// CORRECT: Same thread creates and uses
void processConfigs() {
  Multitasker mt(4);
  mt.addTask(task1);
  mt.addTask(task2);
  mt.waitForResults();
  auto results = mt.getResults();
}

// INCORRECT: Sharing across threads - will throw!
Multitasker mt(4);
std::thread t1([&mt]() { mt.addTask(task1); });  // THROWS
std::thread t2([&mt]() { mt.addTask(task2); });  // THROWS
```

### Result Collection

Tasks return `result_t` pairs (success boolean + message string). The Multitasker collects all results and can provide them individually or merged into a single JSON summary.

## Thread Safety

- **Thread-safe:** Conditional - see details below
- **Concurrent access:** Worker threads access internal state concurrently (protected by mutex)
- **Locking:** Single mutex (`_one4all_mutex`) protects all shared state

### Thread Safety Rules

| Operation | Calling Thread | Thread-Safe? |
|-----------|----------------|--------------|
| Constructor | Any | N/A (creates instance) |
| Destructor | Owner only | Must be same thread as constructor |
| `addTask()` | Owner only | Internal locking for queue access |
| `waitForResults()` | Owner only | Internal locking for condition wait |
| `getResults()` | Owner only | Internal locking for result access |
| `getMergedResults()` | Owner only | Internal locking for result access |
| `isBusy()` | Any thread | Fully thread-safe |

**Owner thread** = The thread that created the `Multitasker` instance.

## Dependencies

| Include | Purpose |
|---------|---------|
| `<algorithm>` | std::min, std::count_if for result processing |
| `<atomic>` | std::atomic for stop flag and task counter |
| `<chrono>` | Time measurement and timeouts |
| `<condition_variable>` | Worker thread synchronization |
| `<functional>` | std::function for task callbacks |
| `<iostream>` | Stream operations (for debugging) |
| `<list>` | std::list for result storage |
| `<mutex>` | std::mutex for thread safety |
| `<queue>` | std::queue for task queue |
| `<random>` | Random number generation (included but unused) |
| `<sstream>` | String stream for result formatting |
| `<string>` | std::string |
| `<thread>` | std::thread for worker threads |
| `<vector>` | std::vector for thread storage |
| `artdaq-database/SharedCommon/returned_result.h` | result_t type definition |

## Classes

### `Multitasker`

A thread pool implementation for parallel execution of database operations with result collection.

**Thread Safety:** Single-owner design - create one instance per controlling thread.

#### Constructor

##### `Multitasker(size_t numThreads = std::thread::hardware_concurrency() / 2)`

**Brief:** Creates a thread pool with the specified number of worker threads.

**Parameters:**
- `numThreads` - Number of worker threads (default: half of hardware concurrency)

**Preconditions:**
- None (automatically clamps to valid range)

**Postconditions:**
- `numThreads` worker threads are running and waiting for tasks
- Thread count is clamped to `[1, hardware_concurrency/2]`

**Thread Safety:** N/A - construction creates new object

**Example:**
```cpp
#include "artdaq-database/ConfigurationDB/Multitasker.h"

using namespace artdaq::database::configuration;

void example() {
  // Use default thread count (half of CPU cores)
  Multitasker mt1;

  // Specify 4 worker threads
  Multitasker mt2(4);

  // Request 100 threads - will be clamped to hardware_concurrency/2
  Multitasker mt3(100);
}
```

---

#### Destructor

##### `~Multitasker()`

**Brief:** Signals all worker threads to stop, waits for them to complete, and cleans up resources.

**Preconditions:**
- Must be called from the same thread that created the instance (asserted)

**Postconditions:**
- All worker threads have terminated
- All resources are released

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `std::runtime_error` (assertion) | Called from different thread than constructor |

**Thread Safety:** Must be called from owner thread only

**Side Effects:**
- Logs warning if tasks are still pending when destroyed
- Blocks until all threads join

---

#### Methods

##### `addTask(task) -> void`

**Brief:** Adds a task to the execution queue for asynchronous processing by a worker thread.

**Parameters:**
- `task` - Function returning `result_t` with no parameters

**Preconditions:**
- Called from owner thread
- Stop has not been signaled (destructor not called)

**Postconditions:**
- Task is queued for execution
- One worker thread is notified

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `std::runtime_error` | Called from wrong thread |
| `std::runtime_error` | Called after stop was signaled |

**Thread Safety:** Must be called from owner thread; internal locking for queue

**Example:**
```cpp
#include "artdaq-database/ConfigurationDB/Multitasker.h"
#include "artdaq-database/ConfigurationDB/configurationdbifc.h"

using namespace artdaq::database::configuration;
using namespace artdaq::database;

void storeConfigsParallel(const std::vector<ConfigData>& configs) {
  Multitasker mt(4);

  for (const auto& config : configs) {
    mt.addTask([config]() -> result_t {
      try {
        ConfigurationInterface ifc;
        return ifc.storeConfiguration(config.name, config.data);
      } catch (const std::exception& e) {
        return {false, e.what()};
      }
    });
  }

  mt.waitForResults();
  auto merged = mt.getMergedResults();

  if (!merged.first) {
    std::cerr << "Some operations failed: " << merged.second << std::endl;
  }
}
```

---

##### `waitForResults() const -> void`

**Brief:** Blocks until all submitted tasks have completed execution.

**Preconditions:**
- Called from owner thread

**Postconditions:**
- All tasks have completed (queue is empty, results collected)

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `std::runtime_error` | Called from wrong thread |

**Thread Safety:** Must be called from owner thread; uses condition variable internally

**Side Effects:**
- Logs warning after 15 seconds of waiting
- Logs error every 15 seconds thereafter

**Example:**
```cpp
Multitasker mt(4);

// Submit many tasks
for (int i = 0; i < 100; ++i) {
  mt.addTask([i]() { return processItem(i); });
}

// Block until all complete
mt.waitForResults();

// Now safe to get results
auto results = mt.getResults();
```

---

##### `getResults() -> std::list<result_t>`

**Brief:** Retrieves individual results from all completed tasks and clears the internal result storage.

**Preconditions:**
- Called from owner thread
- All tasks must be complete (call `waitForResults()` first)

**Returns:** List of `result_t` pairs, one per submitted task

**Postconditions:**
- Internal results list is cleared
- Task counter is reset to 0
- Instance is ready for new tasks

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `std::runtime_error` | Called from wrong thread |
| `std::runtime_error` | Called while tasks are still pending |

**Thread Safety:** Must be called from owner thread; internal locking

**Example:**
```cpp
Multitasker mt(4);

mt.addTask(task1);
mt.addTask(task2);
mt.addTask(task3);

mt.waitForResults();

auto results = mt.getResults();

int successCount = 0;
for (const auto& result : results) {
  if (result.first) {
    ++successCount;
  } else {
    std::cerr << "Task failed: " << result.second << std::endl;
  }
}
std::cout << "Succeeded: " << successCount << "/" << results.size() << std::endl;
```

---

##### `getMergedResults() -> result_t`

**Brief:** Retrieves all results merged into a single JSON-formatted result with summary statistics.

**Preconditions:**
- Called from owner thread
- All tasks must be complete (call `waitForResults()` first)

**Returns:** Single `result_t` where:
- `first`: `true` if ALL tasks succeeded, `false` if any failed
- `second`: JSON string with format shown below

**Result Format:**
```json
{
  "results": [
    {"status": true, "message": "Success message"},
    {"status": false, "message": "Error message"}
  ],
  "total": 10,
  "errors": 2
}
```

**Postconditions:**
- Internal results list is cleared (same as `getResults()`)
- Instance is ready for new tasks

**Throws:**

| Exception | Condition |
|-----------|-----------|
| `std::runtime_error` | Called from wrong thread |
| `std::runtime_error` | Called while tasks are still pending |

**Thread Safety:** Must be called from owner thread; internal locking

**Example:**
```cpp
Multitasker mt(4);

// Submit tasks
for (const auto& item : items) {
  mt.addTask([item]() { return processItem(item); });
}

mt.waitForResults();
auto merged = mt.getMergedResults();

if (merged.first) {
  std::cout << "All " << items.size() << " operations succeeded" << std::endl;
} else {
  std::cerr << "Operation summary:\n" << merged.second << std::endl;
}
```

---

##### `isBusy() const -> bool`

**Brief:** Checks whether tasks are still pending or executing.

**Returns:** `true` if tasks remain in queue or are being executed; `false` if all complete

**Thread Safety:** Fully thread-safe - can be called from any thread

**Complexity:** O(1)

**Example:**
```cpp
Multitasker mt(4);

mt.addTask(longRunningTask);

// Poll-based waiting with progress updates
while (mt.isBusy()) {
  std::cout << "." << std::flush;
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
}
std::cout << "\nComplete!" << std::endl;

auto results = mt.getResults();
```

---

#### Private Methods

##### `workerThread() -> void`

**Brief:** Main loop executed by each worker thread. Waits for tasks, executes them, and stores results.

**Called by:** Constructor (spawned as separate threads)

**Behavior:**
1. Wait on condition variable for task or stop signal
2. If stop signaled and queue empty, exit
3. Pop task from queue
4. Execute task with timing
5. Store result (success or exception caught)
6. Notify waiting threads
7. Repeat

---

##### `signalStop() -> void`

**Brief:** Sets the stop flag and notifies all waiting threads.

**Called by:** Destructor

---

##### `checkSameThreadOrThrow() const -> void`

**Brief:** Verifies the caller is the owner thread, throwing if not.

**Throws:** `std::runtime_error` if called from different thread than constructor

---

#### Private Members

| Member | Type | Purpose |
|--------|------|---------|
| `_threads` | `std::vector<std::thread>` | Worker thread storage |
| `_condition` | `std::condition_variable` | Worker synchronization |
| `_one4all_mutex` | `std::mutex` | Protects all shared state |
| `_tasks` | `std::queue<std::function<result_t()>>` | Pending task queue |
| `_results` | `std::list<result_t>` | Completed task results |
| `_shouldStop` | `std::atomic<bool>` | Stop signal flag |
| `_totalTasks` | `std::atomic<std::size_t>` | Total tasks submitted |
| `_threadid` | `const std::thread::id` | Owner thread ID |

## Functions

### `debug::Multitasker() -> void`

**Brief:** Enables full TRACE logging for the Multitasker component.

**Thread Safety:** Safe (configures thread-local trace state)

**Side Effects:**
- Sets TRACE_NAME to "Multitasker.cpp"
- Enables all trace levels
- Configures trace modes

**Example:**
```cpp
#include "artdaq-database/ConfigurationDB/Multitasker.h"

// Enable debugging
artdaq::database::configuration::debug::Multitasker();

// Now all TLOG messages from Multitasker will be visible
Multitasker mt(4);
mt.addTask(someTask);
// Debug output shows task submission, execution, completion
```

## Usage Examples

### Basic Parallel Processing

```cpp
#include "artdaq-database/ConfigurationDB/Multitasker.h"

using namespace artdaq::database::configuration;

void parallelProcessing() {
  Multitasker mt(4);

  // Submit tasks
  for (int i = 0; i < 100; ++i) {
    mt.addTask([i]() -> result_t {
      // Simulate work
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      return {true, "Processed item " + std::to_string(i)};
    });
  }

  // Wait and collect
  mt.waitForResults();
  auto merged = mt.getMergedResults();

  std::cout << merged.second << std::endl;
}
```

### Bulk Configuration Storage

```cpp
#include "artdaq-database/ConfigurationDB/Multitasker.h"
#include "artdaq-database/ConfigurationDB/configurationdbifc.h"

using namespace artdaq::database::configuration;

result_t storeConfigurationsBulk(
    const std::vector<std::pair<std::string, JsonData>>& configs) {

  Multitasker mt;  // Default thread count

  for (const auto& [name, data] : configs) {
    mt.addTask([name, data]() -> result_t {
      try {
        ConfigurationInterface ifc;
        return ifc.storeConfiguration(name, data);
      } catch (const std::exception& e) {
        return {false, std::string("Failed to store ") + name + ": " + e.what()};
      }
    });
  }

  mt.waitForResults();
  return mt.getMergedResults();
}
```

### Reusing Instance for Multiple Batches

```cpp
void processBatches() {
  Multitasker mt(4);

  // Batch 1
  for (const auto& item : batch1) {
    mt.addTask([item]() { return process(item); });
  }
  mt.waitForResults();
  auto results1 = mt.getResults();

  // Batch 2 - reuse same thread pool
  for (const auto& item : batch2) {
    mt.addTask([item]() { return process(item); });
  }
  mt.waitForResults();
  auto results2 = mt.getResults();
}
```

## Relationship to Other Components

```
configurationdbifc.h (ConfigurationInterface)
        |
        | uses for storeGlobalConfiguration_mt()
        v
Multitasker.h (this file)
        |
        | returns
        v
SharedCommon/returned_result.h (result_t type)
```

- **configurationdbifc.h**: Uses Multitasker for `storeGlobalConfiguration_mt()` method
- **returned_result.h**: Provides the `result_t` type used for task returns

## Common Pitfalls

### Calling from Wrong Thread

```cpp
// DON'T: Share instance across threads
Multitasker mt(4);
std::thread t([&mt]() {
  mt.addTask(task);  // THROWS: wrong thread!
});

// DO: Create instance in the thread that will use it
std::thread t([]() {
  Multitasker mt(4);
  mt.addTask(task);
  mt.waitForResults();
});
```

### Getting Results Before Waiting

```cpp
// DON'T: Get results without waiting
Multitasker mt(4);
mt.addTask(slowTask);
auto results = mt.getResults();  // THROWS: tasks not complete!

// DO: Always wait first
Multitasker mt(4);
mt.addTask(slowTask);
mt.waitForResults();
auto results = mt.getResults();  // OK
```

### Capturing by Reference in Lambdas

```cpp
// DON'T: Capture by reference when object may go out of scope
for (const auto& item : items) {
  mt.addTask([&item]() {  // DANGER: item reference may dangle!
    return process(item);
  });
}

// DO: Capture by value
for (const auto& item : items) {
  mt.addTask([item]() {  // OK: item is copied
    return process(item);
  });
}
```

## Notes for Developers

### Performance Tuning

- Default thread count (`hardware_concurrency/2`) is conservative
- For I/O-bound tasks, consider using more threads
- For CPU-bound tasks, fewer threads may be better
- Monitor with TRACE logging to identify bottlenecks

### Error Handling in Tasks

Tasks should catch exceptions and return error results:

```cpp
mt.addTask([&]() -> result_t {
  try {
    // Your logic here
    return {true, "Success"};
  } catch (const std::exception& e) {
    return {false, e.what()};
  }
});
```

Uncaught exceptions are caught by the worker and converted to error results, but explicit handling provides better error messages.

## See Also

- [Multitasker.cpp](./Multitasker.cpp.md) - Implementation details
- [configurationdbifc.h](./configurationdbifc.h.md) - Uses Multitasker for bulk operations
- [returned_result.h](../SharedCommon/returned_result.h.md) - result_t type definition

---

**Documentation generated for artdaq-database ConfigurationDB module**
