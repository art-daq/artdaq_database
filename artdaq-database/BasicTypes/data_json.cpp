#include "artdaq-database/BasicTypes/data_json.h"

using artdaq::database::basictypes::JsonData;

JsonData::JsonData(std::string const& buffer)
:json_buffer{buffer}{
  
}
