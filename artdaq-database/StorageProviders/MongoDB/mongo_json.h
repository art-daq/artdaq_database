#ifndef _ARTDAQ_DATABASE_PROVIDER_MONGO_JSON_H_
#define _ARTDAQ_DATABASE_PROVIDER_MONGO_JSON_H_

#include <bsoncxx/document/value.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types/value.hpp>

namespace artdaq {
namespace database {
namespace mongo {
namespace compat {
inline std::string to_json(bsoncxx::document::view view) { return bsoncxx::to_json(view); }
inline bsoncxx::document::value from_json(std::string json) { return bsoncxx::from_json(json); }
std::string to_json(bsoncxx::types::value value);
}
}  // namespace mongo
}  // namespace database
}  // namespace artdaq

namespace compat = artdaq::database::mongo::compat;

#endif /* _ARTDAQ_DATABASE_PROVIDER_MONGO_JSON_H_ */
