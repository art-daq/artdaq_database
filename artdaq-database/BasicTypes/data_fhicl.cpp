#include "artdaq-database/BasicTypes/data_json.h"
#include "artdaq-database/BasicTypes/data_json_fusion.h"

#include "artdaq-database/BasicTypes/data_fhicl.h"
#include "artdaq-database/BasicTypes/data_fhicl_fusion.h"

#include "artdaq-database/FhiclJson/fhicljson.h"

namespace artdaq {
namespace database {
namespace basictypes {

template<>
bool JsonData::convert_to(FhiclData& fhicl) const
{
    using artdaq::database::fhicljson::json_to_fhicl;

    return json_to_fhicl(json_buffer, fhicl.fhicl_buffer);
}


template<>
bool JsonData::convert_from(FhiclData const& fhicl)
{
    using artdaq::database::fhicljson::fhicl_to_json;

    return fhicl_to_json(fhicl.fhicl_buffer, json_buffer);
}

} //namespace basictypes
} //namespace database
} //namespace artdaq

