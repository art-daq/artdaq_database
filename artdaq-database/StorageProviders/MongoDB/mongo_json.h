#ifndef _ARTDAQ_DATABASE_PROVIDER_MONGO_JSON_H_
#define _ARTDAQ_DATABASE_PROVIDER_MONGO_JSON_H_

#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/json.hpp>

namespace artdaq {
namespace database {
namespace mongo {
std::string to_json(bsoncxx::document::view view);
}  // namespace mongo
}  // namespace database
}  // namespace artdaq

#endif /* _ARTDAQ_DATABASE_PROVIDER_MONGO_JSON_H_ */
