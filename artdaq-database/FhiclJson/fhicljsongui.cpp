#include "artdaq-database/FhiclJson/common.h"
#include "artdaq-database/FhiclJson/fhicl_reader.h"
#include "artdaq-database/FhiclJson/fhicl_writer.h"
#include "artdaq-database/FhiclJson/json_reader.h"
#include "artdaq-database/FhiclJson/json_writer.h"
#include "artdaq-database/FhiclJson/healper_functions.h"

#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/make_ParameterSet.h"


namespace artdaq{
namespace database{
namespace fhicljsongui{

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

    auto result = bool(false);

    auto json_root = jsn::object_t();

    auto get_object = [&json_root](std::string const & name)->auto& {
        return  boost::get<jsn::object_t>(json_root[name]);
    };

    json_root[literal::data_node] = jsn::array_t();
    json_root[literal::comments_node] = jsn::array_t();
    json_root[literal::origin_node] = jsn::object_t();
    json_root[literal::includes_node] = jsn::array_t();

    get_object(literal::origin_node)[literal::source] = std::string("fhicl_to_json");
    get_object(literal::origin_node)[literal::timestamp] =  artdaq::database::fhicljson::timestamp();

    auto reader = FhiclReader();

    result = reader.read_includes(fcl,  boost::get<jsn::array_t>(json_root[literal::includes_node]));

    //if (!result)
     //   return result;

    result = reader.read_comments(fcl,  boost::get<jsn::array_t>(json_root[literal::comments_node]));

    if (!result)
        return result;

    result = reader.read_data_gui(fcl,  boost::get<jsn::array_t>(json_root[literal::data_node]));

    if (!result)
        return result;


    auto json1 = std::string();

    auto writer = JsonWriter();
    result = writer.write(json_root, json1);

    if (result)
        json.swap(json1);

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
        auto const& data_key = jsn::object_t::key_type(literal::data_node);
        auto const& json_array = boost::get<jsn::array_t>(json_root.at(data_key));

        result = writer.write_data_gui(json_array, fcl_data);

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

} //namespace fhicljson
} //namespace database
} //namespace artdaq

