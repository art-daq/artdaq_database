#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>
#include "boost/program_options.hpp"
#include "artdaq-database/ConfigurationDB/configurationdb.h"
#include "cetlib/coded_exception.h"
#include "artdaq-database/BuildInfo/process_exit_codes.h"
#include "artdaq-database/BuildInfo/printStackTrace.h"
#include <boost/exception/diagnostic_information.hpp>

#include "artdaq-database/FhiclJson/fhicljsondb.h"
#include "artdaq-database/StorageProviders/FileSystemDB/provider_filedb.h"
#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"

namespace  bpo = boost::program_options;

namespace db = artdaq::database;
namespace cf = db::configuration;
namespace cfo = cf::options;
namespace cfol = cfo::literal;

using Options= cfo::LoadStoreOperation;


typedef bool (*test_case)(Options const& /*options*/, std::string const& /*filename*/);

bool test_storeconfig(Options const&, std::string const&);
bool test_loadconfig(Options const&, std::string const&);

int main(int argc, char* argv[]) try
{
    debug::registerUngracefullExitHandlers();
//    artdaq::database::fhicljson::useFakeTime(true);

    cf::trace_enable_LoadStoreOperation();
    cf::trace_enable_LoadStoreOperationMongo();
    cf::trace_enable_LoadStoreOperationFileSystem();

    db::filesystem::trace_enable();
    
    db::fhicljson::trace_enable_fcl2jsondb();
    db::fhicl::trace_enable_FhiclReader();
    db::fhicl::trace_enable_FhiclWriter();
    db::fhicljsondb::trace_enable_fhicljsondb();

    db::jsonutils::trace_enable_JSONDocumentBuilder();
    
    // Get the input parameters via the boost::program_options library,
    // designed to make it relatively simple to define arguments and
    // issue errors if argument list is supplied incorrectly

    namespace db = artdaq::database;
namespace cfol = artdaq::database::configuration::options::literal;
    std::ostringstream descstr;
    descstr << argv[0] << " <-o <operation>>  <-c <config-file>>  <-f <file-format>>";
    descstr <<"  <-t <type>> <-v <version>>  <-g <globalid>>  <-d <database>>" ;

    bpo::options_description desc = descstr.str();

    desc.add_options()
    ("operation,o", bpo::value<std::string>(), "Operation [load/store].")
    ("configuration,c", bpo::value<std::string>(),  "Configuration file name.")
    ("format,f", bpo::value<std::string>(),  "Configuration file format [fhicl/json/gui].")
    ("type,t", bpo::value<std::string>(), "Configuration collection type name.")
    ("version,v", bpo::value<std::string>(), "Configuration version.")
    ("globalid,g", bpo::value<std::string>(), "Gloval config id.")
    ("database,d", bpo::value<std::string>(), "Database provider name.")
    ("help,h", "produce help message");

    bpo::variables_map vm;

    try {
        bpo::store(bpo::command_line_parser(argc, argv).options(desc).run(), vm);
        bpo::notify(vm);
    } catch (bpo::error const& e) {
        std::cerr << "Exception from command line processing in " << argv[0]
                  << ": " << e.what() << "\n";
        return process_exit_code::INVALID_ARGUMENT;
    }

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return process_exit_code::HELP;
    }
    auto options = Options {};

    if (!vm.count("operation")) {
        std::cerr << "Exception from command line processing in " << argv[0]
                  << ": no database operation given.\n"
                  << "For usage and an options list, please do '"
                  << argv[0] <<  " --help"
                  << "'.\n";
        return process_exit_code::INVALID_ARGUMENT|1;
    } else {
        options.operation(vm["operation"].as<std::string>());
    }

    if (!vm.count("configuration")) {
        std::cerr << "Exception from command line processing in " << argv[0]
                  << ": no file name given.\n"
                  << "For usage and an options list, please do '"
                  << argv[0] <<  " --help"
                  << "'.\n";
        return process_exit_code::INVALID_ARGUMENT|2;
    }
    auto file_name = vm["configuration"].as<std::string>();


    if (!vm.count("type")) {
        std::cerr << "Exception from command line processing in " << argv[0]
                  << ": no configuration type given.\n"
                  << "For usage and an options list, please do '"
                  << argv[0] <<  " --help"
                  << "'.\n";
        return process_exit_code::INVALID_ARGUMENT|3;
    } else {
        options.type(vm["type"].as<std::string>());
    }

    if (!vm.count("globalid")) {
        std::cerr << "Exception from command line processing in " << argv[0]
                  << ": no globalid file given.\n"
                  << "For usage and an options list, please do '"
                  << argv[0] <<  " --help"
                  << "'.\n";
        return process_exit_code::INVALID_ARGUMENT|4;
    } else {
        options.globalConfigurationId(vm["globalid"].as<std::string>());
    }

    if (!vm.count("database")) {
        std::cerr << "Exception from command line processing in " << argv[0]
                  << ": no database provider given.\n"
                  << "For usage and an options list, please do '"
                  << argv[0] <<  " --help"
                  << "'.\n";
        return process_exit_code::INVALID_ARGUMENT|5;

    } else {
        options.provider(vm["database"].as<std::string>());
    }

    if (!vm.count("format")) {
        std::cerr << "Exception from command line processing in " << argv[0]
                  << ": no configuration format given.\n"
                  << "For usage and an options list, please do '"
                  << argv[0] <<  " --help"
                  << "'.\n";
        return process_exit_code::INVALID_ARGUMENT|6;

    } else {
        options.dataFormat(vm["format"].as<std::string>());
    }

    if( options.operation().compare(cfol::operation_store)==0) {
        if (!vm.count("version")) {
            std::cerr << "Exception from command line processing in " << argv[0]
                      << ": no configuration version given.\n"
                      << "For usage and an options list, please do '"
                      << argv[0] <<  " --help"
                      << "'.\n";
            return process_exit_code::INVALID_ARGUMENT|7;
        } else {
            options.version(vm["version"].as<std::string>());
        }
    }
    
    std::cout << "Running test:<" << options.operation() << ">\n";

    auto runTest = [](std::string const & name) {
        auto tests = std::map<std::string, test_case> {
            {"store", test_storeconfig},
            {"load", test_loadconfig}
        };

        return tests.at(name);
    };

    auto testResult = runTest(options.operation())(options, file_name);

    if(testResult)
        return process_exit_code::SUCCESS;

    return process_exit_code::FAILURE;
}
catch(...)
{
    std::cerr << "Process exited with error: " << boost::current_exception_diagnostic_information();
    return process_exit_code::UNCAUGHT_EXCEPTION;
}

bool test_storeconfig(Options const& options, std::string const& file_name)
{
    assert(!file_name.empty());

    std::ifstream is(file_name);

    std::string configuration((std::istreambuf_iterator<char>(is)),
                              std::istreambuf_iterator<char>());

    is.close();

    auto result= cf::store_configuration(options,configuration);

    if(!result.first) {
        std::cerr << "Error message: " << result.second << "\n";
        return false;
    }

    return true;
}

bool test_loadconfig(Options const& options, std::string const& file_name)
{
    assert(!file_name.empty());

    auto configuration = std::string();

    auto result= cf::load_configuration(options,configuration);

    if(!result.first) {
        std::cerr << "Error message: " << result.second << "\n";
        return false;
    }

    std::ofstream os(file_name);
    std::copy(configuration.begin(),
              configuration.end(),
              std::ostream_iterator<char>(os));

    os.close();

    return true;
}

