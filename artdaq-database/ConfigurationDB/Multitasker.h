#ifndef _ARTDAQ_DATABASE_MULTITASKER_H_
#define _ARTDAQ_DATABASE_MULTITASKER_H_
#include <algorithm>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <list>
#include <mutex>
#include <queue>
#include <random>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include "artdaq-database/SharedCommon/returned_result.h"

namespace artdaq {
namespace database {
namespace configuration {
class Multitasker {
 public:
  Multitasker(size_t numThreads = std::thread::hardware_concurrency() / 2);
  ~Multitasker();
  void addTask(const std::function<result_t()>& task);
  std::list<result_t> getResults();
  result_t getMergedResults();
  void waitForResults() const;
  bool isBusy() const;

 private:
  void workerThread();
  void signalStop();
  void checkSameThreadOrThrow() const;

 private:
  std::vector<std::thread> _threads;
  std::condition_variable _condition;
  mutable std::mutex _one4all_mutex;
  std::queue<std::function<result_t()>> _tasks;
  std::list<result_t> _results;
  std::atomic<bool> _shouldStop;
  std::atomic<std::size_t> _totalTasks;
  const std::thread::id _threadid;
};
namespace debug {
void Multitasker();
}
}  // namespace configuration
}  // namespace database
}  // namespace artdaq
#endif /* _ARTDAQ_DATABASE_CONFIGURATIONDBIFC_BASE_H_ */
