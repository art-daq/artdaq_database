#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>
#include "boost/program_options.hpp"
#include "artdaq-database/FhiclJson/fhicljsongui.h"
#include "artdaq-database/FhiclJson/fhicljsondb.h"

namespace  bpo = boost::program_options;
using namespace artdaq::database;


int main(int argc, char* argv[]) try
{
// Get the input parameters via the boost::program_options library,
    // designed to make it relatively simple to define arguments and
    // issue errors if argument list is supplied incorrectly

    std::ostringstream descstr;
    descstr << argv[0] << " <-c <config-file>> <other-options>";

    bpo::options_description desc = descstr.str();

    desc.add_options()
    ("config,c", bpo::value<std::string>(), "Configuration file.")
    ("outputformat,f", bpo::value<std::string>(), "Output file format.")
    ("help,h", "produce help message");

    bpo::variables_map vm;

    try {
        bpo::store(bpo::command_line_parser(argc, argv).options(desc).run(), vm);
        bpo::notify(vm);
    } catch (bpo::error const& e) {
        std::cerr << "Exception from command line processing in " << argv[0]
                  << ": " << e.what() << "\n";
        return -1;
    }

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return 1;
    }
    if (!vm.count("config")) {
        std::cerr << "Exception from command line processing in " << argv[0]
                  << ": no configuration file given.\n"
                  << "For usage and an options list, please do '"
                  << argv[0] <<  " --help"
                  << "'.\n";
        return 2;
    }

    auto file_name = vm["config"].as<std::string>();
    auto format = vm["outputformat"].as<std::string>();

    
    std::ifstream is(file_name);

    std::stringstream json_buffer;
    json_buffer << is.rdbuf();
    is.close();

    auto json = json_buffer.str();
    auto fhicl = std::string();

    if(format.compare("db")==0 || format.compare("database")==0 ){
      fhicljsondb::json_to_fhicl(json, fhicl);
    }else {
      fhicljsongui::json_to_fhicl(json, fhicl);
    }
    
    
    std::ofstream os(file_name + ".fcl");
    os << fhicl;
    os.close();

    std::cout << fhicl << "\n";
    return 0;
}

catch (std::string& x)
{
    std::cerr << "Exception (type string) caught in driver: " << x << "\n";
    return 1;
}

catch (char const* m)
{
    std::cerr << "Exception (type char const*) caught in driver: " << std::endl;
    if (m) {
        std::cerr << m;
    } else {
        std::cerr << "[the value was a null pointer, so no message is available]";
    }
    std::cerr << '\n';
}
