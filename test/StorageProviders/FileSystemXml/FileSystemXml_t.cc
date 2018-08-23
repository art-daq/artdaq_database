#include "artdaq-database/BasicTypes/basictypes.h"
#include "test/common.h"

#include "artdaq-database/JsonDocument/JSONDocument.h"
#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"
#include "artdaq-database/StorageProviders/FileSystemDB/provider_filedb.h"
#include "artdaq-database/StorageProviders/storage_providers.h"

namespace bpo = boost::program_options;
using namespace artdaq::database;

using artdaq::database::basictypes::JsonData;
using artdaq::database::basictypes::XmlData;
using artdaq::database::docrecord::JSONDocument;
using artdaq::database::docrecord::JSONDocumentBuilder;
namespace ovl = artdaq::database::overlay;

using test_case = bool (*)(const std::string&, const std::string&, const std::string&);

bool test_insert(std::string const& /*source_xml*/, std::string const& /*compare_xml*/, std::string const& /*filter*/);
bool test_search1(std::string const& /*source_xml*/, std::string const& /*compare_xml*/, std::string const& /*filter*/);
bool test_search2(std::string const& /*source_xml*/, std::string const& /*compare_xml*/,
                  std::string const& /*options*/);
bool test_update(std::string const& /*source_xml*/, std::string const& /*compare_xml*/,
                 std::string const& /*update_xml*/);

int main(int argc, char* argv[]) try {
  artdaq::database::filesystem::debug::enable();
  //    artdaq::database::docrecord::debug::JSONDocument();

  debug::registerUngracefullExitHandlers();
  artdaq::database::useFakeTime(true);

  std::ostringstream descstr;
  descstr << argv[0]
          << " <-s <source-file>> <-c <compare-with-file>> <-t <test-name>> [<-o <options file>>] (available test "
             "names: insert,search1,search1,update)";

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

  if (vm.count("help") != 0u) {
    std::cout << desc << std::endl;
    return process_exit_code::HELP;
  }

  if (vm.count("source") == 0u) {
    std::cerr << "Exception from command line processing in " << argv[0] << ": no source file given.\n"
              << "For usage and an options list, please do '" << argv[0] << " --help"
              << "'.\n";
    return process_exit_code::INVALID_ARGUMENT | 1;
  }

  if (vm.count("compare") == 0u) {
    std::cerr << "Exception from command line processing in " << argv[0] << ": no compare file given.\n"
              << "For usage and an options list, please do '" << argv[0] << " --help"
              << "'.\n";
    return process_exit_code::INVALID_ARGUMENT | 2;
  }

  if (vm.count("testname") == 0u) {
    std::cerr << "Exception from command line processing in " << argv[0] << ": no test name given.\n"
              << "For usage and an options list, please do '" << argv[0] << " --help"
              << "'.\n";
    return process_exit_code::INVALID_ARGUMENT | 3;
  }

  auto input_name = vm["source"].as<std::string>();
  auto compare_name = vm["compare"].as<std::string>();
  auto test_name = vm["testname"].as<std::string>();

  auto input = std::string{};
  db::read_buffer_from_file(input, input_name);

  auto compare = std::string{};
  db::read_buffer_from_file(compare, compare_name);

  auto options = std::string();

  if (vm.count("options") != 0u) {
    auto opts_name = vm["options"].as<std::string>();

    db::read_buffer_from_file(options, opts_name);
  }

  auto runTest = [](std::string const& name) {
    auto tests = std::map<std::string, test_case>{
        {"insert", test_insert}, {"update", test_update}, {"search1", test_search1}, {"search2", test_search2}};

    std::cout << "Running test:<" << name << ">\n";

    return tests.at(name);
  };

  auto testResult = runTest(test_name)(input, compare, options);

  if (testResult) {
    return process_exit_code::SUCCESS;
  }

  return process_exit_code::FAILURE;
} catch (...) {
  std::cerr << "Process exited with error: " << ::debug::current_exception_diagnostic_information();
  return process_exit_code::UNCAUGHT_EXCEPTION;
}

bool test_insert(std::string const& source_xml, std::string const& compare_xml, std::string const& filter) {
  confirm(!source_xml.empty());
  confirm(!compare_xml.empty());

  JsonData source = XmlData(source_xml);
  JsonData compare = XmlData(compare_xml);

  // validate source
  auto insert = JSONDocument(source);

  // validate compare
  auto cmpdoc = JSONDocument(compare);

  namespace DBI = artdaq::database::filesystem;

  auto config = DBI::DBConfig{};
  auto database = DBI::DB::create(config);
  auto provider = DBI::DBProvider<JSONDocument>::create(database);

  auto collection = std::string("testXML_V001");

  auto json = JSONDocument{"{\"document\":" + insert.to_string() + ", \"collection\":\"" + collection + "\"}"};

  auto object_id = provider->writeDocument(json);

  auto search =
      JSONDocument{"{\"filter\":" + (filter.empty() ? object_id : filter) + ", \"collection\":\"" + collection + "\"}"};

  std::cout << "Search criteria " << search << "\n";

  auto results = provider->readDocument(search);

  if (results.size() != 1) {
    std::cout << "Search returned " << results.size() << " results.\n";

    for (auto&& element : results) {
      std::cout << element << "\n";
    }
    return false;
  }

  auto retdoc = JSONDocument{*results.begin()};

  JSONDocumentBuilder returned{retdoc};
  JSONDocumentBuilder expected{cmpdoc};

  using namespace artdaq::database::overlay;
  ovl::useCompareMask(DOCUMENT_COMPARE_MUTE_TIMESTAMPS | DOCUMENT_COMPARE_MUTE_OUIDS | DOCUMENT_COMPARE_MUTE_UPDATES |
                      DOCUMENT_COMPARE_MUTE_COLLECTION);

  auto result = returned == expected;

  if (result.first) {
    return true;
  }

  std::cout << "Error returned!=expected.\n";
  std::cerr << "returned:\n" << returned << "\n";
  std::cerr << "expected:\n" << expected << "\n";
  std::cerr << "error:\n" << result.second << "\n";
  return false;
}

bool test_search1(std::string const& source_xml, std::string const& compare_xml, std::string const& filter) {
  confirm(!source_xml.empty());
  confirm(!compare_xml.empty());

  JsonData source = XmlData(source_xml);
  JsonData compare = XmlData(compare_xml);

  // validate source
  auto insert = JSONDocument(source);

  // validate compare
  auto cmpdoc = JSONDocument(compare);

  using artdaq::database::basictypes::JsonData;
  using artdaq::database::basictypes::XmlData;

  namespace DBI = artdaq::database::filesystem;

  auto config = DBI::DBConfig{};
  auto database = DBI::DB::create(config);
  auto provider = DBI::DBProvider<JSONDocument>::create(database);

  auto collection = std::string("testXML_V001");

  auto json = JSONDocument{"{\"document\":" + insert.to_string() + ", \"collection\":\"" + collection + "\"}"};

  auto object_id = provider->writeDocument(json);

  auto search =
      JSONDocument{"{\"filter\":" + (filter.empty() ? object_id : filter) + ", \"collection\":\"" + collection + "\"}"};

  std::cout << "Search criteria " << search << "\n";

  auto results = provider->readDocument(search);

  if (results.size() != 1) {
    std::cout << "Search returned " << results.size() << " results.\n";

    for (auto&& element : results) {
      std::cout << element << "\n";
    }
    return false;
  }

  auto retdoc = JSONDocument{*results.begin()};
  JSONDocumentBuilder returned{retdoc};
  JSONDocumentBuilder expected{cmpdoc};

  using namespace artdaq::database::overlay;
  ovl::useCompareMask(DOCUMENT_COMPARE_MUTE_TIMESTAMPS | DOCUMENT_COMPARE_MUTE_OUIDS | DOCUMENT_COMPARE_MUTE_UPDATES |
                      DOCUMENT_COMPARE_MUTE_COLLECTION | DOCUMENT_COMPARE_MUTE_COMMENTS);

  auto result = returned == expected;

  if (result.first) {
    return true;
  }

  std::cout << "Error returned!=expected.\n";
  std::cerr << "returned:\n" << returned << "\n";
  std::cerr << "expected:\n" << expected << "\n";
  std::cerr << "error:\n" << result.second << "\n";
  return false;
}
bool test_search2(std::string const& source_xml, std::string const& compare_xml,
                  std::string const& options[[gnu::unused]]) {
  confirm(!source_xml.empty());
  confirm(!compare_xml.empty());

  JsonData source = XmlData(source_xml);
  JsonData compare = XmlData(compare_xml);

  namespace DBI = artdaq::database::filesystem;

  auto config = DBI::DBConfig{};
  auto database = DBI::DB::create(config);
  auto provider = DBI::DBProvider<JSONDocument>::create(database);

  auto collection = std::string("testXML_V001");

  auto json = JSONDocument{"{\"document\":" + source.json_buffer + ", \"collection\":\"" + collection + "\"}"};

  auto repeatCount = std::size_t{10};

  auto object_ids = std::vector<std::string>();

  std::ostringstream oss;
  oss << R"({"_id" : { "$in" : [)";

  auto nodata_pos = oss.tellp();

  for (int i = repeatCount; i != 0; i--) {
    auto object_id = provider->writeDocument(json);
    object_ids.push_back(object_id);
    oss << JSONDocument(object_id).findChildDocument("_id").to_string() << ",";
  }

  if (oss.tellp() != nodata_pos) {
    oss.seekp(-1, oss.cur);
  }

  oss << "]} }";

  auto filter = oss.str();

  auto search =
      JSONDocument{"{\"filter\":" + (filter.empty() ? options : filter) + ", \"collection\":\"" + collection + "\"}"};

  std::cout << "Search criteria " << search << "\n";

  auto results = provider->readDocument(search);

  if (results.size() != repeatCount) {
    std::cout << "Search returned " << results.size() << " results.\n";

    for (auto&& element : results) {
      std::cout << element << "\n";
    }
    return false;
  }

  return true;
}

bool test_update(std::string const& source_xml, std::string const& compare_xml, std::string const& update_xml) {
  confirm(!source_xml.empty());
  confirm(!compare_xml.empty());

  JsonData source = XmlData(source_xml);
  JsonData compare = XmlData(compare_xml);
  JsonData update = XmlData(update_xml);

  // validate source
  auto insert = JSONDocument(source);
  // insert.deleteChild(literal::comments_node);

  // validate compare
  auto cmpdoc = JSONDocument(compare);
  // expected.deleteChild(literal::comments_node);

  auto changes = JSONDocument(update);
  // changes.deleteChild(literal::comments_node);

  namespace DBI = artdaq::database::filesystem;

  auto config = DBI::DBConfig{};
  auto database = DBI::DB::create(config);
  auto provider = DBI::DBProvider<JSONDocument>::create(database);

  auto collection = std::string("testXML_V001");

  auto json = JSONDocument{"{\"document\":" + insert.to_string() + ", \"collection\":\"" + collection + "\"}"};

  auto object_id = provider->writeDocument(json);

  auto search = JSONDocument{"{\"filter\":" + object_id + ", \"collection\":\"" + collection + "\"}"};

  std::cout << "Search criteria " << search << "\n";

  auto results = provider->readDocument(search);

  if (results.size() != 1) {
    std::cout << "Search returned " << results.size() << " results.\n";

    for (auto&& element : results) {
      std::cout << element << "\n";
    }
    return false;
  }

  auto found = JSONDocument{*results.begin()};
  auto payload = changes.findChild("document");
  found.replaceChild(payload, "document");

  json = JsonData{"{\"document\":" + found.to_string() + ", \"filter\":" + object_id + ",\"collection\":\"" +
                  collection + "\"}"};

  object_id = provider->writeDocument(json);

  results = provider->readDocument(search);

  if (results.size() != 1) {
    std::cout << "Search returned " << results.size() << " results.\n";

    for (auto&& element : results) {
      std::cout << element << "\n";
    }
    return false;
  }

  auto retdoc = JSONDocument{*results.begin()};
  JSONDocumentBuilder returned{retdoc};
  JSONDocumentBuilder expected{cmpdoc};

  using namespace artdaq::database::overlay;
  ovl::useCompareMask(DOCUMENT_COMPARE_MUTE_TIMESTAMPS | DOCUMENT_COMPARE_MUTE_OUIDS | DOCUMENT_COMPARE_MUTE_UPDATES |
                      DOCUMENT_COMPARE_MUTE_COLLECTION | DOCUMENT_COMPARE_MUTE_COMMENTS);

  auto result = returned == expected;

  if (result.first) {
    return true;
  }

  std::cout << "Error returned!=expected.\n";
  std::cerr << "returned:\n" << returned << "\n";
  std::cerr << "expected:\n" << expected << "\n";
  std::cerr << "error:\n" << result.second << "\n";
  return false;
}