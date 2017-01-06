#include "artdaq-database/DataFormats/common/shared_result.h"

using namespace artdaq::database;
namespace db = artdaq::database;

result_t db::Failure(std::string const& msg) { return {false, msg}; }
result_t db::Success(std::string const& msg) { return {true, msg}; }
result_t db::Failure(std::ostringstream const& oss) { return {false, oss.str()}; }
result_t db::Success(std::ostringstream const& oss) { return {true, oss.str()}; }
void db::ThrowOnFailure(result_t const& result){
  if(result.first)
    return;
  
  throw std::invalid_argument(result.second);
}