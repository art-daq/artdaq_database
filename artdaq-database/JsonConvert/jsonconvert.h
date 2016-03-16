#ifndef _ARTDAQ_DATABASE_JSONCONVERT_H_
#define _ARTDAQ_DATABASE_JSONCONVERT_H_

#include "artdaq-database/JsonConvert/class_introspection.h"
#include "artdaq-database/JsonConvert/common.h"
#include "artdaq-database/JsonConvert/json_deserializer.h"
#include "artdaq-database/JsonConvert/json_serializer.h"

namespace artdaq {
namespace database {

using artdaq::database::jsonconvert::JsonSerializer;
using artdaq::database::jsonconvert::JsonDeserializer;

struct JSON {
  using writer = JsonSerializer;
  using reader = JsonDeserializer;
};

}  // namespace database
}  // namespace artdaq
#endif /* _ARTDAQ_DATABASE_JSONCONVERT_H_ */
