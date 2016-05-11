#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>
#include "boost/program_options.hpp"
#include "artdaq-database/JsonDocument/JSONDocument.h"
#include "cetlib/coded_exception.h"
#include "artdaq-database/BuildInfo/process_exit_codes.h"
#include "artdaq-database/BuildInfo/printStackTrace.h"
#include <boost/exception/diagnostic_information.hpp>


namespace  bpo = boost::program_options;
using artdaq::database::jsonutils::JSONDocument;

typedef bool (*test_case)(std::string const&);

bool test_insertChild(std::string const& conf);
bool test_replaceChild(std::string const& conf);
bool test_deleteChild(std::string const& conf);
bool test_findChild(std::string const& conf);
bool test_appendChild(std::string const& conf);
bool test_removeChild(std::string const& conf);


int main(int argc, char* argv[]) try
{
    artdaq::database::jsonutils::debug::enableJSONDocument();

    debug::registerUngracefullExitHandlers();

    std::ostringstream descstr;
    descstr << argv[0] << " <-c <config-file>> <other-options>";

    bpo::options_description desc = descstr.str();

    desc.add_options()
    ("config,c", bpo::value<std::string>(), "Configuration file.")
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

    if (!vm.count("config")) {
        std::cerr << "Exception from command line processing in " << argv[0]
                  << ": no configuration file given.\n"
                  << "For usage and an options list, please do '"
                  << argv[0] <<  " --help"
                  << "'.\n";
        return  process_exit_code::INVALID_ARGUMENT|1;
    }

    auto file_name = vm["config"].as<std::string>();


    std::ifstream is(file_name);

    std::string conf((std::istreambuf_iterator<char>(is)),
                     std::istreambuf_iterator<char>());

    auto name = [](auto const & conf) {
        return JSONDocument(conf).value_as<std::string>("operation");
    };

    auto runTest = [](std::string const& name) {
        auto tests = std::map<std::string,test_case> {
            {"insertChild",test_insertChild},
            {"replaceChild",test_replaceChild},
            {"deleteChild",test_deleteChild},
            {"findChild",test_findChild},
            {"appendChild",test_appendChild},
            {"removeChild",test_removeChild}
        };

        std::cout << "Running test:<" << name << ">\n";

        return tests.at(name);
    };

    auto testResult= runTest(name(conf))(conf);

    if(testResult)
        return process_exit_code::SUCCESS;

    return process_exit_code::FAILURE;
}
catch(...)
{
    std::cerr << "Process exited with error: " << boost::current_exception_diagnostic_information();
    return process_exit_code::UNCAUGHT_EXCEPTION;
}

namespace literal
{
constexpr auto operation = "operation";
constexpr auto path = "path";
constexpr auto beginstate = "begin-state";
constexpr auto delta = "delta";
constexpr auto endstate = "end-state";
constexpr auto returnedvalue = "returned-value";
constexpr auto mustsucceed = "must-succeed";
}


bool test_insertChild(std::string const& conf)
{
    assert(!conf.empty());

    auto opts = JSONDocument(conf);
    auto begin = JSONDocument::loadFromFile(opts.value_as<std::string>(literal::beginstate));
    auto delta = JSONDocument::loadFromFile(opts.value_as<std::string>(literal::delta));
    auto path = opts.value_as<std::string>(literal::path);
    auto end = JSONDocument::loadFromFile(opts.value_as<std::string>(literal::endstate));
    auto expected = JSONDocument::loadFromFile(opts.value_as<std::string>(literal::returnedvalue));

    auto mustsucceed = opts.value_as<bool>(literal::mustsucceed);

    try {
        auto returned = std::move(begin.insertChild(delta, path));

        if (begin != end) {
            std::cout << "Error invalid end state. \n" ;
            std::cerr << "begin:\n" << begin << "\n";
            std::cerr << "end:\n" << end << "\n";
            return false;
        }

        if (returned != expected) {
            std::cout << "Error returned!=expected.\n";
            std::cerr << "returned:\n" << returned << "\n";
            std::cerr << "expected:\n" << expected << "\n";
            return false;
        }
    } catch (cet::exception const& e) {
        if (mustsucceed)
            throw;
    }

    return true;
}

bool test_replaceChild(std::string const& conf)
{
    assert(!conf.empty());

    auto opts = JSONDocument(conf);
    auto begin = JSONDocument::loadFromFile(opts.value_as<std::string>(literal::beginstate));
    auto delta = JSONDocument::loadFromFile(opts.value_as<std::string>(literal::delta));
    auto path = opts.value_as<std::string>(literal::path);
    auto end = JSONDocument::loadFromFile(opts.value_as<std::string>(literal::endstate));
    auto expected = JSONDocument::loadFromFile(opts.value_as<std::string>(literal::returnedvalue));

    auto mustsucceed = opts.value_as<bool>(literal::mustsucceed);

    try {
        auto returned = std::move(begin.replaceChild(delta, path));

        if (begin != end) {
            std::cout << "Error invalid end state. \n" ;
            std::cerr << "begin:\n" << begin << "\n";
            std::cerr << "end:\n" << end << "\n";
            return false;
        }

        if (returned != expected) {
            std::cout << "Error returned!=expected.\n";
            std::cerr << "returned:\n" << returned << "\n";
            std::cerr << "expected:\n" << expected << "\n";
            return false;
        }
    } catch (cet::exception const& e) {
        if (mustsucceed)
            throw;
    }

    return true;
}

bool test_deleteChild(std::string const& conf)
{
    assert(!conf.empty());

    auto opts = JSONDocument(conf);
    auto begin = JSONDocument::loadFromFile(opts.value_as<std::string>(literal::beginstate));
    auto path = opts.value_as<std::string>(literal::path);
    auto end = JSONDocument::loadFromFile(opts.value_as<std::string>(literal::endstate));
    auto expected = JSONDocument::loadFromFile(opts.value_as<std::string>(literal::returnedvalue));

    auto mustsucceed = opts.value_as<bool>(literal::mustsucceed);

    try {
        auto returned = std::move(begin.deleteChild(path));

        if (begin != end) {
            std::cout << "Error invalid end state. \n" ;
            return false;
        }

        if (returned != expected) {
            std::cout << "Error returned!=expected.\n";
            return false;
        }
    } catch (cet::exception const& e) {
        if (mustsucceed)
            throw;

//        std::cout << "Caught exception e.what()=" << e.what() << "\n";
    }

    return true;
}

bool test_findChild(std::string const& conf)
{
    assert(!conf.empty());

    auto opts = JSONDocument(conf);
    auto begin = JSONDocument::loadFromFile(opts.value_as<std::string>(literal::beginstate));
    auto path = opts.value_as<std::string>(literal::path);
    auto end = JSONDocument::loadFromFile(opts.value_as<std::string>(literal::endstate));
    auto expected = JSONDocument::loadFromFile(opts.value_as<std::string>(literal::returnedvalue));

    auto mustsucceed = opts.value_as<bool>(literal::mustsucceed);

    try {
        auto returned = std::move(begin.findChild(path));

        if (begin != end) {
            std::cout << "Error invalid end state. \n" ;
            return false;
        }

        if (returned != expected) {
            std::cout << "Error returned!=expected.\n";
            return false;
        }
    } catch (cet::exception const& e) {
        if (mustsucceed)
            throw;

        std::cout << "Caught exception e.what()=" << e.what() << "\n";
    }

    return true;
}


bool test_appendChild(std::string const& conf)
{
    assert(!conf.empty());

    auto opts = JSONDocument(conf);
    auto begin = JSONDocument::loadFromFile(opts.value_as<std::string>(literal::beginstate));
    auto delta = JSONDocument::loadFromFile(opts.value_as<std::string>(literal::delta));
    auto path = opts.value_as<std::string>(literal::path);
    auto end = JSONDocument::loadFromFile(opts.value_as<std::string>(literal::endstate));
    auto expected = JSONDocument::loadFromFile(opts.value_as<std::string>(literal::returnedvalue));

    auto mustsucceed = opts.value_as<bool>(literal::mustsucceed);

    try {
        auto returned = std::move(begin.appendChild(delta, path));

        if (begin != end) {
            std::cout << "Error invalid end state. \n" ;
            std::cerr << "begin:\n" << begin << "\n";
            std::cerr << "end:\n" << end << "\n";
            return false;
        }

        if (returned != expected) {
            std::cout << "Error returned!=expected.\n";
            std::cerr << "returned:\n" << returned << "\n";
            std::cerr << "expected:\n" << expected << "\n";
            return false;
        }
    } catch (cet::exception const& e) {
        if (mustsucceed)
            throw;
    }

    return true;
}



bool test_removeChild(std::string const& conf)
{
    assert(!conf.empty());

    auto opts = JSONDocument(conf);
    auto begin = JSONDocument::loadFromFile(opts.value_as<std::string>(literal::beginstate));
    auto delta = JSONDocument::loadFromFile(opts.value_as<std::string>(literal::delta));
    auto path = opts.value_as<std::string>(literal::path);
    auto end = JSONDocument::loadFromFile(opts.value_as<std::string>(literal::endstate));
    auto expected = JSONDocument::loadFromFile(opts.value_as<std::string>(literal::returnedvalue));

    auto mustsucceed = opts.value_as<bool>(literal::mustsucceed);

    try {
        auto returned = std::move(begin.removeChild(delta, path));

        if (begin != end) {
            std::cout << "Error invalid end state. \n" ;
            std::cerr << "begin:\n" << begin << "\n";
            std::cerr << "end:\n" << end << "\n";
            return false;
        }

        if (returned != expected) {
            std::cout << "Error returned!=expected.\n";
            std::cerr << "returned:\n" << returned << "\n";
            std::cerr << "expected:\n" << expected << "\n";
            return false;
        }

    } catch (cet::exception const& e) {
        if (mustsucceed)
            throw;
    }

    return true;
}
