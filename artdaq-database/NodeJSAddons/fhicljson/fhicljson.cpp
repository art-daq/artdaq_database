#include <fstream>
#include <streambuf>
#include  <sstream>

#include "fhicljson.h"
#include "artdaq-database/FhiclJson/fhicljson.h"

using namespace artdaq::database::fhicljson;

auto make_error_msg = [](auto msg){ return  std::string("{error:\"").append(msg).append(".\"}");};

result_pair_t tojson(std::string const& file_name)
{
    if (file_name.empty())
        return std::make_pair(false, make_error_msg("File name is empty"));

    try {
        std::ifstream is(file_name);

        std::stringstream fhicl_buffer;

        fhicl_buffer << is.rdbuf();
        is.close();

        auto fhicl = fhicl_buffer.str();
        auto json = std::string();

        auto result = fhicl_to_json(fhicl, json);
	
	return std::make_pair(result, json);
    } catch (std::exception const& e) {
        return std::make_pair(false, make_error_msg(e.what()) );
    } catch (...) {
        return std::make_pair(false, make_error_msg("Unknown exception."));
    }
}

result_pair_t tofhicl(std::string const& file_name, std::string const& json)
{
    if (file_name.empty() || json.empty())
        return std::make_pair(false, make_error_msg("File name or JSON buffer is empty."));

    try {
        auto fhicl = std::string();

        auto result = json_to_fhicl(json, fhicl);

        if (!result)
            return std::make_pair(false, make_error_msg("Failure while converting JSON to FHiCL."));

        std::ofstream os(file_name);
        os << fhicl;

        os.flush();
        os.close();

        return std::make_pair(true, "");
    } catch (std::exception const& e) {
        return std::make_pair(false,make_error_msg(e.what()));
    } catch (...) {
        return std::make_pair(false, make_error_msg("Unknown exception."));
    }
}

