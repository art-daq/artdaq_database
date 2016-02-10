#include "artdaq-database/FhiclJson/common.h"
#include "artdaq-database/FhiclJson/fhicl_reader.h"
#include "artdaq-database/FhiclJson/fhicl_writer.h"
#include "artdaq-database/FhiclJson/json_reader.h"
#include "artdaq-database/FhiclJson/json_writer.h"
#include "artdaq-database/FhiclJson/healper_functions.h"

#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/make_ParameterSet.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "FHJS:fhicljsondb_C"

namespace artdaq{
namespace database{
namespace fhicljsondb{

namespace fcl = artdaq::database::fhicl;
namespace jsn = artdaq::database::json;

using artdaq::database::json::JsonReader;
using artdaq::database::json::JsonWriter;

using artdaq::database::fhicl::FhiclReader;
using artdaq::database::fhicl::FhiclWriter;

bool fhicl_to_json(std::string const& fcl, std::string& json)
{
    assert(!fcl.empty());
    assert(json.empty());
    
    TRACE_(2, "fhicl_to_json begin");

    auto result = bool(false);

    auto json_root = jsn::object_t();

    auto get_object = [&json_root](std::string const & name)->auto& {
        return  boost::get<jsn::object_t>(json_root[name]);
    };

    json_root[literal::document_node] = jsn::object_t();
    json_root[literal::comments_node] = jsn::array_t();
    json_root[literal::origin_node] = jsn::object_t();
    json_root[literal::includes_node] = jsn::array_t();

    TRACE_(2, "fhicl_to_json created root nodes");

    get_object(literal::origin_node)[literal::source] = std::string("fhicl_to_json");
    get_object(literal::origin_node)[literal::timestamp] = artdaq::database::fhicljson::timestamp();

    auto reader = FhiclReader();
    
    TRACE_(2, "read_includes begin");
    result = reader.read_includes(fcl,  boost::get<jsn::array_t>(json_root[literal::includes_node]));
    TRACE_(2, "read_includes end result=" << std::to_string(result));
    //if (!result)
     //   return result;

    TRACE_(2, "read_comments begin");    
    result = reader.read_comments(fcl,  boost::get<jsn::array_t>(json_root[literal::comments_node]));
    TRACE_(2, "read_comments end result=" << std::to_string(result));

    if (!result)
        return result;
    
    TRACE_(2, "read_data_db begin");
    result = reader.read_data_db(fcl,  boost::get<jsn::object_t>(json_root[literal::document_node]));
    TRACE_(2, "read_data_db end result=" << std::to_string(result));

    if (!result)
        return result;


    auto json1 = std::string();

    auto writer = JsonWriter();

    TRACE_(2, "JsonWriter::write() begin");
    result = writer.write(json_root, json1);
    TRACE_(2, "JsonWriter::write() end");

    if (result)
        json.swap(json1);

    TRACE_(2, "fhicl_to_json end");
    
    return result;
}

bool json_to_fhicl(std::string const& json , std::string& fcl)
{
    assert(!json.empty());
    assert(fcl.empty());

    auto result = bool(false);

    auto json_root = jsn::object_t();

    auto reader = JsonReader();
    
    result = reader.read(json, json_root);

    if (!result)
        return result;


//    auto writer = JsonWriter();
//    result = writer.write(json_root, fcl1);


    auto writer = FhiclWriter();
    
    auto fcl_includes = std::string();
    {
        auto const& includes_key = jsn::object_t::key_type(literal::includes_node);
	try {
        auto const& json_array = boost::get<jsn::array_t>(json_root.at(includes_key));

        result = writer.write_includes(json_array, fcl_includes);

        if (!result)
            return result;
	}catch(std::out_of_range const&) {
	  
	}
    }

    auto fcl_data = std::string();
    {
        auto const& document_object =  boost::get<jsn::object_t>(json_root[literal::document_node]);

        result = writer.write_data_db(document_object, fcl_data);

        if (!result)
            return result;
    }

    if (result) {
      std::stringstream ss;
      
      ss << fcl_includes;
      
      ss << "\n\n";
      
      ss << fcl_data;
      
      auto result = ss.str();
      
      fcl.swap(result);
    }
    
    return result;
}

void trace_enable_fhicljsondb()
{
    TRACE_CNTL("name",    TRACE_NAME);
    TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
    TRACE_CNTL("modeM", 1LL);
    TRACE_CNTL("modeS", 1LL);

    TRACE_(0, "artdaq::database::fhicljsondb" << "trace_enable");
}
} //namespace fhicljson
} //namespace database
} //namespace artdaq

