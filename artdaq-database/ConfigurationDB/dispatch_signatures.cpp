#include "artdaq-database/ConfigurationDB/dispatch_signatures.h"

namespace artdaq {
namespace database {
namespace configuration {

operations_map_t& getOperations() {
  static auto _operations_map = operations_map_t{};
  return _operations_map;
}

}  // namespace configuration
}  // namespace database
}  // namespace artdaq
