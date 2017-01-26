#include "artdaq-database/BasicTypes/basictypes.h"
#include "test/common.h"

#include "artdaq-database/JsonDocument/JSONDocument.h"
#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"
#include "artdaq-database/StorageProviders/FileSystemDB/provider_filedb.h"
#include "artdaq-database/StorageProviders/storage_providers.h"

namespace bpo = boost::program_options;
using namespace artdaq::database;

using artdaq::database::docrecord::JSONDocument;
using artdaq::database::basictypes::JsonData;
using artdaq::database::basictypes::FhiclData;
using artdaq::database::docrecord::JSONDocumentBuilder;
using artdaq::database::overlay::ovlDatabaseRecord;
namespace ovl = artdaq::database::overlay;

namespace literal = artdaq::database::dataformats::literal;

typedef bool (*test_case)(std::string const&, std::string const&, std::string const&);

bool test_insert(std::string const&, std::string const&, std::string const&);
bool test_search1(std::string const&, std::string const&, std::string const&);
bool test_search2(std::string const&, std::string const&, std::string const&);
bool test_update(std::string const&, std::string const&, std::string const&);

int main(int argc, char* argv[]) try {
  artdaq::database::filesystem::debug::enable();
//  artdaq::database::docrecord::debug::JSONDocument();

  debug::registerUngracefullExitHandlers();
  artdaq::database::useFakeTime(true);

  std::ostringstream descstr;
  descstr << argv[0] << " <-s <source-file>> <-c <compare-with-file>> <-t <test-name>> [<-o <options file>>] "
                        "(available test names: insert,search1,search1,update)";

  bpo::options_description desc = descstr.str();

  desc.add_options()("source,s", bpo::value<std::string>(), "Input source file.")(
      "compare,c", bpo::value<std::string>(), "Expected result.")("testname,t", bpo::value<std::string>(),
                                                                  "Test name.")("options,o", bpo::value<std::string>(),
                                                                                "Test options file.")

      ("help,h", "produce help message");

  bpo::variables_map vm;

  try {
    bpo::store(bpo::command_line_parser(argc, argv).options(desc).run(), vm);
    bpo::notify(vm);
  } catch (bpo::error const& e) {
    std::cerr << "Exception from command line processing in " << argv[0] << ": " << e.what() << "\n";
    return process_exit_code::INVALID_ARGUMENT;
  }

  if (vm.count("help")) {
    std::cout << desc << std::endl;
    return process_exit_code::HELP;
  }

  if (!vm.count("source")) {
    std::cerr << "Exception from command line processing in " << argv[0] << ": no source file given.\n"
              << "For usage and an options list, please do '" << argv[0] << " --help"
              << "'.\n";
    return process_exit_code::INVALID_ARGUMENT | 1;
  }

  if (!vm.count("compare")) {
    std::cerr << "Exception from command line processing in " << argv[0] << ": no compare file given.\n"
              << "For usage and an options list, please do '" << argv[0] << " --help"
              << "'.\n";
    return process_exit_code::INVALID_ARGUMENT | 2;
  }

  if (!vm.count("testname")) {
    std::cerr << "Exception from command line processing in " << argv[0] << ": no test name given.\n"
              << "For usage and an options list, please do '" << argv[0] << " --help"
              << "'.\n";
    return process_exit_code::INVALID_ARGUMENT | 3;
  }

  auto input_name = vm["source"].as<std::string>();
  auto compare_name = vm["compare"].as<std::string>();
  auto test_name = vm["testname"].as<std::string>();

  std::ifstream is1(input_name);

  std::string input((std::istreambuf_iterator<char>(is1)), std::istreambuf_iterator<char>());

  std::ifstream is2(compare_name);

  std::string compare((std::istreambuf_iterator<char>(is2)), std::istreambuf_iterator<char>());

  auto options = std::string();

  if (vm.count("options")) {
    auto opts_name = vm["options"].as<std::string>();

    std::ifstream is3(opts_name);

    std::string tmp((std::istreambuf_iterator<char>(is3)), std::istreambuf_iterator<char>());

    options = std::move(tmp);
  }

  auto runTest = [](std::string const& name) {
    auto tests = std::map<std::string, test_case>{
        {"insert", test_insert}, {"update", test_update}, {"search1", test_search1}, {"search2", test_search2}};

    std::cout << "Running test:<" << name << ">\n";

    return tests.at(name);
  };

  auto testResult = runTest(test_name)(input, compare, options);

  if (testResult) return process_exit_code::SUCCESS;

  return process_exit_code::FAILURE;
} catch (...) {
  std::cerr << "Process exited with error: " << boost::current_exception_diagnostic_information();
  return process_exit_code::UNCAUGHT_EXCEPTION;
}

bool test_insert(std::string const& source_fcl, std::string const& compare_fcl, std::string const& filter) {
  confirm(!source_fcl.empty());
  confirm(!compare_fcl.empty());

  JsonData source = FhiclData(source_fcl);
  JsonData compare = FhiclData(compare_fcl);

  // validate source
  auto insert = JSONDocument(source.json_buffer);

  // validate compare
  auto cmpdoc = JSONDocument(compare.json_buffer);

  namespace DBI = artdaq::database::filesystem;

  auto config = DBI::DBConfig{};
  auto database = DBI::DB::create(config);
  auto provider = DBI::DBProvider<JsonData>::create(database);

  auto collection = std::string("testFHICL_V001");

  auto json = JsonData{"{\"document\":" + insert.to_string() + ", \"collection\":\"" + collection + "\"}"};

  auto object_id = provider->writeDocument(json);

  auto search =
      JsonData{"{\"filter\":" + (filter.empty() ? object_id : filter) + ", \"collection\":\"" + collection + "\"}"};

  std::cout << "Search criteria " << search.json_buffer << "\n";

  auto results = provider->readDocument(search);

  if (results.size() != 1) {
    std::cout << "Search returned " << results.size() << " results.\n";

    for (auto&& element : results) {
      std::cout << element.json_buffer << "\n";
    }
    return false;
  }

  auto retdoc = JSONDocument(results.begin()->json_buffer);

  JSONDocumentBuilder returned{retdoc};
  JSONDocumentBuilder expected{cmpdoc};

  using namespace artdaq::database::overlay;
  ovl::useCompareMask(DOCUMENT_COMPARE_MUTE_TIMESTAMPS | DOCUMENT_COMPARE_MUTE_OUIDS | DOCUMENT_COMPARE_MUTE_UPDATES |
                      DOCUMENT_COMPARE_MUTE_COLLECTION);

  auto result = returned == expected;

  if (result.first) return true;

  std::cout << "Error returned!=expected.\n";
  std::cerr << "returned:\n" << returned << "\n";
  std::cerr << "expected:\n" << expected << "\n";
  std::cerr << "error:\n" << result.second << "\n";
  return false;
}

bool test_search1(std::string const& source_fcl, std::string const& compare_fcl, std::string const& filter) {
  confirm(!source_fcl.empty());
  confirm(!compare_fcl.empty());

  JsonData source = FhiclData(source_fcl);
  JsonData compare = FhiclData(compare_fcl);

  // validate source
  auto insert = JSONDocument(source.json_buffer);

  // validate compare
  auto cmpdoc = JSONDocument(compare.json_buffer);

  using artdaq::database::basictypes::JsonData;
  using artdaq::database::basictypes::FhiclData;

  namespace DBI = artdaq::database::filesystem;

  auto config = DBI::DBConfig{};
  auto database = DBI::DB::create(config);
  auto provider = DBI::DBProvider<JsonData>::create(database);

  auto collection = std::string("testFHICL_V001");

  auto json = JsonData{"{\"document\":" + insert.to_string() + ", \"collection\":\"" + collection + "\"}"};

  auto object_id = provider->writeDocument(json);

  auto search =
      JsonData{"{\"filter\":" + (filter.empty() ? object_id : filter) + ", \"collection\":\"" + collection + "\"}"};

  std::cout << "Search criteria " << search.json_buffer << "\n";

  auto results = provider->readDocument(search);

  if (results.size() != 1) {
    std::cout << "Search returned " << results.size() << " results.\n";

    for (auto&& element : results) {
      std::cout << element.json_buffer << "\n";
    }
    return false;
  }

  auto retdoc = JSONDocument(results.begin()->json_buffer);
  JSONDocumentBuilder returned{retdoc};
  JSONDocumentBuilder expected{cmpdoc};

  using namespace artdaq::database::overlay;
  ovl::useCompareMask(DOCUMENT_COMPARE_MUTE_TIMESTAMPS | DOCUMENT_COMPARE_MUTE_OUIDS | DOCUMENT_COMPARE_MUTE_UPDATES |
                      DOCUMENT_COMPARE_MUTE_COLLECTION|DOCUMENT_COMPARE_MUTE_COMMENTS);

  auto result = returned == expected;

  if (result.first) return true;

  std::cout << "Error returned!=expected.\n";
  std::cerr << "returned:\n" << returned << "\n";
  std::cerr << "expected:\n" << expected << "\n";
  std::cerr << "error:\n" << result.second << "\n";
  return false;
}

bool test_search2(std::string const& source_fcl, std::string const& compare_fcl,
                  std::string const& options[[gnu::unused]]) {
  confirm(!source_fcl.empty());
  confirm(!compare_fcl.empty());

  JsonData source = FhiclData(source_fcl);
  JsonData compare = FhiclData(compare_fcl);

  namespace DBI = artdaq::database::filesystem;

  auto config = DBI::DBConfig{};
  auto database = DBI::DB::create(config);
  auto provider = DBI::DBProvider<JsonData>::create(database);

  auto collection = std::string("testFHICL_V001");

  auto json = JsonData{"{\"document\":" + source.json_buffer + ", \"collection\":\"" + collection + "\"}"};

  auto repeatCount = std::size_t{10};

  auto object_ids = std::vector<std::string>();

  std::ostringstream oss;
  oss << "{\"_id\" : { \"$in\" : [";

  auto nodata_pos = oss.tellp();

  for (int i = repeatCount; i != 0; i--) {
    auto object_id = provider->writeDocument(json);
    object_ids.push_back(object_id);
    oss << JSONDocument(object_id).findChildDocument("_id").to_string() << ",";
  }
  
  if (oss.tellp() != nodata_pos) 
    oss.seekp(-1, oss.cur);
  
  oss << "]} }";

  auto filter = oss.str();

  auto search =
      JsonData{"{\"filter\":" + (filter.empty() ? options : filter) + ", \"collection\":\"" + collection + "\"}"};

  std::cout << "Search criteria " << search.json_buffer << "\n";

  auto results = provider->readDocument(search);

  if (results.size() != repeatCount) {
    std::cout << "Search returned " << results.size() << " results.\n";

    for (auto&& element : results) {
      std::cout << element.json_buffer << "\n";
    }
    return false;
  }

  return true;
}

bool test_update(std::string const& source_fcl, std::string const& compare_fcl, std::string const& update_fcl) {
  confirm(!source_fcl.empty());
  confirm(!compare_fcl.empty());

  JsonData source = FhiclData(source_fcl);
  JsonData compare = FhiclData(compare_fcl);
  JsonData update = FhiclData(update_fcl);

  // validate source
  auto insert = JSONDocument(source.json_buffer);
  //insert.deleteChild(literal::comments);

  // validate compare
  auto cmpdoc = JSONDocument(compare.json_buffer);
  //cmpdoc.deleteChild(literal::comments);

  auto changes = JSONDocument(update.json_buffer);
  //changes.deleteChild(literal::comments);

  namespace DBI = artdaq::database::filesystem;

  auto config = DBI::DBConfig{};
  auto database = DBI::DB::create(config);
  auto provider = DBI::DBProvider<JsonData>::create(database);

  auto collection = std::string("testFHICL_V001");

  auto json = JsonData{"{\"document\":" + insert.to_string() + ", \"collection\":\"" + collection + "\"}"};

  auto object_id = provider->writeDocument(json);

  auto search = JsonData{"{\"filter\":" + object_id + ", \"collection\":\"" + collection + "\"}"};

  std::cout << "Search criteria " << search.json_buffer << "\n";

  auto results = provider->readDocument(search);

  if (results.size() != 1) {
    std::cout << "Search returned " << results.size() << " results.\n";

    for (auto&& element : results) {
      std::cout << element.json_buffer << "\n";
    }
    return false;
  }

  auto found = JSONDocument(results.begin()->json_buffer);
  auto payload = changes.findChild("document");
  found.replaceChild(payload, "document");

  json = JsonData{"{\"document\":" + found.to_string() + ", \"filter\":" + object_id + ",\"collection\":\"" +
                  collection + "\"}"};

  object_id = provider->writeDocument(json);

  results = provider->readDocument(search);

  if (results.size() != 1) {
    std::cout << "Search returned " << results.size() << " results.\n";

    for (auto&& element : results) {
      std::cout << element.json_buffer << "\n";
    }
    return false;
  }

  auto retdoc = JSONDocument(results.begin()->json_buffer);
  JSONDocumentBuilder returned{retdoc};
  JSONDocumentBuilder expected{cmpdoc};

  using namespace artdaq::database::overlay;
  ovl::useCompareMask(DOCUMENT_COMPARE_MUTE_TIMESTAMPS | DOCUMENT_COMPARE_MUTE_OUIDS | DOCUMENT_COMPARE_MUTE_UPDATES |
                      DOCUMENT_COMPARE_MUTE_COLLECTION|DOCUMENT_COMPARE_MUTE_COMMENTS);

  auto result = returned == expected;

  if (result.first) return true;

  std::cout << "Error returned!=expected.\n";
  std::cerr << "returned:\n" << returned << "\n";
  std::cerr << "expected:\n" << expected << "\n";
  std::cerr << "error:\n" << result.second << "\n";
  return false;
}
