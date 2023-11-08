#include "artdaq-database/ConfigurationDB/Multitasker.h"
#include "artdaq-database/ConfigurationDB/common.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "Multitasker.cpp"

using namespace artdaq::database::configuration;
using artdaq::database::result_t;
namespace cf = artdaq::database::configuration;
namespace cfd = artdaq::database::configuration::debug;

inline std::string PointerToHexString(const void* ptr) {
  std::stringstream stream;
  stream << std::hex << ptr;
  return stream.str();
}

Multitasker::Multitasker(size_t numThreads) : _shouldStop{false}, _totalTasks{0}, _threadid{std::this_thread::get_id()} {
  numThreads = std::min(numThreads, size_t{std::thread::hardware_concurrency() / 2});
  for (size_t i = 0; i < numThreads; ++i) {
    _threads.emplace_back([this] { workerThread(); });
  }

  TLOG(20) << "Started with " << numThreads << " worker threads.";
}

void Multitasker::checkSameThreadOrThrow() const {
  if (std::this_thread::get_id() != _threadid) {
    TLOG(TLVL_ERROR) << "Do not share the Multitasker instance across different threads.";
    throw std::runtime_error("Do not share the Multitasker instance across different threads.");
  }
}

Multitasker::~Multitasker() {
  checkSameThreadOrThrow();
  {
    std::unique_lock lock(_one4all_mutex);
    auto isbusy = !_tasks.empty() || _results.size() < _totalTasks;
    TLOG(20) << "Stopping, isBusy is " << (isbusy ? "true" : "false") << ".";
    _shouldStop = true;
  }

  _condition.notify_all();

  TLOG(20) << "Waiting for all worker threads to join.";

  for (auto& thread : _threads) {
    if (thread.joinable()) {
      thread.join();
    }
  }

  TLOG(20) << "Finished work, all threads joined.";
}

void Multitasker::addTask(const std::function<result_t()>& task) {
  checkSameThreadOrThrow();
  {
    std::unique_lock lock(_one4all_mutex);
    if (_shouldStop) {
      TLOG(TLVL_ERROR) << "Submitting more tasks is not allowed after the stop request has been received.";
      throw std::runtime_error("Submitting more tasks is not allowed after the stop request has been received.");
    }
    _tasks.push(std::move(task));
    _totalTasks++;
  }

  _condition.notify_one();

  TLOG(21) << "Received a new task item; total tasks received " << _totalTasks << ".";
}

std::list<result_t> Multitasker::getResults() {
  checkSameThreadOrThrow();

  auto results = std::list<result_t>{};
  {
    std::unique_lock lock(_one4all_mutex);
    auto isbusy = !_tasks.empty() || _results.size() < _totalTasks;
    if (isbusy) {
      TLOG(TLVL_ERROR) << "Not all tasks were completed; " << _results.size() << " out of " << _totalTasks
                       << " tasks were completed. Please call waitForResults() before calling getResults().";

      throw std::runtime_error("Not all tasks were completed yet, call waitForResults() before calling getResults().");
    }
    std::swap(_results, results);
    _totalTasks = 0;
  }

  TLOG(22) << "Returning results for " << results.size() << "task items.";

  return results;
}

result_t Multitasker::getMergedResults() {
  checkSameThreadOrThrow();

  auto results = getResults();

  TLOG(22) << "Merging results " << results.size() << ".";
  int errorCount = std::count_if(results.begin(), results.end(), [](const auto& result) { return !result.first; });

  std::stringstream resultSummary;
  resultSummary << "{\n\"results\":[";
  for (const auto& result : results) {
    resultSummary << "\n{\"status\":" << result.first << ",\"message\":\"" << result.second << "\"},";
  }
  resultSummary.seekp(-1, std::ios_base::end);
  resultSummary << "\n],\n\"total\":" << results.size() << ","
                << "\"errors\":" << errorCount << "\n}";

  TLOG(22) << "Returning results; errorCount is " << errorCount << ".";

  return {errorCount == 0, resultSummary.str()};
}

void Multitasker::waitForResults() const {
  checkSameThreadOrThrow();
  TLOG(23) << "Waiting for all tasks to complete.";

  auto startTime = std::chrono::high_resolution_clock::now();
  bool logWarnPrinted = false;
  int reportAfterSecs = 15;

  while (isBusy()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime).count();
    if (elapsedSeconds >= reportAfterSecs && !logWarnPrinted) {
      TLOG(TLVL_WARNING) << "WaitForResults() has been running for" << elapsedSeconds << " seconds.";
      logWarnPrinted = true;
    } else if (elapsedSeconds % reportAfterSecs == 0 && logWarnPrinted) {
      TLOG(TLVL_ERROR) << "WaitForResults() has been running for " << elapsedSeconds << " seconds.";
    }
  }
}

void Multitasker::signalStop() {
  TLOG(23) << "Signaling threads to stop";
  {
    std::unique_lock lock(_one4all_mutex);
    _shouldStop = true;
  }
  _condition.notify_all();
}

bool Multitasker::isBusy() const {
  std::unique_lock lock(_one4all_mutex);
  auto isbusy = !_tasks.empty() || _results.size() < _totalTasks;
  TLOG(24) << "isBusy is " << (isbusy ? "true" : "false");
  return isbusy;
}

void Multitasker::workerThread() {
  auto this_thread_id = std::this_thread::get_id();

  auto threadStartTime = std::chrono::high_resolution_clock::now();
  TLOG(30) << "thread=" << this_thread_id << " started.";

  while (true) {
    std::function<result_t()> task;
    {
      std::unique_lock lock(_one4all_mutex);
      _condition.wait(lock, [this] { return _shouldStop || !_tasks.empty(); });

      if (_shouldStop && _tasks.empty()) break;

      task = std::move(_tasks.front());
      _tasks.pop();
    }

    auto startTime = std::chrono::high_resolution_clock::now();

    try {
      auto taskid = PointerToHexString(&task);
      TLOG(31) << "Started   task=" << taskid << ", thread=" << this_thread_id << ".";
      auto result = task();
      auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime);

      if (!result.first) {
        TLOG(TLVL_WARNING) << "Error: " << result.second << " thread=" << this_thread_id << ", elapsed time " << duration.count() << " ms.";
      }
      TLOG(31) << "Completed task=" << taskid << ", thread=" << this_thread_id << " in " << duration.count() << " ms.";

      std::unique_lock lock(_one4all_mutex);
      _results.push_back(result);
    } catch (std::exception const& e) {
      auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime);
      TLOG(TLVL_ERROR) << "Exception: " << e.what() << " ; thread=" << this_thread_id << ", elapsed time " << duration.count() << " ms.";
      _results.emplace_back(false, e.what());
    } catch (...) {
      auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime);
      TLOG(TLVL_ERROR) << "Unknown exception; thread=" << this_thread_id << ", elapsed time " << duration.count() << " ms.";
      _results.emplace_back(false, "DB operation threw an unknown exception.");
    }
  }

  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - threadStartTime);
  TLOG(30) << "thread=" << this_thread_id << " was halted after running for " << duration.count() << " ms.";
}

void cfd::Multitasker() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);

  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TLOG(10) << "artdaq::database::configuration::Multitasker trace_enable";
}
