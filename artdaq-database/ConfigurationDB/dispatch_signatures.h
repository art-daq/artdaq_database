#ifndef _ARTDAQ_DATABASE_CONFIGURATIONDB_DISPATCH_SIGNATURES_H_
#define _ARTDAQ_DATABASE_CONFIGURATIONDB_DISPATCH_SIGNATURES_H_

#include <cstring>
#include <map>
#include <memory>
#include <utility>

#include "artdaq-database/ConfigurationDB/operation_dispatch.h"

namespace artdaq {
namespace database {
namespace configuration {

using artdaq::configuration::dispatch::Functor;
using artdaq::configuration::dispatch::RA1Functor;
using artdaq::configuration::dispatch::RA1A2Functor;
using artdaq::configuration::dispatch::RA1R2Functor;

using result_t = std::pair<bool, std::string>;

using opsig_str_t = RA1Functor<result_t, std::string>;
using opsig_strstr_t = RA1A2Functor<result_t, std::string, std::string>;
using opsig_str_rstr_t = RA1R2Functor<result_t, std::string, std::string>;

using operations_map_t = std::map<std::string, std::unique_ptr<Functor<result_t>>>;

operations_map_t& getOperations();

template <typename SIG, typename... Args>
operations_map_t& registerOperation(std::string name, Args&&... args) {
  auto& ops = getOperations();

  ops.emplace(std::make_pair(name, std::make_unique<SIG>(std::forward<Args>(args)...)));

  return ops;
}

}  // namespace configuration
}  // namespace database
}  // namespace artdaq

#endif /* _ARTDAQ_DATABASE_CONFIGURATIONDB_DISPATCH_SIGNATURES_H_ */
