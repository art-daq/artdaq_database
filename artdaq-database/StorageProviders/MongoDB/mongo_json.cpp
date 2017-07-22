#include "artdaq-database/SharedCommon/helper_functions.h"
#include "artdaq-database/StorageProviders/MongoDB/mongo_json.h"

#include <bsoncxx/builder/core.hpp>

std::string compat::to_json(bsoncxx::types::value value) {
  auto core = bsoncxx::builder::core(true);
  core.append(value);
  auto buff = bsoncxx::to_json(core.view_document());
  auto fpos = buff.find(':') + 1;
  auto nchars = buff.rfind('}') - fpos;
  return artdaq::database::trim(buff.substr(fpos, nchars));
}
