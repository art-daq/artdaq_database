#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>
#include "boost/program_options.hpp"
#include "artdaq-database/FhiclJson/fhicljsondb.h"
#include "artdaq-database/FhiclJson/healper_functions.h"
#include "cetlib/coded_exception.h"
#include "artdaq-database/BuildInfo/process_exit_codes.h"
#include "artdaq-database/BuildInfo/printStackTrace.h"
#include <boost/exception/diagnostic_information.hpp>


namespace  bpo = boost::program_options;
using namespace artdaq::database;


typedef bool (*test_case)(std::string const&, std::string const&);

bool test_convert2fcl(std::string const&, std::string const&);
bool test_convert2json(std::string const&, std::string const&);
bool test_roundconvertfcl(std::string const&, std::string const&);
bool test_roundconvertjson(std::string const&, std::string const&);

int main(int argc, char* argv[]) //try
{
    artdaq::database::fhicl::trace_enable_FhiclReader();
    artdaq::database::fhicl::trace_enable_FhiclWriter();
    artdaq::database::fhicljson::trace_enable_fcl2jsondb();
    artdaq::database::fhicljsondb::trace_enable_fhicljsondb();
    
    debug::registerUngracefullExitHandlers();
    artdaq::database::fhicljson::useFakeTime(true);

    // Get the input parameters via the boost::program_options library,
    // designed to make it relatively simple to define arguments and
    // issue errors if argument list is supplied incorrectly

    std::ostringstream descstr;
    descstr << argv[0] << " <-s <source-file>> <-c <compare-with-file>> <-t <test-name>> (available test names: convert2fcl,convert2json,roundconvertfcl,roundconvertjson)";

    bpo::options_description desc = descstr.str();

    desc.add_options()
    ("source,s", bpo::value<std::string>(),  "Input source file.")
    ("compare,c", bpo::value<std::string>(), "Expected result of convertion.")
    ("testname,t", bpo::value<std::string>(), "Test name.")

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
    
    if (!vm.count("source")) {
        std::cerr << "Exception from command line processing in " << argv[0]
                  << ": no source file given.\n"
                  << "For usage and an options list, please do '"
                  << argv[0] <<  " --help"
                  << "'.\n";
        return process_exit_code::INVALID_ARGUMENT|1;
    }

    if (!vm.count("compare")) {
        std::cerr << "Exception from command line processing in " << argv[0]
                  << ": no compare file given.\n"
                  << "For usage and an options list, please do '"
                  << argv[0] <<  " --help"
                  << "'.\n";
        return process_exit_code::INVALID_ARGUMENT|2;
    }

    if (!vm.count("testname")) {
        std::cerr << "Exception from command line processing in " << argv[0]
                  << ": no test name given.\n"
                  << "For usage and an options list, please do '"
                  << argv[0] <<  " --help"
                  << "'.\n";
        return process_exit_code::INVALID_ARGUMENT|3;
    }

    auto input_name = vm["source"].as<std::string>();
    auto compare_name = vm["compare"].as<std::string>();
    auto test_name = vm["testname"].as<std::string>();

    std::ifstream is1(input_name);

    std::string input((std::istreambuf_iterator<char>(is1)),
                      std::istreambuf_iterator<char>());


    std::ifstream is2(compare_name);

    std::string compare((std::istreambuf_iterator<char>(is2)),
                        std::istreambuf_iterator<char>());


    auto runTest = [](std::string const & name) {
        auto tests = std::map<std::string, test_case> {
            {"convert2fcl", test_convert2fcl},
            {"convert2json", test_convert2json},
            {"roundconvertfcl", test_roundconvertfcl},
            {"roundconvertjson", test_roundconvertjson}
        };

        std::cout << "Running test:<" << name << ">\n";

        return tests.at(name);
    };

    auto testResult = runTest(test_name)(input, compare);

    if(testResult)
        return process_exit_code::SUCCESS;

    return process_exit_code::FAILURE;
}
//catch(...)
//{
//    using namespace debug;    
//   std::cerr << current_exception_diagnostic_information();    
//    return process_exit_code::UNCAUGHT_EXCEPTION;
//}

bool test_convert2fcl(std::string const& input, std::string const& compare)
{
    assert(!input.empty());
    assert(!compare.empty());

    auto output = std::string();

    if (!fhicljsondb::json_to_fhicl(input, output))
        return false;

    if (output == compare)
        return true;
    else {
        std::cout << "Convertion failed. \n" ;
        std::cerr << "output:\n" << output << "\n";
        std::cerr << "expected:\n" << compare << "\n";
    }

    return false;
}

bool test_convert2json(std::string const& input, std::string const& compare)
{
    assert(!input.empty());
    assert(!compare.empty());

    auto output = std::string();

    std::cout << "Convertion started. \n" ;

    if (!fhicljsondb::fhicl_to_json(input, output)) {
	std::cout << "Convertion failed. \n" ;
        return false;
    }
    
    if (output == compare)
        return true;
    else {
        std::cout << "Convertion failed. \n" ;
        std::cerr << "output:\n" << output << "\n";
        std::cerr << "expected:\n" << compare << "\n";
    }

    return false;
}

bool test_roundconvertfcl(std::string const& input, std::string const& compare)
{
    assert(!input.empty());
    assert(!compare.empty());

    auto tmp = std::string();
    auto output = std::string();

    try {
        if (!fhicljsondb::fhicl_to_json(input, tmp))
            return false;

        std::cout << "fhicl_to_json succeeded.\n" ;

        if (!fhicljsondb::json_to_fhicl(tmp, output))
            return false;

        std::cout << "json_to_fhicl succeeded.\n" ;

    } catch (...) {
        std::cout << "Convertion failed. \n" ;
        std::cerr << "output:\n" << output << "\n";
        std::cerr << "expected:\n" << compare << "\n";
        std::cerr << "tmp:\n" << tmp << "\n";
        throw;
    }


    if (output == compare)
        return true;
    else {
        std::cout << "Convertion failed. \n" ;
        std::cerr << "output:\n" << output << "\n";
        std::cerr << "expected:\n" << compare << "\n";
        std::cerr << "tmp:\n" << tmp << "\n";
    }

    return false;
}

bool test_roundconvertjson(std::string const& input, std::string const& compare)
{
    assert(!input.empty());
    assert(!compare.empty());


    auto tmp = std::string();
    auto output = std::string();

    try {
        if (!fhicljsondb::json_to_fhicl(input, tmp))
            return false;

        std::cout << "json_to_fhicl succeeded.\n" ;

        if (!fhicljsondb::fhicl_to_json(tmp, output))
            return false;
        std::cout << "fhicl_to_json succeeded.\n" ;

    } catch (...) {
        std::cout << "Convertion failed. \n" ;
        std::cerr << "output:\n" << output << "\n";
        std::cerr << "expected:\n" << compare << "\n";
        std::cerr << "tmp:\n" << tmp << "\n";
        throw;
    }


    if (output == compare)
        return true;
    else {
        std::cout << "Convertion failed. \n" ;
        std::cerr << "output:\n" << output << "\n";
        std::cerr << "expected:\n" << compare << "\n";
        std::cerr << "tmp:\n" << tmp << "\n";
    }

    return false;
}
