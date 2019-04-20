#include "artdaq-database/StorageProviders/MongoDB/mongo_json.h"
#include "artdaq-database/SharedCommon/helper_functions.h"

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>

std::string compat::to_json(const bsoncxx::types::value& value) {
  using bsoncxx::builder::basic::kvp;
  auto doc = bsoncxx::builder::basic::document{};
  doc.append(kvp("dummy",value));  
  std::string buff = bsoncxx::to_json(doc.view());   
  auto fpos = buff.find(':') + 1;
  auto nchars = buff.rfind('}') - fpos;
  return artdaq::database::trim(buff.substr(fpos, nchars));
}

std::string compat::to_json_unescaped(const bsoncxx::types::value& value) {
  std::stringstream ss;
  ss << compat::to_json(value);
  auto retValue = ss.str();
  return artdaq::database::replace_all(retValue, "\\/", "/");
}

std::string compat::to_json_unescaped(bsoncxx::document::view view) {
  std::stringstream ss;
  ss << compat::to_json(view);
  auto retValue = ss.str();
  return artdaq::database::replace_all(retValue, "\\/", "/");
}
