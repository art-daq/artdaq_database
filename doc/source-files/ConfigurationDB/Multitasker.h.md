# Multitasker.h / Multitasker.cpp

## File Overview

These files implement a thread pool class for executing multiple database operations concurrently. The `Multitasker` class manages a pool of worker threads that process tasks in parallel, collect results, and provide merged output - essential for optimizing bulk configuration operations.

**Location**:
- `/home/user/artdaq-database/artdaq-database/ConfigurationDB/Multitasker.h`
- `/home/user/artdaq-database/artdaq-database/ConfigurationDB/Multitasker.cpp`

**Lines of Code**: 54 (header) + 214 (implementation)

**Purpose**: Thread pool for parallel execution of database operations with result collection

## Dependencies

### Standard Library
- `<algorithm>` - std::min, std::count_if
- `<atomic>` - std::atomic for thread-safe counters
- `<chrono>` - Time measurement and delays
- `<condition_variable>` - Thread synchronization
- `<functional>` - std::function for task callbacks
- `<iostream>` - Stream operations
- `<list>` - std::list for results storage
- `<mutex>` - std::mutex for thread safety
- `<queue>` - std::queue for task queue
- `<random>` - Random number generation (included but unused)
- `<sstream>` - String stream operations
- `<string>` - std::string
- `<thread>` - std::thread for worker threads
- `<vector>` - std::vector for thread storage

### Project Headers
- `"artdaq-database/SharedCommon/returned_result.h"` - result_t type definition
- `"artdaq-database/ConfigurationDB/common.h"` - Common macros and includes

## Namespace: artdaq::database::configuration

All types and functions are declared within the `artdaq::database::configuration` namespace.

---

## Class: Multitasker

### Purpose

`Multitasker` is a thread pool implementation designed for parallel execution of database operations. It manages worker threads, distributes tasks, collects results, and provides thread-safe result aggregation. Particularly useful for bulk configuration operations that can be parallelized.

---

## Public Methods

### Constructor

```cpp
Multitasker(size_t numThreads = std::thread::hardware_concurrency() / 2)
```

**Purpose**: Create a multitasker with specified number of worker threads.

**Parameters**:
- `numThreads` - Number of worker threads to create (default: half of hardware concurrency)

**Thread Pool Size**:
- Clamped to maximum of `hardware_concurrency / 2`
- Minimum of 1 thread
- Conservative default to avoid oversubscription

**Initialization**:
1. Sets stop flag to false
2. Initializes task counter to 0
3. Records creating thread ID
4. Spawns worker threads

**Usage Example**:
```cpp
// Use default thread count (hardware_concurrency / 2)
Multitasker mt;

// Use specific thread count
Multitasker mt_custom(4);

// Will use min(8, hardware_concurrency/2)
Multitasker mt_large(8);
```

**Logging**: Logs thread pool size at TLOG level 20.

---

### Destructor

```cpp
~Multitasker()
```

**Purpose**: Stop worker threads and wait for completion.

**Behavior**:
1. Verifies called from same thread as constructor (throws if not)
2. Sets stop flag to signal workers
3. Notifies all worker threads
4. Waits for all threads to join
5. Logs completion

**Thread Safety**: Must be called from the same thread that created the instance.

**Cleanup**: Automatically handles all cleanup - no manual thread management needed.

**Logging**:
- Level 20: Stop request and join completion
- TLVL_ERROR: If instance is being destroyed while busy

**Exception Safety**: Will throw if called from different thread than constructor.

---

### addTask

```cpp
void addTask(const std::function<result_t()>& task)
```

**Purpose**: Add a task to the execution queue.

**Parameters**:
- `task` - Function that returns `result_t` (no parameters)

**Thread Safety**: Thread-safe - can be called from the creating thread only

**Throws**: `std::runtime_error` if:
- Called from different thread than constructor
- Called after stop has been signaled

**Behavior**:
1. Validates caller thread matches constructor thread
2. Checks if stop has been signaled
3. Adds task to queue
4. Increments task counter
5. Notifies one worker thread

**Usage Example**:
```cpp
Multitasker mt(4);

// Add tasks for parallel execution
for (auto const& config : configurations) {
    mt.addTask([config]() {
        // Perform database operation
        return performDatabaseOp(config);
    });
}

mt.waitForResults();
auto results = mt.getResults();
```

**Performance**: Lock held only briefly - minimal contention.

**Logging**: Logs task addition at TLOG level 21 with task count.

---

### waitForResults

```cpp
void waitForResults() const
```

**Purpose**: Block until all submitted tasks have completed.

**Thread Safety**: Must be called from same thread as constructor

**Behavior**:
1. Validates caller thread
2. Polls `isBusy()` every 10ms
3. Logs warnings if takes too long (15+ seconds)
4. Returns when all tasks complete

**Blocking**: This method blocks the calling thread.

**Usage Pattern**:
```cpp
Multitasker mt;

// Submit tasks
mt.addTask(task1);
mt.addTask(task2);
mt.addTask(task3);

// Wait for completion
mt.waitForResults();

// Now safe to get results
auto results = mt.getResults();
```

**Logging**:
- TLVL_WARNING: After 15 seconds
- TLVL_ERROR: Every 15 seconds thereafter
- Level 23: When starting wait

**Best Practice**: Always call before `getResults()` or `getMergedResults()`.

---

### getResults

```cpp
std::list<result_t> getResults()
```

**Purpose**: Retrieve individual results from all completed tasks.

**Returns**: `std::list<result_t>` - List of task results

**Thread Safety**: Must be called from same thread as constructor

**Throws**: `std::runtime_error` if tasks are still pending

**Behavior**:
1. Validates caller thread
2. Checks all tasks completed (throws if not)
3. Swaps internal results list with empty list
4. Resets task counter
5. Returns results

**Usage Example**:
```cpp
mt.waitForResults();
auto results = mt.getResults();

for (auto const& result : results) {
    if (result.first) {
        std::cout << "Success: " << result.second << "\n";
    } else {
        std::cerr << "Error: " << result.second << "\n";
    }
}
```

**Post-Condition**: Internal results are cleared - can submit new tasks after calling.

**Logging**: Logs result count at TLOG level 22, errors at TLVL_ERROR.

---

### getMergedResults

```cpp
result_t getMergedResults()
```

**Purpose**: Get all task results merged into a single JSON result.

**Returns**: `result_t` - Merged result with JSON summary

**Thread Safety**: Must be called from same thread as constructor

**Behavior**:
1. Calls `getResults()` to retrieve all results
2. Counts successes and errors
3. Builds JSON summary with all results
4. Returns merged result

**Result Format**:
```json
{
  "results": [
    {"status": true, "message": "Success message"},
    {"status": false, "message": "Error message"},
    ...
  ],
  "total": 10,
  "errors": 2
}
```

**Success Determination**: Result is successful only if all tasks succeeded (error count == 0).

**Usage Example**:
```cpp
mt.waitForResults();
auto merged = mt.getMergedResults();

if (merged.first) {
    std::cout << "All tasks succeeded\n";
} else {
    std::cerr << "Some tasks failed:\n" << merged.second << "\n";
}
```

**Logging**: Logs merge operation and error count at TLOG level 22.

---

### isBusy

```cpp
bool isBusy() const
```

**Purpose**: Check if tasks are still pending or being executed.

**Returns**: `true` if tasks remain, `false` if all complete

**Thread Safety**: Thread-safe (uses mutex)

**Busy Conditions**:
- Task queue is not empty (tasks waiting)
- OR results count < total tasks submitted (tasks executing)

**Usage**:
```cpp
Multitasker mt;
mt.addTask(task1);
mt.addTask(task2);

while (mt.isBusy()) {
    // Do other work while tasks execute
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

// All tasks complete
auto results = mt.getResults();
```

**Logging**: Logs busy state at TLOG level 24.

---

## Private Methods

### workerThread

```cpp
void workerThread()
```

**Purpose**: Main loop for worker threads - processes tasks from queue.

**Lifecycle**:
1. Records thread start time
2. Logs thread startup
3. Enters main loop:
   - Waits for task or stop signal
   - Retrieves task from queue
   - Executes task with timing
   - Stores result
   - Handles exceptions
4. Logs thread shutdown with total runtime

**Exception Handling**:
- Catches `std::exception` - stores error message in result
- Catches unknown exceptions - stores generic error message
- Task errors logged at TLVL_ERROR or TLVL_WARNING

**Timing**: Measures and logs execution time for each task.

**Thread-Safety**: Uses condition variable and mutex for coordination.

**Logging**:
- Level 30: Thread start/stop
- Level 31: Task start/completion
- TLVL_WARNING: Task returned error result
- TLVL_ERROR: Task threw exception

---

### signalStop

```cpp
void signalStop()
```

**Purpose**: Signal all worker threads to stop.

**Behavior**:
1. Sets stop flag
2. Notifies all waiting threads

**Usage**: Called by destructor - not typically called by users.

---

### checkSameThreadOrThrow

```cpp
void checkSameThreadOrThrow() const
```

**Purpose**: Verify caller is the same thread that created the instance.

**Throws**: `std::runtime_error` if called from different thread

**Rationale**: Multitasker is not designed to be shared across threads - each thread should create its own instance.

**Error Message**: "Do not share the Multitasker instance across different threads."

---

## Private Members

### _threads
```cpp
std::vector<std::thread> _threads;
```

**Purpose**: Storage for worker thread objects.

**Size**: Equal to number of threads specified in constructor.

---

### _condition
```cpp
std::condition_variable _condition;
```

**Purpose**: Condition variable for worker thread synchronization.

**Usage**: Workers wait on this when queue is empty; notified when task added or stop signaled.

---

### _one4all_mutex
```cpp
mutable std::mutex _one4all_mutex;
```

**Purpose**: Single mutex protecting all shared state.

**Protected State**: Task queue, results list, stop flag, task counter.

**Design**: Single mutex simplifies locking protocol and prevents deadlocks.

---

### _tasks
```cpp
std::queue<std::function<result_t()>> _tasks;
```

**Purpose**: FIFO queue of pending tasks.

**Thread-Safety**: Protected by `_one4all_mutex`.

**Task Type**: Functions taking no parameters, returning `result_t`.

---

### _results
```cpp
std::list<result_t> _results;
```

**Purpose**: Collection of completed task results.

**Thread-Safety**: Protected by `_one4all_mutex`.

**Container Choice**: List for efficient insertion from multiple threads.

---

### _shouldStop
```cpp
std::atomic<bool> _shouldStop;
```

**Purpose**: Flag indicating worker threads should stop.

**Thread-Safety**: Atomic for lock-free reads in some contexts.

**Set By**: Destructor when shutting down.

---

### _totalTasks
```cpp
std::atomic<std::size_t> _totalTasks;
```

**Purpose**: Total count of tasks submitted.

**Usage**: Used to determine when all tasks complete (results.size() == _totalTasks).

**Thread-Safety**: Atomic for lock-free updates.

---

### _threadid
```cpp
const std::thread::id _threadid;
```

**Purpose**: ID of the thread that created this Multitasker instance.

**Usage**: Validated in all public methods to prevent cross-thread usage.

**Initialization**: Set in constructor to `std::this_thread::get_id()`.

---

## Debug Functions

### artdaq::database::configuration::debug::Multitasker()

```cpp
void debug::Multitasker()
```

**Purpose**: Enable full trace logging for Multitasker.

**Usage**:
```cpp
#include "artdaq-database/ConfigurationDB/Multitasker.h"

artdaq::database::configuration::debug::Multitasker();
// Now all TLOG messages from Multitasker will be printed
```

**Effect**:
- Sets TRACE_NAME to "Multitasker.cpp"
- Enables all trace levels
- Sets trace modes

---

## Design Patterns

### Thread Pool Pattern

Classic thread pool with task queue:
1. Fixed number of worker threads
2. Shared task queue
3. Workers wait when idle
4. Automatic load balancing

**Benefits**:
- Avoids thread creation overhead
- Bounds resource usage
- Efficient for many small tasks

---

### RAII for Thread Management

```cpp
Multitasker mt(4);  // Threads created

// Use mt...

// Destructor automatically stops threads
```

**Benefits**:
- Exception-safe cleanup
- No manual thread management
- Clear lifetime semantics

---

### Result Collection Pattern

Tasks return `result_t`, stored for later retrieval:

```cpp
using result_t = std::pair<bool, std::string>;
```

**Benefits**:
- Uniform error handling
- Easy aggregation
- Non-throwing task interface

---

## Usage Examples

### Basic Usage

```cpp
#include "artdaq-database/ConfigurationDB/Multitasker.h"

void parallelOperations() {
    Multitasker mt(4);  // 4 worker threads

    // Submit tasks
    for (int i = 0; i < 100; ++i) {
        mt.addTask([i]() {
            // Perform work
            return performDatabaseOperation(i);
        });
    }

    // Wait for completion
    mt.waitForResults();

    // Get merged result
    auto merged = mt.getMergedResults();

    if (merged.first) {
        std::cout << "All operations succeeded\n";
    } else {
        std::cerr << "Errors occurred:\n" << merged.second << "\n";
    }
}
```

---

### Configuration Storage

```cpp
void storeMultipleConfigurations(
    std::vector<Configuration> const& configs) {

    Multitasker mt;  // Use default thread count

    for (auto const& config : configs) {
        mt.addTask([&config]() {
            ConfigurationInterface ifc;
            return ifc.storeVersion<Configuration*, JsonData>(
                &config,
                config.version,
                config.entity
            );
        });
    }

    mt.waitForResults();
    auto results = mt.getResults();

    // Process individual results
    size_t success_count = 0;
    for (auto const& result : results) {
        if (result.first) {
            ++success_count;
        }
    }

    std::cout << "Stored " << success_count << " of "
              << configs.size() << " configurations\n";
}
```

---

### With Progress Reporting

```cpp
void operationsWithProgress() {
    const size_t total = 100;
    Multitasker mt(8);

    // Submit all tasks
    for (size_t i = 0; i < total; ++i) {
        mt.addTask([i]() {
            return performOperation(i);
        });
    }

    // Wait with progress reporting
    while (mt.isBusy()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        // Could add progress bar here
        std::cout << "." << std::flush;
    }
    std::cout << "\n";

    auto merged = mt.getMergedResults();
    std::cout << "Results: " << merged.second << "\n";
}
```

---

## Thread Safety

### Safe Operations
- `addTask()` - From creating thread only
- `waitForResults()` - From creating thread only
- `getResults()` - From creating thread only
- `getMergedResults()` - From creating thread only
- `isBusy()` - Thread-safe, can call from any thread

### Unsafe Operations
- **Sharing instance across threads**: FORBIDDEN - will throw
- **Destroying from different thread**: FORBIDDEN - will throw

### Design Philosophy

Each thread creates its own Multitasker instance:

```cpp
// GOOD: Each thread has its own instance
void thread1() {
    Multitasker mt;
    // Use mt...
}

void thread2() {
    Multitasker mt;
    // Use mt...
}

// BAD: Sharing instance
Multitasker global_mt;  // Don't do this!
void thread1() { global_mt.addTask(...); }  // Will throw!
void thread2() { global_mt.addTask(...); }  // Will throw!
```

---

## Performance Considerations

### Thread Count Selection

```cpp
// Conservative default (avoids oversubscription)
Multitasker mt;  // Uses hardware_concurrency / 2

// I/O-bound workload (can use more threads)
Multitasker mt_io(std::thread::hardware_concurrency());

// CPU-bound workload (use fewer threads)
Multitasker mt_cpu(std::thread::hardware_concurrency() / 4);
```

### Task Granularity

**Good Task Size**: 10ms - 1000ms per task
- Smaller: Too much overhead
- Larger: Poor load balancing

**Example**:
```cpp
// GOOD: Reasonable task size
for (auto const& config : configs) {  // Each takes ~100ms
    mt.addTask([config]() { return processConfig(config); });
}

// BAD: Too fine-grained
for (int i = 0; i < 1000000; ++i) {  // Each takes ~1us
    mt.addTask([i]() { return incrementCounter(i); });
}
```

---

## Error Handling

### Task Exceptions

Tasks should return errors in `result_t`:

```cpp
// GOOD: Return error
mt.addTask([]() {
    if (error_condition) {
        return result_t{false, "Error message"};
    }
    return result_t{true, "Success"};
});

// ACCEPTABLE: Throw exception (will be caught)
mt.addTask([]() {
    if (error_condition) {
        throw std::runtime_error("Error");
    }
    return result_t{true, "Success"};
});
```

Both patterns work, but returning errors is preferred for performance.

---

## Related Files

- **configurationdbifc.h** - Uses Multitasker for `storeGlobalConfiguration_mt`
- **common.h** - Common includes and macros
- **returned_result.h** - Definition of `result_t` type

---

## Best Practices

### Always Wait Before Getting Results

```cpp
Multitasker mt;
// ... add tasks ...

mt.waitForResults();  // MUST call this first
auto results = mt.getResults();  // Now safe
```

### Reuse Instance for Multiple Batches

```cpp
Multitasker mt(4);

// Batch 1
for (auto const& item : batch1) {
    mt.addTask([item]() { return process(item); });
}
mt.waitForResults();
auto results1 = mt.getResults();

// Batch 2 (reuse same instance)
for (auto const& item : batch2) {
    mt.addTask([item]() { return process(item); });
}
mt.waitForResults();
auto results2 = mt.getResults();
```

### Handle Partial Failures

```cpp
auto merged = mt.getMergedResults();

// Parse JSON to check individual results
jsn::object_t json;
jsn::JsonReader().read(merged.second, json);

auto const& results = unwrap(json).value_as<jsn::array_t>("results");
auto total = unwrap(json).value_as<int>("total");
auto errors = unwrap(json).value_as<int>("errors");

std::cout << errors << " of " << total << " operations failed\n";
```

---

**Documentation generated for artdaq-database ConfigurationDB module**
