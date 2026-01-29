# Multitasker.cpp

**Path:** `artdaq-database/ConfigurationDB/Multitasker.cpp`

**Implements:** [Multitasker.h](./Multitasker.h.md)

**Purpose:** Implementation of the `Multitasker` thread pool class. This file contains the worker thread logic, task queue management, result collection, and synchronization primitives that enable parallel execution of database operations.

## Implementation Overview

The implementation follows a classic thread pool pattern with these key components:

1. **Constructor**: Creates worker threads that immediately start waiting for tasks
2. **Task Queue**: Thread-safe FIFO queue protected by mutex
3. **Worker Loop**: Condition-variable-based wait/wake pattern
4. **Result Collection**: Thread-safe result storage with merge capability
5. **Cleanup**: Graceful shutdown with thread joining

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/ConfigurationDB/Multitasker.h` | Header declarations |
| `artdaq-database/ConfigurationDB/common.h` | TRACE macros, common utilities |
| `<cassert>` | Debug assertions |

## Internal Functions

### `PointerToHexString(ptr) -> std::string`

```cpp
inline std::string PointerToHexString(const void* ptr);
```

**Brief:** Converts a pointer address to a hexadecimal string for logging task identifiers.

**Parameters:**
- `ptr` - Pointer to convert

**Returns:** Hexadecimal string representation of the pointer address

**Called by:** `workerThread()` for task identification in logs

**Example Output:** `"0x7f8a4c000b70"`

## Key Algorithms

### Thread Pool Initialization

```cpp
Multitasker::Multitasker(size_t numThreads)
    : _shouldStop{false}, _totalTasks{0}, _threadid{std::this_thread::get_id()} {

  // Clamp to maximum of hardware_concurrency/2
  numThreads = std::min(numThreads, size_t{std::thread::hardware_concurrency() / 2});

  // Spawn worker threads
  for (size_t i = 0; i < numThreads; ++i) {
    _threads.emplace_back([this] { workerThread(); });
  }
}
```

**Design Decisions:**
- Clamping prevents over-subscription on systems with many cores
- Workers start immediately and block on condition variable
- Owner thread ID is recorded for later validation

### Worker Thread Main Loop

```cpp
void Multitasker::workerThread() {
  while (true) {
    std::function<result_t()> task;
    {
      std::unique_lock lock(_one4all_mutex);
      _condition.wait(lock, [this] {
        return _shouldStop || !_tasks.empty();
      });

      if (_shouldStop && _tasks.empty()) break;

      task = std::move(_tasks.front());
      _tasks.pop();
    }

    // Execute task outside lock
    try {
      auto result = task();
      {
        std::unique_lock lock(_one4all_mutex);
        _results.push_back(result);
      }
      _condition.notify_all();
    } catch (...) {
      // Handle exception - store error result
    }
  }
}
```

**Key Design Points:**
- Lock is released before task execution (minimizes contention)
- Condition variable predicate prevents spurious wakeups
- Both stop signal AND empty queue required to exit
- Exceptions are caught and converted to error results
- Notification after each result enables `waitForResults()` to check completion

### Result Merging Algorithm

```cpp
result_t Multitasker::getMergedResults() {
  auto results = getResults();

  int errorCount = std::count_if(results.begin(), results.end(),
    [](const auto& result) { return !result.first; });

  std::stringstream resultSummary;
  resultSummary << "{\n\"results\":[";
  for (const auto& result : results) {
    resultSummary << "\n{\"status\":" << result.first
                  << ",\"message\":\"" << result.second << "\"},";
  }
  resultSummary.seekp(-1, std::ios_base::end);  // Remove trailing comma
  resultSummary << "\n],\n\"total\":" << results.size() << ","
                << "\"errors\":" << errorCount << "\n}";

  return {errorCount == 0, resultSummary.str()};
}
```

**Output Format:**
```json
{
  "results": [
    {"status": true, "message": "Success"},
    {"status": false, "message": "Error occurred"}
  ],
  "total": 2,
  "errors": 1
}
```

### Thread Ownership Validation

```cpp
void Multitasker::checkSameThreadOrThrow() const {
  if (std::this_thread::get_id() != _threadid) {
    TLOG(TLVL_ERROR) << "Do not share the Multitasker instance across threads.";
    throw std::runtime_error(
        "Do not share the Multitasker instance across different threads.");
  }
}
```

**Rationale:** This prevents a common class of concurrency bugs where users might try to share a Multitasker across threads. Each thread should create its own instance.

## Performance Considerations

### Lock Contention

The implementation uses a single mutex for simplicity, which could become a bottleneck with:
- Very high task submission rate
- Very short task execution time
- Many worker threads

For the typical use case (database operations taking 10ms+), this is not a concern.

### Memory Usage

- Worker threads: ~1MB stack per thread (OS default)
- Task queue: O(n) where n = pending tasks
- Result list: O(m) where m = completed tasks

### Timing and Logging

Each task is timed for performance monitoring:

```cpp
auto startTime = std::chrono::high_resolution_clock::now();
auto result = task();
auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
    std::chrono::high_resolution_clock::now() - startTime);
TLOG(31) << "Completed task in " << duration.count() << " ms.";
```

## Error Handling Strategy

### Task-Level Exceptions

```cpp
try {
  auto result = task();
  // Store result
} catch (std::exception const& e) {
  TLOG(TLVL_ERROR) << "Exception: " << e.what();
  {
    std::unique_lock lock(_one4all_mutex);
    _results.emplace_back(false, e.what());
  }
} catch (...) {
  TLOG(TLVL_ERROR) << "Unknown exception";
  {
    std::unique_lock lock(_one4all_mutex);
    _results.emplace_back(false, "DB operation threw an unknown exception.");
  }
}
```

**Behavior:**
- Standard exceptions: Message captured in result
- Unknown exceptions: Generic error message used
- Worker thread continues processing after any exception
- All exceptions are logged at TLVL_ERROR

### Long-Running Task Warnings

```cpp
void Multitasker::waitForResults() const {
  constexpr int reportAfterSecs = 15;
  constexpr auto waitTimeout = std::chrono::seconds(5);

  while (_tasks.size() > 0 || _results.size() < _totalTasks) {
    auto status = _condition.wait_for(lock, waitTimeout);

    if (status == std::cv_status::timeout) {
      auto elapsedSeconds = /* ... */;
      if (elapsedSeconds >= reportAfterSecs) {
        TLOG(TLVL_WARNING) << "WaitForResults() running for "
                           << elapsedSeconds << " seconds.";
      }
    }
  }
}
```

**Logging Behavior:**
- First warning at 15 seconds
- Subsequent warnings every 15 seconds
- Useful for identifying stuck operations

## Testing Notes

- **Unit tests:** `test/ConfigurationDB/Multitasker_t.cc`
- **Key test cases:**
  - Single task execution
  - Multiple task parallel execution
  - Exception handling in tasks
  - Thread safety validation
  - Result merging format

## Maintenance Notes

### TRACE Configuration

The `debug::Multitasker()` function enables comprehensive logging:

```cpp
void cfd::Multitasker() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);
}
```

### Logging Levels Used

| Level | Component | Content |
|-------|-----------|---------|
| 10 | debug::Multitasker | Trace enable confirmation |
| 20 | Constructor/Destructor | Thread pool lifecycle |
| 21 | addTask | Task submission |
| 22 | getResults/getMergedResults | Result retrieval |
| 23 | waitForResults/signalStop | Synchronization |
| 24 | isBusy | Status checks |
| 30 | workerThread | Thread lifecycle |
| 31 | workerThread | Individual task execution |
| TLVL_WARNING | Various | Long waits, task failures |
| TLVL_ERROR | Various | Exceptions, wrong-thread calls |

### Thread Count Tuning

The default `hardware_concurrency/2` is conservative. Consider adjusting for:

**I/O-Bound Tasks (database operations):**
- Can use more threads (up to `hardware_concurrency`)
- I/O wait time allows more parallelism

**CPU-Bound Tasks:**
- Use fewer threads (`hardware_concurrency/4`)
- Avoid context-switching overhead

**Mixed Workloads:**
- Profile to find optimal balance
- Default is reasonable starting point

## Implementation Details

### Destructor Safety

```cpp
Multitasker::~Multitasker() {
  // Assert same-thread destruction
  if (std::this_thread::get_id() != _threadid) {
    TLOG(TLVL_ERROR) << "Destructor called from wrong thread!";
    assert(false && "Multitasker destructor called from wrong thread");
  }

  // Signal stop
  {
    std::unique_lock lock(_one4all_mutex);
    _shouldStop = true;
  }
  _condition.notify_all();

  // Wait for threads
  for (auto& thread : _threads) {
    if (thread.joinable()) {
      thread.join();
    }
  }
}
```

**Key Points:**
- Assertion catches programming errors (wrong-thread destruction)
- Stop signal is set before notifying threads
- All threads are joined before destructor returns
- `joinable()` check handles edge cases

### Task ID Generation

Uses pointer-to-hex conversion for unique task IDs in logs:

```cpp
auto taskid = PointerToHexString(&task);
TLOG(31) << "Started task=" << taskid << ", thread=" << this_thread_id;
```

This provides:
- Unique identifier per task
- Traceable through execution lifecycle
- No additional memory allocation

## See Also

- [Multitasker.h](./Multitasker.h.md) - Header with public API documentation
- [common.h](./common.h.md) - TRACE macros and common utilities
- [returned_result.h](../SharedCommon/returned_result.h.md) - result_t type definition

---

**Documentation generated for artdaq-database ConfigurationDB module**
