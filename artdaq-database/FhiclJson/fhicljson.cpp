#include "artdaq-database/FhiclJson/common.h"
#include "artdaq-database/FhiclJson/fhicl_json.h"
#include "artdaq-database/FhiclJson/fhicl_reader.h"
#include "artdaq-database/FhiclJson/fhicl_writer.h"
#include "artdaq-database/FhiclJson/json_reader.h"
#include "artdaq-database/FhiclJson/json_writer.h"

#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/make_ParameterSet.h"


namespace artdaq{
namespace database{
namespace fhicljson{

using namespace artdaq::database;
using namespace fhicl;

bool fhicl_to_json(std::string const& fcl, std::string& json)
{
    assert(!fcl.empty());
    assert(json.empty());

    auto result = bool(false);

    auto rootNode = table_t();

    auto get_node = [&rootNode](auto const & name)->auto& {
        return boost::get<fhicljson::table_t>(rootNode[name]);
    };

    auto get_time_as_string = []() {
        auto now = std::chrono::system_clock::now();
        std::time_t time = std::chrono::system_clock::to_time_t(now);
        auto timestr = std::string(std::ctime(&time));
        return timestr.substr(0, timestr.size() - 1);
    };

    rootNode[literal::data_node] = fhicljson::table_t();
    rootNode[literal::comments_node] = fhicljson::table_t();
    rootNode[literal::origin_node] = fhicljson::table_t();

    get_node(literal::comments_node)[literal::source] = std::string("fhicl_to_json");
    get_node(literal::origin_node)[literal::timestamp] = get_time_as_string();

    auto reader = FhiclReader();
    result = reader.read(fcl, get_node(literal::data_node));

    if (!result)
        return result;

    auto json1 = std::string();

    auto writer = JsonWriter();
    result = writer.write(rootNode, json1);

    if (result)
        json.swap(json1);

    return result;
}

bool json_to_fhicl(std::string const& json [[gnu::unused]], std::string& fcl [[gnu::unused]])
{
    assert(!json.empty());
    assert(fcl.empty());
    auto result = bool(false);

    return result;
}

} //namespace fhicljson
} //namespace database
} //namespace artdaq

