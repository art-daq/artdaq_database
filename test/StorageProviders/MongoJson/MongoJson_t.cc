#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>
#include <sstream> 


#include "boost/program_options.hpp"
#include "artdaq-database/StorageProviders/common.h"
#include "artdaq-database/BasicTypes/basictypes.h"
#include "artdaq-database/BuildInfo/printStackTrace.h"

#include "artdaq-database/StorageProviders/storage_providers.h"
#include "artdaq-database/StorageProviders/MongoDB/provider_mongodb.h"
#include "artdaq-database/JsonDocument/JSONDocument.h"
#include <bsoncxx/json.hpp>

namespace  bpo = boost::program_options;
using namespace artdaq::database;

using artdaq::database::jsonutils::JSONDocument;


typedef bool (*test_case)(std::string const&, std::string const&,std::string const&);

bool test_insert(std::string const&, std::string const&,std::string const&);
bool test_search1(std::string const&, std::string const&,std::string const&);
bool test_search2(std::string const&, std::string const&,std::string const&);
bool test_update(std::string const&, std::string const&,std::string const&);

int main(int argc, char* argv[])
{
    artdaq::database::mongo::trace_enable();
    //artdaq::database::jsonutils::trace_enable_JSONDocument();

    debug::registerUngracefullExitHandlers();
    //artdaq::database::fhicljson::useFakeTime(true);

    // Get the input parameters via the boost::program_options library,
    // designed to make it relatively simple to define arguments and
    // issue errors if argument list is supplied incorrectly

    std::ostringstream descstr;
    descstr << argv[0] << " <-s <source-file>> <-c <compare-with-file>> <-t <test-name>> [<-o <options file>>] (available test names: insert,search1,search1,update)";

    bpo::options_description desc = descstr.str();

    desc.add_options()
    ("source,s", bpo::value<std::string>(),  "Input source file.")
    ("compare,c", bpo::value<std::string>(), "Expected result.")
    ("testname,t", bpo::value<std::string>(), "Test name.")
    ("options,o", bpo::value<std::string>(), "Test options file.")

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
    if (!vm.count("source")) {
        std::cerr << "Exception from command line processing in " << argv[0]
                  << ": no source file given.\n"
                  << "For usage and an options list, please do '"
                  << argv[0] <<  " --help"
                  << "'.\n";
        return 2;
    }

    if (!vm.count("compare")) {
        std::cerr << "Exception from command line processing in " << argv[0]
                  << ": no compare file given.\n"
                  << "For usage and an options list, please do '"
                  << argv[0] <<  " --help"
                  << "'.\n";
        return 3;
    }

    if (!vm.count("testname")) {
        std::cerr << "Exception from command line processing in " << argv[0]
                  << ": no test name given.\n"
                  << "For usage and an options list, please do '"
                  << argv[0] <<  " --help"
                  << "'.\n";
        return 4;
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


    auto options =std::string();
    
    if (vm.count("options")) {
          auto opts_name = vm["options"].as<std::string>();
  
          std::ifstream is3(opts_name);

          std::string tmp((std::istreambuf_iterator<char>(is3)),
                        std::istreambuf_iterator<char>());
	  
      options= std::move(tmp);
    }
    
    auto runTest = [](std::string const & name) {
        auto tests = std::map<std::string, test_case> {
            {"insert", test_insert}
           ,{"update", test_update}
           ,{"search1", test_search1}
           ,{"search2", test_search2}
        };

        std::cout << "Running test:<" << name << ">\n";

        return tests.at(name);
    };

    auto testResult = runTest(test_name)(input, compare, options);

    return !testResult;
}


bool test_insert(std::string const& source, std::string const& compare,std::string const& filter)
{
  assert(!source.empty());
  assert(!compare.empty());
  
  //validate source
  auto insert = JSONDocument(source);

  //validate compare
  auto expected = JSONDocument(compare);
  
  using  artdaq::database::basictypes::JsonData;
  
  namespace DBI= artdaq::database::mongo;
  
  auto config =DBI::DBConfig{};  
  auto database = DBI::DB::create(config);
  auto provider = DBI::DBProvider<JsonData>::create(database);
  
  auto colle11ctionName=std::string("testJSON_V001");
  
  auto json = JsonData{"{\"document\":" + insert.to_string() + ", \"collection\":\"" +colle11ctionName +"\"}"};

  auto object_id =  provider->store(json);
  
  auto search = JsonData{"{\"filter\":" + ( filter.empty()?object_id:filter)+ ", \"collection\":\"" +colle11ctionName +"\"}"};
  
  std::cout << "Search criteria " <<  search.json_buffer << "\n";

  auto collection = provider->load(search);
  
  if(collection.size()!=1) {    
      std::cout << "Search returned " << collection.size() << "results \n";
    
      for (auto&& element : collection) {
		std::cout << element.json_buffer  << "\n";
      }
      return false;
  }
  
  auto result = JSONDocument(collection.begin()->json_buffer);
  result.deleteChild("_id");

  if (result == expected)
        return true;
  else {
        std::cout << "Convertion failed. \n" ;
        std::cerr << "result:\n" << result << "\n";
        std::cerr << "expected:\n" << expected << "\n";
   }

    return false;
}


bool test_search1(std::string const& source, std::string const& compare,std::string const& filter)
{
  assert(!source.empty());
  assert(!compare.empty());
  
  //validate source
  auto insert = JSONDocument(source);

  //validate compare
  auto expected = JSONDocument(compare);
  
  using  artdaq::database::basictypes::JsonData;
  
  namespace DBI= artdaq::database::mongo;
  
  auto config =DBI::DBConfig{};  
  auto database = DBI::DB::create(config);
  auto provider = DBI::DBProvider<JsonData>::create(database);
  
  auto colle11ctionName=std::string("testJSON_V001");
  
  auto json = JsonData{"{\"document\":" + insert.to_string() + ", \"collection\":\"" +colle11ctionName +"\"}"};

  auto object_id =  provider->store(json);
  
  auto search = JsonData{"{\"filter\":" + ( filter.empty()?object_id:filter)+ ", \"collection\":\"" +colle11ctionName +"\"}"};
  
  std::cout << "Search criteria " <<  search.json_buffer << "\n";

  auto collection = provider->load(search);
  
  if(collection.size()!=1) {    
      std::cout << "Search returned " << collection.size() << "results \n";
    
      for (auto&& element : collection) {
		std::cout << element.json_buffer  << "\n";
      }
      return false;
  }
  
  auto result = JSONDocument(collection.begin()->json_buffer);
  result.deleteChild("_id");

  if (result == expected)
        return true;
  else {
        std::cout << "Convertion failed. \n" ;
        std::cerr << "result:\n" << result << "\n";
        std::cerr << "expected:\n" << expected << "\n";
   }

    return false;
}

bool test_search2(std::string const& source, std::string const& compare,std::string const& options[[gnu::unused]])
{
  assert(!source.empty());
  assert(!compare.empty());
    
  using  artdaq::database::basictypes::JsonData;
  
  namespace DBI= artdaq::database::mongo;
  
  auto config =DBI::DBConfig{};  
  auto database = DBI::DB::create(config);
  auto provider = DBI::DBProvider<JsonData>::create(database);
  
  auto colle11ctionName=std::string("testJSON_V001");
  
  auto json = JsonData{"{\"document\":" + source + ", \"collection\":\"" +colle11ctionName +"\"}"};

  auto repeatCount = std::size_t{100};
  
  auto object_ids = std::vector<std::string>();
  
  auto printComma=bool{false};
  
  std::stringstream ss;  
  ss << "{\"_id\" : { \"$in\" : [";
  
  for (int i=repeatCount; i!=0;i--) {
    auto object_id =  provider->store(json);    
    object_ids.push_back(object_id);
    ss << (printComma?',':' ') << object_id;
    printComma=true;
  }  
  ss << "]} }";
  
  auto filter = ss.str();
    
  auto search = JsonData{"{\"filter\":" + ( filter.empty()?options:filter)+ ", \"collection\":\"" +colle11ctionName +"\"}"};
  
  std::cout << "Search criteria " <<  search.json_buffer << "\n";

  auto collection = provider->load(search);
  
  if(collection.size()!=repeatCount) {    
      std::cout << "Search returned " << collection.size() << "results \n";
    
      for (auto&& element : collection) {
		std::cout << element.json_buffer  << "\n";
      }
      return false;
  }
  
  return true;
}

bool test_update(std::string const& source, std::string const& compare,std::string const& update)
{
  assert(!source.empty());
  assert(!compare.empty());
  
  //validate source
  auto insert = JSONDocument(source);

  //validate compare
  auto expected = JSONDocument(compare);
  
  auto changes = JSONDocument(update);
  
  using  artdaq::database::basictypes::JsonData;
  
  namespace DBI= artdaq::database::mongo;
  
  auto config =DBI::DBConfig{};  
  auto database = DBI::DB::create(config);
  auto provider = DBI::DBProvider<JsonData>::create(database);
  
  auto colle11ctionName=std::string("testJSON_V001");
  
  auto json = JsonData{"{\"document\":" + insert.to_string() + ", \"collection\":\"" +colle11ctionName +"\"}"};

  auto object_id =  provider->store(json);
  
  auto search = JsonData{"{\"filter\":" + object_id + ", \"collection\":\"" +colle11ctionName +"\"}"};
  
  std::cout << "Search criteria " <<  search.json_buffer << "\n";

  auto collection = provider->load(search);
  
  if(collection.size()!=1) {    
      std::cout << "Search returned " << collection.size() << "results \n";
    
      for (auto&& element : collection) {
		std::cout << element.json_buffer  << "\n";
      }
      return false;
  }
  
  auto result = JSONDocument(collection.begin()->json_buffer);
  result.deleteChild("_id");
  result.replaceChild(changes,"document");
  
  json = JsonData{"{\"document\":" + result.to_string() + ", \"filter\":" + object_id +  ",\"collection\":\"" +colle11ctionName +"\"}"};

  object_id =  provider->store(json);

  collection = provider->load(search);
  
  if(collection.size()!=1) {    
      std::cout << "Search returned " << collection.size() << "results \n";
    
      for (auto&& element : collection) {
		std::cout << element.json_buffer  << "\n";
      }
      return false;
  }  
  
  auto updated = JSONDocument(collection.begin()->json_buffer);
  updated.deleteChild("_id");

  if (updated == expected)
        return true;
  else {
        std::cout << "Convertion failed. \n" ;
        std::cerr << "updated:\n" << updated << "\n";
        std::cerr << "expected:\n" << expected << "\n";
   }

    return false;
}
