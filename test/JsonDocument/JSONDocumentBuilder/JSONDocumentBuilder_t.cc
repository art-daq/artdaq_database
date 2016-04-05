#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>
#include "boost/program_options.hpp"
#include "artdaq-database/JsonDocument/JSONDocument.h"
#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"

#include "cetlib/coded_exception.h"
#include "artdaq-database/BuildInfo/printStackTrace.h"

namespace  bpo = boost::program_options;
using artdaq::database::jsonutils::JSONDocument;
using artdaq::database::jsonutils::JSONDocumentBuilder;


typedef bool (*test_case)(std::string const&);

bool test_buildDocument(std::string const& conf);

bool test_addAlias(std::string const& conf);
bool test_setVersion(std::string const& conf);
bool test_removeAlias(std::string const& conf);
bool test_markDeleted(std::string const& conf);
bool test_markReadonly(std::string const& conf);
bool test_setVersion(std::string const& conf);
bool test_addToGlobalConfig(std::string const& conf);



int main(int argc, char* argv[]) 
{
    artdaq::database::jsonutils::trace_enable_JSONDocument();

    artdaq::database::jsonutils::trace_enable_JSONDocumentBuilder();

    artdaq::database::jsonutils::useFakeTime(true);
    
    debug::registerUngracefullExitHandlers();

    // Get the input parameters via the boost::program_options library,
    // designed to make it relatively simple to define arguments and
    // issue errors if argument list is supplied incorrectly

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

    std::ifstream is(file_name);

    std::string conf((std::istreambuf_iterator<char>(is)),
                     std::istreambuf_iterator<char>());

    auto name = [](auto const & conf) {
        return JSONDocument(conf).value_as<std::string>("operation");
    };

    auto runTest = [](std::string const& name) {
        auto tests = std::map<std::string,test_case> {
            {"buildDocument",test_buildDocument},
            {"addAlias",test_addAlias},
            {"removeAlias",test_removeAlias},
	    {"markDeleted",test_markDeleted},
	    {"markReadonly",test_markReadonly},
	    {"addToGlobalConfig",test_addToGlobalConfig},
	    {"setVersion",test_setVersion}
        };

        std::cout << "Running test:<" << name << ">\n";

        return tests.at(name);
    };

    auto testResult= runTest(name(conf))(conf);

    return !testResult;
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


bool test_buildDocument(std::string const& conf)
{
    assert(!conf.empty());

    auto opts = JSONDocument(conf);
    auto begin = JSONDocument::loadFromFile(opts.value_as<std::string>(literal::beginstate));
    auto builder= JSONDocumentBuilder();
    auto end = JSONDocument::loadFromFile(opts.value_as<std::string>(literal::endstate));

    auto mustsucceed = opts.value_as<bool>(literal::mustsucceed);

    try {
        auto returned = std::move(builder.createFromData(begin).extract());

        if (returned != end && mustsucceed ) {
            std::cout << "Error returned!=expected.\n";
            std::cerr << "returned:\n" << returned << "\n";
            std::cerr << "expected:\n" << end << "\n";
            return false;
        }
        
    } catch (cet::exception const& e) {
        if (mustsucceed)
            throw;
    }

    return true;
}

bool test_addAlias(std::string const& conf)
{
    assert(!conf.empty());

    auto opts = JSONDocument(conf);
    auto begin = JSONDocument::loadFromFile(opts.value_as<std::string>(literal::beginstate));
    auto delta = JSONDocument::loadFromFile(opts.value_as<std::string>(literal::delta));
    auto end = JSONDocument::loadFromFile(opts.value_as<std::string>(literal::endstate));

    auto mustsucceed = opts.value_as<bool>(literal::mustsucceed);

    try {
        auto builder= JSONDocumentBuilder(begin);
        auto returned = std::move(builder.addAlias(delta).extract());

        if (returned != end && mustsucceed ) {
            std::cout << "Error returned!=expected.\n";
            std::cerr << "returned:\n" << returned << "\n";
            std::cerr << "expected:\n" << end << "\n";
            return false;
        }
    } catch (cet::exception const& e) {
        if (mustsucceed)
            throw;
    }

    return true;
}

bool test_removeAlias(std::string const& conf)
{
    assert(!conf.empty());

    auto opts = JSONDocument(conf);
    auto begin = JSONDocument::loadFromFile(opts.value_as<std::string>(literal::beginstate));
    auto delta = JSONDocument::loadFromFile(opts.value_as<std::string>(literal::delta));
    auto end = JSONDocument::loadFromFile(opts.value_as<std::string>(literal::endstate));

    auto mustsucceed = opts.value_as<bool>(literal::mustsucceed);

    try {
        auto builder= JSONDocumentBuilder(begin);
        auto returned = std::move(builder.removeAlias(delta).extract());

        if (returned != end && mustsucceed ) {
            std::cout << "Error returned!=expected.\n";
            std::cerr << "returned:\n" << returned << "\n";
            std::cerr << "expected:\n" << end << "\n";
            return false;
        }
    } catch (cet::exception const& e) {
        if (mustsucceed)
            throw;
    }

    return true;
}


bool test_addToGlobalConfig(std::string const& conf)
{
    assert(!conf.empty());

    auto opts = JSONDocument(conf);
    auto begin = JSONDocument::loadFromFile(opts.value_as<std::string>(literal::beginstate));
    auto delta = JSONDocument::loadFromFile(opts.value_as<std::string>(literal::delta));
    auto end = JSONDocument::loadFromFile(opts.value_as<std::string>(literal::endstate));

    auto mustsucceed = opts.value_as<bool>(literal::mustsucceed);

    try {
        auto builder= JSONDocumentBuilder(begin);
        auto returned = std::move(builder.addToGlobalConfig(delta).extract());

        if (returned != end && mustsucceed ) {
            std::cout << "Error returned!=expected.\n";
            std::cerr << "returned:\n" << returned << "\n";
            std::cerr << "expected:\n" << end << "\n";
            return false;
        }
    } catch (cet::exception const& e) {
        if (mustsucceed)
            throw;
    }

    return true;
}

bool test_setVersion(std::string const& conf)
{
    assert(!conf.empty());

    auto opts = JSONDocument(conf);
    auto begin = JSONDocument::loadFromFile(opts.value_as<std::string>(literal::beginstate));
    auto delta = JSONDocument::loadFromFile(opts.value_as<std::string>(literal::delta));
    auto end = JSONDocument::loadFromFile(opts.value_as<std::string>(literal::endstate));

    auto mustsucceed = opts.value_as<bool>(literal::mustsucceed);

    try {
        auto builder= JSONDocumentBuilder(begin);
        auto returned = std::move(builder.setVersion(delta).extract());

        if (returned != end && mustsucceed ) {
            std::cout << "Error returned!=expected.\n";
            std::cerr << "returned:\n" << returned << "\n";
            std::cerr << "expected:\n" << end << "\n";
            return false;
        }
    } catch (cet::exception const& e) {
        if (mustsucceed)
            throw;
    }

    return true;
}

bool test_markReadonly(std::string const& conf)
{
    assert(!conf.empty());

    auto opts = JSONDocument(conf);
    auto begin = JSONDocument::loadFromFile(opts.value_as<std::string>(literal::beginstate));
    auto end = JSONDocument::loadFromFile(opts.value_as<std::string>(literal::endstate));

    auto mustsucceed = opts.value_as<bool>(literal::mustsucceed);

    try {
        auto builder= JSONDocumentBuilder(begin);
        auto returned = std::move(builder.markReadonly().extract());

        if (returned != end && mustsucceed ) {
            std::cout << "Error returned!=expected.\n";
            std::cerr << "returned:\n" << returned << "\n";
            std::cerr << "expected:\n" << end << "\n";
            return false;
        }
    } catch (cet::exception const& e) {
        if (mustsucceed)
            throw;
    }

    return true;
}

bool test_markDeleted(std::string const& conf)
{
    assert(!conf.empty());

    auto opts = JSONDocument(conf);
    auto begin = JSONDocument::loadFromFile(opts.value_as<std::string>(literal::beginstate));
    auto end = JSONDocument::loadFromFile(opts.value_as<std::string>(literal::endstate));

    auto mustsucceed = opts.value_as<bool>(literal::mustsucceed);

    try {
        auto builder= JSONDocumentBuilder(begin);
        auto returned = std::move(builder.markDeleted().extract());

        if (returned != end && mustsucceed ) {
            std::cout << "Error returned!=expected.\n";
            std::cerr << "returned:\n" << returned << "\n";
            std::cerr << "expected:\n" << end << "\n";
            return false;
        }
    } catch (cet::exception const& e) {
        if (mustsucceed)
            throw;
    }

    return true;
}
