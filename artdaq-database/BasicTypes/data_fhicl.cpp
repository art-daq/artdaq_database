#include "artdaq-database/BasicTypes/data_json.h"
#include "artdaq-database/BasicTypes/data_json_fusion.h"

#include "artdaq-database/BasicTypes/data_fhicl.h"
#include "artdaq-database/BasicTypes/data_fhicl_fusion.h"

#include "artdaq-database/FhiclJson/fhicljson.h"
#include "artdaq-database/FhiclJson/shared_literals.h"

#include "artdaq-database/BasicTypes/base64.h"
#include "fhiclcpp/exception.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "FhiclData_C"

namespace regex
{
constexpr auto parse_base64fhicl = "[\\s\\S]*\"fhicl\"\\s*:\\s*\"(\\S*?)\"";
}

namespace artdaq{
namespace database{
namespace basictypes{

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

FhiclData::FhiclData(std::string const& buffer)
    : fhicl_buffer {buffer} {

}

FhiclData::FhiclData(JsonData const& document)
{
    assert(!document.json_buffer.empty());

    TRACE_(1, "FHICL document=" << document.json_buffer);

    auto ex = std::regex( {regex::parse_base64fhicl});

    auto results = std::smatch();

    if (!std::regex_search(document.json_buffer, results, ex))
        throw fhicl::exception(fhicl::parse_error, literal::data_node)
                << ("JSON to FHICL convertion error, regex_search()==false; JSON buffer: " + document.json_buffer);

    if (results.size() != 1)
        throw fhicl::exception(fhicl::parse_error, literal::data_node)
                << ("JSON to FHICL convertion error, regex_search().size()!=1; JSON buffer: " + document.json_buffer);

    auto base64 = std::string(results[0]);
    TRACE_(2, "FHICL base64=" <<  base64);

    auto json = base64_decode(base64);
    TRACE_(3, "FHICL  json=" <<  json);

    JsonData(json).convert_to(*this);

    TRACE_(4, "FHICL fhicl=" <<  fhicl_buffer);
}


FhiclData::operator JsonData() const
{
    TRACE_(5, "FHICL fhicl=" <<  fhicl_buffer);

    auto json = JsonData("");

    if (!json.convert_from(*this))
        throw fhicl::exception(fhicl::parse_error, literal::data_node)
                << ("FHICL to JSON convertion error; FHICL buffer: " + this->fhicl_buffer);

    TRACE_(6, "FHICL  json=" <<  json.json_buffer);

    auto collection = std::string("FhiclData_") + type_version();

    auto base64 = base64_encode(fhicl_buffer);
    TRACE_(7, "FHICL base64=" <<  base64);

    std::ostringstream os;

    os << "{";
    os << 	"\"collection\":\"" << collection << "\"";
    os << " , ";
    //os << 	("\"" + collection + "_id\" : ") << 0;
    //os << " , ";
    
    os << 	"\"document\":";
    os << 	"{";
    os << 		"\"json\": "<< json.json_buffer;
    os <<	 ",";
    os << 		"\"fhicl\": \"" <<  base64 << "\"";
    os << 	" }";
    os << "}";

    TRACE_(8, "FHICL document=" << os.str());

    return {os.str()};
}

} //namespace basictypes
} //namespace database
} //namespace artdaq

