#include "artdaq-database/FhiclJson/common.h"
#include "artdaq-database/FhiclJson/fhicl_reader.h"
#include "artdaq-database/FhiclJson/fhicl_writer.h"
#include "artdaq-database/FhiclJson/json_reader.h"
#include "artdaq-database/FhiclJson/json_writer.h"

#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/make_ParameterSet.h"


namespace artdaq{
namespace database{
namespace fhicljson{

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

    auto get_time_as_string = []() {
        auto now = std::chrono::system_clock::now();
        std::time_t time = std::chrono::system_clock::to_time_t(now);
        auto timestr = std::string(std::ctime(&time));
        return timestr.substr(0, timestr.size() - 1);
    };

    json_root[literal::data_node] = jsn::array_t();
    json_root[literal::comments_node] = jsn::object_t();
    json_root[literal::origin_node] = jsn::object_t();

    get_object(literal::comments_node)[literal::source] = std::string("fhicl_to_json");
    get_object(literal::origin_node)[literal::timestamp] = get_time_as_string();

    auto reader = FhiclReader();
    result = reader.read(fcl,  boost::get<jsn::array_t>(json_root[literal::data_node]));

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

    auto json1 = std::string();
    
    if (!result)
        return result;
    
    auto fcl1 = std::string();

    auto writer = FhiclWriter();
        
    result = writer.write(json_root, fcl1);

    if (result)
        fcl.swap(fcl1);
    
    return result;
}

} //namespace fhicljson
} //namespace database
} //namespace artdaq

