#include "artdaq-database/SharedCommon/returned_result.h"
#include "artdaq-database/SharedCommon/shared_exceptions.h"

namespace db = artdaq::database;

using artdaq::database::result_t;

result_t db::Failure(std::string const& msg) { return {false, msg}; }
result_t db::Success(std::string const& msg) { return {true, msg}; }
result_t db::Failure(std::ostringstream const& oss) { return {false, oss.str()}; }
result_t db::Success(std::ostringstream const& oss) { return {true, oss.str()}; }

void db::ThrowOnFailure(result_t const& result) {
  if (result.first) {
    return;
  }

  throw invalid_argument(result.second);
}